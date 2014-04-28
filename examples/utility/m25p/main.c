/** This file is in the public domain.
 *
 * This program demonstrates use of the M25PE20 SPI serial flash on
 * the TrxEB board, or the M25P10A SPI serial flash on the SuRF board.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/m25p.h>
#include <ctype.h>
#include <string.h>

#ifndef WITH_W25Q80BV
#define WITH_W25Q80BV (BSP430_PLATFORM_EXP430F5529LP - 0)
#endif /* WITH_W25Q80BV */

#if (WITH_W25Q80BV - 0)
/* Winbond W25Q80BV is a 8 Mibit (1 MiBy) serial FLASH supporting 256
 * 4 KiBy sub-sectors (sectors) or 16 64 KiBy sectors (blocks).  PAGE
 * WRITE and PAGE ERASE are not supported; (SUB) SECTOR ERASE is
 * supported. */
#define BSP430_PLATFORM_M25P_SUPPORTS_SSE 1
#define BSP430_PLATFORM_M25P_SECTOR_COUNT 16
#define BSP430_PLATFORM_M25P_SECTOR_SIZE 0x10000
#define BSP430_PLATFORM_M25P_SUBSECTOR_SIZE 0x1000

#if (BSP430_PLATFORM_EXP430F5529LP - 0)
/* SPI on USCI_A0, CSn on P6.6, PWR and RSTn hard-wired */
#define BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT6
#define BSP430_PLATFORM_M25P_CSn_PORT_BIT BIT6
#endif
#endif /* WITH_W25Q80BV */

void dumpMemory (const uint8_t * dp,
                 size_t len,
                 unsigned long base)
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
      cprintf("\n%08lx ", base);
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
 * sixteen bytes which have this useful test pattern. */
const uint8_t flashContents[] = {
  0xAA, 0x55, 0x0F, 0xF0, 0xCC, 0x33, 0xC3, 0x3C,
  0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
};

uint8_t buffer[256];

int readFromAddress (hBSP430m25p m25p,
                     unsigned long addr,
                     unsigned int len)
{
  int rc = -1;

  if (len > sizeof(buffer)) {
    len = sizeof(buffer);
  }
  if (0 == iBSP430m25pInitiateAddressCommand_rh(m25p, BSP430_M25P_CMD_FAST_READ, addr)) {
    rc = iBSP430m25pCompleteTxRx_rh(m25p, NULL, 0, len, buffer);
  }
  return rc;
}

int writeToAddress (hBSP430m25p m25p,
                    uint8_t cmd,
                    unsigned long addr,
                    const uint8_t * data,
                    unsigned int len)
{
  unsigned long t0;
  unsigned long t1;
  int rc;
  int sr;

  rc = iBSP430m25pStrobeCommand_rh(m25p, BSP430_M25P_CMD_WREN);
  if (0 == rc) {
    rc = iBSP430m25pInitiateAddressCommand_rh(m25p, cmd, addr);
  }
  if (0 == rc) {
    rc = iBSP430m25pCompleteTxRx_rh(m25p, data, len, 0, NULL);
  }
  t0 = ulBSP430uptime();
  do {
    sr = iBSP430m25pStatus(m25p);
  } while ((0 <= sr) && (BSP430_M25P_SR_WIP & sr));
  t1 = ulBSP430uptime();
  cprintf("Write %d took %lu\n", len, t1 - t0);
  return rc;
}

