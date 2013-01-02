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
 * @brief Hardware presentation/abstraction for Basic Clock Module+ (BC2).
 *
 * The BC2 ("Basic Clock Module+") peripheral is present in 2xx-family
 * devices.
 *
 * @section h_periph_bc2_opt Module Configuration Options
 *
 * @li #configBSP430_BC2_TRIM_TO_MCLK
 *
 * @section h_periph_bc2_hpl Hardware Presentation Layer
 *
 * As there can be only one instance of BC2 on any MCU, there is no
 * structure supporting a BC2 HPL.  Manipulate the peripheral through its
 * registers directly.
 *
 * @section h_periph_bc2_hal Hardware Adaptation Layer
 *
 * As there can be only one instance of BC2 on any MCU, there is no
 * structure supporting a BC2 HAL.
 *
 * The standard set of capabilities in the bsp430/clocks.h header are
 * supported, with the following details:
 *
 * @li For ulBSP430clockConfigureMCLK_ni() the only recognized
 * frequencies are 1 MHz, 8 MHz, 12 MHz, and 16 MHz.  Availability of
# * a given frequency depends on the specific MCU having
 * factory-calibrated constants supporting that frequency.  The
 * selected frequency may be above or below the requested frequency,
 * but will be the closest supported by available calibrated clocks.
 *
 * @li Dependent on #configBSP430_BC2_TRIM_TO_MCLK the requested
 * frequency may be reached through calibration process executed
 * during ulBSP430clockConfigureMCLK_ni().
 *
 * @li The implementation assumes that MCLK and SMCLK are both
 * sourced from DCOCLK.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_BC2_H
#define BSP430_PERIPH_BC2_H

#include <bsp430/clock.h>
#include <bsp430/periph.h>

/** @def BSP430_MODULE_BC2
 *
 * Defined on inclusion of <bsp430/periph/bc2.h>.  The value evaluates
 * to true if the target MCU supports the Basic Clock Module+, and
 * false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_BC2 defined(__MSP430_HAS_BC2__)

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_BC2 - 0)

/** @def configBSP430_BC2_TRIM_TO_MCLK
 *
 * Define to a true value to request that ulBSP430bc2TrimToMCLK_ni()
 * be made available.
 *
 * Because implementation of this function depends on
 * #BSP430_TIMER_CCACLK, setting this option causes
 * <bsp430/platform/bsp430_config.h> to default
 * #configBSP430_TIMER_CCACLK to true.
 *
 * This value represents an application or system request for the
 * feature; availability of the feature must be tested using
 * #BSP430_BC2_TRIM_TO_MCLK before attempting to invoke the function.
 *
 * @cppflag
 * @affects #BSP430_BC2_TRIM_TO_MCLK
 * @dependency #BSP430_TIMER_CCACLK
 * @defaulted
 */
#ifndef configBSP430_BC2_TRIM_TO_MCLK
#define configBSP430_BC2_TRIM_TO_MCLK 0
#endif /* configBSP430_BC2_TRIM_TO_MCLK */

#if (configBSP430_BC2_TRIM_TO_MCLK - 0) && ! (configBSP430_TIMER_CCACLK - 0)
#warning configBSP430_BC2_TRIM_TO_MCLK requested without configBSP430_TIMER_CCACLK
#endif /* configBSP430_BC2_TRIM_TO_MCLK */

/** @def BSP430_BC2_TRIM_TO_MCLK
 *
 * Defined to a true value if #configBSP430_BC2_TRIM_TO_MCLK was requested
 * and #BSP430_TIMER_CCACLK is available on the platform.
 *
 * In the absence of this flag, iBSP430bc2TrimToMCLK_ni() will not be
 * available and must not be referenced.
 *
 * @dependency #configBSP430_BC2_TRIM_TO_MCLK, #BSP430_TIMER_CCACLK
 * @platformdefault
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_BC2_TRIM_TO_MCLK include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN) || (configBSP430_BC2_TRIM_TO_MCLK - 0)
/** Adjust the DCO as necessary to reach the requested MCLK speed
 *
 * nThe BC2 peripheral supports only a small number of calibrated DCO
 * clock frequencies, and it is not uncommon for them to be off by a
 * significant amount for the cheaper ValueLine MCUs.  If an ACLK
 * source at a trusted rate is available, the MCU can implement a loop
 * to converge on a DCO configuration that supports a specific
 * requested clock speed.
 *
 * @return 0 if the requested speed could be reached, -1 if an error
 * occurred.  Potential errors are inability to access
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * @warning The accuracy of the resulting frequency is proportional to
 * the accuracy of the returned value of #ulBSP430clockACLK_Hz_ni().
 * If ACLK derives from VLOCLK, that estimate may be off by several
 * percent, which will generally result in serial errors if SMCLK is
 * used to generate a baud rate clock.  You may wish to check
 * #BSP430_CLOCK_LFXT1_IS_FAULTED_NI() prior to invoking this, as is done
 * by ulBSP430clockConfigureMCLK_ni() for this peripheral.
 *
 * @dependency #BSP430_BC2_TRIM_TO_MCLK, #BSP430_TIMER_CCACLK
 */
