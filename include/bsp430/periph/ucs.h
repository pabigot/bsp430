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
 * The UCS peripheral is fairly powerful, supporting arbitrary MCLK
 * frequencies with or without an external crystal.  The interface
 * here enforces the following characteristics; see the UCS module
 * description in the <a
 * href="http://www.ti.com/general/docs/lit/getliterature.tsp?baseLiteratureNumber=SLAU208&track=no">MSP430
 * 5xx/6xx Family Users Guide</a> for further details:
 * <ul>
 * 
 * <li>The FLL reference clock may be LFXT1 or REFO; see
 * #configBSP430_UCS_FLLREFCLK_IS_XT1CLK.  FLLREFDIV is /1.
 *
 * <li>MCLK and SMCLK are sourced from DCOCLKDIV
 *
 * <li>DCOCLKDIV is DCOCLK/2 (the power-up value)
 *
 * <li>ACLK may source from VLOCLK, LFXT1CLK, or REFOCLK
 *
 * <li>FLL trimming will leave the FLL disabled if it was disabled on
 * entry; see #BSP430_CLOCK_DISABLE_FLL.
 *
 * <li>Executing an FLL trim (either directly or due to having invoked
 * ulBSP430clockConfigureMCLK_ni()) will record in private state
 * the measured clock frequency.
 *
 * <li> ulBSP430clockMCLK_Hz_ni() and ulBSP430clockSMCLK_Hz_ni()
 * return values are calculated by applying the relevant clock
 * dividers as read from the UCS registers to the recorded measured
 * clock frequency.
 *
 * </ul> 
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

/** @def configBSP430_UCS_FLLREFCLK_IS_XT1CLK
 *
 * The UCS module supports a variety of potential sources for the FLL
 * reference clock; the implementation here supports either LFXT1 or
 * REFO.
 *
 * Define this to true if you want to use LFXT1; otherwise REFO is
 * used.
 *
 * @warning If this is selected, FLL trim will hang if LFXT1 is
 * faulted and cannot be cleared. */
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
 * faulted and cannot be cleared. */
#ifndef configBSP430_UCS_TRIM_ACLK_IS_XT1CLK
#define configBSP430_UCS_TRIM_ACLK_IS_XT1CLK 0
#endif /* configBSP430_UCS_TRIM_ACLK_IS_XT1CLK */

#undef BSP430_CLOCK_LFXT1_IS_FAULTED
/** Check whether the LFXT1 crystal has a fault condition.
 *
 * This definition overrides the generic definition to test the
 * crystal-specific flags.  Note that if somebody has turned off the
 * crystal by setting UCSCTL6.XT1OFF, the crystal is assumed to be
 * faulted.*/
#define BSP430_CLOCK_LFXT1_IS_FAULTED() ((UCSCTL6 & XT1OFF) || (UCSCTL7 & XT1LFOFFG))

#undef BSP430_CLOCK_LFXT1_CLEAR_FAULT
/** Clear the fault associated with LFXT1.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag. */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do {	\
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

/** Call this to initially configure the UCS peripheral.
 *
 * @param mclk_Hz The target frequency for DCOCLKDIV=MCLK.
 *
 * @param rsel The DCO frequency range selection.  The appropriate
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
 * #iBSP430clockConfigureLFXT1_ni and #iBSP430ucsConfigureACLK_ni to ensure
 * the expectation is met.  A call to this function will not return if
 * XT1 cannot be stabilized. */
unsigned long ulBSP430ucsConfigure_ni (unsigned long mclk_Hz,
                                       int rsel);

/** Call this periodically to trim the FLL.
 *
 * The function uses the ratio of SMCLK to ACLK to determine the speed
 * of SMCLK, and if it is "too far" from the value specified in the
 * last call to ulBSP430ucsConfigure_ni() enables the FLL for a short
 * period to see if accuracy can be improved.  See discussion at
 * #BSP430_CLOCK_DISABLE_FLL.
 *
 * For this function to be provided a timer must be identified by
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE.  The specified timer will be
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
unsigned long ulBSP430ucsTrimFLL_ni (void);

#endif /* BSP430_CLOCKS_UCS_H */

