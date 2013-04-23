/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Enable platform LCD where available */
#define configBSP430_UTILITY_U8GLIB 1

/* Request platform help so we can know where to trace SPI for LCD */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
