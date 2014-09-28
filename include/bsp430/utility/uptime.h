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
 * @brief Support for maintaining a system uptime counter.
 *
 * This module provides routines to initialize and query a long-term
 * clock, using a platform-selected timer sourced by an undivided
 * ACLK.
 *
 * vBSP430uptimeStart_ni() must be invoked on hardware initialization
 * after configuring the system clocks.  This is done for you in
 * vBSP430platformInitialize_ni() if #configBSP430_UPTIME is true.
 *
 * Normally #BSP430_PERIPH_TA0 is used for this function, but see
 * #BSP430_UPTIME_TIMER_PERIPH_CPPID if you want to select a different
 * timer.
 *
 * @warning The capture/compare register for
 * #BSP430_TIMER_VALID_COUNTER_READ_CCIDX is reserved for use in
 * overflow management.  Do not use or reconfigure this register.
 *
 * @see grp_utility_uptime_epoch
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 *
 * @defgroup grp_utility_uptime_epoch Epoch Support for the Uptime Timer
 *
 * @brief bsp430/utility/uptime.h data structures and functions
 * supporting conversion between civil time and the uptime facility.
 *
 * @note This functionality depends on #configBSP430_UPTIME_EPOCH.
 *
 * The uptime clock in BSP430 supports a 32-bit monotonically
 * non-decreasing fixed-frequency counter used for low-resolution
 * timing.  Normally the clock is based on a 32 kiHz crystal; this can
 * be divided, or the clock may be based on VLOCLK at roughly 10 kHz.
 * Thus the minimum duration representable without wrapping is about
 * 36 hours.  The *era* of the uptime clock is the number of times it
 * has wrapped since the system started.  (For your amusement, the era
 * can be found in the upper 16 bits of the @link
 * sBSP430halTIMER::overflow_count overflow counter @endlink of the
 * @link hBSP430uptimeTimer associated timer@endlink, but it is not
 * used in this feature.)
 *
 * In some applications it's necessary to correlate the internal time
 * with a time standard such as UTC.  While many MSP430s have a
 * real-time clock that can assist in representing long durations, the
 * key issue is registering the internal clock with a time standard.
 * BSP430 supports this with the concept of an (uptime) *epoch*, being
 * a representation of a time that corresponds to the start of an era,
 * accurate to the resolution of the uptime clock.
 *
 * As the era can change asynchronously to epoch updates, the
 * following expectation affect use of the uptime epoch facility:
 *
 * @li There is no valid epoch until the application provides one,
 * through iBSP430uptimeSetEpochFromNTP() or
 * iBSP430uptimeSetEpochFromTimeval().
 *
 * @li If there is no valid epoch, the functions that convert to time
 * standards (e.g., iBSP430uptimeAsTimeval(),
 * xBSP430uptimeAsPOSIXTime()) return an error.
 *
 * @li The epoch must be explicitly updated at least once every
 * #BSP430_UPTIME_EPOCH_UPDATE_INTERVAL_UTT ticks (1/4 era, or about
 * nine hours at 32 kiHz).
 *
 * @li Times that are to be converted must be within
 * #BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT ticks (3/8 era, or about 13
 * hours at 32 kiHz) before or after the time the epoch was last
 * updated.  @warning Conversion of a time outside this range will
 * invalidate the epoch.
 *
 * @li The uptime epoch is invalidated when the uptime clock is @link
 * vBSP430uptimeSuspend_ni suspended@endlink .
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_UPTIME_H
#define BSP430_UTILITY_UPTIME_H

#include <bsp430/periph/timer.h>
#include <bsp430/clock.h>

/** Define to a true value to enable the uptime infrastructure to
 * maintain a continuous system clock.  A timer that will support this
 * must be identified; see #BSP430_UPTIME_TIMER_PERIPH_CPPID.
 *
 * @cppflag
 * @affects #BSP430_UPTIME
 * @defaulted
 */
#ifndef configBSP430_UPTIME
#define configBSP430_UPTIME 0
#endif /* configBSP430_UPTIME */

/** Defined to a true value if #BSP430_UPTIME_TIMER_PERIPH_CPPID has
 * been provided, making the uptime infrastructure available.
 *
 * @dependency #configBSP430_UPTIME
 * @platformvalue */
#if defined(BSP430_DOXYGEN)
#define BSP430_UPTIME include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** Define to a true value to support uptime timer epochs.
 *
 * This flag enables infrastructure support to use the @ref
 * grp_utility_uptime_epoch infrastructure to translate between civil
 * time values such as UTC (represented as NTP timestamps or
 * <tt>struct timeval</tt>) and uptime clocks, by providing a precise
 * epoch that denotes the time at which the uptime timer last rolled
 * over.
 *
 * @see grp_utility_uptime_epoch
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_UPTIME_EPOCH
#define configBSP430_UPTIME_EPOCH 0
#endif /* configBSP430_UPTIME_EPOCH */

