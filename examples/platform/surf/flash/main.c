/** This file is in the public domain.
 *
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

/* Assert chip select by clearing CSn */
#define CS_ASSERT() do {                                                \
    xBSP430hplLookupPORT(BSP430_PLATFORM_SURF_FLASH_CSn_PORT_PERIPH_HANDLE)->out &= ~BSP430_PLATFORM_SURF_FLASH_CSn_PORT_BIT; \
  } while (0)

/* De-assert chip select by setting CSn */
#define CS_DEASSERT() do {                                              \
    xBSP430hplLookupPORT(BSP430_PLATFORM_SURF_FLASH_CSn_PORT_PERIPH_HANDLE)->out |= BSP430_PLATFORM_SURF_FLASH_CSn_PORT_BIT; \
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

uint8_t out_buffer[10];
uint8_t buffer[256 + sizeof(out_buffer)];

void main ()
{
  int rc;
  hBSP430halSERIAL spi;
  
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

#if 0  
  P1SEL &= ~(BIT2 | BIT3 | BIT4 | BIT7);
  P1OUT &= ~(BIT2 | BIT3 | BIT4);
  P1OUT |= BIT7;
  P1DIR |= BIT2 | BIT3 | BIT4 | BIT7;
#endif
  
  cprintf("Clock speed %lu Hz\n", ulBSP430clockMCLK_Hz());
  spi = hBSP430serialOpenSPI(hBSP430serialLookup(BSP430_PLATFORM_SURF_FLASH_SPI_PERIPH_HANDLE),
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPL | UCMSB | UCMST),
                             UCSSEL_2, 1);
  
  cprintf("SPI %s at HAL %p : %s\n",
          xBSP430serialName(BSP430_PLATFORM_SURF_FLASH_SPI_PERIPH_HANDLE),
          spi,
          xBSP430platformPeripheralHelp(BSP430_PLATFORM_SURF_FLASH_SPI_PERIPH_HANDLE, 0));

  xBSP430hplLookupPORT(BSP430_PLATFORM_SURF_FLASH_CSn_PORT_PERIPH_HANDLE)->sel &= ~BSP430_PLATFORM_SURF_FLASH_CSn_PORT_BIT;
  xBSP430hplLookupPORT(BSP430_PLATFORM_SURF_FLASH_CSn_PORT_PERIPH_HANDLE)->out |= BSP430_PLATFORM_SURF_FLASH_CSn_PORT_BIT;
  xBSP430hplLookupPORT(BSP430_PLATFORM_SURF_FLASH_CSn_PORT_PERIPH_HANDLE)->dir |= BSP430_PLATFORM_SURF_FLASH_CSn_PORT_BIT;

  memset(out_buffer, 0xAB, sizeof(out_buffer));
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 4, buffer);
  CS_DEASSERT();
  cprintf("RES got %d, electronic signature %02x %s", rc, buffer[4],
          (0x10 == buffer[4]) ? "matches" : "DOES NOT MATCH");
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x05;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 1, buffer);
  CS_DEASSERT();
  cprintf("RDSR got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x03;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 4, 0, buffer);
  rc = iBSP430spiTxRx_ni(spi, NULL, 0, 64, buffer);
  CS_DEASSERT();
  cprintf("READ got %d", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x06;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 0, buffer);
  CS_DEASSERT();
  cprintf("WREN got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x05;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 1, buffer);
  CS_DEASSERT();
  cprintf("RDSR got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0xD8;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 4, 0, buffer);
  CS_DEASSERT();
  cprintf("SE got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x05;
  do {
    CS_ASSERT();
    rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 1, buffer);
    CS_DEASSERT();
    cprintf("RDSR got %d: ", rc);
    dumpMemory(buffer, rc, 0);
  } while (buffer[1] & 0x01);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x03;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 4, 64, buffer);
  CS_DEASSERT();
  cprintf("READ got %d", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x06;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 0, buffer);
  CS_DEASSERT();
  cprintf("WREN got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x05;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 1, buffer);
  CS_DEASSERT();
  cprintf("RDSR got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x02;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 4, 64, buffer);
  CS_DEASSERT();
  cprintf("PP got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x05;
  do {
    CS_ASSERT();
    rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 1, buffer);
    CS_DEASSERT();
    cprintf("RDSR got %d: ", rc);
    dumpMemory(buffer, rc, 0);
  } while (buffer[1] & 0x01);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x04;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 0, buffer);
  CS_DEASSERT();
  cprintf("WRDI got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x05;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 1, 1, buffer);
  CS_DEASSERT();
  cprintf("RDSR got %d: ", rc);
  dumpMemory(buffer, rc, 0);

  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer[0] = 0x03;
  CS_ASSERT();
  rc = iBSP430spiTxRx_ni(spi, out_buffer, 4, 64, buffer);
  CS_DEASSERT();
  cprintf("READ got %d", rc);
  dumpMemory(buffer, rc, 0);

}
