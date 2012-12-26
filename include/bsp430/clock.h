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
 * <li> <bsp430/periph/cs.h> for CS and CS_A (Clock System)
 * </ul>
 *
 * @note Some modules (such as BC2) use LFXT1 to denote the source for
 * ACLK regardless of whether that is the external crystal or VLOCLK.
 * Throughout BSP430 the term LFXT1 is exclusively used to denote an
 * external low-frequency crystal.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_CLOCK_H
#define BSP430_CLOCK_H

#include <bsp430/core.h>

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
#define BSP430_CLOCK_US_TO_NOMINAL_MCLK(_delay_us) BSP430_CORE_US_TO_TICKS((_delay_us), BSP430_CLOCK_NOMINAL_MCLK_HZ)

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

/** Oscillator capacitor setting for use by
 * #iBSP430clockConfigureLFXT1_ni.
 *
 * Most MSP430 clock systems allow control of internal capacitors for
 * the low-frequency crystal.  The specific setting is
 * platform-dependent; the value for the setting is
 * peripheral-dependent.  A default value is provided in each
 * peripheral header.
 *
 * The value of this macro should be the bits that need to be set in
 * the peripheral-specific register to control capacitance, normally
 * in the form of a header constant such as #XCAP_1 or #XCAP10PF.
 *
 * @platformdefault
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_LFXT1_XCAP include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** Check whether any clock has a fault condition.
 *
 * This checks all available clocks on the system.  Use
 * peripheral-specific checks for specific clocks.
 *
 * @see #BSP430_CLOCK_CLEAR_FAULTS_NI()
 *
 * @defaulted
 * @platformdep */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_IS_FAULTED_NI() peripheral specific
#endif /* BSP430_DOXYGEN */

/** The delay after clearing clock faults before recurrent faults are expected.
 *
 * When a clock fault is cleared, the clock peripheral will
 * automatically set the fault indicator again if the clock is still
 * faulted.  However there is no evidence in the documentation that
 * this is done instantly.  BSP430 infrastructure code will delay this
 * many microseconds after clearing faults before checking to see
 * whether they are still present.
 * 
 * Delay suggested by SLAU144I "2xx Family Users Guide" section
 * 5.2.7.1 "Sourcing MCLK from a Crystal".  This applies to using XT2
 * and for a specific MCU family, but we're guessing it's a sufficient
 * delay to detect faults in other configurations.  For DCO faults
 * it's probably too long.
 * 
 * @defaulted */
#ifndef BSP430_CLOCK_FAULT_RECHECK_DELAY_US
#define BSP430_CLOCK_FAULT_RECHECK_DELAY_US 50
#endif /* BSP430_CLOCK_FAULT_RECHECK_DELAY_US */

/** Clear faults associated with clocks 
 *
 * This clears the state bits associated with fault in all clock
 * systems supported by the peripheral.  If a clock still exhibits a
 * fault condition, the bits will be set again automatically, though
 * perhaps not immediately.
 *
 * @note Faults are cleared on peripheral-specific registers as well
 * as the system oscillator fault flag.
 *
 * @warning If the underlying peripheral requires that its register
 * bank be unlocked for modification, this macro will do so then
 * re-lock them.  This will result in unexpected behavior if the macro
 * is invoked in a context where the registers are already unlocked.
 *
 * @see #BSP430_CLOCK_IS_FAULTED_NI(),
 * #BSP430_CLOCK_FAULT_RECHECK_DELAY_US
 *
 * @defaulted
 * @platformdep */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_CLEAR_FAULTS_NI() peripheral specific
#endif /* BSP430_DOXYGEN */

/** Check whether the system oscillator fault flag is set.
 *
 * This checks the special function register which reflects roll-up
 * oscillator faults throughout the system.
 * 
 * @platformdep */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define BSP430_CLOCK_OSC_IS_FAULTED_NI() (SFRIFG1 & OFIFG)
#else /* 5xx */
#define BSP430_CLOCK_OSC_IS_FAULTED_NI() (IFG1 & OFIFG)
#endif /* 5xx */