/** Define to a true value to support uptime-driven delays.
 *
 * This flag enables infrastructure support to use the @ref
 * grp_timer_alarm infrastructure on the uptime timer to support
 * application delays.  The core API for this capability is
 * lBSP430uptimeSleepUntil() and #BSP430_UPTIME_DELAY_MS_NI().
 *
 * @note Applications that use the delay functionality should normally
 * enable #configBSP430_CORE_LPM_EXIT_CLEAR_GIE.  See discussion at
 * lBSP430uptimeSleepUntil().
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_UPTIME_DELAY
#define configBSP430_UPTIME_DELAY 0
#endif /* configBSP430_UPTIME_DELAY */

/** Define to the preprocessor-compatible identifier for a timer that
 * should be used to maintain a continuous system clock sourced from
 * ACLK.  The define must appear in the @ref bsp430_config subsystem
 * so that functional resource requests are correctly propagated to
 * the underlying resource instances.
 *
 * @defaulted
 * @platformvalue
 * @affects #BSP430_UPTIME_TIMER_PERIPH_HANDLE */
#if defined(BSP430_DOXYGEN)
#define BSP430_UPTIME_TIMER_PERIPH_CPPID include "bsp430_config.h"
#endif /* BSP430_DOXYGEN */

/** Defined to the peripheral identifier for a timer that can be used
 * to maintain a continuous system clock sourced from ACLK.  This
 * derives directly from #BSP430_UPTIME_TIMER_PERIPH_CPPID, but is a
 * timer peripheral handle suitable for use in code.
 *
 * The corresponding HAL and primary ISR features are automatically
 * enabled for this peripheral.  CC0 ISR inclusion is influenced by
 * #configBSP430_UPTIME_TIMER_HAL_CC0_ISR.
 *
 * @dependency #BSP430_UPTIME_TIMER_PERIPH_CPPID */
#if defined(BSP430_DOXYGEN)
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE platform or application specific
/* !BSP430! instance=@timers functional=uptime_timer subst=functional insert=periph_sethandle */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_sethandle] */

#elif BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA0
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0

#elif BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA1
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA1

#elif BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA2
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA2

#elif BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA3
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA3

#elif BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB0
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE BSP430_PERIPH_TB0

#elif BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB1
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE BSP430_PERIPH_TB1

#elif BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB2
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE BSP430_PERIPH_TB2
/* END AUTOMATICALLY GENERATED CODE [periph_sethandle] */
/* !BSP430! end=periph_sethandle */
#endif /* BSP430_UPTIME_TIMER_PERIPH_CPPID */

/** Indirect control inclusion of the @HAL CC0 interrupt handler for #BSP430_UPTIME_TIMER_PERIPH_HANDLE
 *
 * Propagated as default for #configBSP430_HAL_TA0 or other flag
 * determined by #BSP430_UPTIME_TIMER_PERIPH_CPPID.
 *
 * The dedicated CC0 ISR associated with the uptime timer is an ideal
 * place to support timeslicing for an RTOS.  Normally the CC0
 * interrupt HAL ISR is left unmanaged by BSP430.
 *
 * Set this flag to 1 if you want BSP430 to provide an implementation
 * for the CC0 interrupt for the HAL timer that underlies the
 * implementation, thereby allowing BSP430 to dispatch CC0 events to
 * callbacks registered in #sBSP430halTIMER.
 *
 * Set this flag to 0 (or leave it unset) if you don't need CC0 or
 * intend to implement your own CC0 ISR.
 *
 * @cppflag
 * @defaulted
 * @dependentvalue
 * @ingroup grp_config_functional */
#if defined(BSP430_DOXYGEN)
#define configBSP430_UPTIME_TIMER_HAL_CC0_ISR indirectly defaulted
#endif /* BSP430_DOXYGEN */

/** Source selector for uptime clock.  This should be bits suitable for
 * storage into the TASSEL field of a timer control register.  (Note
 * that TASSEL and TBSSEL support the same values; this works when
 * using TBx as well.)
 *
 * @defaulted */
#ifndef BSP430_UPTIME_TASSEL
#define BSP430_UPTIME_TASSEL TASSEL_1 /* == TASSEL__ACLK */
#endif /* BSP430_UPTIME_TASSEL */

/** Shift value dividing #BSP430_UPTIME_TASSEL for the uptime clock.
 *
 * @defaulted
 */
#ifndef BSP430_UPTIME_DIVIDING_SHIFT
#define BSP430_UPTIME_DIVIDING_SHIFT 0
#endif /* BSP430_UPTIME_DIVIDING_SHIFT */

/** @cond DOXYGEN_EXCLUDE */
extern hBSP430halTIMER xBSP430uptimeTIMER_;
/** @endcond */

/** Get access to the timer controlling the uptime infrastructure.
 *
 * You might need this if you want to hook into its callbacks.
 *
 * Don't hook into the overflow callback, please.
 */
