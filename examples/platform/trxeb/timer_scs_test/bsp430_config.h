/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Support uptime, with delays */
#define configBSP430_UPTIME 1
#define configBSP430_UPTIME_DELAY 1

#ifndef APP_CCIDX
#define APP_CCIDX 1
#endif

/* Allow application to tell user where things are connected */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

/* Use the secondary timer, with INCLK and CC capability.  Take the
 * HAL for the ports to simplify configuration. */
#define configBSP430_TIMER_CCACLK 1
#define configBSP430_TIMER_CCACLK_CLK_PORT 1
#define configBSP430_TIMER_CCACLK_CLK_PORT_HAL 1
#if 0 == APP_CCIDX
#define configBSP430_TIMER_CCACLK_CC0_PORT 1
#define configBSP430_TIMER_CCACLK_CC0_PORT_HAL 1
#define APP_CC_PORT_PERIPH_HANDLE BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE
#define APP_CC_PORT_BIT BSP430_TIMER_CCACLK_CC0_PORT_BIT
#define APP_CC_CCIS BSP430_TIMER_CCACLK_CC0_CCIS
#elif 1 == APP_CCIDX
#define configBSP430_TIMER_CCACLK_CC1_PORT 1
#define configBSP430_TIMER_CCACLK_CC1_PORT_HAL 1
#define APP_CC_PORT_PERIPH_HANDLE BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE
#define APP_CC_PORT_BIT BSP430_TIMER_CCACLK_CC1_PORT_BIT
#define APP_CC_CCIS BSP430_TIMER_CCACLK_CC1_CCIS
#endif

/* Support the unit-test framework */
#define configBSP430_UNITTEST 1

/* Need a GPIO for output clock.  Use the HAL, the generic interface
 * is simpler. */
#define APP_OUTCLK_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT1
#define APP_OUTCLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_OUTCLK_PORT_BIT BIT6

/* Need a GPIO for the capture trigger. */
#define APP_TRIGGER_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT1
#define APP_TRIGGER_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_TRIGGER_PORT_BIT BIT5

#define BSP430_WANT_PERIPH_CPPID APP_OUTCLK_PORT_PERIPH_CPPID
#define BSP430_WANT_CONFIG_HAL 1
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL
#undef BSP430_WANT_PERIPH_CPPID

#define BSP430_WANT_PERIPH_CPPID APP_TRIGGER_PORT_PERIPH_CPPID
#define BSP430_WANT_CONFIG_HAL 1
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL
#undef BSP430_WANT_PERIPH_CPPID

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