int iBSP430bc2TrimToMCLK_ni (unsigned long mclk_Hz);
#endif /* configBSP430_BC2_TRIM_TO_MCLK */

/** @cond DOXYGEN_INTERNAL */
/* Simplify conditionally-defined macros to avoid reference to
 * non-existent values.  The chip supports an external crystal for
 * LFXT1 iff LFXT1S_0 is defined */
#if defined(XT2OF)
#define BSP430_BC2_XT2OF_ XT2OF
#else /* XT2OF */
#define BSP430_BC2_XT2OF_ 0
#endif /* XT2OF */
#if defined(LFXT1OF)
#define BSP430_BC2_LFXT1OF_ LFXT1OF
#else /* LFXT1OF */
#define BSP430_BC2_LFXT1OF_ 0
#endif /* LFXT1OF */
/** @endcond */

/** BC2-specific check for LFXT1 crystal fault condition.
 *
 * This checks exactly for the fault condition.
 *
 * @note A crystal that has never been enabled will not register as
 * faulted. */
#define BSP430_BC2_LFXT1_IS_FAULTED_NI() (BCSCTL3 & BSP430_BC2_LFXT1OF_)

/** BC2-specific check for XT2 crystal fault condition.
 *
 * If the platform does not support an XT2 crystal no fault is
 * diagnosed.
 *
 * @note A crystal that has never been enabled will not register as
 * faulted. */
#define BSP430_BC2_XT2_IS_FAULTED_NI() (BC2CTL7 & BSP430_BC2_XT2OF_)

/** Check whether the BC2-controlled LFXT1 crystal has a fault condition.
 *
 * @note Unlike most other MSP430 clock peripherals, MCUs with BC2
 * power-up with the crystal pins in their peripheral mode and with
 * the external crystal selected as the LFXT1 source.  However, the
 * fault flag is still valid only if the external crystal is selected.
 * This check will indicate a fault if one is present, or if the LFXT1
 * source is not the external crystal.
 *
 * @defaulted
 * @see #BSP430_BC2_LFXT1_IS_FAULTED_NI()
 */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_LFXT1_IS_FAULTED_NI)
#define BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ((BCSCTL3 & (LFXT1S0 | LFXT1S1)) || BSP430_BC2_LFXT1_IS_FAULTED_NI())
#endif /* BSP430_CLOCK_LFXT1_IS_FAULTED_NI */

/** Check whether the BC2-controlled XT2 crystal has a fault condition.
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
 * @see #BSP430_BC2_XT2_IS_FAULTED_NI()
 */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_XT2_IS_FAULTED_NI)
#define BSP430_CLOCK_XT2_IS_FAULTED_NI() ((BCSCTL1 & XT2OFF) || BSP430_BC2_XT2_IS_FAULTED_NI())
#endif /* BSP430_CLOCK_XT2_IS_FAULTED_NI */

/** Clear all clock faults.
 *
 * The BC2 oscillator fault flags are read-only, so this only clears
 * the system oscillator flag.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_CLEAR_FAULTS_NI)
#define BSP430_CLOCK_CLEAR_FAULTS_NI() do {                             \
    BSP430_CLOCK_OSC_CLEAR_FAULT_NI();                                  \
  } while (0)
#endif /*  BSP430_CLOCK_CLEAR_FAULTS_NI */

#ifndef BSP430_CLOCK_LFXT1_XCAP
/** Peripheral default setting for platform-specific constant */
#define BSP430_CLOCK_LFXT1_XCAP XCAP_1
#endif /* BSP430_CLOCK_LFXT1_XCAP */

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 12000U

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_PUC_MCLK_HZ 1100000UL

#endif /* BSP430_MODULE_BC2 */

#endif /* BSP430_PERIPH_BC2_H */