#if defined(BSP430_DOXYGEN) || (BSP430_UPTIME - 0)
static BSP430_CORE_INLINE
hBSP430halTIMER
hBSP430uptimeTimer (void)
{
  return xBSP430uptimeTIMER_;
}
#endif /* DOXYGEN or function available */

#if (BSP430_UPTIME - 0)
/** @cond DOXYGEN_EXCLUDE */
/** This provides the conversion factor between uptime ticks and
 * normal time.  It is not for direct reference by users; see
 * ulBSP430uptimeConversionFrequency_Hz(). */
extern unsigned long ulBSP430uptimeConversionFrequency_Hz_ni_;
/** @endcond */
#endif /* BSP430_UPTIME */

/** Return the uptime clock conversion frequency.
 *
 * This is the number of uptime ticks in a standard second, and is
 * used when translating between tick measurements and second-based
 * durations.  The default value is the estimated frequency of the
 * timer underlying the uptime clock (per
 * ulBSP430timerFrequency_Hz_ni()), but this may be overridden by
 * ulBSP430uptimeSetConversionFrequency_ni() when a more accurate
 * estimate of actual frequency is available.
 *
 * This value is set when vBSP430uptimeStart_ni() or
 * vBSP430uptimeResume_ni() are invoked.  Its value is undefined when
 * the uptime timer is stopped.  Reconfiguration of the timer source
 * or the underlying clock while the timer is running may invalidate
 * its value.
 *
 * @return The conversion frequency of the uptime clock, in Hz. */
unsigned long ulBSP430uptimeConversionFrequency_Hz (void);

/** Set the uptime frequency that will be used for tick/time conversion.
 *
 * In cases where the uptime clock derives from @link
 * eBSP430clockSRC_VLOCLK VLOCLK@endlink, the nominal frequency
 * used by default may be off by as much as 10% from the actual
 * frequency.  This function can be used when a more accurate estimate
 * of actual frequency is available to decrease the error in converted
 * times.
 *
 * @param frequency_Hz the frequency to be used when converting
 * durations in ticks to durations in seconds or related units.  If a
 * value of zero is provided, the current nominal frequency will be
 * used in subsequent conversions.
 *
 * @return the previous value of the conversion frequency, which may
 * be 0 if no conversions had occured. */
unsigned long ulBSP430uptimeSetConversionFrequency_ni (unsigned long frequency_Hz);

#if defined(BSP430_DOXYGEN) || (BSP430_UPTIME - 0)
/** Convert from milliseconds to ticks of the uptime timer.
 * @note Evaluation is valid only when the uptime timer is running.  The result is rounded down. */
#define BSP430_UPTIME_MS_TO_UTT(ms_) BSP430_CORE_MS_TO_TICKS((ms_), ulBSP430uptimeConversionFrequency_Hz())
/** Convert from ticks of the uptime timer to milliseconds.
 * @note Evaluation is valid only when the uptime timer is running.  The result is rounded down. */
#define BSP430_UPTIME_UTT_TO_MS(utt_) BSP430_CORE_TICKS_TO_MS((utt_), ulBSP430uptimeConversionFrequency_Hz())
/** Convert from microseconds to ticks of the uptime timer.
 * @note Evaluation is valid only when the uptime timer is running.  The result is rounded down. */
#define BSP430_UPTIME_US_TO_UTT(us_) BSP430_CORE_US_TO_TICKS((us_), ulBSP430uptimeConversionFrequency_Hz())
/** Convert from ticks of the uptime timer to microseconds.
 * @note Evaluation is valid only when the uptime timer is running.  The result is rounded down. */
#define BSP430_UPTIME_UTT_TO_US(utt_) BSP430_CORE_TICKS_TO_US((utt_), ulBSP430uptimeConversionFrequency_Hz())
#endif /* BSP430_UPTIME */

#if defined(BSP430_DOXYGEN) || (BSP430_UPTIME - 0)
/** Return system uptime in clock ticks with disabled interrupts.
 *
 * @note The returned value is adjusted for a single pending overflow
 * event that occurs while interrupts are disabled, but will be wrong
 * if interrupts remain disabled long enough for a second overflow to
 * occur.  See ulBSP430timerCounter_ni() for details.
 *
 * vBSP430timerSafeCounterInitialize_ni() will have been called on
 * #BSP430_UPTIME_TIMER_PERIPH_HANDLE so
 * #BSP430_TIMER_VALID_COUNTER_READ_CCIDX is reserved to support
 * accurate counter reads. */
static BSP430_CORE_INLINE
unsigned long
ulBSP430uptime_ni (void)
{
  return ulBSP430timerCounter_ni(hBSP430uptimeTimer(), 0);
}

/** Return the system uptime in clock ticks. */
static BSP430_CORE_INLINE
unsigned long
ulBSP430uptime (void)
{
  return ulBSP430timerCounter(hBSP430uptimeTimer(), 0);
}

