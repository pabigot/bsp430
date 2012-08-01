/* You can test alternative configurations without modifying this
 * file:

make realclean \
   EXT_CPPFLAGS='-DBSP430_CLOCK_NOMINAL_MCLK_HZ=20000000' \
   install

*/

/* All configuration options in this file may be overridden at compile
 * time by providing an alternative definition on the compiler command
 * line */

/* Application does output: support spin-for-jumper */
#ifndef configBSP430_PLATFORM_SPIN_FOR_JUMPER
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 1
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

/* Request console resources */
#ifndef configBSP430_CONSOLE
#define configBSP430_CONSOLE 1
#endif /* configBSP430_CONSOLE */

/* Expose the clocks */
#ifndef configBSP430_PERIPH_EXPOSED_CLOCKS
#define configBSP430_PERIPH_EXPOSED_CLOCKS 1
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */

/* Do not disable the FLL. */
#ifndef configBSP430_CLOCK_DISABLE_FLL
#define configBSP430_CLOCK_DISABLE_FLL 0
#endif /* configBSP430_CLOCK_DISABLE_FLL */

/* By default, ensure MCLK and SMCLK can be distinguished at the test
 * points. */
#ifndef BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT
#define BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT 1
#endif /* BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT */

/* Request a differential timer for SMCLK/ACLK if available */
#ifndef configBSP430_TIMER_CCACLK
#define configBSP430_TIMER_CCACLK 1
#endif /* configBSP430_TIMER_CCACLK */

/* We really want the ACLK source to fall back to VLOCLK if XT1CLK is
 * not stable.  Otherwise those CCACLK timings will hang. */
#ifndef BSP430_PLATFORM_BOOT_ACLKSRC
#if defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__)
#define BSP430_PLATFORM_BOOT_ACLKSRC eBSP430clockSRC_XT1CLK_OR_REFOCLK
#else /* UCS */
#define BSP430_PLATFORM_BOOT_ACLKSRC eBSP430clockSRC_XT1CLK_OR_VLOCLK
#endif /* UCS */
#endif /* BSP430_PLATFORM_BOOT_ACLKSRC */

#include <bsp430/platform/bsp430_config.h>

