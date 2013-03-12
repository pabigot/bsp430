/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/eui64.h>
#include <string.h>

void main ()
{
  int rv;
  uBSP430eui64 eui64;
  int i;
  
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cputs("\n\n\nEUI-64 Verification");
#if ! (BSP430_EUI64 - 0)
  cputs("EUI-64 not supported by platform\n");
#else /* BSP430_EUI64 */
  while (1) {
    memset(&eui64, 0, sizeof(eui64));
    rv = iBSP430eui64(&eui64);
    cprintf("Request got %d: ", rv);
    for (i = 0; i < sizeof(eui64.bytes); ++i) {
      cprintf("%02x", eui64.bytes[i]);
    }
    cputchar('\n');
  }
#endif /* BSP430_EUI64 */
}
