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
 * @brief Hardware presentation/abstraction for Clock System (CS) on FR4xx/2xx chips.
 *
 * The Clock System (CS) in FR4xx/2xx-family devices is completely
 * different from the CS system in FR5xx-family devices but it is most
 * same as a UCS peripheral in the F5xx/6xx-family devices cross-bred
 * with an FLL+ peripheral from the 4xx family.
 *
 * @section h_periph_cs4_opt Module Configuration Options
 *
 * @section h_periph_cs4_hpl Hardware Presentation Layer
 *
 * As there can be only one instance of CS4 on any MCU, there is no
 * structure supporting a CS4 @HPL.  Manipulate the peripheral through its
 * registers directly.
 *
 * @section h_periph_cs4_hal Hardware Adaptation Layer
 *
 * As there can be only one instance of CS4 on any MCU, there is no
 * structure supporting a CS4 @HAL.
 *
 * The standard set of capabilities in the bsp430/clocks.h header are
 * supported, with the following details:
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_CS4_H
#define BSP430_PERIPH_CS4_H

#include <bsp430/clock.h>
#include <bsp430/periph.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Defined on inclusion of <bsp430/periph/cs4.h>.  The value
 * evaluates to true if the target MCU supports the CS peripheral of
 * the FR4xx/2xx devices, and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_CS4 (BSP430_PERIPH_CS_IS_CS4 - 0)

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_CS4 - 0)

/** Preferred source for FLL clock
 *
 * This should be a constant denoting the bits to be set in the CS4
 * control register for SELREF selecting the FLL reference clock.  The
 * default value selects XT1CLK, which will internally fall back to
 * REFOCLK if XT1CLK is faulted.
 *
 * @cppflag
 */
#ifndef BSP430_CS4_FLL_SELREF
#define BSP430_CS4_FLL_SELREF SELREF__XT1CLK
#endif /* BSP430_CS4_FLL_SELREF */

/** CS4-specific check for DCO fault condition */
#define BSP430_CS4_DCO_IS_FAULTED_NI() (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1))

/** CS4-specific check for LFXT1 crystal fault condition.
 *
 * This checks exactly for the fault condition.
 *
 * @note A crystal that has never been enabled will not register as
 * faulted. */
#define BSP430_CS4_LFXT1_IS_FAULTED_NI() (CSCTL7 & XT1OFFG)

/** CS4-specific check for XT2 crystal fault condition.
 *
 * If the platform does not support an XT2 crystal no fault is
 * diagnosed.
 *
 * @note A crystal that has never been enabled will not register as
 * faulted. */
#define BSP430_CS4_XT2_IS_FAULTED_NI() 0

/** Check whether the CS4-controlled LFXT1 crystal has a fault condition.
 *
 * @note Oscillator fault flags are not set unless a fault has been
 * detected.  If the crystal has never been enabled, no fault will
 * have been detected.  On power-up, the XIN function is not enabled
 * and CSCTL6.XT1OFF is set, and BSP430 treats CSCTL6.XT1OFF as an
 * indication that the pins are not configured for crystal use, either
 * because LFXT1 has not been configured or has been configured and
 * found to be faulted.  Although it is perfectly acceptable to have
 * CSCTL6.XT1OFF set and the crystal working fine, the complexity of
 * detecting that case is not supported by this implementation.
 *
 * @defaulted
 * @see #BSP430_CS4_LFXT1_IS_FAULTED_NI()
 */
#define BSP430_CLOCK_LFXT1_IS_FAULTED_NI() BSP430_CS4_LFXT1_IS_FAULTED_NI()

/** CS4 does not support XT2 so it is always in fault.
 *
 * @see #BSP430_CS4_XT2_IS_FAULTED_NI()
 */
#define BSP430_CLOCK_XT2_IS_FAULTED_NI() (1)

/** Clear all CS4-specific faults. */
#define BSP430_CS4_CLEAR_FAULTS_NI() do {       \
    CSCTL7 &= ~(FLLUNLOCKHIS0 | FLLUNLOCKHIS1 | FLLULIFG | XT1OFFG | DCOFFG); \
  } while (0)

/** Clear all clock faults.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || ! defined(BSP430_CLOCK_CLEAR_FAULTS_NI)
#define BSP430_CLOCK_CLEAR_FAULTS_NI() do {                             \
    BSP430_CS4_CLEAR_FAULTS_NI();                                       \
    BSP430_CLOCK_OSC_CLEAR_FAULT_NI();                                  \
  } while (0)
#endif /*  BSP430_CLOCK_CLEAR_FAULTS_NI */

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 10000U

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_PUC_MCLK_HZ 1048576UL

/** The CS4 module supports an external crystal which is normally
 * low-frequency.  On some devices it may be allowed to be
 * high-frequency. */
#define BSP430_CS4_NOMINAL_XT1CLK_HZ 32768U

/** The CS4 module supports an internally trimmed reference oscillator
 * running at a nominal 32 KiHz rate, for use where XT1 is not
 * populated. */
#define BSP430_CS4_NOMINAL_REFOCLK_HZ 32768U

/** The CS4 module supports an internal low-precision high-frequency
 * module clock source running at a nominal 5 MHz rate, for use where
 * SMCLK is unsuitable.  Its rate varies with temperature, voltage,
 * and individual device.
 *
 * @defaulted */
#ifndef BSP430_CS4_NOMINAL_MODCLK_HZ
#define BSP430_CS4_NOMINAL_MODCLK_HZ 5000000UL
#endif /* BSP430_CS4_NOMINAL_MODCLK_HZ */

/** @cond DOXYGEN_EXCLUDE */
/* Provide definition for clock system */
#define BSP430_NOMINAL_MODCLK_HZ BSP430_CS4_NOMINAL_MODCLK_HZ
/** @endcond */

#endif /* BSP430_MODULE_CS4 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP430_PERIPH_CS4_H */
