/* Copyright (c) 2012, Peter A. Bigot
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
 * @brief Hardware presentation/abstraction for Unified Clock System (UCS).
 *
 * The Unified Clock System (UCS) is present in 5xx/6xx-family
 * devices.  A variant UCS_RF differs from UCS by adding support for
 * XT2.  This difference is currently ignored.
 *
 * @section h_periph_ucs_opt Module Configuration Options
 *
 * @li #configBSP430_UCS_TRIM_DCOCLKDIV enables specific MCLK
 * frequencies to be configured
 *
 * @li #configBSP430_UCS_FLLREFCLK_IS_XT1CLK controls the selection of
 * clock used for the FLL
 *
 * @li #configBSP430_UCS_TRIM_ACLK_IS_XT1CLK controls the selection of
 * clock used for DCO trimming
 *
 * @section h_periph_ucs_hpl Hardware Presentation Layer
 *
 * As there can be only one instance of UCS on any MCU, there is no
 * structure supporting a UCS @hpl.  Manipulate the peripheral through its
 * registers directly.
 *
 * @section h_periph_ucs_hal Hardware Adaptation Layer
 *
 * As there can be only one instance of UCS on any MCU, there is no
 * structure supporting a UCS @hal.
 *
 * The standard set of capabilities in the bsp430/clocks.h header are
 * supported, with the following details:
 *
 * @li Comparison of SMCLK against a clock of known frequency is
 * required to set MCLK frequencies as there are no DCO calibration
 * constants for this peripheral.  bsp430/platform/bsp430_config.h
 * will automatically request the required #configBSP430_TIMER_CCACLK
 * in support of this unless #configBSP430_UCS_TRIM_DCOCLKDIV is
 * explicitly disabled.
 *
 * @li FLLREFDIV is /1.
 *
 * @li MCLK and SMCLK are sourced from DCOCLKDIV
 *
 * @li DCOCLKDIV is DCOCLK/2 (the power-up value)
 *
 * @li FLL trimming will leave the FLL disabled if it was disabled on
 * entry; see #configBSP430_CORE_DISABLE_FLL.
 *
 * @li Executing an FLL trim (either directly or due to having invoked
 * ulBSP430clockConfigureMCLK_ni()) will record in private state
 * the measured clock frequency.
 *
 * @li ulBSP430clockMCLK_Hz_ni() and ulBSP430clockSMCLK_Hz_ni() return
 * values are calculated by applying the relevant clock dividers as
 * read from the UCS registers to the recorded measured clock
 * frequency.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_UCS_H
#define BSP430_PERIPH_UCS_H

#include <bsp430/clock.h>
#include <bsp430/periph.h>

/** @def BSP430_MODULE_UCS
 *
 * Defined on inclusion of <bsp430/periph/ucs.h>.  The value evaluates
 * to true if the target MCU supports the Unified Clock System, and
 * false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_UCS (defined(__MSP430_HAS_UCS__)          \
                           || defined(__MSP430_HAS_UCS_RF__))

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_UCS - 0)

/** @def configBSP430_UCS_TRIM_DCOCLKDIV
 *
 * Define to a true value to request that
 * ulBSP430ucsTrimDCOCLKDIV_ni() be made available.
 *
 * Because implementation of this function depends on
 * #BSP430_TIMER_CCACLK, setting this option causes
 * <bsp430/platform/bsp430_config.h> to default
 * #configBSP430_TIMER_CCACLK to true.
 *
 * This value represents an application or system request for the
 * feature; availability of the feature must be tested using
 * #BSP430_UCS_TRIM_DCOCLKDIV before attempting to invoke the function.
 *
 * @note This function is the only mechanism by which the UCS DCO can
 * be configured by BSP430.
 *
 * @cppflag
 * @affects #BSP430_UCS_TRIM_DCOCLKDIV
 * @dependency #BSP430_TIMER_CCACLK
 * @defaulted
 */
#ifndef configBSP430_UCS_TRIM_DCOCLKDIV
#define configBSP430_UCS_TRIM_DCOCLKDIV 1
#endif /* configBSP430_UCS_TRIM_DCOCLKDIV */

#if (configBSP430_UCS_TRIM_DCOCLKDIV - 0) && ! (configBSP430_TIMER_CCACLK - 0)
#warning configBSP430_UCS_TRIM_DCOCLKDIV requested without configBSP430_TIMER_CCACLK
#endif /* configBSP430_UCS_TRIM_DCOCLKDIV */

