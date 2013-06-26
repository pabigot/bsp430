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
#include <bsp430/periph/timer.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/led.h>

#if ! (BSP430_PLATFORM_BUTTON0 - 0)
#error No button available on this platform
#endif /* BSP430_PLATFORM_BUTTON0 */

unsigned long pulseWidth_tt;
unsigned int overflows;
unsigned int activehighs;


static int
pulsecap_callback_ni (hBSP430timerPulseCapture pulsecap)
{
  static const unsigned int both = BSP430_TIMER_PULSECAP_START_VALID | BSP430_TIMER_PULSECAP_END_VALID;
  static const unsigned int active_high = BSP430_TIMER_PULSECAP_START_VALID | BSP430_TIMER_PULSECAP_ACTIVE_HIGH;
  int do_clear = 0;
  int rv = 0;

  cprintf("pc flags %04x start %lu end %lu\n", pulsecap->flags, pulsecap->start_tt, pulsecap->end_tt);
  if (both == (both & pulsecap->flags)) {
    pulseWidth_tt = pulsecap->end_tt - pulsecap->start_tt;
    do_clear = 1;
  }
  if (active_high == (active_high & pulsecap->flags)) {
    ++activehighs;
    do_clear = 1;
  }
  if (BSP430_TIMER_PULSECAP_OVERFLOW & pulsecap->flags) {
    rv = BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
    ++overflows;
    do_clear = 1;
  }
  if (do_clear) {
    vBSP430timerPulseCaptureClear_ni(pulsecap);
  }
  return rv;
}

void main ()
{
  volatile sBSP430hplPORTIE * b0hpl;
  hBSP430halTIMER b0timer_hal;
  int b0pin = iBSP430portBitPosition(BSP430_PLATFORM_BUTTON0_PORT_BIT);
  struct sBSP430timerPulseCapture pulsecap_state;
  hBSP430timerPulseCapture pulsecap;
  unsigned long freq_Hz;
  int rc;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\npulsecap " __DATE__ " " __TIME__ "\n");

  cprintf("There's supposed to be a button at %s.%u\n",
          xBSP430portName(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE),
          b0pin);
  b0hpl = xBSP430hplLookupPORTIE(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE);
  if (NULL == b0hpl) {
    cprintf("Guess it's not there.  Never mind.\n");
    return;
  }
  cprintf("Found it at %p.  Now looking for timer HAL.\n", b0hpl);
  b0timer_hal = hBSP430timerLookup(BUTTON0_TIMER_PERIPH_HANDLE);
  if (NULL == b0timer_hal) {
    cprintf("That's not there, though.  Going away now.\n");
    return;
  }
  vBSP430timerResetCounter_ni(b0timer_hal);
  b0timer_hal->hpl->ctl = TASSEL__SMCLK | MC__CONTINOUS | TBCLR | TBIE;
  vBSP430timerInferHints_ni(b0timer_hal);
  freq_Hz = ulBSP430timerFrequency_Hz_ni(BUTTON0_TIMER_PERIPH_HANDLE);

  cprintf("Cool, we're good to go with a %lu Hz timer at %p, now %lu.\n",
          freq_Hz, b0timer_hal, ulBSP430timerCounter_ni(b0timer_hal, NULL));

  /* Configure the port for its peripheral function */
  b0hpl->dir &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
#if (BSP430_PORT_SUPPORTS_REN - 0)
  b0hpl->ren |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
  b0hpl->ren |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
#endif /* BSP430_PORT_SUPPORTS_REN */
  b0hpl->sel0 |= BSP430_PLATFORM_BUTTON0_PORT_BIT;

  pulsecap = iBSP430timerPulseCaptureInitialize(&pulsecap_state,
                                                BUTTON0_TIMER_PERIPH_HANDLE,
                                                BUTTON0_TIMER_CCIDX,
                                                BUTTON0_TIMER_CCIS,
                                                BSP430_TIMER_PULSECAP_START_CALLBACK | BSP430_TIMER_PULSECAP_END_CALLBACK,
                                                pulsecap_callback_ni);
  if (NULL == pulsecap) {
    cprintf("Sorry, pulsecap initialization failed\n");
    return;
  }
  cprintf("%s.%u%c cctl %04x; pulsecap %p flags %04x\n",
          xBSP430timerName(BUTTON0_TIMER_PERIPH_HANDLE),
          BUTTON0_TIMER_CCIDX,
          'A' + (BUTTON0_TIMER_CCIS / CCIS0),
          b0timer_hal->hpl->cctl[BUTTON0_TIMER_CCIDX],
          pulsecap, pulsecap->flags);

  /* Need to enable interrupts so timer overflow events are properly
   * acknowledged */
  BSP430_CORE_ENABLE_INTERRUPT();

  rc = iBSP430timerPulseCaptureEnable(pulsecap);
  cprintf("Enable got %d\n", rc);
  while (1) {
    cprintf("Timer %lu flags %04x ; %u over %u activehigh\n",
            ulBSP430timerCounter(b0timer_hal, NULL),
            pulsecap->flags,
            overflows, activehighs);
    if (0 != pulseWidth_tt) {
      cprintf("\tNew width: %lu ticks = %lu us\n", pulseWidth_tt,
              (1000ul * pulseWidth_tt) / (freq_Hz / 1000UL));
      pulseWidth_tt = 0;
    }
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
