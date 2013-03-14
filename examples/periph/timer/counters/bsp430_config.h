/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* To validate synchronous fast reads, define APP_UPTIME_USE_SMCLK. */
#if (APP_USE_SMCLK - 0)
#define BSP430_UPTIME_TASSEL TASSEL_2
#ifndef BSP430_UPTIME_DIVIDING_SHIFT
#define BSP430_UPTIME_DIVIDING_SHIFT 3
#endif /* BSP430_UPTIME_DIVIDING_SHIFT */
#endif /* APP_USE_SMCLK */

/* Use the secondary timer for high-resolution timing without overflow
 * support. */
#define configBSP430_TIMER_CCACLK 1

/* Enable valid counter read infrastructure, unless testing its
 * behavior when disabled. */
#ifndef configBSP430_TIMER_VALID_COUNTER_READ
#define configBSP430_TIMER_VALID_COUNTER_READ 1
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
