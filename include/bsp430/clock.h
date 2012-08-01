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
 * @note Some modules (such as BC2) use LFXT1 to denote the source for
 * ACLK regardless of whether that is the external crystal or VLOCLK.
 * Throughout BSP430 the term LFXT1 is exclusively used to denote an
 * external low-frequency crystal.
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
 * If #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS is true,
 * vBSP430platformInitialize_ni() will use this value to initialize
 * the system clocks.  The value should not be used after this point,
 * as ulBSP430clockMCLK_Hz_ni() is more likely to reflect the actual
 * MCLK rate.
 *
 * @note The default value is calculated from
 * 3*3*5*5*512*69=115200*69, and is the value nearest 8MHz which is
 * also compatible with as many serial baud rates as possible.
 *
 * @note Applications may wish to provide an alternative value suited
 * to specific needs.  It is suggested that the value include the
 * suffix L in case the compiler does not automatically promote
 * constants as necessary.
 *
 * @defaulted  */
#ifndef BSP430_CLOCK_NOMINAL_MCLK_HZ
#define BSP430_CLOCK_NOMINAL_MCLK_HZ 7948800UL
#endif /* BSP430_CLOCK_NOMINAL_MCLK_HZ */

/** @def BSP430_CLOCK_US_TO_NOMINAL_MCLK
 *
 * A function macro that converts a value specified in microseconds to
 * an approximate duration counted in MCLK cycles.  This is primarily
 * used to provide the parameter to #BSP430_CORE_DELAY_CYCLES when the
 * desired delay is specified by time rather than cycles.
 *
 * @param _delay_us the delay in microseconds.  Expected, but not
 * required, to be a compile-time integer constant compatible with
 * unsigned long. */
#define BSP430_CLOCK_US_TO_NOMINAL_MCLK(_delay_us) (((_delay_us) * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000UL)

/** @def configBSP430_CLOCK_DISABLE_FLL
 *
 * This macro may be defined to a true value to request that #SCG0 ben
 * set after vBSP430platformInitialize_ni() configures the clocks,
 * preventing the FLL from changing the DCO configuration without
 * application intervention.  It may be referenced in other
 * situations, such as leaving low-power mode, to determine whether
 * the bit should remain set.
 *
 * The UCS peripheral has several errata which result in severe clock
 * instabilities when the FLL is allowed to run unmanaged.  These
 * include UCS7 ("DCO drifts when servicing short ISRs when in LPM0 or
 * exiting active from ISRs for short periods of time" and UCS10
 * ("Modulation causes shift in DCO frequency").  The latter is
 * documented in <a href="http://www.ti.com/lit/pdf/SLAA489">UCS10
 * Guidance</a>.  The UCS implementation of #ulBSP430clockTrimFLL_ni()
 * supports the UCS10 workaround.
 *
 * Stability in the presence of UCS7 and UCS10 may be further enhanced
 * by setting this option.  It is made generic in case there are other
 * cases where #SCG0 should left set throughout application execution.
 *
 * @note If the application manipulates the status register directly,
 * the effect of this option may not be preserved.
 *
 * @defaulted  */
#ifndef configBSP430_CLOCK_DISABLE_FLL
#define configBSP430_CLOCK_DISABLE_FLL 0
#endif /* configBSP430_CLOCK_DISABLE_FLL */

/** @def configBSP430_CLOCK_TRIM_FLL
 *
 * Define to a true value to request that ulBSP430clockTrimFLL_ni() be
 * made available.
 *
 * <bsp430/platform/bsp430_config.h> may default this to true based on
 * the available clock peripheral.  In particular, the UCS and UCS_RF
 * modules require this function to be able to set the clock to a
 * non-powerup rate.
 *
 * Because implementation of this function depends on
 * #BSP430_TIMER_CCACLK, setting this option causes
 * <bsp430/platform/bsp430_config.h> to default
 * #configBSP430_TIMER_CCACLK to true.
 *
 * This value represents an application or system request for the
 * feature; availability of the feature must be tested using
 * #BSP430_CLOCK_TRIM_FLL before attempting to invoke the function.
 *
 * @platformdefault
 */
#ifndef configBSP430_CLOCK_TRIM_FLL
#define configBSP430_CLOCK_TRIM_FLL 0
#endif /* configBSP430_CLOCK_TRIM_FLL */

#if (configBSP430_CLOCK_TRIM_FLL - 0) && ! (configBSP430_TIMER_CCACLK - 0)
#warning configBSP430_CLOCK_TRIM_FLL requested without configBSP430_TIMER_CCACLK
#endif /* configBSP430_CLOCK_TRIM_FLL */

