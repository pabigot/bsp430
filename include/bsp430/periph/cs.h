/* Copyright 2012-2014, Peter A. Bigot
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
 * @brief Hardware presentation/abstraction for Clock System (CS).
 *
 * The Clock System (CS) peripheral is present in FR5xx-family
 * devices.  Variant CS is used in FR57xx devices; variant CS_A is
 * used in FR58xx/FR59xx devices.  They differ in the names for
 * constants related to low and high frequency crystals, in the set of
 * supported DCOCLK frequencies, and in whether DCOCLK can source
 * ACLK.
 *
 * @note The CS peripheral on the FR4xx/2xx-family devices is
 * completely different and its supporting material is provided in the
 * <bsp430/periph/cs4.h> header.
 *
 * @section h_periph_cs_opt Module Configuration Options
 *
 * None supported.
 *
 * @section h_periph_cs_hpl Hardware Presentation Layer
 *
 * As there can be only one instance of CS on any MCU, there is no
 * structure supporting a CS HPL.  Manipulate the peripheral through
 * its registers directly.
 *
 * @section h_periph_cs_hal Hardware Adaptation Layer
 *
 * As there can be only one instance of CS on any MCU, there is no
 * structure supporting a CS HAL.
 *
 * The standard set of capabilities in the bsp430/clocks.h header are
 * supported, with the following details:
 *
 * @li (CS) The peripheral supports only a few factory-trimmed
 * frequencies: for CS these are 5.33, 6.67, and 8 MHz on low-speed
 * devices, and additionally 16, 20, and 24 MHz on high-speed--capable
 * devices.  For CS_A these are 1, 2.67, 3.33, 4.0, 5.33, and 6.67 MHz
 * on low-speed devices, and additionally 8, 16, 20, and 24 MHz on
 * high-speed-capable devices.  ulBSP430clockConfigureMCLK_ni() will
 * select and return the closest available frequency.
 *
 * @li The implementation assumes that MCLK and SMCLK are both sourced
 * from DCOCLK.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_CS_H
#define BSP430_PERIPH_CS_H

#include <bsp430/clock.h>
#include <bsp430/periph.h>

/** Determine whether target has a CS or CS_A peripheral.
 *
 * The base CS peripheral as defined in the FR57xx family has a
 * different set of available clocks and differences in register
 * layout and configurable clock rates from the variant in the
 * FR58xx/FR59xx, which was distinguished as CS_A in the header files
 * originally released for the Wolverine chips.  Subsequently TI
 * decided to eliminate the difference in name, while retaining the
 * difference in interface and functionality.  This occurs in public
 * release version 1.073 of the MSP430 headers.
 *
 * This macro is defined to a true value if the target peripheral has
 * the FR58xx variant of the CS module.  It uses @c
 * __MSP430_HAS_CS_A__ if available, otherwise depends on the presence
 * of @c DCOFSEL2, a bit not available on the original CS peripheral.
 *
 * @cppflag */
#define BSP430_CS_IS_FR58XX (BSP430_PERIPH_CS_IS_CSA - 0)

/** Determine whether target has a CS or CS_A peripheral.
 *
 * This macro is defined to a true value if the target peripheral has
 * the FR57xx variant of the CS module.
 *
 * @see #BSP430_CS_IS_FR58XX
 *
 * @cppflag */
#define BSP430_CS_IS_FR57XX (defined(__MSP430_HAS_CS__) && (! ((BSP430_PERIPH_CS_IS_CSA - 0) || (BSP430_PERIPH_CS_IS_CS4 - 0))))

/** Defined on inclusion of <bsp430/periph/cs.h>.  The value evaluates
 * to true if the target MCU supports the Clock System peripheral, and
 * false if it does not.
 *
 * @note Although TI calls it a CS module, the Clock System on the
 * FR4xx/2xx devices is not anywhere like this one and is provided by
 * the <bsp430/periph/cs4.h> header.
 *
 * @cppflag
 */
#define BSP430_MODULE_CS ((BSP430_CS_IS_FR57XX - 0) || (BSP430_CS_IS_FR58XX - 0))

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_CS - 0)

