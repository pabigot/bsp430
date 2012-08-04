/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#ifndef configBSP430_PLATFORM_SPIN_FOR_JUMPER
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

#define configBSP430_CONSOLE 1
#define configBSP430_UPTIME 1

/* We're going to need a timer to capture a frequency, which is what
 * BSP430_TIMER_CCACLK does. */
#define configBSP430_TIMER_CCACLK 1

/* We need to know the port and pin for the clock source for
 */
#if __MSP430FR5739__ - 0
/* Pick P1.1 == TA1CLK */
#define APP_HH10D_PORT_HPL BSP430_HPL_PORT1
#define configBSP430_HPL_PORT1 1
#define APP_HH10D_PORT_PIN BIT1
#define APP_HH10D_TIMER_HPL BSP430_HPL_TA1
#define configBSP430_HPL_TA1 1
#else
#define APP_HH10D_PORT_HPL xBSP430hplLookupPORT(BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE)
#define APP_HH10D_PORT_PIN BSP430_TIMER_CCACLK_CLK_PORT_PIN
#define APP_HH10D_TIMER_HPL xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE)
#endif /* msp430fr5739 */
#define APP_HH10D_UPTIME_CC_INDEX 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
