/** This file is in the public domain.
 *
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
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
  while (1) {
    cprintf("Up %lu ACLK ticks\n", ulBSP430uptime_ni());
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
