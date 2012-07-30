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
 * This module declares functions and macros of general use in
 * managing MSP430 clocks.  The interface here is independent of the
 * underlying clock peripheral, but in some cases behavior is refined
 * or clarified in the peripheral-specific header.
 *
 * This module will include the peripheral-specific header if a
 * functional presence preprocessor symbol is available that is
 * recognized as a supported clock peripheral.  Recognized peripherals
 * and the corresponding BSP430 header are:
 * <ul>
 * <li> <bsp430/periph/bc2.h> for BC2 (Basic Clock Module+)
 * <li> <bsp430/periph/fllplus.h> for FLLPLUS and FLLPLUS_SMALL (FLL Plus)
 * <li> <bsp430/periph/ucs.h> for UCS and UCS_RF (Unified Clock System)
 * <li> <bsp430/periph/cs.h> for CS (Clock System)
 * </ul>
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
 * clock.
 *
 * If this macro is defined to a nonzero value,
 * vBSP430platformInitialize_ni() will use it to initialize the system
 * clocks.  If it is defined as zero, the power-up clock configuration
 * will be left unchanged.
 *
 * @note The default value is calculated from
 * 3*3*5*5*512*69=115200*69, and is the value nearest 8MHz which is
 * also compatible with as many serial baud rates as possible.
 *
 * @note Applications may wish to provide an alternative value better
 * suited to specific needs.
 *
 * @defaulted  */
#ifndef BSP430_CLOCK_NOMINAL_MCLK_HZ
#define BSP430_CLOCK_NOMINAL_MCLK_HZ 7948800U
#endif /* BSP430_CLOCK_NOMINAL_MCLK_HZ */


/** @def BSP430_CLOCK_DISABLE_FLL
 *
 * This macro may be defined to a true value to request that BSP430
 * attempt to ensure #SCG0 remain set, preventing the FLL from
 * changing the DCO configuration without application intervention.
 * Its primary effect is in the selection of bits cleared when leaving
 * interrupts.
 *
 * The UCS peripheral has several errata which result in severe clock
 * instabilities when the FLL is allowed to run unmanaged.  These
 * include UCS7 ("DCO drifts when servicing short ISRs when in LPM0 or
 * exiting active from ISRs for short periods of time" and UCS10
 * ("Modulation causes shift in DCO frequency").  The latter is
 * documented in <a href="http://www.ti.com/lit/pdf/SLAA489">UCS10
 * Guidance</a>.  The BSP430 function #ulBSP430ucsTrimFLL_ni function
 * is provided to support UCS10 workaround.
 *
 * Stability in the presence of UCS7 and UCS10 may be further enhanced
 * by setting this option.  It is made generic in case there are other
 * cases where #SCG0 should not be cleared on wakeup.
 *
 * @defaulted  */
#ifndef BSP430_CLOCK_DISABLE_FLL
#define BSP430_CLOCK_DISABLE_FLL 0
#endif /* BSP430_CLOCK_DISABLE_FLL */

/** @def BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT
 *
 * SMCLK is normally configured to divide another clock by shifting it
 * left this many positions.  E.g., if MCLK is 20 MHz, a dividing
 * shift of 2 will produce a clock divisor of 4 and an SMCLK at 5 MHz.
 * The range of division and default divisor may be MCU-specific.  The
 * value reflects the divisor of MCLK.  If the clock source for SMCLK
 * is different than an undivided MCLK, the code that configures the
 * clock may modify the shift value that is stored in the peripheral
 * registers.
 *
 * If this macro is defined to a non-negative value,
 * vBSP430platformInitialize_ni() will pass it to
 * iBSP430clockConfigureSMCLKDividingShift_ni() to configure the clock
 * divisors after setting the nominal MCLK frequency.
 *
 * @note iBSP430clockSMCLKDividingShift_ni() should always be used in
 * preference to this constant to determine the current relative
 * MCLK/SMCLK frequencies.
 *
 * @defaulted  */
#ifndef BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT
#define BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT 0
#endif /* BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT */

/** @def BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES
 *
 * Define this to the number of MCLK cycles that
 * #iBSP430clockConfigureLFXT1_ni should delay, after clearing
 * oscillator faults, before checking for oscillator stability.  This
 * must be a compile-time constant integer compatible with
 * <tt>unsigned long</tt>.
 *
 * Crystal stabilization can take hundreds of milliseconds.  If this
 * value is too short, #iBSP430clockConfigureLFXT1_ni may prematurely
 * decide that the crystal is working; if it is too long, the return
 * from #iBSP430clockConfigureLFXT1_ni is delayed.
 *
 * The default value is chosen to reflect a 20msec delay at the PUC
 * MCLK frequency of roughly 1MHz.  This allows
 * #BSP430_CORE_WATCHDOG_CLEAR() to be invoked within the loop to
 * prevent a watchdog reset while waiting for stabilization.  It does
 * assume that an unstable crystal will indicate a fault within
 * 10msec, which may not be true.
 *
 * @defaulted
 */
