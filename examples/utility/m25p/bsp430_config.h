/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Request the SPI flash */
#define configBSP430_PLATFORM_M25P 1

/* For external M25P-compatible chips */
#if (BSP430_PLATFORM_EXP430F5529LP - 0)
/* SPI on USCI_A0, CSn on P6.6, PWR and RSTn hard-wired */
#define configBSP430_HAL_USCI5_A0 1
#define configBSP430_SERIAL_ENABLE_SPI 1
#define BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE BSP430_PERIPH_USCI5_A0
#define configBSP430_HPL_PORT6 1
#endif /* BSP430_PLATFORM_EXP430F5529LP */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
