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
 * @note Where LFXT1 is used in this module, it is assumed to refer to
 * an external 32 kiHz crystal.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_CLOCK_H
#define BSP430_CLOCK_H

#include <bsp430/common.h>

/** @def BSP430_CLOCK_NOMINAL_MCLK_HZ
 *
 * A constant representing the desired clock speed of the master
 * clock.  Platform-specific configuration will use this value to
 * initialize the system clocks, and it may affect the return value
 * for functions like #ulBSP430clockMCLK_Hz_ni and
 * #ulBSP430clockSMCLK_Hz_ni.
 *
 * @note The default value is calculated from
 * 3*3*5*5*512*69=115200*69, and is the value nearest 8MHz which is
 * also compatible with as many serial baud rates as possible.
 *
 * @note Applications may wish to provide an alternative value better
 * suited to specific needs. */
#ifndef BSP430_CLOCK_NOMINAL_MCLK_HZ
#define BSP430_CLOCK_NOMINAL_MCLK_HZ 7948800U
#endif /* BSP430_CLOCK_NOMINAL_MCLK_HZ */

/** @def BSP430_CLOCK_SMCLK_DIVIDING_SHIFT
 *
 * SMCLK is normally configured to divide another clock by shifting it
 * left this many positions.  E.g., if MCLK is 20 MHz, a dividing shift
 * of 2 will produce a clock divisor of 4 and an SMCLK at 5 MHz.
 *
 * @note The value of define reflects the divisor of MCLK.  If the
 * clock source for SMCLK is different than an undivided MCLK, the
 * code that configures the clock may need to modify the shift
 * value. */
#ifndef BSP430_CLOCK_SMCLK_DIVIDING_SHIFT
#define BSP430_CLOCK_SMCLK_DIVIDING_SHIFT 0
#endif /* BSP430_CLOCK_SMCLK_DIVIDING_SHIFT */

/** @def BSP430_CLOCK_DISABLE_FLL
 *
 * The UCS peripheral has several errata which result in severe clock
 * instabilities when the FLL is allowed to run unmanaged.  These
 * include UCS7 ("DCO drifts when servicing short ISRs when in LPM0 or
 * exiting active from ISRs for short periods of time" and UCS10
 * ("Modulation causes shift in DCO frequency").  The latter is
 * documented in <a href="http://www.ti.com/lit/pdf/SLAA489">UCS10
 * Guidance</a>.  The BSP430 function #ulBSP430ucsTrimFLL_ni function
 * is provided to support UCS10 workaround.  To further reduce the
 * likelihood either erratum affects clock stability, this macro may
 * be defined to a true value to request that BSP430 ensure that #SCG0
 * remain set, preventing the FLL from changing the DCO configuration
 * without application intervention. */
#ifndef BSP430_CLOCK_DISABLE_FLL
#define BSP430_CLOCK_DISABLE_FLL 0
#endif /* BSP430_CLOCK_DISABLE_FLL */

/** @def BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES
 *
 * Define this to the number of MCLK cycles that
 * #iBSP430clockConfigureXT1_ni should delay, after clearing oscillator
 * faults, before checking for oscillator stability.  This must be a
 * compile-time constant integer compatible with <tt>unsigned
 * long</tt>.  If this value is too short, #iBSP430clockConfigureXT1_ni
 * may prematurely decide that the crystal is working; if it is too
 * long, the return from #iBSP430clockConfigureXT1_ni is delayed.
 *
 * @note The value is in MCLK ticks, so depends on the MCLK frequency
 * at the time #iBSP430clockConfigureXT1_ni is invoked.  It is suggested
 * that the crystal be checked and ACLK configured prior to
 * configuring MCLK for the application, when the MCU power-up MCLK
 * frequency of 1 MHz is in effect.  If this is done, the nominal
 * value for this option should delay roughly 100msec, which should be
 * long enough to detect an unstable crystal.  Repeated checks are
 * required to detect a stable crystal, so to delay for up to one
 * second use:
 *
 * @code
 * if (0 >= iBSP430clockConfigureXT1_ni(1, 1000000L / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES)) {
 *    // XT1 not available
 * }
 * @endcode
 *
 * Crystal stabilization can take hundreds of milliseconds; on some
 * platforms even the one second delay above is insufficient.
 */
#ifndef BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES
#define BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES 100000UL
#endif /* BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES */

/** Return the best available estimate of MCLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #BSP430_CLOCK_NOMINAL_MCLK_HZ, or it may return a value calculated
 * from observations.
 *
 * @return an estimate of the MCLK frequency, in Hz */
unsigned long ulBSP430clockMCLK_Hz_ni ();

/** Interruptible-preserving wrapper for #ulBSP430clockMCLK_Hz_ni */
static unsigned long
__inline__
ulBSP430clockMCLK_Hz ()
{
	unsigned long rv;
	BSP430_CORE_INTERRUPT_STATE_T istate;
	BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
	BSP430_CORE_DISABLE_INTERRUPT();
	rv = ulBSP430clockMCLK_Hz_ni();
	BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
	return rv;
}

/** Return the best available estimate of SMCLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #BSP430_CLOCK_NOMINAL_MCLK_HZ >> #BSP430_CLOCK_SMCLK_DIVIDING_SHIFT, or
 * it may return a value calculated from observations.
 *
 * @return an estimate of the SMCLK frequency, in Hz */
unsigned long ulBSP430clockSMCLK_Hz_ni ();

