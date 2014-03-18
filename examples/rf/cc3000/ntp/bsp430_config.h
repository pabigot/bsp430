/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1
#define BSP430_CONSOLE_TX_BUFFER_SIZE 80

/* Monitor uptime and provide generic ACLK-driven timer with
 * uptime-based delay and epoch support. */
#define configBSP430_UPTIME 1
#define configBSP430_UPTIME_DELAY 1
#define configBSP430_UPTIME_EPOCH 1

/* Support for CC30000 BoosterPack.  This conflicts with RFEM use. */
#ifndef configBSP430_RF_CC3000BOOST
#define configBSP430_RF_CC3000BOOST 0
#endif /* configBSP430_RF_CC3000BOOST */

#if ! (configBSP430_RF_CC3000BOOST - 0)
/* Request RFEM interface resources specific to the CC3000EM.  NB:
 * CCEM maps SPI_IRQ to P6.5 on EXP430F5529LP; since that port is not
 * interrupt-enabled the CC3000 can't be used on that platform. */
#define configBSP430_RFEM_CCEM 1
#define configBSP430_RFEM 1
#define configBSP430_RF_CC3000EM 1
#endif /* RFEM */

#define BSP430_RF_CC3000SPI_RX_BUFFER_SIZE 1500
#define BSP430_RF_CC3000SPI_TX_BUFFER_SIZE 1500

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>

/* We use HAL for CSn, perhaps because it's a generic interface to the
 * DIR and OUT registers regardless of IE or non-IE register
 * layout. */
#define BSP430_WANT_CONFIG_HAL 1
#define BSP430_WANT_PERIPH_CPPID BSP430_RF_CC3000_CSn_PORT_PERIPH_CPPID
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL
