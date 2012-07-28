/** This file is in the public domain.
 *
 * This program initializes the platform then spins toggling the first LED.
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
  /* First thing you do in main is configure the platform. */
  vBSP430platformSetup_ni();

  /* Now initialize the LEDs */
  vBSP430ledInitialize_ni();

  /* Turn the first one on */
  vBSP430ledSet(0, 1);

  /* And blink it every nominal half second */
  while (1) {
    __delay_cycles(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
    vBSP430ledSet(0, -1);
  }

}
