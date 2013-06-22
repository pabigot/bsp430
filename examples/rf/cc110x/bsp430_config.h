/* Use a crystal if one is installed.  Much more accurate timing
 * results.  Except, don't do this on a G2 where the crystal uses the
 * same line as the Anaren CSn, and if we happen to run the
 * application on a board with that populated it'd be used for the
 * serial clock. */
#if (BSP430_PLATFORM_EXP430G2 - 0)
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 0
#else /* BSP430_PLATFORM_EXP430G2 */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1
#endif /* BSP430_PLATFORM_EXP430G2 */

/* 8 MHz system clock, 4 MHz SMCLK */
#define BSP430_CLOCK_NOMINAL_MCLK_HZ (8*1000*1000UL)
#define BSP430_PLATFORM_BOOT_SMCLK_DIVIDING_SHIFT 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Request RFEM interface resources */
#define configBSP430_RFEM 1
#define configBSP430_PLATFORM_RF 1
#define configBSP430_RF_CC110XEMK 1
#define configBSP430_RF_ANAREN_CC110L (BSP430_PLATFORM_EXP430G2 - 0)

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