/** Return the system uptime in clock ticks at its full precision
 *
 * This incorporates the overflow and the current counter.  On the
 * MSP430 the result is expected to be a 64-bit value of which the low
 * 48-bits are valid. */
static BSP430_CORE_INLINE
unsigned long long
ullBSP430uptime (void)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  unsigned int overflow;
  unsigned long ul;

  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    ul = ulBSP430timerCounter_ni(hBSP430uptimeTimer(), &overflow);
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return (((unsigned long long)overflow) << (8 * sizeof(ul))) | ul;
}

/** Adjust a captured 16-bit counter to a full resolution timestamp.
 *
 * This obtains the current time, then adjusts it backwards to be the
 * most recent time for which the low 16 bits match @p ctr.  The
 * result should be a 48-bit timestamp expressed in a 64-bit unsigned
 * integer.
 *
 * @see ullBSP430timerCorrected() */
static BSP430_CORE_INLINE
unsigned long long
ullBSP430uptimeCorrected (unsigned int ctr)
{
  return ullBSP430timerCorrected(hBSP430uptimeTimer(), ctr);
}

/** Return the low word of the system uptime counter.
 *
 * This is suitable for use by applications that wish to configure
 * capture/compare registers on hBSP430uptimeTimer().
 */
static BSP430_CORE_INLINE
unsigned int
uiBSP430uptimeCounter_ni (void)
{
  hBSP430halTIMER utp = hBSP430uptimeTimer();
  return uiBSP430timerBestCounterRead_ni(utp->hpl, utp->hal_state.flags);
}

#endif /* BSP430_UPTIME */

/** Configure the system uptime clock.
 *
 * The timer associated with the uptime clock is reset to zero and
 * begins counting up.
 *
 * @warning This should be invoked once, normally by
 * vBSP430platformInitialize_ni().  Invoking it a second time may
 * result in undefined behavior.  Use vBSP430uptimeSuspend_ni() and
 * vBSP430uptimeResume_ni() to turn the uptime timer off and on. */
void vBSP430uptimeStart_ni (void);

/** Suspend the system uptime clock.
 *
 * Suspension means that the clocks are halted and interrupts from
 * them inhibited.  Counter values are not modified.
 * #vBSP430uptimeResume_ni should be invoked to release the clocks to
 * continue counting.
 *
 * The system clock may be suspended prior to entering certain low
 * power modes.
 */
void vBSP430uptimeSuspend_ni (void);

/** Resume the system uptime clock
 *
 * The clocks are re-enabled to continue counting from their last
 * recorded value.  Also, the conversion frequency used by
 * ulBSP430uptimeConversionFrequency_Hz() is updated based on the
 * current timer configuration.
 */
void vBSP430uptimeResume_ni (void);

/** Expected length for a buffer used by xBSP430uptimeAsText().
 *
 * This macro may be used to allocate such a buffer.
 *
 * At 32 kiHz resolution a 32-bit integer holding ticks can only
 * represent 36 hours before wrapping, but at 10 kHz resolution the
 * same counter might express up to 119 hours, so this allows
 * durations up to up to 999:59:59.999 to be expressed. */
#define BSP430_UPTIME_AS_TEXT_LENGTH sizeof("HHH:MM:SS.mmm")

/** Convert an uptime count to text HHH:MM:SS.mmm format.
 *
 * At least the MM:SS.mmm portion is present, with minutes
 * space-padded on the left.  If the duration exceeds 59:59.999, then
 * space-padded hours will be included as well, and minutes will be
 * zero-padded.
 *
 * The conversion factor from ulBSP430uptimeConversionFrequency_Hz()
 * is used.
 *
 * @param duration_utt a duration in uptime ticks
 *
 * @param buffer a pointer to a buffer large enough to hold the
 * represented data, normally #BSP430_UPTIME_AS_TEXT_LENGTH characters
 *
 * @return @p buffer */
const char * xBSP430uptimeAsText (unsigned long duration_utt,
                                  char * buffer);

/** Convert an uptime count to text HH:MM:SS.mmm format in a static
 * buffer.
 *
 * Invokes xBSP430uptimeAsText() with a static 14-character buffer
 * adequate for representing durations up to 999:59:59.999 in contexts
 * where allocating storage is problematic.
 *
 * @param duration_utt see xBSP430uptimeAsText()
 *
 * @return a pointer to the static storage holding the formatted time.
 *
 * @deprecated Legacy interface, use xBSP430uptimeAsText(). */
const char * xBSP430uptimeAsText_ni (unsigned long duration_utt);