/** @def BSP430_UCS_TRIM_DCOCLKDIV
 *
 * Defined to a true value if #configBSP430_UCS_TRIM_DCOCLKDIV was requested
 * and #BSP430_TIMER_CCACLK is available on the platform.
 *
 * In the absence of this flag, ulBSP430uscTrimDCOCLKDIV_ni() will not be
 * available and must not be referenced.
 *
 * @dependency #configBSP430_UCS_TRIM_DCOCLKDIV, #BSP430_TIMER_CCACLK
 * @platformdefault
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_UCS_TRIM_DCOCLKDIV include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN) || (configBSP430_UCS_TRIM_DCOCLKDIV - 0)
/** Adjust the FLL as necessary to maintain the last configured DCOCLKDIV speed
 *
 * This function is most likely to be used if
 * #configBSP430_CORE_DISABLE_FLL is set, but is also a required
 * subroutine of the UCS implementation of
 * ulBSP430clockConfigureMCLK_ni().
 *
 * The function is expected to be used periodically to maintain an
 * already configured clock in the face of varying voltage,
 * temperature, and other factors such as chip errata that introduce
 * clock drift.  The implementation is not entitled to reconfigure to
 * a different DCO range selection, should that be supported by the
 * clock.  Consequently, oscillator faults may result if the system
 * drifts so far that the target frequency cannot be represented
 * within the current range.  In such a situation,
 * ulBSP430clockConfigureMCLK_ni() should be re-invoked to configure
 * the clock.
 *
 * @warning This function will temporarily reconfigure MCLK, SMCLK,
 * and ACLK.  Any peripherals that depend on those clocks should be
 * disabled while the function is executing.
 *
 * @return 0 if the trimming was completed successfully, -1 if an
 * error occurred.  Potential errors are a dependency on XT1CLK while
 * that is faulted and inability to access
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * @dependency #BSP430_UCS_TRIM_DCOCLKDIV
 */
int iBSP430ucsTrimDCOCLKDIV_ni ();
#endif /* configBSP430_UCS_TRIM_DCOCLKDIV */

/** @def configBSP430_UCS_FLLREFCLK_IS_XT1CLK
 *
 * The UCS module supports a variety of potential sources for the FLL
 * reference clock; the implementation here supports either LFXT1 or
 * REFO.
 *
 * Define this to true if you want to use LFXT1; otherwise REFO is
 * used.
 *
 * @warning If this is selected, DCOCLKDIV trim will hang if LFXT1 is
 * faulted and cannot be cleared.
 *
 * @cppflag
 */
#ifndef configBSP430_UCS_FLLREFCLK_IS_XT1CLK
#define configBSP430_UCS_FLLREFCLK_IS_XT1CLK 0
#endif /* configBSP430_UCS_FLLREFCLK_IS_XT1CLK */

/** @def configBSP430_UCS_TRIM_ACLK_IS_XT1CLK
 *
 * When measuring the actual clock speed to determine whether the FLL
 * has drifted, a reliable clock at a known frequency is required.
 * The implementation here configures ACLK to source from that clock
 * for the duration of the trim operation.
 *
 * Alternative sources supported here are LFXT1 and REFO, both of
 * which are nominally 32 kiHz though LFXT1 could be a different
 * speed.  As LFXT1 is likely more accurate than REFO, there may be
 * some benefit in using it when FLLREFCLK is REFO.
 *
 * Define this to true if you want to use LFXT1; otherwise REFO is
 * used.
 *
 * @warning If this is selected, FLL trim will hang if LFXT1 is
 * faulted and cannot be cleared.
 *
 * @cppflag
 */
#ifndef configBSP430_UCS_TRIM_ACLK_IS_XT1CLK
#define configBSP430_UCS_TRIM_ACLK_IS_XT1CLK 0
#endif /* configBSP430_UCS_TRIM_ACLK_IS_XT1CLK */

#undef BSP430_CLOCK_LFXT1_IS_FAULTED_NI
/** Check whether the LFXT1 crystal has a fault condition.
 *
 * This definition overrides the generic definition to test the
 * crystal-specific flags.  Note that if somebody has turned off the
 * crystal by setting UCSCTL6.XT1OFF, the crystal is assumed to be
 * faulted.*/
#define BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ((UCSCTL6 & XT1OFF) || (UCSCTL7 & XT1LFOFFG))

#undef BSP430_CLOCK_LFXT1_CLEAR_FAULT_NI
/** Clear the fault associated with LFXT1.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag. */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT_NI() do {	\
    UCSCTL7 &= ~XT1LFOFFG;                      \
    SFRIFG1 &= ~OFIFG;                          \
  } while (0)

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 10000U

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_PUC_MCLK_HZ 1048576UL

/** @def BSP430_UCS_NOMINAL_REFOCLK_HZ
 *
 * The UCS module supports an internally trimmed reference oscillator
 * running at a nominal 32 kiHz rate, for use where XT1 is not
 * populated. */
#define BSP430_UCS_NOMINAL_REFOCLK_HZ 32768U

#endif /* BSP430_MODULE_UCS */

#endif /* BSP430_PERIPH_UCS_H */

