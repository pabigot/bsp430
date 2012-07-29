/* You can test alternative configurations without modifying this
 * file:

make realclean \
   EXT_CPPFLAGS='-DBSP430_CLOCK_NOMINAL_MCLK_HZ=20000000' \
   install

*/

/* Expose the clocks */
#define configBSP430_PERIPH_EXPOSED_CLOCKS 1

/* Do not disable the FLL. */
#define configBSP430_CLOCK_DISABLE_FLL 0

#ifndef BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT
/* Ensure MCLK and SMCLK can be distinguished */
#define BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT 1
#endif /* BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT */

/* Request a differential timer for SMCLK/ACLK if available */
#define configBSP430_PLATFORM_TIMER_CCACLK 1

/* Disable uptime so we don't conflict with CCACLK if that uses TA0 */
#define configBSP430_UPTIME 0

#include <bsp430/platform/bsp430_config.h>
