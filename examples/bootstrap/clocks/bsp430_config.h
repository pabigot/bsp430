/* You can test alternative configurations without modifying this
 * file:

make realclean \
   EXT_CPPFLAGS='-DBSP430_CLOCK_NOMINAL_MCLK_HZ=20000000' \
   install

*/

/* Application does output: support spin-for-jumper */
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1
/* Request console resources */
#define configBSP430_CONSOLE 1

/* Expose the clocks */
#define configBSP430_PERIPH_EXPOSED_CLOCKS 1

/* Do not disable the FLL. */
#define configBSP430_CLOCK_DISABLE_FLL 0

#ifndef BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT
/* By default, ensure MCLK and SMCLK can be distinguished at the test
 * points. */
#define BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT 1
#endif /* BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT */

/* Request a differential timer for SMCLK/ACLK if available */
#ifndef configBSP430_TIMER_CCACLK
#define configBSP430_TIMER_CCACLK 1
#endif /* configBSP430_TIMER_CCACLK */

#include <bsp430/platform/bsp430_config.h>