/** The capture/compare index to be used for delays.
 *
 * If #configBSP430_UPTIME_DELAY is enabled this capture/compare
 * register index will be used on the uptime timer to support delays
 * using the @ref grp_timer_alarm infrastructure.
 *
 * @note Most applications should avoid using CC 0 because it is
 * normally reserved for RTOS context switch support, and CC 2 because
 * it is normally used in uiBSP430timerSafeCounterRead_ni() from
 * ulBSP430timerCounter_ni() when
 * #configBSP430_TIMER_VALID_COUNTER_READ is enabled.  The uptime
 * timer is generally asynchronous to MCLK so valid counter reads are
 * critical.
 *
 * @warning If you assign the same value to #BSP430_UPTIME_DELAY_CCIDX
 * and #BSP430_TIMER_VALID_COUNTER_READ_CCIDX, platform initialization
 * will hang attempting to configure the delay alarm.
 *
 * @defaulted
 */
#if defined(BSP430_DOXYGEN) || (configBSP430_UPTIME_DELAY - 0)
#ifndef BSP430_UPTIME_DELAY_CCIDX
#define BSP430_UPTIME_DELAY_CCIDX 1
#endif /* BSP430_UPTIME_DELAY_CCIDX */
#endif /* BSP430_DOXYGEN */

/** Sleep until the uptime timer reaches a particular value.
 *
 * This uses #BSP430_UPTIME_DELAY_CCIDX and the @ref grp_timer_alarm
 * to sleep until either a specific time has been reached or an
 * interrupt wakes the processor.
 *
 * @note Interrupts are enabled during this sleep, and an interrupt
 * unrelated to the wakeup alarm will cause an early return.  If such
 * an interrupt does not clear #GIE on exit there will be a short
 * window between wakeup and when this function clears #GIE again in
 * which additional interrupts may execute.  See @ref enh_interrupts.
 *
 * @blocking
 *
 * @param setting_utt The uptime counter value at which the
 * application wishes to be awakened if no events have occured before
 * then.  Note that the call is a wrapper around
 * iBSP430timerAlarmSet_ni() and may return immediately with an error
 * result if the specified time appears to be in the past.
 *
 * @param lpm_bits The bits to be set in the status register to
 * control the low power mode used while awaiting the wakeup time.  A
 * value such as #LPM0_bits or #LPM2_bits should be used depending on
 * application needs.  #GIE will be added to these bits.
 *
 * @warning Consider whether the LPM mode might disable the clock or
 * timer on which the wakeup depends.  In many recent MCUs the LPM
 * mode will be internally masked to ensure this does not happen.
 * However, on 4xx MCUs setting #LPM4_bits when ACLK drives the uptime
 * timer will cause the system to hang.  Use #LPM3_bits as a default
 * when only the delay is important.
 *
 * @return The number of timer ticks remaining until the
 * originally-requested time.  If an error occurs, a zero value will
 * be returned.  There is no facility to tell whether wakeup occurred
 * due to the alarm or another interrupt, except that if the return
 * value is positive it is likely another interrupt is the cause.
 *
 * @note The function is not implemented unless
 * #configBSP430_UPTIME_DELAY is enabled.
 *
 * @warning This function does not diagnose specific errors such as
 * invoking it while the uptime timer or the delay alarm are disabled.
 *
 * @dependency #configBSP430_UPTIME_DELAY */
#if defined(BSP430_DOXYGEN) || (configBSP430_UPTIME_DELAY - 0)
long lBSP430uptimeSleepUntil (unsigned long setting_utt,
                              unsigned int lpm_bits);
#endif /* configBSP430_UPTIME_DELAY */

/** Enable or disable the alarm used by ulBSP430uptimeDelayUntil().
 *
 * This delegates to iBSP430timerAlarmSetEnabled_ni().
 *
 * The alarm is enabled by vBSp430uptimeStart_ni() if
 * #configBSP430_UPTIME_DELAY is enabled.
 *
 * @dependency #configBSP430_UPTIME_DELAY */
#if defined(BSP430_DOXYGEN) || (configBSP430_UPTIME_DELAY - 0)
int iBSP430uptimeDelaySetEnabled_ni (int enablep);
#endif /* configBSP430_UPTIME_DELAY */

/** Macro to simplify basic delay-in-ticks operations
 *
 * This invokes lBSP430uptimeSleepUntil() in a loop for a specified
 * duration or until another event has occurred.
 *
 * @param delay_utt_ duration, in uptime ticks, that application should sleep
 *
 * @param lpm_bits_ the LPM mode in which the application should sleep.
 * See corresponding parameter in lBSP430uptimeSleepUntil_ni().
 *
 * @param exit_expr_ an expression that will be tested prior to
 * sleeping.  If it evaluates to true, the delay will be aborted
 * immediately regardless of remaining time.  Pass @c 0 if you wish to
 * delay regardless of other activity.
 *
 * @dependency #configBSP430_UPTIME_DELAY */
#if defined(BSP430_DOXYGEN) || (configBSP430_UPTIME_DELAY - 0)
#define BSP430_UPTIME_DELAY_UTT_NI(delay_utt_, lpm_bits_, exit_expr_) do { \
    unsigned long wake_utt;                                             \
    wake_utt = ulBSP430uptime_ni() + (delay_utt_);                      \
    while ((! (exit_expr_)) && (0 < lBSP430uptimeSleepUntil(wake_utt, lpm_bits_))) { \
      /* nop */                                                         \
    }                                                                   \
  } while (0)
