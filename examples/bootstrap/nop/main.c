/** This file is in the public domain.
 *
 * This program is the smallest BSP430 program, doing nothing more
 * than initializing the platform.  Its role is primarily as a sanity
 * check when bootstrapping a new platform: the first step is to make
 * this build and link.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

/* Include the generic platform header.  This assumes that
 * #BSP430_PLATFORM_EXP430FR5739 (or another marker that identifies a
 * platform BSP430 supports) has been defined for the preprocessor.
 * If not, you may include the platform-specific version,
 * e.g. <bsp430/platform/exp430g2.h>. */
#include <bsp430/platform.h>

void main ()
{
  /* First thing you do in main is configure the platform. */
  vBSP430platformInitialize_ni();
  /* And that's all this one does. */
}
