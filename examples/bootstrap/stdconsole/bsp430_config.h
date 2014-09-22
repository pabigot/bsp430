/* Application does output: support spin-for-jumper */
#ifndef configBSP430_PLATFORM_SPIN_FOR_JUMPER
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

/* Request console resources */
#define configBSP430_CONSOLE 1

/* In verbose mode, the number of characters transmitted and received, and
 * the raw status of the UART, are displayed on each iteration. */
#ifndef APP_VERBOSE
#define APP_VERBOSE 0
#endif /* APP_VERBOSE */

#include <bsp430/platform/bsp430_config.h>
