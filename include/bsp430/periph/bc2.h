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
 * @brief Hardware presentation/abstraction for Basic Clock Module+ (BC2).
 *
 * This module supports the BC2 ("Basic Clock Module+") peripheral,
 * which is present in 2xx-family devices.
 * <ul>
 *
 * <li>For #ulBSP430clockConfigureMCLK_ni the only recognized
 * frequencies are 1 MHz, 8 MHz, 12 MHz, and 16 MHz.  Availability of
 * a given frequency depends on the specific MCU.  The selected
 * frequency may be above or below the requested frequency, but will
 * be the closest supported by available calibrated clocks.  The
 * selected frequency may also be refined if a stable crystal is
 * available; see #configBSP430_BC2_TRIM_TO_MCLK.
 *
 * <li>The implementation assumes that MCLK and SMCLK are both
 * sourced from DCOCLK.
 *
 * </ul>
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
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

#if BSP430_MODULE_BC2 - 0

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
 * the accuracy of the returned value of #usBSP430clockACLK_Hz_ni().
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

#undef BSP430_CLOCK_LFXT1_IS_FAULTED_NI
/** Check whether the LFXT1 crystal has a fault condition.
 *
 * This definition overrides the generic definition to test the
 * crystal-specific flags.  The LFXT1OF bit reflects the stability of
 * the external crystal only if the external crystal is the source for
 * LFXT1, so check that too: all is good only if LFXT1S is an external
 * crystal that is not faulted. */
#define BSP430_CLOCK_LFXT1_IS_FAULTED_NI() (BCSCTL3 & (LFXT1S1 | LFXT1S0 | LFXT1OF))

#undef BSP430_CLOCK_LFXT1_CLEAR_FAULT_NI
/** Clear the fault associated with LFXT1.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag. */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT_NI() do {                   \
    /* User's guide says BCSCTL3.LFXT1OF is read-only. */	\
    IFG1 &= ~OFIFG;                                             \
  } while (0)

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 12000U

/** Unconditional define for peripheral-specific constant */
#define BSP430_CLOCK_PUC_MCLK_HZ 1100000UL

#endif /* BSP430_MODULE_BC2 */

#endif /* BSP430_PERIPH_BC2_H */
