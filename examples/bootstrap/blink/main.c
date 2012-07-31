/** This file is in the public domain.
 *
 * This program initializes the platform then spins toggling each LED
 * in turn.
 *
 * Its role is primarily as a sanity check when bootstrapping a new
 * platform: the second step is to make LEDs blink.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

/* Include the generic platform header.  This assumes that
 * #BSP430_PLATFORM_EXP430FR5739 (or another marker that identifies a
 * platform BSP430 supports) has been defined for the preprocessor.
 * If not, you may include the platform-specific version,
 * e.g. <bsp430/platform/exp430g2.h>. */
#include <bsp430/platform.h>

/* We're going to use LEDs.  Enable them. */
#include <bsp430/utility/led.h>

/* We want to know the nominal clock speed so we can delay. */
#include <bsp430/clock.h>

void main ()
{
  int led = 0;

  /* First thing you do in main is configure the platform. */
  vBSP430platformInitialize_ni();

  /* Now initialize the LEDs */
  vBSP430ledInitialize_ni();

  while (1) {
    /* A common complaint is that "blink doesn't work!" when the issue
     * is that the watchdog is resetting the board.  Don't let that
     * happen to you. */
    BSP430_CORE_WATCHDOG_CLEAR();
    /* Turn each LED on in turn wait a half second, then turn it off
     * and move to the next LED */
    vBSP430ledSet(led, 1);
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
    vBSP430ledSet(led, -1);
    if (++led == ucBSP430leds) {
      led = 0;
    }
  }

}
