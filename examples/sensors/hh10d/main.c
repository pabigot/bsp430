/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <string.h>
#include <bsp430/platform.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/onewire.h>
#include <bsp430/periph/timer.h>
#include <bsp430/sensors/hh10d.h>

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif /* BSP430_CONSOLE */
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif /* BSP430_UPTIME */
/* Sanity check that we have the HH10D interface */
#ifndef APP_HH10D_PORT_PERIPH_HANDLE
#error No HH10D port specified
#endif /* APP_HH10D_PORT_PERIPH_HANDLE */

#ifndef APP_HH10D_INTERVAL_S
#define APP_HH10D_INTERVAL_S 2
#endif /* APP_HH10D_INTERVAL_S */

void main ()
{
  sBSP430sensorsHH10Dstate hh10d;

  sBSP430halPORT * hh10d_port = hBSP430portLookup(APP_HH10D_PORT_PERIPH_HANDLE);
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_HH10D_I2C_PERIPH_HANDLE);
  unsigned long uptime_Hz;
  int rc;

  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();
  cprintf("\nhh10d " __DATE__ " " __TIME__ "\n");
  uptime_Hz = ulBSP430uptimeConversionFrequency_Hz_ni();
  BSP430_CORE_DELAY_CYCLES(10000);

  if (NULL == hh10d_port) {
    cprintf("\nERROR: No port HAL; did you enable configBSP430_HAL_%s?\n", xBSP430portName(APP_HH10D_PORT_PERIPH_HANDLE) ?: "whatever");
    return;
  }

  /* Initialize the state information used in the HH10D ISR. */
  memset(&hh10d, 0, sizeof(hh10d));
  hh10d.cb.callback_ni = iBSP430sensorsHH10DperiodicCallback_ni;
  hh10d.freq_timer = xBSP430hplLookupTIMER(APP_HH10D_TIMER_PERIPH_HANDLE);
  hh10d.interval_tck = APP_HH10D_INTERVAL_S * uptime_Hz;
  hh10d.flags = BSP430_SENSORS_HH10D_FLAG_WAKE_ON_COUNT | BSP430_SENSORS_HH10D_FLAG_AUTOSAMPLE;

  /* Set up so we can safely read the counter value, since the clock
   * is asynchronous to MCLK. */
  vBSP430timerSafeCounterInitialize_ni(hh10d.freq_timer);

  cprintf("HH10D I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
          xBSP430serialName(APP_HH10D_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
          i2c, (unsigned long)BSP430_SERIAL_I2C_BUS_SPEED_HZ,
          APP_HH10D_I2C_ADDRESS);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("HH10D I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_HH10D_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  cprintf("Monitoring HH10D on %s.%u using timer %s\n",
          xBSP430portName(APP_HH10D_PORT_PERIPH_HANDLE) ?: "P?",
          iBSP430portBitPosition(APP_HH10D_PORT_BIT),
          xBSP430timerName(APP_HH10D_TIMER_PERIPH_HANDLE) ?: "T?");
  cprintf("Uptime CC block %s.%u at %lu Hz sample duration %u ticks for interval %us\n",
          xBSP430timerName(BSP430_UPTIME_TIMER_PERIPH_HANDLE),
          APP_HH10D_UPTIME_CC_INDEX, uptime_Hz,
          hh10d.interval_tck, APP_HH10D_INTERVAL_S);

  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  rc = iBSP430sensorsHH10DgetCalibration(i2c, &hh10d);
  if (0 != rc) {
    cprintf("ERR: getCalibration returned %d\n", rc);
    return;
  }
  cprintf("I2C read offset %u sensitivity %u\n", hh10d.cal_offs, hh10d.cal_sens);

  /* Select input peripheral function mode for CCACLK timer clock
   * signal, which is where the HH10D's frequency signal should be
   * found, and configure the assigned timer to count that input
   * continuously. */
  BSP430_PORT_HAL_HPL_SEL(hh10d_port) |= APP_HH10D_PORT_BIT;
  BSP430_PORT_HAL_HPL_DIR(hh10d_port) &= ~APP_HH10D_PORT_BIT;
  hh10d.freq_timer->ctl = TASSEL_0 | MC_2 | TACLR;

  /* Hook into the uptime infrastructure, then set the alarm to
   * execute immediately, synthesizing an interrupt request for the
   * current time. */
  BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRIndexedChainNode,
                                  hBSP430uptimeTimer()->cc_cbchain_ni[APP_HH10D_UPTIME_CC_INDEX],
                                  hh10d.cb,
                                  next_ni);
  hBSP430uptimeTimer()->hpl->ccr[APP_HH10D_UPTIME_CC_INDEX] = uiBSP430uptimeCounter_ni();
  hBSP430uptimeTimer()->hpl->cctl[APP_HH10D_UPTIME_CC_INDEX] = CCIFG | CCIE;

  /* Configuration done; enable interrupts */
  BSP430_CORE_ENABLE_INTERRUPT();

  while (1) {
    BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
    char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];
    unsigned long now_utt;
    unsigned int lpc;
    int hum_ppth;

    BSP430_CORE_LPM_ENTER(LPM1_bits);
    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      now_utt = ulBSP430uptime_ni();
      lpc = hh10d.last_period_count;
      hum_ppth = iBSP430sensorsHH10Dconvert_ppth_ni(&hh10d, APP_HH10D_INTERVAL_S);
    } while (0);
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
    cprintf("%s: Count %u ; humidity %u ppth\n",
            xBSP430uptimeAsText(now_utt, as_text), lpc, hum_ppth);
  }
}
