#ifndef BSP430_5XX_SYSRST_H
#define BSP430_5XX_SYSRST_H

/** @file
 *
 * Support for system reset and related diagnostics.
 *
 * The behavior of this module is modified with the following
 * configuration options:
 *
 * bsp430CONFIG_5XX_SYSRST_WITH_DESCRIPTION : Define to a true value
 * to enable text descriptions of reset causes.  If left undefined,
 * the text descriptions are dropped reducing application size by
 * several hundred bytes.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @copyright @link http://www.opensource.org/licenses/BSD-3-Clause BSD 3-Clause @endlink
 */

#include "platform.h"

/** Generate the events recorded within the system reset vector.
 *
 * This routine can be used to determine the cause of a reset.  It can
 * also detect whether the reset involves a brownout reset (BOR),
 * which restores everything to its power-on condition.  Recall that
 * an MSP430 power on reset (POR) does not in fact return all values
 * to power on defaults, nor does a power up clear (PUC).
 *
 * @param pucHaveBOR Optional pointer to a flag that is set to a
 * nonzero value if the reset cause that will be returned induced a
 * BOR.  The flag is left unmodified if the reset cause only induced a
 * POR or PUC.
 *
 * @param ppcDescription Optional parameter that returns a text
 * description of the reset cause that is being returned.
 *
 * @return A positive integer value corresponding to a SYSRST_type
 * supported on the current microcontroller, or zero if all such
 * values have been returned. */
portBASE_TYPE xBSP430sysrstGenerator (unsigned char * pucHaveBOR,
										const char ** ppcDescription);

/** Cause a brown-out reset */
#define vBSP430sysrstInduceBOR() do {			\
		PMMCTL0 = PMMPW | PMMSWBOR;				\
	} while (0)

/** Cause a power-on reset */
#define vBSP430sysrstInducePOR() do {			\
		PMMCTL0 = PMMPW | PMMSWPOR;				\
	} while (0)

#endif /* BSP430_5XX_SYSRST_H */