#endif /* configBSP430_UPTIME_DELAY */

/** Wrapper around BSP430_UPTIME_DELAY_UTT_NI() suitable for use when
 * interrupts are enabled.
 *
 * @dependency #configBSP430_UPTIME_DELAY */
#if defined(BSP430_DOXYGEN) || (configBSP430_UPTIME_DELAY - 0)
#define BSP430_UPTIME_DELAY_UTT(delay_utt_, lpm_bits_, exit_expr_) do { \
    BSP430_CORE_SAVED_INTERRUPT_STATE(istate);                          \
    BSP430_CORE_DISABLE_INTERRUPT();                                    \
    BSP430_UPTIME_DELAY_UTT_NI(BSP430_UPTIME_UTT_TO_UTT(delay_utt_), lpm_bits_, exit_expr_); \
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);                        \
  } while (0)
#endif /* configBSP430_UPTIME_DELAY */

/** Macro to simplify basic delay-in-milliseconds operations.
 *
 * Wrapper around #BSP430_UPTIME_DELAY_UTT_NI() to support delays
 * specified in milliseconds.
 *
 * @param delay_ms_ duration, in milliseconds, that application should sleep
 *
 * @param lpm_bits_ as with #BSP430_UPTIME_DELAY_UTT_NI()
 *
 * @param exit_expr_ as with #BSP430_UPTIME_DELAY_UTT_NI()
 *
 * @see #BSP430_UPTIME_MS_TO_UTT
 *
 * @dependency #configBSP430_UPTIME_DELAY */
#if defined(BSP430_DOXYGEN) || (configBSP430_UPTIME_DELAY - 0)
#define BSP430_UPTIME_DELAY_MS_NI(delay_ms_, lpm_bits_, exit_expr_) do { \
    BSP430_UPTIME_DELAY_UTT_NI(BSP430_UPTIME_MS_TO_UTT(delay_ms_), lpm_bits_, exit_expr_); \
  } while (0)
#endif /* configBSP430_UPTIME_DELAY */

/** Wrapper around BSP430_UPTIME_DELAY_MS_NI() suitable for use when
 * interrupts are enabled.
 *
 * @dependency #configBSP430_UPTIME_DELAY */
#if defined(BSP430_DOXYGEN) || (configBSP430_UPTIME_DELAY - 0)
#define BSP430_UPTIME_DELAY_MS(delay_ms_, lpm_bits_, exit_expr_) do {   \
    BSP430_CORE_SAVED_INTERRUPT_STATE(istate);                          \
    BSP430_CORE_DISABLE_INTERRUPT();                                    \
    BSP430_UPTIME_DELAY_UTT_NI(BSP430_UPTIME_MS_TO_UTT(delay_ms_), lpm_bits_, exit_expr_); \
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);                        \
  } while (0)
#endif /* configBSP430_UPTIME_DELAY */

#if defined(BSP430_DOXYGEN) || (configBSP430_UPTIME_EPOCH - 0)

#include <sys/time.h>

/** The time of the POSIX epoch (1970-01-01T00:00:00Z) as represented
 * in integral seconds since the NTP epoch (1900-01-01T00:00:00Z).
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
#define BSP430_UPTIME_POSIX_EPOCH_NTPIS 2208988800UL

/** An NTP timestamp for a time known to be within 34 years of any
 * current server.  The calculations of offset and round trip in
 * section 8 "On-wire Protocol" of <a
 * href="http://tools.ietf.org/html/rfc5905">RFC5905</a> are valid
 * only when the client and server are no more than 34 years apart.
 * This is the epoch used when @p bypass_validity is used in
 * iBSP430uptimeAsNTP(), as is the case when doing NTP client/server
 * communications prior to obtaining a valid epoch.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
#define BSP430_UPTIME_BYPASS_EPOCH_NTP ((uint64_t)(BSP430_UPTIME_POSIX_EPOCH_NTPIS + 1388534400UL) << 32)

/** <a href="http://tools.ietf.org/html/rfc5905#section-6">RFC5905 (NTP v4)</a>
 * structure to represent a short-format time (more accurately, a
 * duration, as there is no epoch).
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
typedef struct sBSP430uptimeNTPShortFormat {
  uint16_t integral;            /**< Whole seconds */
  uint16_t fractional;          /**< Fractional seconds */
} sBSP430uptimeNTPShortFormat;

/** <a href="http://tools.ietf.org/html/rfc5905#section-6">RFC5905
 * (NTP v4)</a> structure to represent a timestamp, measured in 2^32
 * Hz ticks since the NTP epoch 1900-01-01T00:00:00Z.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
typedef struct sBSP430uptimeNTPTimestamp {
  uint32_t integral;            /**< Whole seconds */
  uint32_t fractional;          /**< Fractional seconds */
} sBSP430uptimeNTPTimestamp;