/** @cond DOXYGEN_INTERNAL */
/* Simplify conditionally-defined macros to avoid reference to
 * non-existent values and inconsistencies between CS and CS_A. */
#if (BSP430_CS_IS_FR58XX - 0)
#define BSP430_CS_XT2DRIVE_ HFXTDRIVE
#define BSP430_CS_XT2OFF_ HFXTOFF
#define BSP430_CS_XT1DRIVE_ LFXTDRIVE
#define BSP430_CS_XTS_ 0
#define BSP430_CS_XT1BYPASS_ LFXTBYPASS
#define BSP430_CS_XT1OFF_ LFXTOFF
#define BSP430_CS_XT2OFFG_ HFXTOFFG
#define BSP430_CS_XT1OFFG_ LFXTOFFG
#else /* CS or CS_A */
#define BSP430_CS_XT2DRIVE_ XT2DRIVE
#define BSP430_CS_XT2OFF_ XT2OFF
#define BSP430_CS_XT1DRIVE_ XT1DRIVE
#define BSP430_CS_XTS_ XTS
#define BSP430_CS_XT1BYPASS_ XT1BYPASS
#define BSP430_CS_XT1OFF_ XT1OFF
#if defined(XT2OFFG)
#define BSP430_CS_XT2OFFG_ XT2OFFG
#else /* XT2OFFG */
#define BSP430_CS_XT2OFFG_ 0
#endif /* XT2OFFG */
#define BSP430_CS_XT1OFFG_ XT1OFFG
#endif /* CS or CS_A */
/** @endcond */

#if defined(BSP430_DOXYGEN) || (BSP430_CS_IS_FR58XX - 0)
/* CS_A has a significantly different set of capabilities */

/** The CS_A module supports an internal low-precision high-frequency
 * module clock source running at a nominal 5 MHz rate, for use where
 * SMCLK is unsuitable.  Its rate varies with temperature, voltage,
 * and individual device.
 *
 * This source exists only in the CS_A version of the CS peripheral.

 * @defaulted */
#ifndef BSP430_CS_NOMINAL_MODCLK_HZ
#define BSP430_CS_NOMINAL_MODCLK_HZ 5000000UL
#endif /* BSP430_CS_NOMINAL_MODCLK_HZ */

/* Provide definition for clock system */
#define BSP430_NOMINAL_MODCLK_HZ BSP430_CS_NOMINAL_MODCLK_HZ

/** Nominal rate of the CS_A peripheral LFMODCLK.
 *
 * This source exists only in the CS_A version of the CS peripheral.
 * It is driven by an internal low-power oscillator. */
#define BSP430_CS_NOMINAL_LFMODCLK_HZ (BSP430_CS_NOMINAL_MODCLK_HZ / 128)
#endif /* BSP430_CS_IS_FR58XX */

/** CS-specific check for LFXT1 crystal fault condition.
 *
 * This checks exactly for the fault condition.
 *
 * @note A crystal that has never been enabled will not register as
 * faulted. */
#define BSP430_CS_LFXT1_IS_FAULTED_NI() (CSCTL5 & BSP430_CS_XT1OFFG_)

/** CS-specific check for XT2 crystal fault condition.
 *
 * If the platform does not support an XT2 crystal no fault is
 * diagnosed.
 *
 * @note A crystal that has never been enabled will not register as
 * faulted. */
#define BSP430_CS_XT2_IS_FAULTED_NI() (CSCTL5 & BSP430_CS_XT2OFFG_)

/** Check whether the CS-controlled LFXT1 crystal has a fault condition.
 *
 * @note Oscillator fault flags are not set unless a fault has been
 * detected.  If the crystal has never been enabled, no fault will
 * have been detected.  On power-up, the XIN function is not enabled
 * and CSCTL4.XT1OFF is set, and BSP430 treats CSCTL4.XT1OFF as an
 * indication that the pins are not configured for crystal use, either
 * because LFXT1 has not been configured or has been configured and
 * found to be faulted.  Although it is perfectly acceptable to have
 * CSCTL4.XT1OFF set and the crystal working fine, the complexity of
 * detecting that case is not supported by this implementation.
 *
 * @defaulted
 * @see #BSP430_CS_LFXT1_IS_FAULTED_NI()
 */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_LFXT1_IS_FAULTED_NI)
