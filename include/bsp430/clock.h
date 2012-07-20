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
 * @brief Clock-related functions implemented on all MSP430 MCUs.
 *
 * The actual implementation of the functions are within the
 * peripheral-specific clock modules.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_CLOCK_H
#define BSP430_CLOCK_H

#include <bsp430/common.h>

/** @def configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT
 *
 * SMCLK is normally configured to divide another clock by shifting it
 * left this many positions.  E.g., if MCLK is 20 MHz, a dividing shift
 * of 2 will produce a clock divisor of 4 and an SMCLK at 5 MHz.
 *
 * @note The value of define reflects the divisor of MCLK.  If the
 * clock source for SMCLK is different than an undivided MCLK, the
 * code that configures the clock may need to modify the shift
 * value. */
#ifndef configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT
#define configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT 0
#endif /* configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT */

/** @def configBSP430_CLOCK_XT1_STABILIZATION_DELAY_CYCLES
 *
 * Define this to the number of MCLK cycles that
 * #iBSP430clockConfigureXT1 should delay, after clearing oscillator
 * faults, before checking for oscillator stability.  This must be a
 * compile-time constant integer compatible with <tt>unsigned
 * long</tt>.  If this value is too short, #iBSP430clockConfigureXT1
 * may prematurely decide that the crystal is working; if it is too
 * long, the return from #iBSP430clockConfigureXT1 is delayed.
 *
 * @note The value depends on the MCLK frequency at the time
 * #iBSP430clockConfigureXT1 is invoked.  It is suggested that the
 * crystal be checked and ACLK configured prior to configuring MCLK
 * for the application, when the MCU power-up MCLK frequency of 1 MHz
 * is in effect.  If this is done, the nominal value for this option
 * should delay roughly 100msec, which should be long enough to detect
 * an unstable crystal.  Repeated checks are required to detect a
 * stable crystal, so to delay for up to one second use:
 *
 * @code
 * if (0 >= iBSP430clockConfigureXT1(1, 1000000L / configBSP430_CLOCK_XT1_STABILIZATION_DELAY_CYCLES)) {
 *    // XT1 not available
 * }
 * @endcode
 *
 * Crystal stabilization can take hundreds of milliseconds; on some
 * platforms the one second delay above is insufficient.
 */
#ifndef configBSP430_CLOCK_XT1_STABILIZATION_DELAY_CYCLES
#define configBSP430_CLOCK_XT1_STABILIZATION_DELAY_CYCLES 100000UL
#endif /* configBSP430_CLOCK_XT1_STABILIZATION_DELAY_CYCLES */


/** Return the best available estimate of MCLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #configCPU_CLOCK_HZ, or it may return a value calculated from
 * observations.
 *
 * @return an estimate of the MCLK frequency, in Hz */
unsigned long ulBSP430clockMCLK_Hz ();

/** Return the best available estimate of SMCLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #configCPU_CLOCK_HZ >> #configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT, or
 * it may return a value calculated from observations.
 *
 * @return an estimate of the SMCLK frequency, in Hz */
unsigned long ulBSP430clockSMCLK_Hz ();

/** Return the best available estimate of ACLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #portACLK_FREQUENCY_HZ, or it may return a value calculated from
 * observations.
 *
 * @return an estimate of the ACLK frequency, in Hz */
unsigned short usBSP430clockACLK_Hz ();

/** Configure (or deconfigure) XT1 as a clock source.
 *
 * The peripheral-specific implementation will use
 * #iBSP430platformConfigurePeripheralPinsFromISR with #BSP430_PERIPH_XT1 to
 * configure the crystal.  If crystal functionality has been
 * requested, it then clears oscillator faults, delays
 * #configBSP430_CLOCK_XT1_STABILIZATION_DELAY_CYCLES, then detects
 * whether the crystal is functioning.  It terminates with success
 * once the oscillator remains unfaulted after the delay, and
 * otherwise repeats the clear/delay/check process as specified by @a
 * loop_limit.
 * 
 * @param enablep Pass a nonzero value to configure XIN/XOUT for
 * crystal functionality and to loop until the crystal is stabilized
 * or has failed to stabilize.  Pass a zero value to turn off the
 * crystal function.
 *
 * @param loop_limit The number of times the stabilization check
 * should be repeated.  If stabilization has not been achieved after
 * this many loops, assume the crystal is absent and configure for
 * VLOCLK.  A negative value indicates the process should loop until
 * stabilization is detected.  The parameter is ignored if @a enablep
 * is zero.
 *
 * @return Zero if XT1 was disabled by the call, and a positive value
 * if XT1 is stable on completion of the call (available as a clock
 * source).  A negative value indicates an error, such as inability to
 * configure XIN/XOUT pins.
 */
int iBSP430clockConfigureXT1 (int enablep,
							  int loop_limit);

#endif /* BSP430_CLOCK_H */
