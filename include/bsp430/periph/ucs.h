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
 * @brief Hardware presentation/abstraction for Unified Clock System (UCS).
 *
 * This module supports the Unified Clock System (UCS) and UCS_RF
 * peripherals, which are present in 5xx/6xx-family devices.
 *
 * UCS_RF differs from UCS by adding support for XT2.  This difference
 * is currently ignored.
 *
 * Based on experimentation, the following is assumed or enforced for
 * all supported clock configurations:
 * 
 * @li SELREF is XT1CLK running at 32768 Hz.  #ulBSP430ucsConfigure_ni
 * will enforce this.
 *
 * @li FLLD is consistently set to /2, which is the PUC value and
 *     which is adequate to support speeds up to 32 MiHz with a 32768
 *     Hz REFCLK.
 *
 * @li FLLREFDIV is consistently set to /1, which is the PUC value.
 *
 * @li MCLK and SMCLK are set to DCOCLKDIV, and ACLK to XT1CLK.
 *
 * @li Due to UCS10 and UCS7, normal practice on UCS-based MCUs is to
 *     leave the FLL disabled, and adjust it periodically when the
 *     clocks are otherwise unused.  Thus #SCG0 is expected to be set
 *     at all times except when trimming; see #BSP430_CLOCK_DISABLE_FLL.
 *
 * Other refinements in this module:
 *
 * @li #ulBSP430clockMCLK_Hz_ni returns the most recent measured trimmed
 * frequency.
 * 
 * @li #ulBSP430clockSMCLK_Hz_ni returns the most recent measured trimmed
 * frequency shifted right by
 * #BSP430_CLOCK_SMCLK_DIVIDING_SHIFT.
 *
 * @li #usBSP430clockACLK_Hz_ni assumes returns 32768 if XT1CLK is the
 * selected source for ACLK and OFIFG is clear, and returns 10000 (the
 * nominal VLOCLK frequency) otherwise.  Be aware that the actual
 * VLOCLK frequency may be different by 10-20%, and that if ACLK is
 * not actually based on XT1CLK the rest of this module may not work.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_CLOCKS_UCS_H
#define BSP430_CLOCKS_UCS_H

#include <bsp430/clock.h>
#include <bsp430/periph.h>
#if ! (defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__))
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_UCS__ */

#undef BSP430_CLOCK_LFXT1_IS_FAULTED
/** Check whether the LFXT1 crystal has a fault condition.
 *
 * This definition overrides the generic definition to test the
 * crystal-specific flags. */
#define BSP430_CLOCK_LFXT1_IS_FAULTED() (UCSCTL7 & XT1LFOFFG)

#undef BSP430_CLOCK_LFXT1_CLEAR_FAULT
/** Clear the fault associated with LFXT1.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag. */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do {	\
		UCSCTL7 &= ~XT1LFOFFG;					\
		SFRIFG1 &= ~OFIFG;						\
	} while (0)

/** Call this to initially configure the UCS peripheral.
 *
 * @param ulFrequency_Hz The target frequency for DCOCLKDIV=MCLK.
 * SMCLK will be set to DCOCLKDIV divided in accordance with
 * #BSP430_CLOCK_SMCLK_DIVIDING_SHIFT.
 *
 * @param sRSEL The DCO frequency range selection.  The appropriate
 * value is frequency-dependent, and specified in the "DCO Frequency"
 * table in the device-specific data sheet.  The same target frequency
 * may be reachable with different RSELs but with different
 * accuracies.  If a negative value is given, the function may be able
 * to select a default from an internal table if that table has been
 * updated to include information on the relevant device.
 * 
 * @return an estimate of the actual running frequency.
 *
 * @note This function expects a valid 32 kiHz clock source on XT1,
 * and that this will be used as the source for ACLK.  It invokes
 * #iBSP430clockConfigureXT1_ni and #iBSP430ucsConfigureACLK_ni to ensure
 * the expectation is met.  A call to this function will not return if
 * XT1 cannot be stabilized. */
