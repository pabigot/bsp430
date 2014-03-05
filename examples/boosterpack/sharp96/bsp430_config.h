/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Request help for figuring out where SPI connects */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer, and support
 * delay functionality */
#define configBSP430_UPTIME 1
#define configBSP430_UPTIME_DELAY 1

/* Request a button */
#define configBSP430_PLATFORM_BUTTON0 1

/* Request support for this boosterpack */
#define configBSP430_PLATFORM_BOOSTERPACK_SHARP96 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
