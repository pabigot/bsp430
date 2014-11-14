/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Request help for figuring out where I2C connects */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

/* Need a console for output */
#define configBSP430_CONSOLE 1

/* Need the uptime infrastructure, with delay support */
#define configBSP430_UPTIME 1
#define configBSP430_UPTIME_DELAY 1

/* Need I2C */
#define configBSP430_SERIAL_ENABLE_I2C 1

#if (BSP430_PLATFORM_EXP430F5438 - 0) || (BSP430_PLATFORM_TRXEB - 0)
#define APP_MCP9808_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B3
#define configBSP430_HAL_USCI5_B3 1
#elif (BSP430_PLATFORM_EXP430F5529 - 0)
#define APP_MCP9808_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0
#define configBSP430_HAL_USCI5_B0 1
#elif (BSP430_PLATFORM_EXP430F5529LP - 0)
#define APP_MCP9808_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B1
#define configBSP430_HAL_USCI5_B1 1
#elif ((BSP430_PLATFORM_EXP430FR5739 - 0)       \
       || (BSP430_PLATFORM_EXP430FR4133 - 0)    \
       || (BSP430_PLATFORM_EXP430FR5969 - 0)    \
       || (BSP430_PLATFORM_WOLVERINE - 0))
#define APP_MCP9808_I2C_PERIPH_HANDLE BSP430_PERIPH_EUSCI_B0
#define configBSP430_HAL_EUSCI_B0 1
#else
#define APP_MCP9808_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI_B0
#define configBSP430_HAL_USCI_B0 1
#endif
/* Address for the thing. */
#define APP_MCP9808_I2C_ADDRESS 0x18

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