/** <a href="http://tools.ietf.org/html/rfc5905#section-7.3">RFC5905
 * (NTP v4)</a> packet header.  Extension fields are not supported in
 * this implementation.
 *
 * @note In almost no situations does a user need to inspect or mutate
 * the fields of this structure.  Use
 * iBSP430uptimeInitializeNTPRequest() to initialize the structure,
 * and iBSP430uptimeSetNTPXmtField() to set the transmission time.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
typedef struct sBSP430uptimeNTPPacketHeader {
  uint8_t li_vn_mode;          /**< Leap indicator, version, and packet mode; fixed for client */
  uint8_t stratum;             /**< Peer stratum, used for kiss-of-death indicator */
  uint8_t ppoll;               /**< Unused: peer poll interval */
  int8_t precision;            /**< Precision of clock as power of 2 (e.g. -15 for 32 kiHz) */
  sBSP430uptimeNTPShortFormat rootdelay; /**< Unused: roundtrip delay to primary source */
  sBSP430uptimeNTPShortFormat rootdisp;  /**< Unused: dispersion to primary source */
  uint32_t refid;              /**< Unused: reference id */
  sBSP430uptimeNTPTimestamp reftime; /**< Unused last update time */
  sBSP430uptimeNTPTimestamp org;     /**< Client #xmt time stamp in packet received by server */
  sBSP430uptimeNTPTimestamp rec;     /**< Server time when client packet received */
  sBSP430uptimeNTPTimestamp xmt;     /**< Server time when server response transmitted */
} sBSP430uptimeNTPPacketHeader;

/** Return 0 if there is an epoch that is presumed to be valid, or a
 * negative error if no epoch is available.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
int iBSP430uptimeCheckEpochValidity ();

/** Return the uptime clock value at which the epoch was last updated.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
unsigned long ulBSP430uptimeLastEpochUpdate ();

/** Return the number of uptime ticks since the epoch was last updated.
 *
 * A negative value is returned if the epoch is not valid, or has not
 * been updated within the last half era.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
long lBSP430uptimeEpochAge ();

/** The maximum number of uptime ticks between updates to the epoch,
 * if epoch consistency is to be maintained.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
#define BSP430_UPTIME_EPOCH_UPDATE_INTERVAL_UTT 0x40000000L

/** The maximum number of uptime ticks between an uptime value and the
 * last time the epoch was updated that will not cause epoch
 * invalidation if used to convert the uptime value.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
#define BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT 0x60000000L

/** Determine which era @p time_utt belongs to for the purposes of
 * conversion using the current epoch.
 *
 * Reasons for invalidity include:
 * @li There is no valid epoch at all (iBSP430uptimeCheckEpochValidity())
 *
 * @li @p time_utt is more than #BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT
 * ticks earlier or later than the time the epoch was last updated
 *
 * @param time_utt the time at which conversion is being proposed.
 *
 * @return a negative code if @p time_utt cannot be converted.
 * Non-negative results indicate the era, relative to the era the
 * epoch was set for, in which @p time_utt lies (0 for the preceding,
 * 1 for the same, 2 for the next).
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
int iBSP430uptimeEpochEra (unsigned long time_utt);

/** Initialize an NTP packet to serve as a request for the current time.
 *
 * This initializes the leap-information, version, and mode flags, and
 * sets the precision based on the local uptime clock.  No timestamps
 * are stored; see iBSP430uptimeNTPUpdateXmtField().
 *
 * @return 0 on success, a negative error code on failure.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
int iBSP430uptimeInitializeNTPRequest (sBSP430uptimeNTPPacketHeader * ntpp);

/** Update the sBSP430uptimeNTPPacketHeader::xmt field.
 *
 * This should be called just prior to (or while) transmitting an NTP
 * request to ensure it holds the most accurate estimate of
 * transmission time available.
 *
 * @param ntpp pointer to the NTP packet to be updated
 *
 * @param putt pointer to the uptime clock value to be stored as
 * transmission time.  If this is a null pointer, the current time is
 * stored.
 *
 * @return 0 on success, a negative error code on failure.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch  */
int iBSP430uptimeSetNTPXmtField (sBSP430uptimeNTPPacketHeader * ntpp,
                                 unsigned long * putt);

/** Set the uptime epoch using an absolute NTP timestamp.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch  */
int iBSP430uptimeSetEpochFromNTP (uint64_t epoch_ntp);

/** Adjust the uptime epoch using a relative NTP timestamp.
 *
 * @param adjustment_ntp the number of ticks of a 2^32 Hz clock that
 * must be added to the epoch to cause the local time to coincide with
 * a reference clock.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch  */
int iBSP430uptimeAdjustEpochFromNTP (int64_t adjustment_ntp);

