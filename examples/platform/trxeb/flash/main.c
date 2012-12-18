/** This file is in the public domain.
 *
 * This program demonstrates use of the M25PE20 SPI serial flash on
 * the TrxEB board.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>
#include <ctype.h>
#include <string.h>

/* FLASH SPI interface */
static hBSP430halSERIAL spi_;

/* FLASH CSn HPL */
static volatile sBSP430hplPORT * hplCSn_;

/* Assert chip select by clearing CSn */
#define CS_ASSERT() do {                                                \
    hplCSn_->out &= ~BSP430_PLATFORM_TRXEB_FLASH_CSn_PORT_BIT;          \
  } while (0)

/* De-assert chip select by setting CSn */
#define CS_DEASSERT() do {                                             \
    hplCSn_->out |= BSP430_PLATFORM_TRXEB_FLASH_CSn_PORT_BIT;          \
  } while (0)

void dumpMemory (const uint8_t * dp,
                 size_t len,
                 size_t base)
{
  const uint8_t * const edp = dp + len;
  const uint8_t * adp = dp;

  while (dp < edp) {
    if (0 == (base & 0x0F)) {
      if (adp < dp) {
        cprintf("  ");
        while (adp < dp) {
          cputchar(isprint(*adp) ? *adp : '.');
          ++adp;
        }
      }
      adp = dp;
      cprintf("\n%08x ", base);
    } else if (0 == (base & 0x07)) {
      cputchar(' ');
    }
    cprintf(" %02x", *dp++);
    ++base;
  }
  if (adp < dp) {
    while (base & 0x0F) {
      if (0 == (base & 0x07)) {
        cputchar(' ');
      }
      cprintf("   ");
      ++base;
    }
    cprintf("  ");
    while (adp < dp) {
      cputchar(isprint(*adp) ? *adp : '.');
      ++adp;
    }
  }
  cputchar('\n');
}

/* As-delivered TrxEB flash is completely erased except for the first
 * sixteen bytes. */
const uint8_t flashContents[] = { 0xAA, 0x55, 0x0F, 0xF0, 0xCC, 0x33, 0xC3, 0x3C,
                                  0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
                                };

uint8_t output[10];
uint8_t buffer[256];

int sendCommand (uint8_t cmd,
                 uint8_t * result,
                 unsigned int len)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  CS_ASSERT();
  do {
    rc = iBSP430spiTxRx_ni(spi_, &cmd, sizeof(cmd), 0, NULL);
    if (sizeof(cmd) == rc) {
      rc = len;
      if (0 < len) {
        rc = iBSP430spiTxRx_ni(spi_, NULL, 0, len, result);
      }
    } else {
      rc = -1;
    }
  } while (0);
  CS_DEASSERT();
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  if (len != rc) {
    return -1;
  }
  return rc;
}

int rdsr (void)
{
  uint8_t sr;
  if (1 == sendCommand(0x05, &sr, 1)) {
    return sr;
  }
  return -1;
}

int readFromAddress (unsigned long addr,
                     unsigned int len)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  uint8_t cmdb[4];
  int rc;

  cmdb[0] = 3;
  cmdb[1] = 0xFF & (addr >> 16);
  cmdb[2] = 0xFF & (addr >> 8);
  cmdb[3] = 0xFF & addr;

  if (len > sizeof(buffer)) {
    len = sizeof(buffer);
  }

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  CS_ASSERT();
  do {
    rc = iBSP430spiTxRx_ni(spi_, cmdb, sizeof(cmdb), 0, NULL);
    if (sizeof(cmdb) == rc) {
      rc = iBSP430spiTxRx_ni(spi_, NULL, 0, len, buffer);
    } else {
      rc = -1;
    }
  } while (0);
  CS_DEASSERT();
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  if (len != rc) {
    return -1;
  }
  return rc;
}

int writeToAddress (uint8_t cmd,
                    unsigned long addr,
                    const uint8_t * data,
                    unsigned int len)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  uint8_t cmdb[4];
  unsigned long t0;
  unsigned long t1;
  int rc;
  int sr;

  cmdb[0] = cmd;
  cmdb[1] = 0xFF & (addr >> 16);
  cmdb[2] = 0xFF & (addr >> 8);
  cmdb[3] = 0xFF & addr;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  CS_ASSERT();
  do {
    uint8_t u8;

    u8 = 0x06; /* WREN */
    rc = iBSP430spiTxRx_ni(spi_, &u8, 1, 0, NULL);
    if (1 != rc) {
      rc = -1;
      break;
    }
    CS_DEASSERT();
    CS_ASSERT();
    rc = iBSP430spiTxRx_ni(spi_, cmdb, sizeof(cmdb), 0, NULL);
    if (sizeof(cmdb) != rc) {
      rc = -1;
      break;
    }
    rc = len;
    if (0 < len) {
      rc = iBSP430spiTxRx_ni(spi_, data, len, 0, NULL);
    }
  } while (0);
  CS_DEASSERT();
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  t0 = ulBSP430uptime();
  do {
    sr = rdsr();
  } while ((0 <= sr) && (1 & sr));
  t1 = ulBSP430uptime();
  cprintf("Write %d took %lu\n", len, t1 - t0);
  return rc;
}