/** Clear the system oscillator fault flag.
 *
 * This clears the bit the special function register which reflects
 * roll-up oscillator faults throughout the system.
 * 
 * @platformdep */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define BSP430_CLOCK_OSC_CLEAR_FAULT_NI() do { SFRIFG1 &= ~OFIFG; } while (0)
#else /* 5xx */
#define BSP430_CLOCK_OSC_CLEAR_FAULT_NI() do { IFG1 &= ~OFIFG; } while (0)
#endif /* 5xx */

/** Check whether the LFXT1 crystal has a fault condition.
 *
 * The implementation of this is specific to the clock peripheral.
 *
 * @defaulted
 * @platformdep */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_LFXT1_IS_FAULTED_NI() peripheral specific
#endif /* BSP430_DOXYGEN */

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
 * The default value is chosen to reflect a 50msec delay at the PUC
 * MCLK frequency of roughly 1MHz.  This allows
 * #BSP430_CORE_WATCHDOG_CLEAR() to be invoked within the loop to
 * prevent a watchdog reset while waiting for stabilization.  It does
 * assume that an unstable crystal will indicate a fault within this
 * period, which may not be true.
 *
 * @defaulted
 * @platformdep
 */
#ifndef BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES
#define BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES (BSP430_CLOCK_FAULT_RECHECK_DELAY_US * 1000UL)
#endif /* BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES */

/** Check whether the XT2 crystal has a fault condition.
 *
 * The implementation of this is specific to the clock peripheral.
 *
 * @defaulted
 * @platformdep */
#if defined(BSP430_DOXYGEN)
#define BSP430_CLOCK_XT2_IS_FAULTED_NI() peripheral specific
#endif /* BSP430_DOXYGEN */

/** @def BSP430_CLOCK_NOMINAL_XT1CLK_HZ
 *
 * Nominal rate of the external low-frequency crystal.  This is
 * expected to be a 32 KiHz watch crystal, so that's what the default
 * is.
 * @defaulted */
#ifndef BSP430_CLOCK_NOMINAL_XT1CLK_HZ
#define BSP430_CLOCK_NOMINAL_XT1CLK_HZ 32768U
#endif /* BSP430_CLOCK_NOMINAL_XT1CLK_HZ */

/** @def BSP430_CLOCK_NOMINAL_XT2CLK_HZ
 *
 * Nominal rate of a secondary external clock.  This must be defined
 * externally if #eBSP430clockSRC_XT2CLK is to be used.
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
   * low-frequency watch crystal running at 32 KiHz.  The nominal
   * speed for this clock is #BSP430_CLOCK_NOMINAL_XT1CLK_HZ. */
  eBSP430clockSRC_XT1CLK,

  /** VLOCLK is a low-speed internal clock, normally around 10-12 kHz.
   * It is relatively imprecise.  The nominal speed for this clock is
   * #BSP430_CLOCK_NOMINAL_VLOCLK_HZ.  */
  eBSP430clockSRC_VLOCLK,

  /** REFOCLK is an internally low power trimmed 32 KiHz oscillator
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

  /** Fallback: use XT1CLK if #BSP430_CLOCK_LFXT1_IS_FAULTED_NI() is
   * false, otherwise use VLOCLK.
   *
   * The main value of this is in BC2-based clocks, where selecting
   * XT1CLK as the source for ACLK will cause ACLK to fall back to
   * VLOCLK if the crystal is faulted, but using the resulting ACLK as
   * a timer source will not work. */
  eBSP430clockSRC_XT1CLK_OR_VLOCLK,

  /** Fallback: use XT1CLK if #BSP430_CLOCK_LFXT1_IS_FAULTED_NI() is
   * false, otherwise use REFOCLK.
   *
   * Similar to #eBSP430clockSRC_XT1CLK_OR_VLOCLK for UCS-based
   * systems. */
  eBSP430clockSRC_XT1CLK_OR_REFOCLK,

  /** Fallback: use XT1CLK if #BSP430_CLOCK_LFXT1_IS_FAULTED_NI() is
   * false, otherwise use the most precise available internal
   * reference clock.
   *
   * Generally this will be equivalent to
   * #eBSP430clockSRC_XT1CLK_OR_VLOCLK or
   * #eBSP430clockSRC_XT1CLK_OR_REFOCLK, depending on what clock
   * peripheral is available.  Use ulBSP430clockACLK_Hz_ni() to
   * determine what the actual (nominal) speed is. */
  eBSP430clockSRC_XT1CLK_FALLBACK,

} eBSP430clockSource;

