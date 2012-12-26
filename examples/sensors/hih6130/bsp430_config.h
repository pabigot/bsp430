/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Then pick the best available source for ACLK, allowing for an
 * absent crystal */
#define BSP430_PLATFORM_BOOT_ACLKSRC eBSP430clockSRC_XT1CLK_FALLBACK

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Request help for figuring out where I2C connects */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

/* Need a console for output */
#define configBSP430_CONSOLE 1

/* Need the uptime infrastructure */
#define configBSP430_UPTIME 1

/* Need I2C */
#define configBSP430_SERIAL_ENABLE_I2C 1

#if (BSP430_PLATFORM_EXP430F5438 - 0) || (BSP430_PLATFORM_TRXEB - 0)
#define APP_HIH6130_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B3
#define configBSP430_HAL_USCI5_B3 1
#elif (BSP430_PLATFORM_EXP430F5529 - 0)
#define APP_HIH6130_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0
#define configBSP430_HAL_USCI5_B0 1
#elif (BSP430_PLATFORM_EXP430FR5739 - 0)
#define APP_HIH6130_I2C_PERIPH_HANDLE BSP430_PERIPH_EUSCI_B0
#define configBSP430_HAL_EUSCI_B0 1
#else
#define APP_HIH6130_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI_B0
#define configBSP430_HAL_USCI_B0 1
#endif
/* HIH6130 wants I2C bus between 100kHz and 400 kHz.  Use SMCLK/30
 * (=267 kHz). */
#define APP_HIH6130_I2C_PRESCALER 30
/* Address for the thing. */
#define APP_HIH6130_I2C_ADDRESS 0x27

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