/** @def BSP430_CLOCK_TRIM_FLL
 *
 * Defined to a true value if #configBSP430_CLOCK_TRIM_FLL was
 * requested, #BSP430_TIMER_CCACLK is available on the platform, and
 * the underlying clock peripheral provides an implementation of
 * ulBSP430clockTrimFLL_ni().
 *
 * In the absence of this flag, ulBSP430clockTrimFLL_ni() will not be
 * available and must not be referenced.
 *
 * @dependency #configBSP430_CLOCK_TRIM_FLL
 * @platformdefault
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_TRIM_FLL include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN) || (configBSP430_CLOCK_TRIM_FLL - 0)
/** Adjust the FLL as necessary to maintain the last configured MCLK speed
 *
 * This function is most likely to be used if
 * #configBSP430_CLOCK_DISABLE_FLL is set, but may also be used as a
 * subroutine by peripheral-specific implementations of
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
 * @warning This function will affect the stability of MCLK and SMCLK,
 * and may temporarily reconfigure ACLK.  Any peripherals that depend
 * on those clocks should be disabled while the function is executing.
 *
 * @return the value of ulBSP430clockMCLK_Hz_ni() after any trimming,
 * or zero if trimming could not be performed.
 *
 * @dependency #BSP430_CLOCK_TRIM_FLL, #BSP430_TIMER_CCACLK
 */
unsigned long ulBSP430clockTrimFLL_ni ();
#endif /* configBSP430_CLOCK_TRIM_FLL */

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
 * If #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS is true,
 * vBSP430platformInitialize_ni() will invoke to
 * iBSP430clockConfigureSMCLKDividingShift_ni() with this setting to
 * configure the clock divisors after setting the nominal MCLK
 * frequency.
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

/** @def BSP430_CLOCK_NOMINAL_XT1CLK_HZ
 *
 * Nominal rate of the external low-frequency crystal.  This is
 * expected to be a 32 kiHz watch crystal, so that's what the default
 * is.
 * @defaulted */
#ifndef BSP430_CLOCK_NOMINAL_XT1CLK_HZ
#define BSP430_CLOCK_NOMINAL_XT1CLK_HZ 32768U
#endif /* BSP430_CLOCK_NOMINAL_XT1CLK_HZ */

/** @def BSP430_CLOCK_NOMINAL_XT2CLK_HZ
 *
 * Nominal rate of a secondary external clock.  This must be defined
 * externally if #eBSP430clockSEL_XT2CLK is to be used.
 * 
 * @nodefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_NOMINAL_XT2CLK_HZ must be externally provided
#endif /* BSP430_DOXYGEN */

/** @def BSP430_CLOCK_NOMINAL_VLOCLK_HZ
 *
 * Nominal frequency of VLOCLK, in Hz.
 *
 * The value is constant for all platforms using a specific clock
 * peripheral, and is defined in the peripheral header.  It is usually
 * in the range of 10-12 kHz, but is highly sensitive to temperature
 * and voltage.  The actual value may be off by as much as 20%, making
 * the clock relatively useless for anything that requires accuracy
 * (such as serial baud rates).
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_NOMINAL_VLOCLK_HZ platform specific around 10-12 kHz
#endif /* BSP430_DOXYGEN */

/** @def BSP430_CLOCK_PUC_MCLK_HZ
 *
 * Nominal frequency of MCLK at power-up, in Hz.
 *
 * The value is constant for all platforms using a specific clock
 * peripheral, and is defined in the peripheral header.  It is usually
 * around 1 MHz. */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_PUC_MCLK_HZ platform specific around 1 MHz
#endif /* BSP430_DOXYGEN */

/** Constants used for MCU-independent specification of clocks.
 *
 * Use these instead of peripheral-specific values like #SELA_1 or
 * #LFXT1S_2 to identify clock sources by their function.  This also
 * permits finer control within iBSP430clockConfigureACLK_ni(),
 * allowing that to select a secondary source given oscillator faults
 * in the primary source. */
