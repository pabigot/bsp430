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
 * timer..
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_UPTIME_H
#define BSP430_UTILITY_UPTIME_H

#include <bsp430/periph/timer.h>
#include <bsp430/clock.h>

/** @def configBSP430_UPTIME
 *
 * Define to a true value to enable the uptime infrastructure to
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

/** @def BSP430_UPTIME
 *
 * Defined to a true value if #BSP430_UPTIME_TIMER_PERIPH_CPPID has
 * been provided, making the uptime infrastructure available.
 *
 * @dependency #configBSP430_UPTIME
 * @platformdefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_UPTIME include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_UPTIME_TIMER_PERIPH_CPPID
 *
 * Define to the preprocessor-compatible identifier for a timer that
 * should be used to maintain a continuous system clock sourced from
 * ACLK.  The define must appear in the @ref bsp430_config subsystem
 * so that functional resource requests are correctly propagated to
 * the underlying resource instances.
 *
 * @defaulted
 * @platformdefault
 * @affects #BSP430_UPTIME_TIMER_PERIPH_HANDLE */
#if defined(BSP430_DOXYGEN)
#define BSP430_UPTIME_TIMER_PERIPH_CPPID include "bsp430_config.h"
#endif /* BSP430_DOXYGEN */

/** @def BSP430_UPTIME_TIMER_PERIPH_HANDLE
 *
 * Defined to the peripheral identifier for a timer that can be used
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
 * Set this flag to 0 (or leave it unset) if don't need CC0 or intend
 * to implement your own CC0 ISR.
 *
 * @cppflag
 * @ingroup grp_config_functional
 * @nodefault */
#if defined(BSP430_DOXYGEN)
#define configBSP430_UPTIME_TIMER_HAL_CC0_ISR indirectly defaulted
#endif /* BSP430_DOXYGEN */

/** @def BSP430_UPTIME_TASSEL
 *
 * Source selector for uptime clock.  This should be bits suitable for
 * storage into the TASSEL field of a timer control register.  (Note
 * that TASSEL and TBSSEL support the same values; this works when
 * using TBx as well.)
 *
 * @defaulted */
#ifndef BSP430_UPTIME_TASSEL
#define BSP430_UPTIME_TASSEL TASSEL_1 /* == TASSEL__ACLK */
#endif /* BSP430_UPTIME_TASSEL */

/** @def BSP430_UPTIME_DIVIDING_SHIFT
 *
 * Shift value dividing #BSP430_UPTIME_TASSEL for the uptime clock.
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
static hBSP430halTIMER
BSP430_CORE_INLINE
xBSP430uptimeTimer (void)
{
  return xBSP430uptimeTIMER_;
}
#endif /* DOXYGEN or function available */

#if (BSP430_UPTIME - 0)
/** @cond DOXYGEN_EXCLUDE */
/** This provides the conversion factor between uptime ticks and
 * normal time.  It is not for direct reference by users; see
 * ulBSP430uptimeConversionFrequency_Hz_ni(). */
extern unsigned long ulBSP430uptimeConversionFrequency_Hz_ni_;
/** @endcond */
#endif /* BSP430_UPTIME */

/** Return the uptime clock conversion frequency.
 *
 * This is the number of uptime ticks in a standard second, and is
 * used when translating between tick measurements and second-based
 * durations.  The default value is the nominal frequency of the timer
 * underlying the uptime clock, but this may be overridden by
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
unsigned long ulBSP430uptimeConversionFrequency_Hz_ni (void);

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
 * @note The result is rounded down. */
#define BSP430_UPTIME_MS_TO_UTT(ms_) BSP430_CORE_MS_TO_TICKS((ms_), ulBSP430uptimeConversionFrequency_Hz_ni_)
/** Convert from ticks of the uptime timer to milliseconds.
 * @note The result is rounded down. */
#define BSP430_UPTIME_UTT_TO_MS(utt_) BSP430_CORE_TICKS_TO_MS((utt_), ulBSP430uptimeConversionFrequency_Hz_ni_)
/** Convert from microseconds to ticks of the uptime timer.
 * @note The result is rounded down. */
#define BSP430_UPTIME_US_TO_UTT(us_) BSP430_CORE_US_TO_TICKS((us_), ulBSP430uptimeConversionFrequency_Hz_ni_)
/** Convert from ticks of the uptime timer to microseconds.
 * @note The result is rounded down. */
#define BSP430_UPTIME_UTT_TO_US(utt_) BSP430_CORE_TICKS_TO_US((utt_), ulBSP430uptimeConversionFrequency_Hz_ni_)
#endif /* BSP430_UPTIME */

#if defined(BSP430_DOXYGEN) || (BSP430_UPTIME - 0)
/** Return system uptime in clock ticks with disabled interrupts. */
static unsigned long
BSP430_CORE_INLINE
ulBSP430uptime_ni (void)
{
  return ulBSP430timerCounter_ni(xBSP430uptimeTimer(), 0);
}

/** Return the system uptime in clock ticks. */
static unsigned long
BSP430_CORE_INLINE
ulBSP430uptime (void)
{
  return ulBSP430timerCounter(xBSP430uptimeTimer(), 0);
}
#endif /* BSP430_UPTIME */

/** Configure the system uptime clock.
 *
 * The timer associated with the uptime clock is reset to zero and
 * begins counting up. */
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
 * ulBSP430uptimeConversionFrequency_Hz_ni() is updated based on the
 * current timer configuration.
 */
void vBSP430uptimeResume_ni (void);

/** Convert an uptime count to text HH:MM:SS.mmm format.
 *
 * At least the MM:SS.mmm portion is present, with minutes
 * space-padded on the left.  If the duration exceeds 59:59.999, then
 * space-padded hours will be included as well, and minutes will be
 * zero-padded.
 *
 * The conversion factor from
 * ulBSP430uptimeConversionFrequency_Hz_ni() is used.
 *
 * @param duration_utt a duration in uptime ticks
 * @return pointer to formatted time.  The pointer is to static storage. */
const char * xBSP430uptimeAsText_ni (unsigned long duration_utt);

#endif /* BSP430_UTILITY_UPTIME_H */
