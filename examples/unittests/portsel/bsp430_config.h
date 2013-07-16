/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Support the unit-test framework */
#define configBSP430_UNITTEST 1

#if (BSP430_PLATFORM_EXP430FR5739 - 0)
/* Use EUSCI_A1 on P2.5 */
#define configBSP430_HAL_PORT2 1
#define PORT_HAL BSP430_HAL_PORT2
#define PORT_HPL BSP430_HPL_PORT2
#define PORT_BIT BIT5
#define PORT_SEL0 P2SEL0
#define PORT_SEL1 P2SEL1
#elif (BSP430_PLATFORM_EXP430G2 - 0)
/* Use ADC/CapSens on P1.3 */
#define configBSP430_HAL_PORT1 1
#define PORT_HAL BSP430_HAL_PORT1
#define PORT_HPL BSP430_HPL_PORT1
#define PORT_BIT BIT3
#define PORT_SEL0 P1SEL
#define PORT_SEL1 P1SEL2
#elif (BSP430_PLATFORM_TRXEB - 0) || (BSP430_PLATFORM_EXP430F5438 - 0)
/* Use A0 on P6.0 */
#define configBSP430_HAL_PORT6 1
#define PORT_HAL BSP430_HAL_PORT6
#define PORT_HPL BSP430_HPL_PORT6
#define PORT_BIT BIT0
#define PORT_SEL0 P6SEL
#endif /* PLATFORM */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
