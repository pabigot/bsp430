/* Use a crystal if one is installed.  Much more accurate timing
 * results. */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1

/* Support console output */
#define configBSP430_CONSOLE 1

/* Unless told otherwise, keep FLL enabled when active.  On UCS this
 * would default to disabling the FLL to avoid UCS10 and other errata
 * that cause clock anomalies. */
#ifndef configBSP430_CORE_DISABLE_FLL
#define configBSP430_CORE_DISABLE_FLL 0
#endif /* configBSP430_CORE_DISABLE_FLL */

/* Include HPL support for TA0, just lets us use the peripheral
 * handle. */
#define configBSP430_HPL_TA0 1

/* Explicitly exclude HAL support for TA0 which we will be using.
 * CCACLK_TIMER is still used internally for MCLK calibration. */
#define configBSP430_HAL_TA0 0

/* Get platform defaults */
#include <bsp430/platform/bsp430_config.h>
