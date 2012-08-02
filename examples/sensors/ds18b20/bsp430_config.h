/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#ifndef configBSP430_PLATFORM_SPIN_FOR_JUMPER
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

#define configBSP430_CONSOLE 1
#define configBSP430_UPTIME 1

/* Specify placement on P1.7  */
#define APP_DS18B20_PORT hBSP430port_PORT1
#define APP_DS18B20_BIT BIT7

/* Request the corresponding HAL */
#define configBSP430_HAL_PORT1 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
