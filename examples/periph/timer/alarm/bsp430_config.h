/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output, and buffer it so we don't unnecessarily
 * delay the alarm interrupts */
#define configBSP430_CONSOLE 1
#define BSP430_CONSOLE_TX_BUFFER_SIZE 80

/* Monitor uptime and provide generic ACLK-driven timer so we can see
 * how long we've been running. */
#define configBSP430_UPTIME 1

/* Enable an 8-character rx buffer for the console */
#define BSP430_CONSOLE_RX_BUFFER_SIZE 8

/* Enable an 80-character command buffer */
#define BSP430_CLI_CONSOLE_BUFFER_SIZE 80

/* Enable command completion */
#define configBSP430_CLI_COMMAND_COMPLETION 1

/* We'll use CCACLK as the basis for alarms.  Also grab the HAL
 * support so we can count overflows, and the CC0 ISR so we can put
 * alarms on it too. */
#define configBSP430_TIMER_CCACLK 1
#define configBSP430_TIMER_CCACLK_HAL 1
#define configBSP430_TIMER_CCACLK_HAL_CC0_ISR 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
