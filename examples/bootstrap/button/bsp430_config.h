/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* By default run with interrupts disabled whenever CPU is in active mode */
#ifndef configBSP430_CORE_LPM_EXIT_CLEAR_GIE
#define configBSP430_CORE_LPM_EXIT_CLEAR_GIE 1
#endif /* configBSP430_CORE_LPM_EXIT_CLEAR_GIE */

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Request a button */
#define configBSP430_PLATFORM_BUTTON0 1

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
