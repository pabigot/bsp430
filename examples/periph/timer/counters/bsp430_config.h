/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Use the secondary timer for high-resolution timing without overflow
 * support. */
#define configBSP430_TIMER_CCACLK 1

/* Enable valid counter read infrastructure */
#ifndef configBSP430_TIMER_VALID_COUNTER_READ
#define configBSP430_TIMER_VALID_COUNTER_READ 1
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
