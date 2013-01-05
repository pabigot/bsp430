/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic timer, with delay support. */
#define configBSP430_UPTIME 1
#define configBSP430_UPTIME_DELAY 1
#define configBSP430_CORE_LPM_EXIT_CLEAR_GIE 1

/* Use the TLV infrastructure if available */
#define configBSP430_TLV 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
