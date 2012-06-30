/* Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the software nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @file
 *
 * Support for system reset and related diagnostics.
 *
 * The behavior of this module is modified with the following
 * configuration options:
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_5XX_SYSRST_H
#define BSP430_5XX_SYSRST_H

#include "platform.h"

/** @def configBSP430_5XX_SYSRST_USE_DESCRIPTION
 *
 * Define to a true value to enable text descriptions of reset causes.
 * If left undefined, the text descriptions are dropped reducing
 * application size by several hundred bytes.
 */
#ifndef configBSP430_5XX_SYSRST_USE_DESCRIPTION
#define configBSP430_5XX_SYSRST_USE_DESCRIPTION 0
#endif /* configBSP430_5XX_SYSRST_USE_DESCRIPTION */

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
 * description of the reset cause that is being returned.  If
 * requested but #configBSP430_5XX_SYSRST_USE_DESCRIPTION is not
 * enabled a single character string "?" will be returned.
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