#ifndef BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES
#define BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES (20000UL)
#endif /* BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES */

/** Check whether the LFXT1 crystal has a fault condition.
 *
 * The generic implementation looks for any oscillator fault by
 * checking the system-wide register.  It is overridden in
 * peripheral-specific clock headers where an ability exists to check
 * specifically for a LFXT1 fault.  The value is nonzero iff an
 * external crystal is the source for LFXT1 and a fault has been
 * detected.
 *
 * @note This function macro is implicitly not to be interrupted, and
 * should be implemented where possible as a single instruction test.
 *
 * @note This test is intended specifically to validate an external
 * watch crystal running at 32 kiHz.  Where LFXT1 is sourced from
 * other means, it is assumed that the (missing) crystal is faulted
 * even if the peripheral-specific bit suggests it is not.  Similarly,
 * the state of the system oscillator fault bit is not reflected in
 * the value for a peripheral-specific check.
 *
 * @see #BSP430_CLOCK_LFXT1_CLEAR_FAULT()
 *
 * @defaulted
 * @overridable */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define BSP430_CLOCK_LFXT1_IS_FAULTED() (SFRIFG1 & OFIFG)
#else /* 5xx */
#define BSP430_CLOCK_LFXT1_IS_FAULTED() (IFG1 & OFIFG)
#endif /* 5xx */

/** Clear the fault associated with LFXT1.
 *
 * This clears the state bits associated with a fault in the LFXT1
 * crystal.  If the crystal still exhibits a fault condition, the bits
 * will be set again automatically.
 *
 * @note This function macro is implicitly FromISR: should be called
 * with interrupts disabled and will not induce a task switch.
 *
 * @note Where faults can be cleared on peripheral-specific registers,
 * the system oscillator fault is also cleared.  This is in contrast
 * to what is tested by #BSP430_CLOCK_LFXT1_IS_FAULTED().
 *
 * @see #BSP430_CLOCK_LFXT1_IS_FAULTED()
 *
 * @defaulted
 * @overridable */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do { SFRIFG1 &= ~OFIFG; } while (0)
#else /* 5xx */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do { IFG1 &= ~OFIFG; } while (0)
#endif /* 5xx */


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
 * deferring to VLOCLK if LFXT1 is faulted.
 *
 * @defaulted  */
#ifndef BSP430_CLOCK_NOMINAL_ACLK_HZ
#define BSP430_CLOCK_NOMINAL_ACLK_HZ (BSP430_CLOCK_LFXT1_IS_FAULTED() ? BSP430_CLOCK_NOMINAL_VLOCLK_HZ : 32768U)
#endif /* BSP430_CLOCK_NOMINAL_ACLK_HZ */

/** Configure MCLK to a desired frequency.
 *
 * The peripheral-specific implementation will configure MCLK to a
 * frequency as close as possible to the requested frequency.  The
 * actual frequency may be higher or lower than the requested one.
 *
 * @note Although passing @a mclk_Hz zero is a short-hand for using
 * #BSP430_CLOCK_PUC_MCLK_HZ, the result may not be to restore the
 * clock to its power-up configuration.  To avoid manipulating the
 * clocks during platform initialization, see
 * #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS.
 *
 * @param mclk_Hz Desired frequency for the master clock, in Hz.  If
 * zero, #BSP430_CLOCK_PUC_MCLK_HZ is used.
 *
 * @return Configured frequency in Hz.  This may be higher or lower
 * than the requested frequency.
 */
unsigned long ulBSP430clockConfigureMCLK_ni (unsigned long mclk_Hz);

/** Return the best available estimate of MCLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #BSP430_CLOCK_NOMINAL_MCLK_HZ, or it may return a value calculated
 * from observations.
 *
 * @return an estimate of the MCLK frequency, in Hz */
unsigned long ulBSP430clockMCLK_Hz_ni (void);