void main ()
{
  int rc;
  sBSP430m25p m25p_data;
  hBSP430m25p m25p;
  unsigned long addr;
  unsigned long t0;
  unsigned long t1;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  cputchar('\n');
  BSP430_CORE_DELAY_CYCLES(100 * (BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000));
  cprintf("\nBuild " __DATE__ " " __TIME__ "\n");
  cprintf("SPI is %s: %s\n",
          xBSP430serialName(BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE),
          xBSP430platformPeripheralHelp(BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE, 0));

#ifdef BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE
  cprintf("PWR at %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_M25P_PWR_PORT_BIT));
#else /* BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE */
  cprintf("PWR is hard-wired\n");
#endif /* BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE */

#ifdef BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE
  cprintf("RSTn at %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_M25P_RSTn_PORT_BIT));
#else /* BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE */
  cprintf("RSTn is hard-wired\n");
#endif /* BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE */
  cprintf("CSn at %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE),
          iBSP430portBitPosition(BSP430_PLATFORM_M25P_CSn_PORT_BIT));

  memset(&m25p_data, 0, sizeof(m25p_data));
  m25p_data.spi = hBSP430serialLookup(BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE);
  m25p_data.csn_port = xBSP430hplLookupPORT(BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE);
  m25p_data.csn_bit = BSP430_PLATFORM_M25P_CSn_PORT_BIT;
#ifdef BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE
  m25p_data.rstn_port = xBSP430hplLookupPORT(BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE);
  m25p_data.rstn_bit = BSP430_PLATFORM_M25P_RSTn_PORT_BIT;
#endif /* BSP430_PLATFORM_M25P_RSTn_PORT_PERIPH_HANDLE */

  m25p = hBSP430m25pInitialize(&m25p_data,
                               BSP430_PLATFORM_M25P_SPI_CTL0_BYTE,
                               UCSSEL_2, 1);
  if (NULL == m25p) {
    cprintf("M25P device initialization failed.\n");
    return;
  }

  {
#if (BSP430_MODULE_USCI5 - 0)
    volatile sBSP430hplUSCI5 * hpl = BSP430_SERIAL_HAL_GET_HPL_USCI5(m25p->spi);
#else
    void * hpl = NULL;
#endif
    int sm = -1;

    if (hpl) {
      sm = 0;
      if (hpl->ctl0 & BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPL)) {
        sm |= 0x02;
      }
      if (! (hpl->ctl0 & BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPH))) {
        sm |= 0x01;
      }
    }
    if (0 > sm) {
      cprintf("Unable to extract SPI mode from %s\n",
              xBSP430serialName(BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE));
    } else {
      cprintf("%s initialized in SPI mode %u\n", xBSP430serialName(BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE), sm);
    }
  }

#ifdef BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE
  {
    volatile sBSP430hplPORT * pwr_hpl;
    /* Turn on power, then wait 10 ms for chip to stabilize before releasing RSTn. */
    pwr_hpl = xBSP430hplLookupPORT(BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE);
    pwr_hpl->out &= ~BSP430_PLATFORM_M25P_PWR_PORT_BIT;
    pwr_hpl->dir |= BSP430_PLATFORM_M25P_PWR_PORT_BIT;
    pwr_hpl->out |= BSP430_PLATFORM_M25P_PWR_PORT_BIT;
    BSP430_CORE_DELAY_CYCLES(10 * (BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000));
  }
#endif /* BSP430_PLATFORM_M25P_PWR_PORT_PERIPH_HANDLE */
  BSP430_M25P_RESET_CLEAR(m25p);

  cprintf("Status register %d\n", iBSP430m25pStatus_rh(m25p));
  rc = iBSP430m25pStrobeCommand_rh(m25p, BSP430_M25P_CMD_WREN);
  cprintf("WREN got %d, status register %d\n", rc, iBSP430m25pStatus_rh(m25p));
  rc = iBSP430m25pStrobeCommand_rh(m25p, BSP430_M25P_CMD_WRDI);
  cprintf("WRDI got %d, status register %d\n", rc, iBSP430m25pStatus_rh(m25p));

  rc = iBSP430m25pInitiateCommand_rh(m25p, BSP430_M25P_CMD_RDID);
  if (0 == rc) {
    rc = iBSP430m25pCompleteTxRx_rh(m25p, NULL, 0, 20, buffer);
  }

  BSP430_CORE_ENABLE_INTERRUPT();
  cprintf("READ_IDENTIFICATION got %d\n", rc);
  if (0 <= rc) {
    dumpMemory(buffer, rc, 0);
  }
  cprintf("Config identified %u sectors of %lu bytes each: %lu bytes total\n",
          BSP430_PLATFORM_M25P_SECTOR_COUNT,
          (unsigned long)BSP430_PLATFORM_M25P_SECTOR_SIZE,
          BSP430_PLATFORM_M25P_SECTOR_COUNT * (unsigned long)BSP430_PLATFORM_M25P_SECTOR_SIZE);
#if (BSP430_PLATFORM_M25P_SUBSECTOR_SIZE - 0)
  cprintf("Config supports access as %u sub-sectors of %u bytes each\n",
          (unsigned int)(BSP430_PLATFORM_M25P_SECTOR_COUNT * (BSP430_PLATFORM_M25P_SECTOR_SIZE / BSP430_PLATFORM_M25P_SUBSECTOR_SIZE)),
          (unsigned int)BSP430_PLATFORM_M25P_SUBSECTOR_SIZE);
#else /* BSP430_PLATFORM_M25P_SUBSECTOR_SIZE */
  cprintf("Config indicates device is not sub-sector addressable\n");
#endif /* BSP430_PLATFORM_M25P_SUBSECTOR_SIZE */
  cprintf("RDID identified %lu bytes total capacity\n", 0x1UL << buffer[2]);

  addr = 0;

  rc = readFromAddress(m25p, addr, sizeof(flashContents));
  if (sizeof(flashContents) != rc) {
    cprintf("ERROR %d reading initial block\n", rc);
  } else {
    dumpMemory(buffer, rc, addr);
    if (0 == memcmp(flashContents, buffer, rc)) {
      cprintf("Found expected contents.\n");
    } else {
      cprintf("Did NOT find expected contents.\n");
    }
  }

  cprintf("\nTest pattern (expected contents):");
  dumpMemory(flashContents, sizeof(flashContents), 0);

#if (BSP430_PLATFORM_M25P_SUPPORTS_PE - 0)
  rc = writeToAddress(m25p, BSP430_M25P_CMD_PE, addr, NULL, 0);
  cprintf("PAGE_ERASE got %d\n", rc);
#else /* BSP430_PLATFORM_M25P_SUPPORTS_PE */
  rc = writeToAddress(m25p, BSP430_M25P_CMD_SE, addr, NULL, 0);
  cprintf("SECTOR_ERASE got %d\n", rc);
#endif /* BSP430_PLATFORM_M25P_SUPPORTS_PE */
  rc = readFromAddress(m25p, addr, sizeof(buffer));
  if (0 < rc) {
    dumpMemory(buffer, rc, addr);
  }

  rc = writeToAddress(m25p, BSP430_M25P_CMD_PP, addr, flashContents, sizeof(flashContents));
  cprintf("PAGE_PROGRAM got %d\n", rc);
  rc = readFromAddress(m25p, addr, sizeof(buffer));
  if (0 < rc) {
    dumpMemory(buffer, rc, addr);
  }

  /* PAGE PROGRAM is the one that only clears 1s to 0s so needs a
   * prior page or sector erase */
  rc = writeToAddress(m25p, BSP430_M25P_CMD_PP, addr, flashContents + 4, 4);
  cprintf("PAGE_PROGRAM to %lx returned %d\n", addr, rc);
  rc = readFromAddress(m25p, 0, sizeof(flashContents));
  dumpMemory(buffer, rc, 0);
  /*
  Write 4 took 8
  PAGE_PROGRAM to 0 returned 4
  00000000  88 11 03 30 cc 33 c3 3c  01 23 45 67 89 ab cd ef  ...0.3.<.#Eg....
  */

  /* PAGE_WRITE is the one that does not need a prior erase cycle */
  addr = 8;
#if (BSP430_PLATFORM_M25P_SUPPORTS_PW - 0)
  rc = writeToAddress(m25p, BSP430_M25P_CMD_PW, addr, flashContents + 4, 4);
  cprintf("PAGE_WRITE to %lx returned %d\n", addr, rc);
#else
  rc = writeToAddress(m25p, BSP430_M25P_CMD_PP, addr, flashContents + 4, 4);
  cprintf("overwrite PAGE_PROGRAM to unerased %lx returned %d\n", addr, rc);
#endif
  rc = readFromAddress(m25p, 0, sizeof(flashContents));
  dumpMemory(buffer, rc, 0);
  /*
  Write 4 took 204
  PAGE_WRITE to 8 returned 4
  00000000  88 11 03 30 cc 33 c3 3c  cc 33 c3 3c 89 ab cd ef  ...0.3.<.3.<....
  */

  cprintf("Initiating bulk erase...");
  t0 = t1 = 0;
  rc = iBSP430m25pStrobeCommand_rh(m25p, BSP430_M25P_CMD_WREN);
  if (0 == rc) {
    rc = iBSP430m25pStrobeCommand_rh(m25p, BSP430_M25P_CMD_BE);
  }
  if (0 == rc) {
    int sr;

    t0 = ulBSP430uptime();
    do {
      sr = iBSP430m25pStatus_rh(m25p);
    } while ((0 <= sr) && (BSP430_M25P_SR_WIP & sr));
    t1 = ulBSP430uptime();
  }
  cprintf("\nBULK_ERASE got %d\n", rc);
  if (0 == rc) {
    char tstr[BSP430_UPTIME_AS_TEXT_LENGTH];
    cprintf("Bulk erase took %lu utt = %s\n", t1-t0, xBSP430uptimeAsText(t1 - t0, tstr));
  }
  rc = readFromAddress(m25p, 0, sizeof(flashContents));
  dumpMemory(buffer, rc, 0);

  rc = writeToAddress(m25p, BSP430_M25P_CMD_PP, 0, flashContents, sizeof(flashContents));
  cprintf("Restore got %d\n", rc);

  addr = 0;
  while (addr < (256 * 1025L)) {
    rc = readFromAddress(m25p, addr, sizeof(buffer));
    if (0 > rc) {
      break;
    }
    dumpMemory(buffer, rc, addr);
    addr += rc;
    break;
  }

}