unsigned long ulBSP430ucsConfigure_ni (unsigned long ulFrequency_Hz,
									short sRSEL);

/** Call this to configure ACLK via the UCS peripheral.
 *
 * Prior to invoking this, use #iBSP430clockConfigureXT1_ni to check for
 * crystal stability, if ACLK is to be sourced from XT1.
 * 
 * @param sela The constant to assign to the SELA field of UCSCTL4.
 * Standard values are @c SELA__XT1CLK and @c SELA__VLOCLK.
 *
 * @return Zero if the configuration was successful; -1 if the value
 * for @a sela was not valid.
 */
int iBSP430ucsConfigureACLK_ni (unsigned int sela);

/** Call this periodically to trim the FLL.
 *
 * The function uses the ratio of SMCLK to ACLK to determine the speed
 * of SMCLK, and if it is "too far" from the value specified in the
 * last call to ulBSP430ucsConfigure_ni() enables the FLL for a short
 * period to see if accuracy can be improved.  See discussion at
 * #BSP430_CLOCK_DISABLE_FLL.
 *
 * For this function to be provided a timer must be identified by
 * #BSP430_UCS_TRIMFLL_TIMER_HAL_HANDLE.  The specified timer will be
 * reconfigured in various ways while trimming occurs, and will be
 * left disabled on exit.  It may be used for other purposes when this
 * function is not being invoked.
 *
 * @warning MCLK, SMCLK, and any clocks derived from them are unstable
 * while this routine is being run, so UART, SPI, and other
 * peripherals should be turned off prior to invoking it.
 * 
 * @note This function is named in accordance with the FreeRTOS
 * standards that indicate it should be called with interrupts
 * disabled and will not block or induce a context switch.  It will,
 * however, delay for as much as 32 milliseconds while waiting for the
 * FLL to settle.  The common case of delay is much less, but it would
 * still be wise not to invoke this from within an interrupt handler.
 *
 * @return an estimate of the actual running frequency.
 */
unsigned long ulBSP430ucsTrimFLL_ni ();

/** @def BSP430_UCS_TRIMFLL_TIMER_PERIPH_HANDLE
 *
 * #ulBSP430ucsTrimFLL_ni requires a timer that can be used while
 * the FLL is being trimmed.  The timer must have a capture/compare
 * block which can be configured to use ACLK as its input.  CCI0B on
 * TB0.6 is a candidate for at least some 5xx/6xx family MCUs.
 * 
 * The value of this parameter should be a reference to one of the
 * timer HPL handles, such as #xBSP43periph_TB0.  The corresponding
 * #configBSP430_PERIPH_TB0 must also be set.
 * 
 * @note The timer may be shared among other users.  It is the
 * caller's responsibility to ensure that no other users of the timer
 * are active while the clock is being trimmed.
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_UCS_TRIMFLL_TIMER_PERIPH_HANDLE no default value
#endif /* BSP430_DOXYGEN */

/** @def BSP430_UCS_TRIMFLL_TIMER_ACLK_CC_INDEX
 *
 * The index of a capture/compare block within
 * #BSP430_UCS_TRIMFLL_TIMER_PERIPH_HANDLE that can take input from
 * ACLK.  The capability is MCU-specific.
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_UCS_TRIMFLL_TIMER_ACLK_CC_INDEX no default value
#endif /* BSP430_DOXYGEN */

/** @def BSP430_UCS_TRIMFLL_TIMER_ACLK_CCIS
 *
 * The value to be written to the CCIS field of the control register
 * for capture/compare block #BSP430_UCS_TRIMFLL_TIMER_ACLK_CC_INDEX
 * of timer #BSP430_UCS_TRIMFLL_TIMER_PERIPH_HANDLE in order to select
 * ACLK as the input source.  The capability is MCU-specific, but
 * consider CCI0B (CCIS_1) of TB0.6.
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_UCS_TRIMFLL_TIMER_ACLK_CCIS no default value
#endif /* BSP430_DOXYGEN */

#endif /* BSP430_CLOCKS_UCS_H */

