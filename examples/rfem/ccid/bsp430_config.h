/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* 8 MHz system clock, 4 MHz SMCLK */
#define BSP430_CLOCK_NOMINAL_MCLK_HZ (8*1000*1000UL)
#define BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* We're going to use USCI_B0 to access the radio */
#define configBSP430_HAL_USCI_B0 1

/* We're going to use P3.0 for chip select, and P3.2 for GDO1. */
#define configBSP430_HPL_PORT3 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
