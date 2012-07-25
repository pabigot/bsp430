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
 * @brief Hardware presentation/abstraction for FLL Plus (FLLPLUS).
 *
 * This module supports the FLLPLUS and FLLPLUS_SMALL peripherals,
 * which are present in 4xx-family devices.
 *
 * FLLPLUS_SMALL differs from FLLPLUS in lack of support for XT2 and
 * for SELM/SELS; this difference is irrelevant to the implementation
 * supported here.
 *
 * @note This implementation currently does not support configuring
 * ACLK.  ACLK is always assumed to follow LFXT1 running at 32 kiHz.
 * (The issue is not all chips with FLLPLUS support selecting the low
 * frequency clock.)
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_FLLPLUS_H
#define BSP430_PERIPH_FLLPLUS_H

#include <bsp430/clock.h>
#include <bsp430/periph.h>

#if ! defined(__MSP430_HAS_FLLPLUS__)
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_FLLPLUS__ */

#undef BSP430_CLOCK_LFXT1_IS_FAULTED
/** Check whether the LFXT1 crystal has a fault condition.
 *
 * This definition overrides the generic definition to test the
 * crystal-specific flags. */
#define BSP430_CLOCK_LFXT1_IS_FAULTED() (FLL_CTL0 & LFOF)

#undef BSP430_CLOCK_LFXT1_CLEAR_FAULT
/** Clear the fault associated with LFXT1.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag. */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do {				\
		/* User's guide says FLL_CTL0.LFOF is read-only. */ \
		IFG1 &= ~OFIFG;										\
	} while (0)

/** @def BSP430_FLLPLUS_XCAPxPF
 *
 * Oscillator capacitor section to be applied to FLL_CTL0 by
 * #iBSP430clockConfigureXT1_ni.
 *
 * @note Most examples use XCAP14PF, but my crude tests suggest that
 * of the capacitances available 0pF produces the closest to 32768 Hz,
 * at least on the EXP430FG4618 board.  This also happens to be the
 * power-up default.
 */
#ifndef BSP430_FLLPLUS_XCAPxPF
#define BSP430_FLLPLUS_XCAPxPF XCAP0PF
#endif /* BSP430_FLLPLUS_XCAPxPF */

/** Structure used to configure the frequency-locked loop clock module.
 *
 * The structure fields are assigned in the listed order.  See the 4xx
 * Family User's Guide for details, but in short the DCO frequency
 * will be (N+1)*f_crystal, optionally scaled by a feedback
 * pre-divider (multiplying the output frequency).
 */
typedef struct xBSP430fllplusConfig {
	/** Value for FLL_CTL0.  This configures the pre-divider DCOPLUS,
	 * the LFTX1 mode XTS_FLL, and the oscilator capacitor.  Other
	 * bits denote oscillator faults and are read-only. */
	unsigned char ucFLL_CTL0;

	/** Value for FLL_CTL1.  This selects among the crystal sources
	 * and in some cases changes the source clock for MCLK and SMCLK.
	 * The power-up value of XT2OFF is generally appropriate. */
	unsigned char ucFLL_CTL1;

	/** Value for SCFI0.  This sets the FLL+ loop divider FLLDx, and
	 * the DCO range control FN_x.  The MODx bits should be left as
	 * zero in most uses, as the FLL will update them
	 * automatically. */
	unsigned char ucSCFI0;

	/** Value for SCFQCTL.  This enables/disables modulation through
	 * SCFQ_M, and sets the DCO multiplier N.  The value of the low 7
	 * bits must not be zero. */
	unsigned char ucSCFQCTL;

} xBSP430fllplusConfig;

/** Call this to reconfigure the FLL+ peripheral.
 *
 * The relevant FLL+ registers are configured as requested.
 *
 * @note On 5xx-family MCUs it will be necessary to configure the
 * XIN/XOUT pins to their peripheral function.  Whether that is
 * necessary for any 4xx family device is not known.
 *
 * @return pdTrue if the oscillator has stabilized within the
 * default wait time, and pdFalse if not. */
unsigned char ucBSP430fllplusConfigure_ni (const xBSP430fllplusConfig * pxConfig);

#endif /* BSP430_PERIPH_FLLPLUS_H */
