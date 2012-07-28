/** @page ex_bootstrap_nop A program that does nothing
 *
 * This program is the smallest BSP430 program, doing nothing more
 * than initializing the platform.  Its role is primarily as a sanity
 * check when bootstrapping a new platform: the first step is to make
 * this build and link.
 */

#include <bsp430/platform.h>

void main ()
{
  vBSP430platformSetup_ni();
}