/** Interruptible-preserving wrapper for #ulBSP430clockMCLK_Hz_ni */
static unsigned long
__inline__
ulBSP430clockMCLK_Hz (void)
{
  unsigned long rv;
  BSP430_CORE_INTERRUPT_STATE_T istate;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = ulBSP430clockMCLK_Hz_ni();
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Configure SMCLK by dividing MCLK.
 *
 * The peripheral clock registers are configured to produce SMCLK from
 * the same source as MCLK at a rate where SMCLK divides MCLK by a
 * specified power of two.  The range of supported shift values is
 * specific to the peripheral.  Where MCLK is itself divided, the
 * underlying implementation will take that into account.
 *
 * See also #BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT.
 *
 * @param shift_pos the number of bit positions by which the incoming
 * clock is divided; e.g. a value of 2 produces /4.
 *
 * @return iBSP430clockSMCLKDividingShift_ni() as evaluated after the
 * configuration is complete */
int iBSP430clockConfigureSMCLKDividingShift_ni (int shift_pos);

/** Return the shift used to divide MCLK to produce SMCLK.
 *
 * This is extracted from the peripheral-specific registers and
 * accounts for any division by MCLK of a shared source clock.
 *
 * @return The power of 2 by which MCLK is divided to produce SMCLK. */
int iBSP430clockSMCLKDividingShift_ni (void);

/** Return the best available estimate of SMCLK frequency.
 *
 * This simply estimates MCLK then divides it based on the
 * peripheral-specific divider.  If clocks are configured through a
 * mechanism other than #ulBSP430clockConfigureMCLK_ni assumptions
 * made by the implementation may be incorrect.
 *
 * @return an estimate of the SMCLK frequency, in Hz */
static unsigned long
__inline__
ulBSP430clockSMCLK_Hz_ni (void)
{
  unsigned long mclk_Hz = ulBSP430clockMCLK_Hz_ni();
  return mclk_Hz >> iBSP430clockSMCLKDividingShift_ni();
}

/** Interruptible-preserving wrapper for #ulBSP430clockSMCLK_Hz_ni */
static unsigned long
__inline__
ulBSP430clockSMCLK_Hz (void)
{
  unsigned long rv;
  BSP430_CORE_INTERRUPT_STATE_T istate;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = ulBSP430clockSMCLK_Hz_ni();
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Configure (or deconfigure) XT1 as a clock source.
 *
 * The peripheral-specific implementation will use
 * #iBSP430platformConfigurePeripheralPins_ni with #BSP430_PERIPH_LFXT1 to
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
 * stabilization is detected.  A zero value indicates the function
 * should assume the crystal is faulted without even checking.  The
 * parameter is ignored if @a enablep is zero.
 *
 * @return Zero if XT1 was disabled by the call, and a positive value
 * if XT1 is stable on completion of the call (available as a clock
 * source).  A negative value indicates an error, such as inability to
 * configure XIN/XOUT pins.
 */
int iBSP430clockConfigureLFXT1_ni (int enablep,
                                   int loop_limit);

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
unsigned short usBSP430clockACLK_Hz_ni (void);

/** Interruptible-preserving wrapper for usBSP430clockACLK_Hz_ni() */
static unsigned short
__inline__
usBSP430clockACLK_Hz (void)
{
  unsigned short rv;
  BSP430_CORE_INTERRUPT_STATE_T istate;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = usBSP430clockACLK_Hz_ni();
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/* Include peripheral-specific header where recognized */
#if defined(__MSP430_HAS_BC2__)
#include <bsp430/periph/bc2.h>
#endif /* __MSP430_HAS_BC2__ */
#if defined(__MSP430_HAS_FLLPLUS__) || defined(__MSP430_HAS_FLLPLUS_SMALL__)
#include <bsp430/periph/fllplus.h>
#endif /* __MSP430_HAS_FLLPLUS__ */
#if defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__)
#include <bsp430/periph/ucs.h>
#endif /* __MSP430_HAS_UCS__ */
#if defined(__MSP430_HAS_CS__) || defined(__MSP430_HAS_CS_A__)
#include <bsp430/periph/cs.h>
#endif /* __MSP430_HAS_CS__ */

/** @def BSP430_CLOCK_NOMINAL_VLOCLK_HZ
 *
 * Nominal frequency of VLOCLK, in Hz.
 *
 * This is a family-specific value normally somewhere near 10-12 kHz.
 * The value should be an unsigned integer constant.
 *
 * @note The value of this clock is often off by as much as 20%.
 *
 * @defaulted */
#ifndef BSP430_CLOCK_NOMINAL_VLOCLK_HZ
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ _BSP430_CLOCK_NOMINAL_VLOCLK_HZ
#endif /* BSP430_CLOCK_NOMINAL_VLOCLK_HZ */

/** @def BSP430_CLOCK_PUC_MCLK_HZ
 *
 * A constant representing the clock speed of the master clock at
 * power-up.
 *
 * @defaulted  */
#ifndef BSP430_CLOCK_PUC_MCLK_HZ
#define BSP430_CLOCK_PUC_MCLK_HZ _BSP430_CLOCK_PUC_MCLK_HZ
#endif /* BSP430_CLOCK_PUC_MCLK_HZ */

#endif /* BSP430_CLOCK_H */