/** Interruptible-preserving wrapper for #ulBSP430clockSMCLK_Hz_ni */
static unsigned long
__inline__
ulBSP430clockSMCLK_Hz ()
{
	unsigned long rv;
	BSP430_CORE_INTERRUPT_STATE_T istate;
	BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
	BSP430_CORE_DISABLE_INTERRUPT();
	rv = ulBSP430clockSMCLK_Hz_ni();
	BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
	return rv;
}

/** Return the best available estimate of ACLK frequency.
 *
 * Depending on clock capabilities, this may simply return the
 * peripheral-specific value of #BSP430_CLOCK_NOMINAL_ACLK_HZ, or it
 * may return a value calculated from observations.
 *
 * @note When considering whether to use this function or the macro
 * #BSP430_CLOCK_NOMINAL_ACLK_HZ, recall that the generic definition
 * of the macro will defer to #BSP430_CLOCK_NOMINAL_VLOCLK_HZ if there
 * is an oscillator fault in the system, while this function will
 * check whether ACLK is sourced from LFXT1, and if so whether there
 * is a fault specific to that oscillator.  The two forms (function
 * versus macro) may yield different results.
 *
 * @return an estimate of the ACLK frequency, in Hz */
unsigned short usBSP430clockACLK_Hz_ni ();

/** Interruptible-preserving wrapper for #ulBSP430clockACLK_Hz_ni */
static unsigned short
__inline__
usBSP430clockACLK_Hz ()
{
	unsigned short rv;
	BSP430_CORE_INTERRUPT_STATE_T istate;
	BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
	BSP430_CORE_DISABLE_INTERRUPT();
	rv = usBSP430clockACLK_Hz_ni();
	BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
	return rv;
}

/** Check whether the LFXT1 crystal has a fault condition.
 *
 * The generic implementation looks for any oscillator fault by
 * checking the system-wide register.  It is overridden in
 * peripheral-specific clock headers where an ability exists to check
 * specifically for a LFXT1 fault.  The value is nonzero iff a fault
 * has been detected.
 *
 * @note This function macro is implicitly FromISR, and should be
 * implemented where possible as a single instruction test.
 *
 * @note Where the test is overridden to check peripheral-specific
 * flags, the state of the system oscillator fault bit is not
 * reflected in the value.
 *
 * @see #BSP430_CLOCK_LFXT1_CLEAR_FAULT() */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define BSP430_CLOCK_LFXT1_IS_FAULTED() (SFRIFG1 & OFIFG)
#else /* 5xx */
#define BSP430_CLOCK_LFXT1_IS_FAULTED() (IFG1 & OFIFG)
#endif /* 5xx */

/** Clear the fault associated with LFXT1.
 *
 * This clears the state bits associated with a fault in the LFXT1
 * crystal.  If the crystal still exhibits a fault condition, the bits
 * will be set again.
 *
 * @note This function macro is implicitly FromISR: should be called
 * with interrupts disabled and will not induce a task switch.
 *
 * @note Where faults can be cleared on peripheral-specific registers,
 * the system oscillator fault is also cleared.  This is in contrast
 * to what is tested by #BSP430_CLOCK_LFXT1_IS_FAULTED(). 
 *
 * @see #BSP430_CLOCK_LFXT1_IS_FAULTED() */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do { SFRIFG1 &= ~OFIFG; } while (0)
#else /* 5xx */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do { IFG1 &= ~OFIFG; } while (0)
#endif /* 5xx */


/** @def BSP430_CLOCK_NOMINAL_VLOCLK_HZ
 * 
 * Nominal frequency of VLOCLK, in Hz.
 *
 * This is a family-specific value normally somewhere near 10-12 kHz.
 * The value should be an unsigned integer constant.
 * 
 * @note The value of this clock is often off by as much as 10%. */
#ifndef BSP430_CLOCK_NOMINAL_VLOCLK_HZ
#if defined(__MSP430_HAS_BC2__)					\
	|| defined(__MSP430_HAS_FLLPLUS__)			\
	|| defined(__MSP430_HAS_FLLPLUS_SMALL__)
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 12000U
#elif defined(__MSP430_HAS_CS__)				\
	|| defined(__MSP430_HAS_UCS__) 				\
	|| defined(__MSP430_HAS_UCS_RF__)
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 10000U
#else /* clock system */
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ 12000U
#endif /* clock system*/
#endif /* BSP430_CLOCK_NOMINAL_VLOCLK_HZ */

/** @def BSP430_CLOCK_NOMINAL_ACLK_HZ
 *
 * Nominal frequency of ACLK, in Hz.
 *
 * This value should be quickly calculatable, and be either a
 * compile-time constant or a selection between two compile-time
 * constants based on a single-instruction test.
 *
 * Note that the calculated value of this assumes that ACLK is
 * configured to source from a 32 kiHz watch crystal normally, but
 * deferring to VLOCLK if LFXT1 is faulted. */
#ifndef BSP430_CLOCK_NOMINAL_ACLK_HZ
#define BSP430_CLOCK_NOMINAL_ACLK_HZ (BSP430_CLOCK_LFXT1_IS_FAULTED() ? BSP430_CLOCK_NOMINAL_VLOCLK_HZ : 32768U)
#endif /* BSP430_CLOCK_NOMINAL_ACLK_HZ */

/** Configure (or deconfigure) XT1 as a clock source.
 *
 * The peripheral-specific implementation will use
 * #iBSP430platformConfigurePeripheralPins_ni with #BSP430_PERIPH_XT1 to
 * configure the crystal.  If crystal functionality has been
 * requested, it then clears oscillator faults, delays
 * #BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES, then detects
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
int iBSP430clockConfigureXT1_ni (int enablep,
								 int loop_limit);

#endif /* BSP430_CLOCK_H */
