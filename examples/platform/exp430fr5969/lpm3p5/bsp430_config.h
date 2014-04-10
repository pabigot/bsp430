/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output, at 115200 to save energy */
#define configBSP430_CONSOLE 1
#define BSP430_CONSOLE_BAUD_RATE 115200

/* Enable uptime clock */
#define configBSP430_UPTIME 1

/* Need a button; heck, give me two */
#define configBSP430_PLATFORM_BUTTON0 1
#define configBSP430_PLATFORM_BUTTON1 1

#ifndef APP_CONFIGURE_PORTS_FOR_LPM
#define APP_CONFIGURE_PORTS_FOR_LPM 1
#endif /* APP_CONFIGURE_PORTS_FOR_LPM */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
