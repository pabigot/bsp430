/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Then pick the best available source for ACLK, allowing for an
 * absent crystal */
#define BSP430_PLATFORM_BOOT_ACLKSRC eBSP430clockSRC_XT1CLK_FALLBACK

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

#define configBSP430_CONSOLE 1
#define configBSP430_UPTIME 1

/* We're going to need a timer to capture a frequency, which is what
 * BSP430_TIMER_CCACLK does.  We only need the HPL for the timer, but
 * we need the HAL for the port. */
#define configBSP430_TIMER_CCACLK 1
#define configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL 1

/* We need to know the port and pin to attach the HH10D to, which is
 * the clock source for the timer we're going to use. */
#define APP_HH10D_PORT_PERIPH_HANDLE BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
#define APP_HH10D_PORT_PIN BSP430_TIMER_CCACLK_CLK_PORT_PIN
#define APP_HH10D_TIMER_PERIPH_HANDLE BSP430_TIMER_CCACLK_PERIPH_HANDLE

/* And we need a CC block on the uptime counter that we can use to
 * determine the frequency of the HH10D signal. */
#define APP_HH10D_UPTIME_CC_INDEX 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
