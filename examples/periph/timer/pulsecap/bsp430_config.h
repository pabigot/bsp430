/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Request a button */
#define configBSP430_PLATFORM_BUTTON0 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>

#if (BSP430_PLATFORM_EXP430FR5739 - 0)
#define BUTTON0_TIMER_PERIPH_HANDLE BSP430_PERIPH_TB2
#define BUTTON0_TIMER_CCIDX 0
#define BUTTON0_TIMER_CCIS (1 * CCIS0)
#define BUTTON0_TIMER_SEL 1
#define configBSP430_HAL_TB2 1
#define configBSP430_HAL_TB2_CC0_ISR 1
#endif /* BSP430_PLATFORM_EXP430FR5739 */
