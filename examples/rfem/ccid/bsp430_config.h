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

/* We need to know the USCI device that supplies the SPI interface to
 * the radio, as well as the port and pins for the three GDO lines to
 * the radio and the CSn line.
 *
 * Because this application is cross-family and the RF headers are not
 * consistent on whether pins use interrupt-enabled (P1/P2) or
 * non-enabled (P3+) ports for CSn and GDO1, we have to enable the HAL
 * interface for ports associated with those pins.  The GDO0 and GDO2
 * lines are always on interrupt-capable ports, so we would need only
 * ask for the HPL interface on those, except that at some point we'd
 * like to have interrupts from the radio. */
#define configBSP430_SERIAL_ENABLE_SPI 1
#if BSP430_PLATFORM_EXP430G2 - 0
#define configBSP430_HAL_USCI_B0 1
#define configBSP430_HAL_PORT1 1
#define configBSP430_HAL_PORT2 1
#define APP_SPI_PERIPH_HANDLE BSP430_PERIPH_USCI_B0
#define APP_GDO0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define APP_GDO0_PORT_BIT BIT6
#define APP_GDO1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_GDO1_PORT_BIT BIT6
#define APP_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_GDO2_PORT_BIT BIT0
#define APP_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define APP_CSn_PORT_BIT BIT7
#endif /* BSP430_PLATFORM_EXP430G2 */
#if BSP430_PLATFORM_EXP430F5438 - 0
#define APP_SPI_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0
#define configBSP430_HAL_USCI5_B0 1
#define configBSP430_HAL_PORT1 1
#define configBSP430_HAL_PORT3 1
#define APP_GDO0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_GDO0_PORT_BIT BIT7
#define APP_GDO1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define APP_GDO1_PORT_BIT BIT2
#define APP_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_GDO2_PORT_BIT BIT3
#define APP_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define APP_CSn_PORT_BIT BIT0
#endif /* BSP430_PLATFORM_EXP430F5438 */
#if BSP430_PLATFORM_EXP430FR5739 - 0
#define APP_SPI_PERIPH_HANDLE BSP430_PERIPH_EUSCI_B0
#define configBSP430_HAL_EUSCI_B0 1
#define configBSP430_HAL_PORT1 1
#define configBSP430_HAL_PORT2 1
#define configBSP430_HAL_PORT3 1
#define configBSP430_HAL_PORT4 1
#define APP_GDO0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4
#define APP_GDO0_PORT_BIT BIT1
#define APP_GDO1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_GDO1_PORT_BIT BIT7
#define APP_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define APP_GDO2_PORT_BIT BIT3
#define APP_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_CSn_PORT_BIT BIT3
#endif /* BSP430_PLATFORM_EXP430FR5739 */
#if BSP430_PLATFORM_RF2500T - 0
#define APP_SPI_PERIPH_HANDLE BSP430_PERIPH_USCI_B0
#define configBSP430_HAL_USCI_B0 1
#define configBSP430_HAL_PORT2 1
#define configBSP430_HAL_PORT3 1
#define APP_GDO0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define APP_GDO0_PORT_BIT BIT6
#define APP_GDO1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define APP_GDO1_PORT_BIT BIT2
#define APP_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define APP_GDO2_PORT_BIT BIT7
#define APP_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define APP_CSn_PORT_BIT BIT0
#endif /* BSP430_PLATFORM_RF2500T */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
