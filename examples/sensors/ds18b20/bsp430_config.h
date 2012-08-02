/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

#define configBSP430_CONSOLE 1
#define configBSP430_UPTIME 1

/* Specify placement on P1.5  */
#define APP_DS18B20_PORT BSP430_HPL_PORT1
#define APP_DS18B20_BIT BIT5

/* Request the corresponding HPL */
#define configBSP430_HPL_PORT1 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