/** Configure MCLK to a desired frequency.
 *
 * The peripheral-specific implementation will configure MCLK to a
 * frequency as close as possible to the requested frequency.  The
 * actual frequency may be higher or lower than the requested one.
 *
 * Peripheral-specific notes:
 * <ul>

 * <li>The <bsp430/periph/bc2.h> implementation (2xx family) is
 * normally limited to the calibrated DCO values recorded within the
 * MCU, which are a subset of 1 MHz, 8 MHz, 12 MHz, and 16 MHz.  The
 * peripheral will initially be configured to the closest available
 * calibrated frequency.  Where a crystal is available
 * #configBSP430_BC2_TRIM_TO_MCLK may be used to allow this routine to
 * adjust the DCO to match the specific frequency as closely as
 * possible.  The invocation of iBSP430bc2TrimToMCLK_ni() that is used
 * to do this is inhibited if ACLK appears to derive from VLOCLK.
 *
 * <li>The <bsp430/periph/fllplus.h> implementation (4xx family)
 * relies on presence of a stable LFXT1, and configures to the
 * requested frequency.
 *
 * <li>The <bsp430/periph/ucs.h> implementation (5xx/6xx family) will
 * set the configuration to the power-up defaults unless
 * #BSP430_UCS_TRIM_DCOCLKDIV is available.  If it is, then any
 * frequency supported by the MCU may be configured.
 *
 * <li>The <bsp430/periph/cs.h> implementation (FR5xx family) will
 * select the closest calibrated frequency supported by the MCU.
 * There is no facility for adjusting this to an arbitrary frequency.
 * </ul>
 *
 * @note Although passing @p mclk_Hz zero is a short-hand for using
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
BSP430_CORE_INLINE
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
BSP430_CORE_INLINE
ulBSP430clockSMCLK_Hz_ni (void)
{
  unsigned long mclk_Hz = ulBSP430clockMCLK_Hz_ni();
  return mclk_Hz >> iBSP430clockSMCLKDividingShift_ni();
}

/** Interruptible-preserving wrapper for #ulBSP430clockSMCLK_Hz_ni */
static unsigned long
BSP430_CORE_INLINE
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
 * otherwise repeats the clear/delay/check process as specified by @p
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
 * zero value as @p enablep.
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
 * #BSP430_CLOCK_LFXT1_IS_FAULTED_NI() is true this will return
 * #BSP430_CLOCK_NOMINAL_VLOCLK_HZ instead of
 * #BSP430_CLOCK_NOMINAL_XT1CLK_HZ.
 *
 * @return an estimate of the ACLK frequency, in Hz */
unsigned long ulBSP430clockACLK_Hz_ni (void);

/** Interruptible-preserving wrapper for uiBSP430clockACLK_Hz_ni() */
static unsigned int
BSP430_CORE_INLINE
ulBSP430clockACLK_Hz (void)
{
  unsigned long rv;
  BSP430_CORE_INTERRUPT_STATE_T istate;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = ulBSP430clockACLK_Hz_ni();
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Return the best available estimate of slow ACLK frequency.
 *
 * Same as ulBSP430clockACLK_Hz_ni() but for use where ACLK is a
 * low-frequency clock.
 *
 * @deprecated: Use ulBSP430clockACLK_Hz_ni() */
static unsigned int
BSP430_CORE_INLINE
uiBSP430clockACLK_Hz_ni (void)
{
  return ulBSP430clockACLK_Hz_ni();
}

/** Interruptible-preserving wrapper for uiBSP430clockACLK_Hz_ni()
 * @deprecated: Use ulBSP430clockACLK_Hz() */
static unsigned int
BSP430_CORE_INLINE
uiBSP430clockACLK_Hz (void)
{
  unsigned int rv;
  BSP430_CORE_INTERRUPT_STATE_T istate;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = uiBSP430clockACLK_Hz_ni();
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
