/** This file is in the public domain.
 *
 * Performance measurement on timer infrastructure.
 *
 * Use EXT_CPPFLAGS=-DconfigBSP430_TIMER_VALID_COUNTER_READ=0 to test
 * with safe timer deferring to sync reads.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>

void main ()
{
  volatile sBSP430hplTIMER * const lrt = xBSP430hplLookupTIMER(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
  hBSP430halTIMER uthal;
  volatile sBSP430hplTIMER * const hrt = xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  uthal = hBSP430uptimeTimer();

  cprintf("\n\nTimer counter analysis " __DATE__ " " __TIME__ "\n");

  /* Configure a timer off SMCLK to measure the performance of
   * different actions. */
  if (NULL == hrt) {
    cprintf("High-resolution timer not available\n");
    return;
  }
  vBSP430timerSafeCounterInitialize_ni(hrt);
  hrt->ctl = TASSEL_2 | MC_2 | TACLR;

  cprintf("ACLK %lu Hz source %u\n", ulBSP430clockACLK_Hz_ni(), (int)xBSP430clockACLKSource());
  cprintf("SMCLK %lu Hz source %u\n", ulBSP430clockSMCLK_Hz_ni(), (int)xBSP430clockSMCLKSource());
  cprintf("MCLK %lu Hz source %u\n", ulBSP430clockMCLK_Hz_ni(), (int)xBSP430clockMCLKSource());
  cprintf("LRT is %s source %u at %lu Hz, CTL %04x\n",
          xBSP430timerName(xBSP430periphFromHPL(lrt)),
          (int)xBSP430timerClockSource(lrt), 
          ulBSP430timerFrequency_Hz_ni(xBSP430periphFromHPL(lrt)),
          lrt->ctl);
  cprintf("HRT is %s source %u at %lu Hz, CTL %04x\n",
          xBSP430timerName(xBSP430periphFromHPL(hrt)),
          (int)xBSP430timerClockSource(hrt),
          ulBSP430timerFrequency_Hz_ni(xBSP430periphFromHPL(hrt)),
          hrt->ctl);
  cprintf("Uptime clock flags 0x%02x\n", uthal->hal_state.flags);

  /* Need to enable interrupts so timer overflow events are properly
   * acknowledged. */
  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    unsigned int t0;
    unsigned int t1;
    unsigned int hrto;
    unsigned int s;
    unsigned long ut;

    BSP430_CORE_DISABLE_INTERRUPT();
    t0 = uiBSP430timerSyncCounterRead_ni(hrt);
    t1 = uiBSP430timerSyncCounterRead_ni(hrt);
    BSP430_CORE_ENABLE_INTERRUPT();
    hrto = t1 - t0;
    cprintf("\n%u hrt overhead subtracted from each result\n", t1-t0);

    BSP430_CORE_DISABLE_INTERRUPT();
    t0 = uiBSP430timerSyncCounterRead_ni(hrt);
    s = uiBSP430timerSyncCounterRead_ni(lrt);
    t1 = uiBSP430timerSyncCounterRead_ni(hrt);
    BSP430_CORE_ENABLE_INTERRUPT();
    cprintf("%u hrt to sync read value %u\n", t1-t0-hrto, s);

    if (uthal->hal_state.flags & BSP430_TIMER_FLAG_SLOW) {
      BSP430_CORE_DISABLE_INTERRUPT();
      t0 = uiBSP430timerSyncCounterRead_ni(hrt);
      s = uiBSP430timerAsyncCounterRead_ni(lrt);
      t1 = uiBSP430timerSyncCounterRead_ni(hrt);
      BSP430_CORE_ENABLE_INTERRUPT();
      cprintf("%u hrt to async read value %u\n", t1-t0-hrto, s);
    } else {
      cprintf("async read skipped, not slow\n");
    }
    
#if (configBSP430_TIMER_VALID_COUNTER_READ - 0)
    BSP430_CORE_DISABLE_INTERRUPT();
    t0 = uiBSP430timerSyncCounterRead_ni(hrt);
    vBSP430timerLatchedCounterInitialize_ni(lrt, BSP430_TIMER_VALID_COUNTER_READ_CCIDX);
    t1 = uiBSP430timerSyncCounterRead_ni(hrt);
    BSP430_CORE_ENABLE_INTERRUPT();
    cprintf("%u hrt to configure latched counter\n", t1-t0-hrto);

    BSP430_CORE_DISABLE_INTERRUPT();
    t0 = uiBSP430timerSyncCounterRead_ni(hrt);
    s = uiBSP430timerLatchedCounterRead_ni(lrt, BSP430_TIMER_VALID_COUNTER_READ_CCIDX);
    t1 = uiBSP430timerSyncCounterRead_ni(hrt);
    BSP430_CORE_ENABLE_INTERRUPT();
    cprintf("%u hrt to read latched counter %u\n", t1-t0-hrto, s);
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */

    BSP430_CORE_DISABLE_INTERRUPT();
    t0 = uiBSP430timerSyncCounterRead_ni(hrt);
    vBSP430timerSafeCounterInitialize_ni(lrt);
    t1 = uiBSP430timerSyncCounterRead_ni(hrt);
    BSP430_CORE_ENABLE_INTERRUPT();
    cprintf("%u hrt to configure safe counter\n", t1-t0-hrto);

    BSP430_CORE_DISABLE_INTERRUPT();
    t0 = uiBSP430timerSyncCounterRead_ni(hrt);
    s = uiBSP430timerSafeCounterRead_ni(lrt);
    t1 = uiBSP430timerSyncCounterRead_ni(hrt);
    BSP430_CORE_ENABLE_INTERRUPT();
    cprintf("%u hrt to read safe counter %u\n", t1-t0-hrto, s);

    BSP430_CORE_DISABLE_INTERRUPT();
    t0 = uiBSP430timerSyncCounterRead_ni(hrt);
    s = uiBSP430timerBestCounterRead_ni(lrt, uthal->hal_state.flags);
    t1 = uiBSP430timerSyncCounterRead_ni(hrt);
    BSP430_CORE_ENABLE_INTERRUPT();
    cprintf("%u hrt to read best counter %u\n", t1-t0-hrto, s);

    BSP430_CORE_DISABLE_INTERRUPT();
    t0 = uiBSP430timerSyncCounterRead_ni(hrt);
    ut = ulBSP430uptime_ni();
    t1 = uiBSP430timerSyncCounterRead_ni(hrt);
    BSP430_CORE_ENABLE_INTERRUPT();
    cprintf("%u hrt to read uptime %lu\n", t1-t0-hrto, ut);

    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
