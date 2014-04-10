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
#include <bsp430/utility/led.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/u8glib.h>
#include <string.h>

static u8g_t u8g_data;

#if (BSP430_U8GLIB_REFRESH_INTERVAL_MS - 0)

#ifndef REFRESH_ALARM_CCIDX
#define REFRESH_ALARM_CCIDX 1
#endif /* REFRESH_ALARM_CCIDX */

typedef struct sRefreshTimerState {
  sBSP430timerAlarm alarm;
  hBSP430timerAlarm active_alarm;
  unsigned long interval_tck;
  u8g_t * u8g;
} sRefreshTimerState;

static sRefreshTimerState refresh_state;

static int
refresh_cb_ni (hBSP430timerAlarm alarm)
{
  sRefreshTimerState * rtsp = (sRefreshTimerState *)alarm;
  unsigned long setting_tck;
  int rc;
  int rv;

  vBSP430ledSet(BSP430_LED_GREEN, -1);
  rv = iBSP430u8gRefresh(rtsp->u8g);
  setting_tck = alarm->setting_tck;
  do {
    setting_tck += rtsp->interval_tck;
    rc = iBSP430timerAlarmSet_ni(alarm, setting_tck);
  } while (0 < rc);
  return rv;
}

static int
enableRefreshTimer ()
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  hBSP430timerAlarm ah;
  int rc = 0;

  refresh_state.interval_tck = BSP430_UPTIME_MS_TO_UTT(BSP430_U8GLIB_REFRESH_INTERVAL_MS);
  if (0 == refresh_state.interval_tck) {
    cprintf("Refresh timer defined but empty interval?");
    return -1;
  }
  cprintf("Enable refresh every %lu ticks\n", refresh_state.interval_tck);
  refresh_state.u8g = &u8g_data;
  ah = hBSP430timerAlarmInitialize(&refresh_state.alarm,
                                   BSP430_UPTIME_TIMER_PERIPH_HANDLE,
                                   REFRESH_ALARM_CCIDX,
                                   refresh_cb_ni);
  if (ah) {
    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      rc = iBSP430timerAlarmSetEnabled_ni(ah, 1);
      if (0 == rc) {
        rc = iBSP430timerAlarmSet_ni(ah, ulBSP430uptime_ni() + refresh_state.interval_tck);
      }
    } while (0);
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  }
  if (0 != rc) {
    cprintf("Failed to initialize alarm\n");
  }
  return rc;
}

#else /* BSP430_U8GLIB_REFRESH_INTERVAL_MS */

static int
enableRefreshTimer ()
{
  cprintf("No refresh timer required\n");
  return 0;
}

#endif /* BSP430_U8GLIB_REFRESH_INTERVAL_MS */


void main ()
{
  int w;
  int h;
  int rc;
  unsigned long t0;
  unsigned long t1;
  hBSP430halSERIAL spi;
  u8g_t * u8g = &u8g_data;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  BSP430_CORE_ENABLE_INTERRUPT();

  cprintf("\nBuild " __DATE__ " " __TIME__ "\n");

  rc = u8g_Init(u8g, &xBSP430u8gDevice);
  cprintf("U8G device initialization got %d\n", rc);
  spi = hBSP430u8gSPI();
  cprintf("SPI is %s: %s\n", xBSP430serialName(xBSP430periphFromHPL(spi->hpl.any)),
          xBSP430platformPeripheralHelp(xBSP430periphFromHPL(spi->hpl.any), BSP430_PERIPHCFG_SERIAL_SPI3));

  if (0 != enableRefreshTimer()) {
    /* Failure to configure the refresh timer is a fatal error, on the
     * theory that it could result in damage due to DC bias */
    cprintf("ERROR: Unable to configure refresh timer\n");
    return;
  }

  cprintf("Device size %u x %u, mode %d\n", u8g_GetWidth(u8g), u8g_GetHeight(u8g), u8g_GetMode(u8g));

  u8g_SetDefaultForegroundColor(u8g);
  u8g_SetFont(u8g, u8g_font_10x20);
  w = u8g_GetFontBBXWidth(u8g);
  h = u8g_GetFontBBXHeight(u8g);
  cprintf("Font cell %u x %u\n", w, h);

  t0 = ulBSP430uptime_ni();
  u8g_FirstPage(u8g);
  do {
    u8g_DrawLine(u8g, 0, 0, u8g_GetWidth(u8g), u8g_GetHeight(u8g));
    u8g_DrawStr(u8g, 0, h, __DATE__);
    u8g_DrawStr(u8g, 0, 3*h, __TIME__);
  } while (u8g_NextPage(u8g));
  t1 = ulBSP430uptime_ni();

  cprintf("Write screen took %lu ticks\n", t1-t0);

#if 0
  {
    unsigned int contrast;
    cprintf("Cycling through contrast levels\n");
    for (contrast = 0; contrast < 256; contrast += 16) {
      u8g_SetContrast(u8g, contrast);
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
    }
    /* Don't muck with contrast unless you have to.  Legibility is
     * different on different LCDs; TrxEB works with 22/64,
     * EXP430F5529 works at 8/64, EXP430F5438 works at 0x58/128.
     * None is legible at another's setting. */
    u8g_SetContrast(u8g, 22 << 2);
  }
#endif

  cprintf("Toggling sleep mode\n");
  while (1) {
    u8g_SleepOff(u8g);
    vBSP430ledSet(BSP430_LED_RED, 1);
    BSP430_CORE_DELAY_CYCLES(1 * BSP430_CLOCK_NOMINAL_MCLK_HZ);
    vBSP430ledSet(BSP430_LED_RED, 0);
    u8g_SleepOn(u8g);
    BSP430_CORE_DELAY_CYCLES(2 * BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