/** Set the uptime epoch using an absolute Unix time value.
 *
 * @param tv a civil time value
 *
 * @param when_utt the time within the current era at which @p tv is
 * current.
 *
 * @return 0 on success, otherwise a negative error code.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch  */
int iBSP430uptimeSetEpochFromTimeval (const struct timeval * tv,
                                      unsigned long when_utt);

/** Convert a time to an NTP timestamp.
 *
 * @param utt count of ticks since the last time the uptime clock
 * rolled over.
 *
 * @param ntpp where to store the time represented by @p utt as the
 * number of 2^32 Hz ticks since the NTP epoch (1900-01-01T00:00:00Z).
 *
 * @param bypass_validation a flag that bypasses the normal check to
 * see that the epoch is valid.  When set to a true value and the
 * epoch is invalid, a pseudo-epoch corresponding to
 * 2014-01-01T00:00:00Z is used so that a client NTP request packet
 * can be formed with a valid transmission timestamp.  You will need
 * to set this flag when obtaining the @p rec_ntp parameter to
 * iBSP430uptimeProcessNTPResponse() when the epoch is invalid.  If
 * the epoch is valid, this flag is ignored.
 *
 * @return 0 if conversion is successful, otherwise a negative error
 * code.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch  */
int iBSP430uptimeAsNTP (unsigned long utt,
                        uint64_t * ntpp,
                        int bypass_validation);

/** Determine the current time to microsecond resolution by adding the
 * uptime clock time in @p utt to a configured offset.
 *
 * @param utt the uptime clock value
 *
 * @param tv where to store the converted time.  The referenced object
 * is left unchanged if the conversion fails.
 *
 * @return 0 on success, a negative error code if the epoch is not
 * valid or something else went wrong.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch  */
int iBSP430uptimeAsTimeval (unsigned long utt,
                            struct timeval * tv);

/** Return the current time expressed as seconds since the POSIX
 * epoch.
 *
 * This invokes iBSP430uptimeAsTimeval() and returns the @c tv_sec
 * field of the resulting structure.
 *
 * @note If the epoch is not valid for the time, the returned value
 * will be @c (time_t)-1.
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
time_t xBSP430uptimeAsPOSIXTime (unsigned long utt);

/** Process an NTP response.
 *
 * This calculates the offset between the local uptime clock and the
 * time provided by an external server.
 *
 * The request and response packets are validated for server stratum,
 * mismatched, and duplicate/bogus/replayed packets.  Failure of these
 * checks results in an error return.
 *
 * @warning On successful processing, the internally-stored uptime
 * epoch is adjusted to account for the difference between local and
 * server times.
 *
 * @param req pointer to the locally-generated request packet.  This
 * may be null if you don't care to (or cannot) validate that the
 * response origin time matches the request transmit time.
 *
 * @param resp pointer to the response packet.  This must be provided
 * and must be valid.
 *
 * @param rec_ntp the time when the response packet was received
 * locally, in NTP 2^32 Hz ticks since the NTP epoch.  Use
 * iBSP430uptimeAsNTP() to convert from a local uptime measurement.
 *
 * @param adjustment_ntp optional (but recommended) location into
 * which the delta between the local time and the server will be
 * stored (a positive value indicates that the local time is behind
 * the server).  When a valid epoch is already available, this value
 * is suitable for passing unmodified to
 * iBSP430uptimeAdjustEpochFromNTP().  When a valid epoch is not
 * available, this value should be added to
 * #BSP430_UPTIME_BYPASS_EPOCH_NTP and the result passed to
 * iBSP430uptimeSetEpochFromNTP().
 *
 * @param adjustment_ms optional location in which the adjustment,
 * converted to milliseconds, will be stored.  This is primarily
 * intended for diagnostics, as a value in milliseconds is more useful
 * than the 2^32 Hz tick count provided by @p adjustment_ntp.  If the
 * offset cannot be represented in the space available, the signed
 * maximum value will be stored.  If the offset is not of interest,
 * pass a null pointer.  The value is not stored if this function
 * returns an error.
 *
 * @param rtt_us optional location in which to store the estimated
 * round-trip time for request and response.  If the round-trip time
 * cannot be represented (which should never happen), the maximum
 * representable value will be stored.  If the round-trip time is not
 * of interest, pass a null pointer.  The value is not stored if this
 * function returns an error.
 *
 * @return 0 on success, a negative error code on failure
 *
 * @dependency #configBSP430_UPTIME_EPOCH
 * @ingroup grp_utility_uptime_epoch */
int iBSP430uptimeProcessNTPResponse (const sBSP430uptimeNTPPacketHeader * req,
                                     const sBSP430uptimeNTPPacketHeader * resp,
                                     uint64_t rec_ntp,
                                     int64_t * adjustment_ntp,
                                     long * adjustment_ms,
                                     unsigned long * rtt_us);

#endif /* configBSP430_UPTIME_EPOCH */

#endif /* BSP430_UTILITY_UPTIME_H */
