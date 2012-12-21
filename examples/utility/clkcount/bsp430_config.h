/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Then pick the best available source for ACLK, allowing for an
 * absent crystal */
#define BSP430_PLATFORM_BOOT_ACLKSRC eBSP430clockSRC_XT1CLK_FALLBACK

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output.  Use an output buffer so we can transmit
 * while in LPM with interrupts enabled. */
#define configBSP430_CONSOLE 1
#define BSP430_CONSOLE_TX_BUFFER_SIZE 64

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* We're going to need a timer we can configure to the clock we want
 * to measure. */
#define configBSP430_TIMER_CCACLK 1

/* Also grab the HAL support so we can count overflows.  We'll use the
 * CC0 ISR, and we'll need the port for that. */
#define configBSP430_TIMER_CCACLK 1
#define configBSP430_TIMER_CCACLK_HAL 1
#define configBSP430_TIMER_CCACLK_HAL_ISR 1
#define configBSP430_TIMER_CCACLK_HAL_CC0_ISR 1
#define configBSP430_TIMER_CCACLK_CC0_PORT 1
#define configBSP430_TIMER_CCACLK_CC0_PORT_HAL 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
