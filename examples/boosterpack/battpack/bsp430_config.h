/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output with buffered output and input */
#define configBSP430_CONSOLE 1
#define BSP430_CONSOLE_TX_BUFFER_SIZE 80
#define BSP430_CONSOLE_RX_BUFFER_SIZE 16

/* Monitor uptime with delay support */
#define configBSP430_UPTIME 1
#define configBSP430_UPTIME_DELAY 1

/* Request help for figuring out where I2C connects */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

/* Need I2C */
#define configBSP430_SERIAL_ENABLE_I2C 1

/* Address for the thing. */
#define APP_BQ27510_I2C_ADDRESS 0x55

/* Platform-specific port assignments.  Really ought to have a
 * boosterpack indirection facility as is used for RFEM. */
#if (BSP430_PLATFORM_EXP430F5529LP - 0)
#define APP_BQ27510_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0
#define configBSP430_HAL_USCI5_B0 1
#define configBSP430_HPL_PORT6 1
#define APP_CHGn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT6
#define APP_CHGn_PORT_BIT BIT5
#define configBSP430_HPL_PORT1 1
#define APP_ENn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define APP_ENn_PORT_BIT BIT6
#define configBSP430_HPL_PORT3 1
#define APP_PGn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define APP_PGn_PORT_BIT BIT2
#define configBSP430_HAL_USCI5_B0 1
#endif

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
