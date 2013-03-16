/** This file is in the public domain.
 *
 * This program demonstrates whether CPU erratum CPU42 is present on
 * the running chip.  Note that BSP430 is not subject to this bug when
 * #BSP430_CORE_ENABLE_INTERRUPT() is used instead of the raw
 * __enable_interrupt().
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>

#pragma vector=PORT1_VECTOR
__interrupt void
port_isr (void)
{
  (void)P1IV;
  __delay_cycles(10000);
}

void main ()
{
  unsigned int t0;
  unsigned int t1;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cputs("\n\nCPU42 evaluator\n");

  P1IE |= BIT0;
  P1IFG |= BIT0;
  TA0CTL = TASSEL_1 | MC_2 | TACLR;

  /* Synchronize on tick */
  t0 = TA0R;
  while (t0 == TA0R) {
    ;
  }

  t0 = TA0R;
  t1 = TA0R;
  cprintf("DINT: Timer read %u %u delta %u\n", t0, t1, t1-t0);

  /* Synchronize on tick */
  t0 = TA0R;
  while (t0 == TA0R) {
    ;
  }

  /* If CPU42 is present and __enable_interrupt() does not work around
   * it, the first read of TA0R will execute before the interrupt
   * handler that introduces a 10 kCycle delay, resulting in a large
   * difference between the values. */
  __enable_interrupt();
  t0 = TA0R;
  t1 = TA0R;
  cprintf("EINT: Timer read %u %u delta %u\n", t0, t1, t1-t0);

  if (1 < (t1-t0)) {
    vBSP430ledSet(BSP430_LED_RED, 1);
    cputs("CPU42 appears to be present\n");
  } else {
    vBSP430ledSet(BSP430_LED_GREEN, 1);
    cputs("Unable to confirm presence of CPU42\n");
  }
}
