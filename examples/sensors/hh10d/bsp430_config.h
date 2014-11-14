/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Request help for figuring out where I2C connects */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

#define configBSP430_CONSOLE 1
#define configBSP430_UPTIME 1

/* We're going to need a timer to capture a frequency, which is what
 * BSP430_TIMER_CCACLK does.  We only need the HPL for the timer, but
 * we need the CLK port and its HAL. */
#define configBSP430_TIMER_CCACLK 1
#define configBSP430_TIMER_CCACLK_CLK_PORT 1
#define configBSP430_TIMER_CCACLK_CLK_PORT_HAL 1

/* We need to know the port and pin to attach the HH10D to, which is
 * the clock source for the timer we're going to use. */
#define APP_HH10D_PORT_PERIPH_HANDLE BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
#define APP_HH10D_PORT_BIT BSP430_TIMER_CCACLK_CLK_PORT_BIT
#define APP_HH10D_TIMER_PERIPH_HANDLE BSP430_TIMER_CCACLK_PERIPH_HANDLE

/* And we need a CC block on the uptime counter that we can use to
 * determine the frequency of the HH10D signal.  Don't use CC0; we
 * didn't ask for configBSP430_UPTIME_TIMER_HAL_CC0_ISR. */
#define APP_HH10D_UPTIME_CC_INDEX 1

/* Need I2C to access the calibration constants */
#define configBSP430_SERIAL_ENABLE_I2C 1

#if (BSP430_PLATFORM_EXP430F5438 - 0) || (BSP430_PLATFORM_TRXEB - 0)
#define APP_HH10D_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B3
#define configBSP430_HAL_USCI5_B3 1
#elif (BSP430_PLATFORM_EXP430F5529 - 0)
#define APP_HH10D_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0
#define configBSP430_HAL_USCI5_B0 1
#elif (BSP430_PLATFORM_EXP430F5529LP - 0)
#define APP_HH10D_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI5_B1
#define configBSP430_HAL_USCI5_B1 1
#elif ((BSP430_PLATFORM_EXP430FR5739 - 0)       \
       || (BSP430_PLATFORM_EXP430FR4133 - 0)    \
       || (BSP430_PLATFORM_EXP430FR5969 - 0)    \
       || (BSP430_PLATFORM_WOLVERINE - 0))
#define APP_HH10D_I2C_PERIPH_HANDLE BSP430_PERIPH_EUSCI_B0
#define configBSP430_HAL_EUSCI_B0 1
#else
#define APP_HH10D_I2C_PERIPH_HANDLE BSP430_PERIPH_USCI_B0
#define configBSP430_HAL_USCI_B0 1
#endif
/* Address for the thing. */
#define APP_HH10D_I2C_ADDRESS 0x51

/* Need CLK capability on CCACLK for EXP430F5529 */
#if (BSP430_PLATFORM_EXP430F5529 - 0)
#define configBSP430_PLATFORM_EXP430F5529_CCACLK_NEED_CLK 1
#endif /* BSP430_PLATFORM_EXP430F5529 */

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
