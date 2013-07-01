/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Need CCACLK support including CCIS.  Need the CC1 port HAL too so
 * we can get to the HPL to set the peripheral function. */
#define configBSP430_TIMER_CCACLK 1
#define configBSP430_TIMER_CCACLK_HAL 1
#define configBSP430_TIMER_CCACLK_CC1_PORT 1
#define configBSP430_TIMER_CCACLK_CC1_PORT_HAL 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