void main ()
{
  int rc;
  volatile sBSP430hplPORT * rst_hpl;
  volatile sBSP430hplPORT * pwr_hpl;
  unsigned long addr;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\nBuild " __DATE__ " " __TIME__ "\n");
  cprintf("SPI is %s: %s\n",
          xBSP430serialName(BSP430_PLATFORM_TRXEB_FLASH_SPI_PERIPH_HANDLE),
          xBSP430platformPeripheralHelp(BSP430_PLATFORM_TRXEB_FLASH_SPI_PERIPH_HANDLE, 0));

  rst_hpl = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_FLASH_RSTn_PORT_PERIPH_HANDLE);
  pwr_hpl = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_FLASH_PWR_PORT_PERIPH_HANDLE);
  cprintf("PWR at %s.%u ; RSTn at %s.%u ; CSn at %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_TRXEB_FLASH_PWR_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_TRXEB_FLASH_PWR_PORT_BIT),
          xBSP430portName(BSP430_PLATFORM_TRXEB_FLASH_RSTn_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_TRXEB_FLASH_RSTn_PORT_BIT),
          xBSP430portName(BSP430_PLATFORM_TRXEB_FLASH_CSn_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_TRXEB_FLASH_CSn_PORT_BIT));

  /* Deassert chip select for flash */
  hplCSn_ = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_FLASH_CSn_PORT_PERIPH_HANDLE);
  hplCSn_->dir |= BSP430_PLATFORM_TRXEB_FLASH_CSn_PORT_BIT;
  CS_DEASSERT();

  /* Drive RSTn low during power-on*/
  rst_hpl->out &= ~BSP430_PLATFORM_TRXEB_FLASH_RSTn_PORT_BIT;
  rst_hpl->dir |= BSP430_PLATFORM_TRXEB_FLASH_RSTn_PORT_BIT;

  /* Turn on power, then wait 10 ms for chip to stabilize before releasing RSTn. */
  pwr_hpl->out &= ~BSP430_PLATFORM_TRXEB_FLASH_PWR_PORT_BIT;
  pwr_hpl->dir |= BSP430_PLATFORM_TRXEB_FLASH_PWR_PORT_BIT;
  pwr_hpl->out |= BSP430_PLATFORM_TRXEB_FLASH_PWR_PORT_BIT;
  BSP430_CORE_DELAY_CYCLES(10 * (BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000));
  rst_hpl->out |= BSP430_PLATFORM_TRXEB_FLASH_RSTn_PORT_BIT;

  /* Now configure for SPI. */
  spi_ = hBSP430serialOpenSPI(hBSP430serialLookup(BSP430_PLATFORM_TRXEB_FLASH_SPI_PERIPH_HANDLE),
                              BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPL | UCMSB | UCMST),
                              UCSSEL_2, 100);

  BSP430_CORE_ENABLE_INTERRUPT();

  cprintf("Status register %d\n", rdsr());

  rc = sendCommand(0x9f, buffer, 20);
  cprintf("READ_IDENTIFICATION got %d\n", rc);
  if (0 <= rc) {
    dumpMemory(buffer, rc, 0);
  }

  addr = 0;

  rc = readFromAddress(addr, sizeof(flashContents));
  if (sizeof(flashContents) != rc) {
    cprintf("ERROR %d reading initial block\n", rc);
  } else {
    dumpMemory(buffer, rc, addr);
    if (0 == memcmp(flashContents, buffer, rc)) {
      cprintf("Found expected contents.\n");
    }
  }

  rc = writeToAddress(0xDB, addr, NULL, 0);
  cprintf("PAGE_ERASE got %d\n", rc);
  rc = readFromAddress(addr, sizeof(buffer));
  if (0 < rc) {
    dumpMemory(buffer, rc, addr);
  }

  rc = writeToAddress(0x02, addr, flashContents, sizeof(flashContents));
  cprintf("PAGE_PROGRAM got %d\n", rc);
  rc = readFromAddress(addr, sizeof(buffer));
  if (0 < rc) {
    dumpMemory(buffer, rc, addr);
  }

  /* PAGE PROGRAM is the one that only clears 1s to 0s so needs a
   * prior page erase */
  rc = writeToAddress(0x02, addr, flashContents + 4, 4);
  cprintf("PAGE_PROGRAM to %lx returned %d\n", addr, rc);
  rc = readFromAddress(0, sizeof(flashContents));
  dumpMemory(buffer, rc, 0);
  /*
  Write 4 took 8
  PAGE_PROGRAM to 0 returned 4
  00000000  88 11 03 30 cc 33 c3 3c  01 23 45 67 89 ab cd ef  ...0.3.<.#Eg....
  */

  /* PAGE_WRITE is the one that does not need a prior erase cycle */
  addr = 8;
  rc = writeToAddress(0x0a, addr, flashContents + 4, 4);
  cprintf("PAGE_WRITE to %lx returned %d\n", addr, rc);
  rc = readFromAddress(0, sizeof(flashContents));
  dumpMemory(buffer, rc, 0);
  /*
  Write 4 took 204
  PAGE_WRITE to 8 returned 4
  00000000  88 11 03 30 cc 33 c3 3c  cc 33 c3 3c 89 ab cd ef  ...0.3.<.3.<....
  */

  rc = writeToAddress(0x0a, 0, flashContents, sizeof(flashContents));
  cprintf("Restore got %d\n", rc);

  addr = 0;
  while (addr < (256 * 1025L)) {
    rc = readFromAddress(addr, sizeof(buffer));
    if (0 > rc) {
      break;
    }
    dumpMemory(buffer, rc, addr);
    addr += rc;
    break;
  }

}
