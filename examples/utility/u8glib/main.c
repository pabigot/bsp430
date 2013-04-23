/** This file is in the public domain.
 *
 * Demonstrate configuration and basic interaction with LCD using
 * u8glib.
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
#include <bsp430/utility/u8glib.h>
#include <string.h>

void main ()
{
  int w;
  int h;
  u8g_t u8g;
  int rc;
  unsigned long t0;
  unsigned long t1;
  hBSP430halSERIAL spi;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\nBuild " __DATE__ " " __TIME__ "\n");

  rc = u8g_Init(&u8g, &xBSP430u8gDevice);
  cprintf("U8G device initialization got %d\n", rc);
  spi = hBSP430u8gSPI();
  cprintf("SPI is %s: %s\n", xBSP430serialName(xBSP430periphFromHPL(spi->hpl.any)),
          xBSP430platformPeripheralHelp(xBSP430periphFromHPL(spi->hpl.any), BSP430_PERIPHCFG_SERIAL_SPI3));
  cprintf("Device size %u x %u, mode %d\n", u8g_GetWidth(&u8g), u8g_GetHeight(&u8g), u8g_GetMode(&u8g));

  /* Use maximum color value */
  u8g_SetColorIndex(&u8g, (1 << u8g_GetMode(&u8g)) - 1);
  u8g_SetFont(&u8g, u8g_font_10x20);
  w = u8g_GetFontBBXWidth(&u8g);
  h = u8g_GetFontBBXHeight(&u8g);
  cprintf("Font cell %u x %u\n", w, h);

  t0 = ulBSP430uptime_ni();
  u8g_FirstPage(&u8g);
  do {
    u8g_DrawStr(&u8g, 0, h, "Hello World!");
    u8g_DrawStr(&u8g, 0, 3*h, "(Hi there!)");
  } while (u8g_NextPage(&u8g));
  t1 = ulBSP430uptime_ni();

  cprintf("Write screen took %lu ticks\n", t1-t0);

#if 0
  {
    unsigned int contrast;
    cprintf("Cycling through contrast levels\n");
    for (contrast = 0; contrast < 256; contrast += 16) {
      u8g_SetContrast(&u8g, contrast);
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
    }
    /* Don't muck with contrast unless you have to.  Legibility is
     * different on different LCDs; TrxEB works with 22/64,
     * EXP430F5529 works at 8/64, EXP430F5438 works at 0x58/128.
     * None is legible at another's setting. */
    u8g_SetContrast(&u8g, 22 << 2);
  }
#endif

  cprintf("Toggling sleep mode\n");
  while (1) {
    u8g_SleepOn(&u8g);
    BSP430_CORE_DELAY_CYCLES(2 * BSP430_CLOCK_NOMINAL_MCLK_HZ);
    u8g_SleepOff(&u8g);
    BSP430_CORE_DELAY_CYCLES(1 * BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}


