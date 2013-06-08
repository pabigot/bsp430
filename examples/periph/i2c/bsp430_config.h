/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Request help for figuring out where I2C connects */
#define configBSP430_PLATFORM_PERIPHERAL_HELP 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Need two distinct I2C busses */
#define configBSP430_SERIAL_ENABLE_I2C 1
#define configBSP430_HAL_USCI5_B2 1
#define configBSP430_HAL_USCI5_B3 1
#define APP_I2C_BUSA BSP430_PERIPH_USCI5_B3
#define APP_I2C_BUSB BSP430_PERIPH_USCI5_B2
#define BSP430_SERIAL_I2C_BUS_SPEED_HZ 100000

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
