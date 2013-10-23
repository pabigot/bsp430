/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

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
#if (BSP430_PLATFORM_EXP430F5529LP - 0)
#define configBSP430_RFEM_CCEM 1
#else /* native platform */
#define configBSP430_RFEM 1
#endif /* native platform */
#define configBSP430_PLATFORM_RF 1
#define configBSP430_RF_CC1125EMK 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
