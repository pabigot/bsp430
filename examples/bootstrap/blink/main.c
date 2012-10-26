/** This file is in the public domain.
 *
 * This program initializes the platform then spins toggling each LED
 * in turn.
 *
 * Its role is primarily as a sanity check when bootstrapping a new
 * platform: the second step is to make LEDs blink.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>

/* We're going to use LEDs so we need the interface file */
#include <bsp430/utility/led.h>

/* We want to know the nominal clock speed so we can delay. */
#include <bsp430/clock.h>

void main ()
{
  int led = 0;

  /* First thing you do in main is configure the platform.
   * Note that this configures the LEDs. */
  vBSP430platformInitialize_ni();

  while (1) {
    /* A common complaint is that "blink doesn't work!" when the issue
     * is that the watchdog is resetting the board.  Don't let that
     * happen to you. */
    BSP430_CORE_WATCHDOG_CLEAR();
    if (1 < nBSP430led) {
      /* If there are multiple LEDs, turn each LED on in turn, wait a
       * half second, then turn it off and move to the next LED.  If
       * there's only one LED, the invert state handles the on as well
       * as the off. */
      vBSP430ledSet(led, 1);
    }
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
    /* -1 means "invert state", which in this case will turn it off */
    vBSP430ledSet(led, -1);
    if (++led == nBSP430led) {
      led = 0;
    }
  }

}
