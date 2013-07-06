/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output, with interrupt-driven output so alarms are
 * not made late by serial output. */
#define configBSP430_CONSOLE 1
#ifndef BSP430_CONSOLE_TX_BUFFER_SIZE
#define BSP430_CONSOLE_TX_BUFFER_SIZE 80
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Use the secondary timer as an SMCLK-based multiplexed alarm.  We'll
 * use CC1 so the default HAL is all that's necessary. */
#define configBSP430_TIMER_CCACLK 1
#define configBSP430_TIMER_CCACLK_HAL 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
