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

void main ()
{
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  /* Need to enable interrupts so timer overflow events are properly
   * acknowledged */
  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    cprintf("Up %lu ACLK ticks\n", ulBSP430uptime());
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
