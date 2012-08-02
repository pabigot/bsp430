/** This file is in the public domain.
 *
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

#include <bsp430/platform.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/onewire.h>

const struct xBSP430onewireBus ds18b20 = {
        .port = APP_DS18B20_PORT,
        .bit = APP_DS18B20_BIT
};

void main ()
{
  vBSP430platformInitialize_ni();
  (void)xBSP430consoleInitialize();
  cprintf("Here we go\n");
}
