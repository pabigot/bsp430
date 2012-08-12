/* Although a crystal supports more accurate timing, it does increase
 * current.  Disable it unless otherwise requested */
#ifndef BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 0
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 */

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Request a button */
#define configBSP430_PLATFORM_BUTTON0 1

/* And LEDs, configured at boot */
#define configBSP430_LED 1
#define BSP430_PLATFORM_BOOT_CONFIGURE_LEDS 1

#ifndef APP_CONFIGURE_PORTS_FOR_LPM
#define APP_CONFIGURE_PORTS_FOR_LPM 1
#endif /* APP_CONFIGURE_PORTS_FOR_LPM */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
