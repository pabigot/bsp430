/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Monitor uptime and provide generic ACLK-driven timer */
#define configBSP430_UPTIME 1

/* Need a port that is an capture input for a CCR on the uptime
 * timer. */
#if (BSP430_PLATFORM_EXP430F5529LP - 0)
/* Use P1.5 = TA0.4A */
#define HRM_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define configBSP430_HAL_PORT1 1
#else
#error No HRM configuration specified */
#endif


/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