#define BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ((CSCTL4 & BSP430_CS_XT1OFF_) || BSP430_CS_LFXT1_IS_FAULTED_NI())
#endif /* BSP430_CLOCK_LFXT1_IS_FAULTED_NI */

/** Check whether the CS-controlled XT2 crystal has a fault condition.
 *
 * @note Oscillator fault flags are not set unless a fault has been
 * detected.  If the crystal has never been enabled, no fault will
 * have been detected.  On power-up, the XT2IN function is not enabled
 * and CSCTL4.XT2OFF is set, and BSP430 treats CSCTL4.XT2OFF as an
 * indication that the pins are not configured for crystal use, either
 * because XT2 has not been configured or has been configured and
 * found to be faulted.  Although it is perfectly acceptable to have
 * CSCTL4.XT2OFF set and the crystal working fine, the complexity of
 * detecting that case is not supported by this implementation.
 *
 * @defaulted
 * @see #BSP430_CS_XT2_IS_FAULTED_NI()
 */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_XT2_IS_FAULTED_NI)
#define BSP430_CLOCK_XT2_IS_FAULTED_NI() ((CSCTL4 & BSP430_CS_XT2OFF_) || BSP430_CS_XT2_IS_FAULTED_NI())
#endif /* BSP430_CLOCK_XT2_IS_FAULTED_NI */

/** Clear all CS-specific faults.
 *
 * @note This does not unlock the CS registers.  Invoking it while
 * registers are locked will result in a restart. */
#define BSP430_CS_CLEAR_FAULTS_NI() do {                        \
    CSCTL5 &= ~(BSP430_CS_XT2OFFG_ | BSP430_CS_XT1OFFG_);       \
  } while (0)

/** Clear all clock faults.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_CLEAR_FAULTS_NI)
#define BSP430_CLOCK_CLEAR_FAULTS_NI() do {                     \
    CSCTL0_H = 0xA5;                                            \
    BSP430_CS_CLEAR_FAULTS_NI();                                \
    CSCTL0_H = !0xA5;                                           \
    BSP430_CLOCK_OSC_CLEAR_FAULT_NI();                          \
  } while (0)
#endif /*  BSP430_CLOCK_CLEAR_FAULTS_NI */

/** Determine FRAM wait states required for clock speed.
 *
 * This is a platform-specific macro to select the appropriate number
 * of wait states required for reliable FRAM access when using a given
 * system clock speed.  It should be defined in the platform.h header,
 * based on values specified in the data sheet under Recommended
 * Operating Conditions for f_SYSTEM.  A value of zero is used if a
 * negative value is requested.
 *
 * @param freq_ Desired f_SYSTEM (maximum RAM access frequency)
 *
 * @note For FR57xx devices this macro is optional, as that family
 * supports an automatic setting for wait states; if the macro is
 * defined it will be used, otherwise it will not.  For FR58xx devices
 * setting wait stats manually is required, but if the macro is
 * missing this definition will be used:
 * @note
 * @code
 * #define BSP430_CS_FRAM_NWAITS_FOR_FREQ(freq_) (((int)((freq_)/8000000L)) - 1)
 * @endcode
 *
 * @warning Though the gloss in the datasheet references MCLK, the
 * FR58xx user's guide documents that wait states also apply to DMA,
 * which may run at a higher speed if MCLK's divisor is greater than
 * SMCLK's.  BSP430 assumes MCLK's divisor is one, and so uses the
 * actual configured DCO frequency to determine the wait states.  This
 * is sub-optimal, but correct, if all clocks divide that frequency.
 *
 * @defaulted
 * @platformvalue
 * @dependency #BSP430_FR58XX */
#if defined(BSP430_DOXYGEN)
#define BSP430_CS_FRAM_NWAITS_FOR_FREQ(mclk_) include <bsp430/platform.h>
#endif /* BSP430_PMM_COREV_FOR_MCLK */

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 10000U

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_PUC_MCLK_HZ 1048576UL

#endif /* BSP430_MODULE_CS */

#endif /* BSP430_PERIPH_CS_H */
