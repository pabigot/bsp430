/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Explicitly require SPI via serial abstraction */
#define configBSP430_SERIAL_ENABLE_SPI 1

/* SD card is on USCI B1, which by default is port-mapped to P4 */
#define configBSP430_HAL_USCI5_B1 1
#define APP_SD_SPI_PERIPH_HANDLE BSP430_PERIPH_USCI5_B1

/* For some SD cards/holders, MISO may need to be pulled up.  Normally
 * SPI doesn't require this so the pin configuration doesn't do it.
 * Unfortunately we don't provide a way to introspect into the
 * specific pins and ports used by the peripheral handle, so we need
 * to hard-code this, and on the exp430f5529 MISO is on P4.2 */
#define configBSP430_HPL_PORT4 1
#define APP_SD_MISO_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4
#define APP_SD_MISO_PORT_BIT BIT2

/* The chip select for the SD card is on P3.7 */
#define configBSP430_HPL_PORT3 1
#define APP_SD_CS_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define APP_SD_CS_PORT_BIT BIT7

/* MMC SD requires that the dummy byte that cues a read be 0xFF */
#define BSP430_SERIAL_SPI_READ_TX_BYTE(i_) 0xFF

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
