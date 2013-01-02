/* Copyright 2012-2013, Peter A. Bigot
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
 * The FLLPLUS and FLLPLUS_SMALL peripherals are present in 4xx-family
 * devices.
 *
 * FLLPLUS_SMALL differs from FLLPLUS in lack of support for XT2 and
 * for SELM/SELS.  The difference is not reflected in this
 * implementation.
 *
 * @section h_periph_fllplus_opt Module Configuration Options
 *
 * None supported.
 *
 * @section h_periph_fllplus_hpl Hardware Presentation Layer
 *
 * As there can be only one instance of FLLPLUS on any MCU, there is no
 * structure supporting a FLLPLUS @HPL.  Manipulate the peripheral through its
 * registers directly.
 *
 * @section h_periph_fllplus_hal Hardware Adaptation Layer
 *
 * As there can be only one instance of FLLPLUS on any MCU, there is no
 * structure supporting a FLLPLUS @HAL.
 *
 * The standard set of capabilities in the bsp430/clocks.h header are
 * supported, with the following details:
 *
 * @li The peripheral requires LFXT1 be available and running at a
 * stable #BSP430_CLOCK_NOMINAL_XT1CLK_HZ.  The value for that crystal
 * rate defaults to 32 KiHz.
 *
 * @li This peripheral does not support dividing SMCLK.  Attempts to
 * configure an alternative divisor will result in a zero divisor.

 * @li The implementation does not support configuring ACLK to
 * anything other than #eBSP430clockSRC_XT1CLK or a fallback from that
 * clock.  (The issue is that not all chips with FLLPLUS support
 * selecting the #eBSP430clockSRC_VLOCLK.)
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_FLLPLUS_H
#define BSP430_PERIPH_FLLPLUS_H

#include <bsp430/clock.h>
#include <bsp430/periph.h>

/** @def BSP430_MODULE_FLLPLUS
 *
 * Defined on inclusion of <bsp430/periph/fllplus.h>.  The value
 * evaluates to true if the target MCU supports the FLL Plus module,
 * and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_FLLPLUS (defined(__MSP430_HAS_FLLPLUS__)          \
                               || defined(__MSP430_HAS_FLLPLUS_SMALL__))

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_FLLPLUS - 0)

/** @cond DOXYGEN_INTERNAL */
/* Simplify conditionally-defined macros to avoid reference to
 * non-existent values. */
#if defined(XT2OF)
#define BSP430_FLLPLUS_XT2OF_ XT2OF
#else /* XT2OF */
#define BSP430_FLLPLUS_XT2OF_ 0
#endif /* XT2OF */
/** @endcond */

/** FLL+-specific check for LFXT1 crystal fault condition.
 *
 * This checks exactly for the fault condition. */
#define BSP430_FLLPLUS_LFXT1_IS_FAULTED_NI() (FLL_CTL0 & LFOF)

/** FLL+-specific check for XT2 crystal fault condition.
 *
 * If the platform does not support an XT2 crystal no fault is
 * diagnosed.
 *
 * @note A crystal that has never been enabled will not register as
 * faulted. */
#define BSP430_FLLPLUS_XT2_IS_FAULTED_NI() (FLL_CTL0 & BSP430_FLLPLUS_XT2OF_)

/** FLL+-specific check for DCO fault condition.
 *
 * This checks exactly for the fault condition. */
#define BSP430_FLLPLUS_DCO_IS_FAULTED_NI() (FLL_CTL0 & DCOF)

/** Check whether the FLL+-controlled LFXT1 crystal has a fault condition.
 *
 * @note BSP430 currently does not support MSP430F41x2 devices which
 * allow LXFT1 to be something other than a crystal, so the standard
 * check for disabled LFXT1 does not apply on this peripheral.
 *
 * @defaulted
 * @see #BSP430_FLLPLUS_LFXT1_IS_FAULTED_NI()
 */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_LFXT1_IS_FAULTED_NI)
#define BSP430_CLOCK_LFXT1_IS_FAULTED_NI() BSP430_FLLPLUS_LFXT1_IS_FAULTED_NI()
#endif /* BSP430_CLOCK_LFXT1_IS_FAULTED_NI */

/** Check whether the FLL+-controlled XT2 crystal has a fault condition.
 *
 * @note Oscillator fault flags are not set unless a fault has been
 * detected.  If the crystal has never been enabled, no fault will
 * have been detected.  On power-up, the XT2IN function is not enabled
 * and BCSCTL1.XT2OFF is set, and BSP430 treats BCSCTL1.XT2OFF as an
 * indication that the pins are not configured for crystal use, either
 * because XT2 has not been configured or has been configured and
 * found to be faulted.  Although it is perfectly acceptable to have
 * BCSCTL1.XT2OFF set and the crystal working fine, the complexity of
 * detecting that case is not supported by this implementation.
 *
 * @defaulted
 * @see #BSP430_FLLPLUS_XT2_IS_FAULTED_NI()
 */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_XT2_IS_FAULTED_NI)
#define BSP430_CLOCK_XT2_IS_FAULTED_NI() ((BCSCTL1 & XT2OFF) || BSP430_FLLPLUS_XT2_IS_FAULTED_NI())
#endif /* BSP430_CLOCK_XT2_IS_FAULTED_NI */

/** Clear all clock faults.
 *
 * The FLL+ oscillator fault flags are read-only, so this only clears
 * the system oscillator flag.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_CLEAR_FAULTS_NI)
#define BSP430_CLOCK_CLEAR_FAULTS_NI() do {                             \
    BSP430_CLOCK_OSC_CLEAR_FAULT_NI();                                  \
  } while (0)
#endif /*  BSP430_CLOCK_CLEAR_FAULTS_NI */

/** @def BSP430_FLLPLUS_XCAPxPF
 *
 * @deprecated Use #BSP430_CLOCK_LFXT1_XCAP
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN)
#define BSP430_FLLPLUS_XCAPxPF XCAP0PF
#endif /* BSP430_FLLPLUS_XCAPxPF */

#ifndef BSP430_CLOCK_LFXT1_XCAP
#if defined(BSP430_FLLPLUS_XCAPxPF) && ! defined(BSP430_DOXYGEN)
/* Default capacitance using legacy option */
#define BSP430_CLOCK_LFXT1_XCAP BSP430_FLLPLUS_XCAPxPF
#else  /* BSP430_FLLPLUS_XCAPxPF */
/** Peripheral default setting for platform-specific constant.
 *
 * Note: TI examples tend to use XCAP14PF but on the EXP430FG4618 that
 * results in a clock rate of 32765, or 100 ppm error.  XCAP0F is the
 * power-up default, and measures 32769 or only 30ppm error. */
#define BSP430_CLOCK_LFXT1_XCAP XCAP0PF
#endif /* BSP430_FLLPLUS_XCAPxPF */
#endif /* BSP430_CLOCK_LFXT1_XCAP */

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 12000U

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_PUC_MCLK_HZ 1048576UL

#endif /* BSP430_MODULE_FLLPLUS */

#endif /* BSP430_PERIPH_FLLPLUS_H */
