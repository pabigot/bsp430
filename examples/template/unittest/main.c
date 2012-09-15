/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/unittest.h>

void main ()
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  vBSP430unittestFinalize();
}
