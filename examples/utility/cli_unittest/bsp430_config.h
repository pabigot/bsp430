/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Support the unit-test framework */
#define configBSP430_UNITTEST 1

/* Provide a console buffer for testing */
#define BSP430_CLI_CONSOLE_BUFFER_SIZE 16

/* Enable completion */
#define configBSP430_CLI_COMMAND_COMPLETION 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