typedef enum eBSP430clockSource {
  /** A marker value indicating that no clock is selected.  Generally
   * this is not an appropriate value to use as a parameter. */
  eBSP430clockSRC_NONE,

  /** XT1 is the primary external clock.  It is generally a
   * low-frequency watch crystal running at 32 kiHz.  The nominal
   * speed for this clock is #BSP430_CLOCK_NOMINAL_XT1CLK_HZ. */
  eBSP430clockSRC_XT1CLK,

  /** VLOCLK is a low-speed internal clock, normally around 10-12 kHz.
   * It is relatively imprecise.  The nominal speed for this clock is
   * #BSP430_CLOCK_NOMINAL_VLOCLK_HZ.  */
  eBSP430clockSRC_VLOCLK,

  /** REFOCLK is an internally low power trimmed 32 kiHz oscillator
   * supported in the UCS peripheral */
  eBSP430clockSRC_REFOCLK,

  /** DCOCLK is whatever the DCO clock frequency is.  It's probably
   * related to #BSP430_CLOCK_NOMINAL_MCLK_HZ, but may be a multiple
   * of that. */
  eBSP430clockSRC_DCOCLK,

  /** DCOCLKDIV is a divided DCO clock.  It's probably
   * #BSP430_CLOCK_NOMINAL_MCLK_HZ, but may be a multiple of that. */
  eBSP430clockSRC_DCOCLKDIV,

  /** A secondary external clock.  This is only recognized if
   * #BSP430_CLOCK_NOMINAL_XT2CLK_HZ has been provided. */
  eBSP430clockSRC_XT2CLK,

  /** Fallback: use XT1CLK if #BSP430_CLOCK_LFXT1_IS_FAULTED() is
   * false, otherwise use VLOCLK.
   *
   * The main value of this is in BC2-based clocks, where selecting
   * XT1CLK as the source for ACLK will cause ACLK to fall back to
   * VLOCLK if the crystal is faulted, but using the resulting ACLK as
   * a timer source will not work. */
  eBSP430clockSRC_XT1CLK_OR_VLOCLK,
  
  /** Fallback: use XT1CLK if #BSP430_CLOCK_LFXT1_IS_FAULTED() is
   * false, otherwise use REFOCLK.
   *
   * Similar to #eBSP430clockSRC_XT1CLK_OR_VLOCLK for UCS-based
   * systems. */
  eBSP430clockSRC_XT1CLK_OR_REFOCLK,
  
} eBSP430clockSource;

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
 * @note This function will hang until any oscillator fault associated
 * with the DCO or its peripheral-specific analogue has been cleared.
 * If the clock configuration is not stable, this function may not
 * return.
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
 * crystal function: this reconfigures pins to their digital port
 * function, disables the crystal, and clears any crystal capacitance
 * setting.
 *
 * @param loop_limit The number of times the stabilization check
 * should be repeated.  If stabilization has not been achieved after
 * this many loops, assume the crystal is absent and configure for
 * VLOCLK.  A negative value indicates the process should loop until
 * stabilization is detected.  A zero value is equivalent to passing a
 * zero value as @a enablep..
 *
 * @return Zero if XT1 was disabled by the call, and a positive value
 * if XT1 is stable on completion of the call (available as a clock
 * source).  A negative value indicates an error, such as inability to
 * configure XIN/XOUT pins.  Thus, the crystal is available and stable
 * only if a positive value is returned.
 */
int iBSP430clockConfigureLFXT1_ni (int enablep,
                                   int loop_limit);

/** Configure ACLK to a source clock.
 *
 * The peripheral-specific implementation will configure ACLK to
 * source from the requested clock.
 *
 * @param sel the source from which the clock should be selected.
 * Note that the values permit an internal decision, e.g. to prefer
 * LFXT1 but to use an alternative if that is faulted.  Configuration
 * is rejected if the requested clock source does not exist on the
 * platform.
 *
 * @return 0 if the configuration was accepted, a negative error if it
 * was rejected.  
 */
int iBSP430clockConfigureACLK_ni (eBSP430clockSource sel);

/** Return the best available estimate of ACLK frequency.
 *
 * Depending on clock configuration, this will return one of
 * #BSP430_CLOCK_NOMINAL_XT1CLK_HZ, #BSP430_CLOCK_NOMINAL_VLOCLK_HZ, or
 * another constant or possibly measured value.
 *
 * A value of 0 may be returned if the clock is configured for an
 * unidentified external source.
 *
 * @note If the ACLK is configured to source from LFXT1 but
 * #BSP430_CLOCK_LFXT1_IS_FAULTED() is true this will return
 * #BSP430_CLOCK_NOMINAL_VLOCLK_HZ instead of
 * #BSP430_CLOCK_NOMINAL_XT1CLK_HZ.
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

#endif /* BSP430_CLOCK_H */
