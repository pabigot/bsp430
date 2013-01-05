/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* For this application, ensure infrastructure turns off #GIE on wakeup */
#define configBSP430_CORE_LPM_EXIT_CLEAR_GIE 1

/* Support buffered console output and input */
#define configBSP430_CONSOLE 1
#define BSP430_CONSOLE_TX_BUFFER_SIZE 80
#define BSP430_CONSOLE_RX_BUFFER_SIZE 16

/* Enable the uptime infrastructure, including its delay capabilities on CCIDX 1. */
#define configBSP430_UPTIME 1
#define configBSP430_UPTIME_DELAY 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
