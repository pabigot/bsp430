/* Copyright 2012-2013, Peter A. Bigot
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
 * @brief Hardware presentation/abstraction for generic timers (Timer_A/Timer_B)
 *
 * Timer functionality exists on most MSP430 MCUs.  This module
 * supports both Timer_A and Timer_B, but does not provide any
 * distinction between them.  At this time, Timer_D is not supported
 * by BSP430.
 *
 * Conventional peripheral handles are #BSP430_PERIPH_TA0 and
 * #BSP430_PERIPH_TB0.  The handles are available only when the
 * corresponding @HPL is requested.  The number of capture/compare
 * blocks supported by the MCU is not an identifying characteristic of
 * the peripheral instance.
 *
 * @note The original nomenclature for MSP430 timers in MSP430 has
 * proven to be inconsistent as the number of instances grew.  While
 * the identifier @c TA3 in early MCUs specified the presence of a
 * Timer_A instance with 3 CCs, this led to the need to denote a
 * second Timer_A instance with 5 CCs as T1A5 within the MCU-specific
 * headers.  The data sheets, however, refer to that same instance as
 * TA1, with a dot-separated CC number where necessary (e.g., TA1.4
 * refers to the fifth CC on the second Timer_A instance, and is
 * controlled via TA1CCTL4).
 *
 * @note In BSP430 timer instance names uniformly begin with instance
 * zero, even on MCUs where the documented peripheral name does not
 * include an instance.  For example, peripheral called @c TA2 on the
 * MSP430G2231 is simply the first Timer_A instance and has 2 CCs.  It
 * is identified here as @c TA0, with the number of CCs left implicit.
 *
 * @section h_periph_timer_opt Module Configuration Options
 *
 * @li #configBSP430_HPL_TA0 to enable the HPL handle declarations
 *
 * @li #configBSP430_HAL_TA0 to enable the HAL infrastructure
 *
 * @li #configBSP430_HAL_TA0_ISR to enable the HAL ISR for
 * overflow and capture/compare blocks 1 and higher
 *
 * @li #configBSP430_HAL_TA0_CC0_ISR to enable the HAL ISR for
 * capture/compare block 0
 *
 * @li #configBSP430_TIMER_CCACLK to request a platform-specific clock
 * with certain capture/compare capabilities be identified
 *
 * Substitute other instance names (e.g., @b TB1) as necessary.
 *
 * @section h_periph_timer_hpl Hardware Presentation Layer
 *
 * Though Timer_A and Timer_B have distinct capabilities, their
 * underlying register maps are compatible and #sBSP430hplTIMER is
 * used for both.  The structure includes space for all eight
 * potential capture/compare registers, but only the ones supported by
 * the device should be accessed.
 *
 * For convenience several alternative methods of accessing a timer
 * counter are provided:
 *
 * @li uiBSP430timerSyncCounterRead_ni() reads the peripheral counter
 * register without validating it.  It is the fastest approach, but
 * can produce invalid results when the timer clock is asynchronous to
 * the CPU clock.

 * @li uiBSP430timerAsyncCounterRead_ni() repeatedly reads the counter
 * until the same value is read twice.  It is appropriate when the
 * timer is asynchronous to the CPU clock (e.g., timers using ACLK
 * derived from LFXT1).  Calls to it may hang if the timer runs so
 * fast that the counter changes between successive reads.
 *
 * @li uiBSP430timerLatchedCounterRead_ni() reserves a capture/compare
 * register to be used to latch the counter value and return it.  It
 * is safe and valid for synchronous and asynchronous timers at any
 * rate, but requires a capture/compare register be identified and
 * pre-configured using vBSP430timerLatchedCounterInitialize_ni().
 *
 * @li uiBSP430timerSafeCounterRead_ni() selects between
 * uiBSP430timerSyncCounterRead_ni() and
 * uiBSP430timerLatchedCounterRead_ni() depending on how the
 * application has configured #configBSP430_TIMER_VALID_COUNTER_READ.
 * Here "safe" means it will return the best counter value that can be
 * found in constant time.  Validity of the returned value is
 * guaranteed only when #configBSP430_TIMER_VALID_COUNTER_READ is
 * enabled.  This routine also does some validation on the
 * configuration of the capture/compare register if
 * #BSP430_CORE_NDEBUG is false.  It is the slowest but also the most
 * reliable absent information about how the timer is configured, so
 * is the solution underlying ulBSP430timerCounter_ni().
 *
 * @section h_periph_timer_hal Hardware Adaptation Layer
 *
 * The timer @HAL uses the sBSP430halTIMER structure.
 *
 * Enabling #configBSP430_HAL_TA0 also enables
 * #configBSP430_HAL_TA0_ISR unless previously disabled.  When this
 * primary ISR is enabled an interrupt service routine is provided
 * supporting interrupts related to timer overflow and capture/compare
 * events in CC blocks 1 and higher.  The corresponding interrupts
 * must be enabled by the application.  On an overflow interrupt, the
 * sBSP430halTIMER.overflow_count is incremented and the
 * sBSP430halTIMER.overflow_cbchain_ni callback chain is invoked.  On
 * a capture/compare interrupt, the corresponding chain from
 * sBSP430halTIMER.cc_cbchain_ni is invoked.  (Note that the index in
 * this array is the CC number.)
 *
 * The secondary ISR controlled by #configBSP430_HAL_TA0_CC0_ISR is
 * left disabled unless explicitly enabled.  If enabled, an interrupt
 * service routine is provided supporting capture/compare interrupts
 * related to CC0.  The first chain in the
 * sBSP430halTIMER.cc_cbchain_ni array is invoked by this ISR.  The
 * interrupt enable bit is controlled by the application.
 *
 * @section h_periph_timer_extra Additional Capabilities
 *
 * Some of the material provided by this header is documented in
 * separate sections:
 *
 * @li @ref grp_timer_alarm
 * @li @ref grp_timer_ccaclk
 * @li @ref grp_config_functional
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 *
 * @defgroup grp_timer_alarm Alarm-related Timer Functionality
 *
 * @brief bsp430/periph/timer.h data structures and functions supporting alarms
 *
 * BSP430 provides infrastructure that uses the capture/compare
 * interrupt callbacks of the timer peripheral to support two types of
 * configurable alarm system:
 * @li @ref grp_timer_alarm_dedicated supports one deadline+callback
 * associated with specific capture/compare register
 * @li @ref grp_timer_alarm_muxed supports an arbitrary number of
 * independent deadline+callbacks that share a single dedicated
 * alarm
 *
 * @section grp_timer_alarm_dedicated Dedicated Alarm Infrastructure
 *
 * Each dedicated alarm is configured to reference a specific
 * capture/compare register within a specific timer peripheral.  The
 * HAL infrastructure for that peripheral must also have been
 * requested.  The number of capture/compare registers supported by a
 * particular peripheral may be obtained at runtime using
 * iBSP430timerSupportedCCs().  If capture/compare register zero is to
 * be used for alarms, the corresponding CC0 ISR infrastructure must
 * also be enabled.  The table below indicates the options that should
 * be enabled for various timers:
 *
 * Underlying Timer                   | HAL Request           | CC0 ISR Request
 * :--------------------------------- | :-------------------- | :--------------
 * #BSP430_PERIPH_TA3                 | #configBSP430_HAL_TA3 | #configBSP430_HAL_TA3_CC0_ISR
 * #BSP430_UPTIME_TIMER_PERIPH_HANDLE | @em automatic         | #configBSP430_UPTIME_TIMER_HAL_CC0_ISR
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE | #configBSP430_TIMER_CCACLK_HAL | #configBSP430_TIMER_CCACLK_HAL_CC0_ISR
 *
 * @note It is the responsibility of the application to configure the
 * underlying timer to continuous mode with the appropriate clock
 * source, any desired dividers, and enabling the interrupt that
 * maintains the 32-bit overflow counter timer HAL infrastructure.  If
 * you are using #BSP430_UPTIME_TIMER_PERIPH_HANDLE the timer is
 * already configured this way.  For other timers, use something like:
 * @code

  alarmHAL_ = hBSP430timerLookup(ALARM_TIMER_PERIPH_HANDLE);
  if (alarmHAL_) {
    alarmHAL_->hpl->ctl = 0;
    vBSP430timerResetCounter_ni(alarmHAL_);
    alarmHAL_->hpl->ctl = APP_TASSEL | MC_2 | TACLR | TAIE;
    vBSP430timerInferHints_ni(alarmHAL_);
  }

 * @endcode
 *
 * Alarms depend on a persistent object that describes their current
 * state and holds the callback structures necessary to connect them
 * to the peripheral interrupt infrastructure.  These objects must be
 * initialized once using hBSP430timerAlarmInitialize().  After that
 * point, the objects are not directly referenced; operations on the
 * alarm are performed using a @link hBSP430timerAlarm handle@endlink.
 *
 * Alarms are @link iBSP430timerAlarmEnable() enabled@endlink and
 * @link iBSP430timerAlarmDisable() disabled@endlink as needed.  The
 * act of enabling an alarm links its callbacks into the timer
 * interrupt system.  Because these callbacks introduce overhead even
 * when the alarm is not set, alarms should only be enabled when they
 * are likely to be used.
 *
 * Alarms are set with iBSP430timerAlarmSet() using the absolute time
 * of the underlying 32-bit timer, which can be obtained through
 * ulBSP430timerCounter().  Because timers run asynchronously to the
 * code, even when interrupts are disabled, the set function return
 * value should be examined to ensure that the alarm was, in fact,
 * set.  Alarms may not be set if the duration is too short to
 * reliably implement them, or if the time specified appears to be in
 * the past.  The infrastructure detects times that are past the
 * 16-bit overflow of the underlying timer and uses the overflow
 * interrupt to enable the capture/compare register interrupt in the
 * correct cycle.
 *
 * Alarms that are set may be reliably cancelled using
 * iBSP430timerAlarmCancel_ni().  This should always be done with
 * interrupts disabled, to ensure that the alarm is not firing while
 * the cancellation occurs.  The return value will indicate if the
 * alarm cancellation failed, e.g. due to it having already gone off.
 *
 * When the alarm fires, the callback registered in
 * sBSP430timerAlarm::callback_ni is invoked.  The callback may invoke
 * iBSP430timerAlarmSet_ni() to reschedule a periodic alarm, but
 * complex processing should not be done.  The callback should instead
 * set a volatile global variable and return a value such as
 * #BSP430_HAL_ISR_CALLBACK_EXIT_LPM.  See
 * #iBSP430timerAlarmCallback_ni.
 *
 * The @ref ex_utility_alarm example program provides an environment
 * where the behavior of alarms can be interactively probed.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 *
 * @defgroup grp_timer_ccaclk Platform-independent Secondary Timer Functionality
 *
 * @brief bsp430/periph/timer.h identifies a CCACLK secondary timer
 * feature in a platform-independent manner.
 *
 * The primary capability of this timer and the origin of its name is
 * the ability to use ACLK as a capture/compare input source.  This
 * capability enables measuring the frequency of one clock relative to
 * the frequency of ACLK, and is the foundation of several DCO
 * calibration capabilities including iBSP430ucsTrimDCOCLKDIV_ni() and
 * iBSP430bc2TrimToMCLK_ni().  Use of CCACLK in this role can be seen
 * in @ref ex_bootstrap_clocks for DCO measurement, and in @ref
 * ex_sensors_hh10d for measuring an externally-supplied frequency.
 *
 * A secondary role of this infrastructure is to identify a timer
 * distinct from #BSP430_UPTIME_TIMER_PERIPH_HANDLE which can be used
 * by platform-independent applications for various purposes, such as
 * alarms drive by a clock distinct from the bsp430/utility/uptime.h
 * infrastructure.  Use of CCACLK in this role can be seen in @ref
 * ex_utility_alarm.
 *
 * @note Many of these features are enabled through functional
 * configuration settings documented in @ref grp_config_functional.
 *
 * @section grp_timer_alarm_muxed Multiplexed Alarm Infrastructure
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_TIMER_H
#define BSP430_PERIPH_TIMER_H

/* !BSP430! periph=timer */
/* !BSP430! instance=@timers */

#include <bsp430/periph.h>
#include <bsp430/clock.h>

/** Defined on inclusion of <bsp430/periph/timer.h>.  The value
 * evaluates to true if the target MCU supports the Timer_A Module,
 * and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_TIMER_A (defined(__MSP430_HAS_TA2__)      \
                               || defined(__MSP430_HAS_TA3__)   \
                               || defined(__MSP430_HAS_TA5__)   \
                               || defined(__MSP430_HAS_T0A3__)  \
                               || defined(__MSP430_HAS_T0A5__))

/** Defined on inclusion of <bsp430/periph/timer.h>.  The value
 * evaluates to true if the target MCU supports the Timer_B Module,
 * and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_TIMER_B (defined(__MSP430_HAS_TB3__)      \
                               || defined(__MSP430_HAS_TB7__)   \
                               || defined(__MSP430_HAS_T0B3__)  \
                               || defined(__MSP430_HAS_T0B7__))


/** Defined on inclusion of <bsp430/periph/timer.h>.  The value
 * evaluates to true if the target MCU supports the Timer_A or
 * Timer_B, and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_TIMER (BSP430_MODULE_TIMER_A || BSP430_MODULE_TIMER_B)

/* Surprisingly, not every MCU has a Timer_A.  Some have only a Basic
 * Timer (BT). */
#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_TIMER - 0)

/** Define to true to indicate that the application or infrastructure
 * would like to use a timer that supports using ACLK as a
 * capture/compare input signal.  The timer is identified by
 * #BSP430_TIMER_CCACLK_PERIPH_CPPID, and that macro is defined only
 * if such a timer is available on the platform.
 *
 * This timer is used by selecting an alternative clock source, then
 * using the ACLK signal to capture the number of pulses on that
 * source over a known period.  For example, this is used to measure
 * the relative speed of SMCLK and ACLK when manually trimming the FLL
 * in iBSP430ucsTrimDCOCLKDIV_ni().
 *
 * @note Where possible, the selected timer should not conflict with
 * the #BSP430_UPTIME_TIMER_PERIPH_HANDLE.  If this cannot be
 * satisfied, the timer should still be made available; code that is
 * concerned about the possibility of conflict should validate that
 * the timers are different, which can be done at compile time:
 *
 * @code
 * #if BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_TIMER_CCACLK_PERIPH_CPPID
 * #error Cannot use both
 * #endif // uptime == ccaclk
 * @endcode
 *
 * @note Setting this flag only enables the @HPL for
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE.  To delegate control of
 * additional capabilities see #configBSP430_TIMER_CCACLK_HAL,
 * #configBSP430_TIMER_CCACLK_HAL_ISR, and
 * #configBSP430_TIMER_CCACLK_HAL_CC0_ISR.
 *
 * @cppflag
 * @affects #BSP430_TIMER_CCACLK
 * @defaulted
 * @ingroup grp_config_functional */
#ifndef configBSP430_TIMER_CCACLK
#define configBSP430_TIMER_CCACLK 0
#endif /* configBSP430_TIMER_CCACLK */

/** Defined to a true value if ACLK-triggered timer captures are
 * supported due to somebody providing
 * #BSP430_TIMER_CCACLK_PERIPH_CPPID.
 *
 * @note It is recommended that application code use of this feature
 * be done in a non-interruptible context that is run-to-completion,
 * to be sure that no other user is active.  The configuration of the
 * timer as a whole should be considered volatile.  Proper use
 * requires initializing the timer before use and stopping it when
 * finished.
 *
 * @warning If ACLK is configured to source from XT1CLK and the
 * crystal is faulted resulting in an internal fallback to VLOCLK, the
 * capture/compare input remains the LFXT1 signal and no events will
 * occur.  This can be demonstrated on the EXP430G2 LaunchPad.
 *
 * @dependency #configBSP430_TIMER_CCACLK
 * @platformvalue
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** The preprocessor-compatible identifier for a timer that is capable
 * of capturing on ACLK ticks.
 *
 * The define must appear in the @ref bsp430_config subsystem
 * so that functional resource requests are correctly propagated to
 * the underlying resource instances.
 *
 * @defaulted
 * @platformvalue
 * @ingroup grp_timer_ccaclk
 * @affects #BSP430_TIMER_CCACLK_PERIPH_HANDLE */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_PERIPH_CPPID include "bsp430_config.h"
#endif /* BSP430_DOXYGEN */

/** Defined to the peripheral identifier for a timer that can be used
 * to maintain a continuous system clock sourced from ACLK.  This
 * derives directly from #BSP430_TIMER_CCACLK_PERIPH_CPPID, but is a
 * timer peripheral handle suitable for use in code.
 *
 * @ingroup grp_timer_ccaclk
 * @dependency #BSP430_TIMER_CCACLK_PERIPH_CPPID */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE derived platform or application specific
/* !BSP430! instance=@timers functional=timer_ccaclk subst=functional insert=periph_sethandle */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_sethandle] */

#elif BSP430_TIMER_CCACLK_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA0
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TA0

#elif BSP430_TIMER_CCACLK_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA1
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TA1

#elif BSP430_TIMER_CCACLK_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA2
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TA2

#elif BSP430_TIMER_CCACLK_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA3
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TA3

#elif BSP430_TIMER_CCACLK_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB0
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TB0

#elif BSP430_TIMER_CCACLK_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB1
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TB1

#elif BSP430_TIMER_CCACLK_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB2
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TB2
/* END AUTOMATICALLY GENERATED CODE [periph_sethandle] */
/* !BSP430! end=periph_sethandle */
#endif /* BSP430_TIMER_CCACLK_PERIPH_CPPID */

/** Indirect control inclusion of the @HAL interface for #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * Propagated as the default for #configBSP430_HAL_TA0 or other timer
 * peripheral determined by #BSP430_TIMER_CCACLK_PERIPH_CPPID.
 *
 * @cppflag
 * @defaulted
 * @dependentvalue
 * @ingroup grp_config_functional */
#ifndef configBSP430_TIMER_CCACLK_HAL
#define configBSP430_TIMER_CCACLK_HAL indirectly defaulted
#endif /* configBSP430_TIMER_CCACLK_HAL */

/** Indirect control inclusion of the primary @HAL interrupt handler for #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * Propagated as default for #configBSP430_HAL_TA0_ISR or other timer
 * peripheral determined by #BSP430_TIMER_CCACLK_PERIPH_CPPID.
 *
 * @cppflag
 * @defaulted
 * @dependentvalue
 * @ingroup grp_config_functional */
#if defined(BSP430_DOXYGEN)
#define configBSP430_TIMER_CCACLK_HAL_ISR indirectly defaulted
#endif /* BSP430_DOXYGEN */

/** Indirect control inclusion of the CC0 @HAL interrupt handler for #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * Propagated as default for #configBSP430_HAL_TA0_CC0_ISR or other
 * timer peripheral determined by #BSP430_TIMER_CCACLK_PERIPH_CPPID.
 *
 * @cppflag
 * @defaulted
 * @dependentvalue
 * @ingroup grp_config_functional */
#if defined(BSP430_DOXYGEN)
#define configBSP430_TIMER_CCACLK_HAL_CC0_ISR indirectly defaulted
#endif /* BSP430_DOXYGEN */

/** Indirect control inclusion of the @HPL for #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
 *
 * Propagated as default for #configBSP430_HPL_PORT1 or other flag
 * determined by #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID.
 *
 * You would use this if the source for the clock being timed was
 * external to the microcontroller.  See @ref ex_sensors_hh10d for an
 * example.
 *
 * @note Setting this flag only enables the @HPL for
 * #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE.  To delegate control
 * of additional capabilities see
 * #configBSP430_TIMER_CCACLK_CLK_PORT_HAL.  Port ISR capabilities are
 * not available through this interface.
 *
 * @cppflag
 * @defaulted
 * @affects #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
 * @see #configBSP430_TIMER_CCACLK_CLK_PORT_HAL
 * @ingroup grp_config_functional */
#ifndef configBSP430_TIMER_CCACLK_CLK_PORT
#define configBSP430_TIMER_CCACLK_CLK_PORT indirectly defaulted
#endif /* configBSP430_TIMER_CCACLK_CLK_PORT */

/** Indirect control inclusion of the @HAL for #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
 *
 * Propagated as default for #configBSP430_HAL_PORT1 or other flag
 * determined by #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID.
 *
 * You might need to use this to select the peripheral function for
 * the clock source.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_functional */
#ifndef configBSP430_TIMER_CCACLK_CLK_PORT_HAL
#define configBSP430_TIMER_CCACLK_CLK_PORT_HAL indirectly defaulted 0
#endif /* configBSP430_TIMER_CCACLK_CLK_PORT_HAL */

/** Indirect control inclusion of the @HPL for #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE
 *
 * Propagated as default for #configBSP430_HPL_PORT1 or other flag
 * determined by #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID.
 *
 * You would use this if the source for the clock being timed was
 * external to the microcontroller.  See @ref ex_sensors_hh10d for an
 * example.
 *
 * @note Setting this flag only enables the @HPL for
 * #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE.  To delegate control
 * of additional capabilities see
 * #configBSP430_TIMER_CCACLK_CC0_PORT_HAL.  Port ISR capabilities are
 * not available through this interface.
 *
 * @cppflag
 * @defaulted
 * @affects #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE
 * @see #configBSP430_TIMER_CCACLK_CC0_PORT_HAL
 * @ingroup grp_config_functional */
#ifndef configBSP430_TIMER_CCACLK_CC0_PORT
#define configBSP430_TIMER_CCACLK_CC0_PORT indirectly defaulted
#endif /* configBSP430_TIMER_CCACLK_CC0_PORT */

/** Indirect control inclusion of the @HAL for #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE
 *
 * Propagated as default for #configBSP430_HAL_PORT1 or other flag
 * determined by #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID.
 *
 * You might need to use this to select the peripheral function for
 * the clock source.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_functional */
#ifndef configBSP430_TIMER_CCACLK_CC0_PORT_HAL
#define configBSP430_TIMER_CCACLK_CC0_PORT_HAL indirectly defaulted 0
#endif /* configBSP430_TIMER_CCACLK_CC0_PORT_HAL */

/** Indirect control inclusion of the @HPL for #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE
 *
 * Propagated as default for #configBSP430_HPL_PORT1 or other flag
 * determined by #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID.
 *
 * You would use this if the source for the clock being timed was
 * external to the microcontroller.  See @ref ex_sensors_hh10d for an
 * example.
 *
 * @note Setting this flag only enables the @HPL for
 * #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE.  To delegate control
 * of additional capabilities see
 * #configBSP430_TIMER_CCACLK_CC1_PORT_HAL.  Port ISR capabilities are
 * not available through this interface.
 *
 * @cppflag
 * @defaulted
 * @affects #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE
 * @see #configBSP430_TIMER_CCACLK_CC1_PORT_HAL
 * @ingroup grp_config_functional */
#ifndef configBSP430_TIMER_CCACLK_CC1_PORT
#define configBSP430_TIMER_CCACLK_CC1_PORT indirectly defaulted
#endif /* configBSP430_TIMER_CCACLK_CC1_PORT */

/** Indirect control inclusion of the @HAL for #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE
 *
 * Propagated as default for #configBSP430_HAL_PORT1 or other flag
 * determined by #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID.
 *
 * You might need to use this to select the peripheral function for
 * the clock source.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_functional */
#ifndef configBSP430_TIMER_CCACLK_CC1_PORT_HAL
#define configBSP430_TIMER_CCACLK_CC1_PORT_HAL indirectly defaulted 0
#endif /* configBSP430_TIMER_CCACLK_CC1_PORT_HAL */

/** The capture/compare block index within
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE that can use ACLK as an input
 * signal.
 *
 * @warning See cautionary notes at #BSP430_TIMER_VALID_COUNTER_READ_CCIDX.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @defaulted
 * @platformvalue
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_ACLK_CCIDX include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** The capture/compare block input signal bits to be selected in the
 * control word for CC block #BSP430_TIMER_CCACLK_ACLK_CCIDX of
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE to use ACLK as an input signal.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @defaulted
 * @platformvalue
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_ACLK_CCIS include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** The preprocessor-compatible identifier for a port that provides
 * the input clock function for #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * The define must appear in the @ref bsp430_config subsystem so that
 * functional resource requests are correctly propagated to the
 * underlying resource instances.
 *
 * @defaulted
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID platform or application specific
#endif /* BSP430_DOXYGEN */

/** The BSP430 port peripheral corresponding to
 * #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID.
 *
 * @dependency #configBSP430_TIMER_CCACLK_CLK_PORT
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE derived platform or application specific
/* !BSP430! instance=@ports functional=timer_ccaclk_clk_port subst=functional insert=periph_sethandle */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_sethandle] */

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT1
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT2
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT3
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT4
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT5
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT5

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT6
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT6

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT7
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT7

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT8
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT9
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT9

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT10
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT10

#elif BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT11
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT11
/* END AUTOMATICALLY GENERATED CODE [periph_sethandle] */
/* !BSP430! end=periph_sethandle instance=@timers */
#endif /* BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID */

/** The pin on #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE to which the
 * external clock source for #BSP430_TIMER_CCACLK_PERIPH_HANDLE can be
 * connected.
 *
 * @defaulted
 * @platformvalue
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** The preprocessor-compatible identifier for a port that provides
 * an external trigger on CC0 for #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * The define must appear in the @ref bsp430_config subsystem so that
 * functional resource requests are correctly propagated to the
 * underlying resource instances.
 *
 * @defaulted
 * @dependency #configBSP430_TIMER_CCACLK_CC0_PORT
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID platform or application specific
#endif /* BSP430_DOXYGEN */

/** The BSP430 port peripheral corresponding to
 * #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID.
 *
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE derived platform or application specific
/* !BSP430! instance=@ports functional=timer_ccaclk_cc0_port subst=functional insert=periph_sethandle */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_sethandle] */

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT1
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT2
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT3
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT4
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT5
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT5

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT6
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT6

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT7
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT7

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT8
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT9
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT9

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT10
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT10

#elif BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT11
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT11
/* END AUTOMATICALLY GENERATED CODE [periph_sethandle] */
/* !BSP430! end=periph_sethandle instance=@timers */
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID */

/** The pin on #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE to which
 * the external trigger for CC0 on #BSP430_TIMER_CCACLK_PERIPH_HANDLE
 * can be connected.
 *
 * @defaulted
 * @platformvalue
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CC0_PORT_BIT include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** The CCIS setting required to support CC0 on pin
 * #BSP430_TIMER_CCACLK_CC0_PORT_BIT of
 * #BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_HANDLE.
 *
 * The default setting corresponds to CCIxA.
 *
 * @defaulted
 * @ingroup grp_timer_ccaclk */
#ifndef BSP430_TIMER_CCACLK_CC0_CCIS
#define BSP430_TIMER_CCACLK_CC0_CCIS CCIS_0
#endif /* BSP430_TIMER_CCACLK_CC0_CCIS */

/** The preprocessor-compatible identifier for a port that provides
 * an external trigger on CC1 for #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * The define must appear in the @ref bsp430_config subsystem so that
 * functional resource requests are correctly propagated to the
 * underlying resource instances.
 *
 * @defaulted
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID platform or application specific
#endif /* BSP430_DOXYGEN */

/** The BSP430 port peripheral corresponding to
 * #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID.
 *
 * @dependency #configBSP430_TIMER_CCACLK_CC1_PORT
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE derived platform or application specific
/* !BSP430! instance=@ports functional=timer_ccaclk_cc1_port subst=functional insert=periph_sethandle */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_sethandle] */

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT1
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT2
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT3
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT4
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT5
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT5

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT6
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT6

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT7
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT7

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT8
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT8

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT9
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT9

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT10
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT10

#elif BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID == BSP430_PERIPH_CPPID_PORT11
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT11
/* END AUTOMATICALLY GENERATED CODE [periph_sethandle] */
/* !BSP430! end=periph_sethandle instance=@timers */
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID */

/** The pin on #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE to which
 * the external trigger for CC1 on #BSP430_TIMER_CCACLK_PERIPH_HANDLE
 * can be connected.
 *
 * @defaulted
 * @platformvalue
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CC1_PORT_BIT include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** The CCIS setting required to support CC1 on pin
 * #BSP430_TIMER_CCACLK_CC1_PORT_BIT of
 * #BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_HANDLE.
 *
 * The default setting corresponds to CCIxA.
 *
 * @defaulted
 * @ingroup grp_timer_ccaclk */
#ifndef BSP430_TIMER_CCACLK_CC1_CCIS
#define BSP430_TIMER_CCACLK_CC1_CCIS CCIS_0
#endif /* BSP430_TIMER_CCACLK_CC1_CCIS */

/** Provide valid counter reads for timers.
 *
 * The MSP430 timer infrastructure does not guarantee that a read of a
 * timer counter (e.g. using uiBSP430timerSyncCounterRead_ni_())
 * produces a valid result when the timer clock is asynchronous to
 * MCLK.  Where the timer is significantly slower than MCLK a majority
 * vote approach (e.g. using uiBSP430timerAsyncCounterRead_ni_()) can
 * ensure the result is valid, but if the timer runs at a similar
 * speed this may fail.
 *
 * The problem can be avoided by capturing the counter into a capture
 * register instead of reading it directly.  Doing this costs a couple
 * cycles extra when reading the counter, and due to infrastructure
 * limitations prevents use of that capture/compare register for other
 * purposes.
 *
 * Since most asynchronous reads involve access to an LFXT1-based
 * timer while using a much-faster MCLK, and ulBSP430timerCounter_ni()
 * depends on uiBSP430timerSafeCounterRead_ni(), this defaults to
 * enabled so that safe reads are also valid reads.
 *
 * @see #BSP430_TIMER_VALID_COUNTER_READ_CCIDX
 * @defaulted
 */
#ifndef configBSP430_TIMER_VALID_COUNTER_READ
#define configBSP430_TIMER_VALID_COUNTER_READ 1
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */

/* 20130312 TODO REMOVE 20130630 Detect legacy issues */
#ifdef configBSP430_TIMER_SAFE_COUNTER_READ
#warning configBSP430_TIMER_SAFE_COUNTER_READ has been superseded by configBSP430_TIMER_VALID_COUNTER_READ
#endif /* configBSP430_TIMER_SAFE_COUNTER_READ */

/** Capture/compare index reserved for valid reading of timer
 * counters.
 *
 * CC 0 is normally reserved for high-priority capture/compare
 * operations or RTOS scheduling using the dedicated interrupt
 * handler.  It is also used with #BSP430_TIMER_CCACLK_PERIPH_HANDLE
 * by iBSP430ucsTrimDCOCLKDIV_ni().
 *
 * CC 1 is normally reserved for use by
 * #configBSP430_TIMER_CCACLK_CC1_PORT functionality, and is the
 * standard choice for #BSP430_UPTIME_DELAY_CCIDX.
 *
 * Therefore this functionality normally uses CC 2.  To keep the
 * infrastructure as simple as possible, the same index is used for
 * all timers on which valid counter reading is required.  If a timer
 * does not support the requested index, the attempt to enable valid
 * reading will hang.
 *
 * @warning Certain low-end value-line MCUS such as the MSP430G2231 do
 * not have a third capture/compare register.  Platforms targeting
 * those MCUs should provide an alternative definition for this macro
 * in their bsp430_config.h file.
 *
 * @warning If you assign the same value to #BSP430_UPTIME_DELAY_CCIDX
 * and #BSP430_TIMER_VALID_COUNTER_READ_CCIDX, platform initialization
 * will hang attempting to configure the delay alarm.
 *
 * @warning If you have selected #configBSP430_TIMER_CCACLK, make sure
 * your use of #BSP430_TIMER_CCACLK_ACLK_CCIDX does not interfere with
 * the expectation that the configuration of
 * #BSP430_TIMER_VALID_COUNTER_READ_CCIDX is never changed.  Consider
 * adding the following after any use of
 * #BSP430_TIMER_CCACLK_ACLK_CCIDX:
 * @code
 * #if ((configBSP430_TIMER_VALID_COUNTER_READ - 0) \
 *      && (BSP430_TIMER_CCACLK_ACLK_CCIDX == BSP430_TIMER_VALID_COUNTER_READ_CCIDX))
 *    vBSP430timerSafeCounterInitialize_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
 * #endif
 * @endcode
 *
 * @dependency #configBSP430_TIMER_VALID_COUNTER_READ
 * @defaulted
 */
#if defined(BSP430_DOXYGEN) || (configBSP430_TIMER_VALID_COUNTER_READ - 0)
#ifndef BSP430_TIMER_VALID_COUNTER_READ_CCIDX
#define BSP430_TIMER_VALID_COUNTER_READ_CCIDX 2
#endif /* BSP430_TIMER_VALID_COUNTER_READ_CCIDX */
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */

/** Count number of timer transitions over a span of ACLK ticks
 *
 * This function uses the timer capture/compare infrastructure to
 * count clock cycles.  The source for a timer is configured
 * externally (for example, to SMCLK, or to an external source).  The
 * capture/control block @p ccidx is configured based on @p
 * capture_mode.  The code synchronizes with a capture, then loops for
 * @p period captures.  The timer counter is recorded at the start of
 * the first and end of the last cycle.  The capture/control block
 * control is reset, and the difference between first and last
 * counters is returned.
 *
 * The expectation is that the events triggered by the input selection
 * are slow relative to the MCLK and timer sources.  If the timer is
 * slower than the input, the use of the #SCS flag in the
 * implementation will result in the function returning @p count.
 *
 * @note The function does not modify the timer-level configuration;
 * the timer source must be assigned and the timer started prior to
 * invoking this function.
 *
 * @note If #configBSP430_TIMER_VALID_COUNTER_READ is enabled and @p
 * ccidx is equal to #BSP430_TIMER_VALID_COUNTER_READ_CCIDX this
 * function will invoke #vBSP430timerSafeCounterInitialize_ni(@p
 * periph) prior to returning.
 *
 * @param periph the peripheral identifier for the timer on which the
 * capture is to be made.
 *
 * @param ccidx the capture/compare block index to use.  No validity
 * checks on this are made; you must be sure the index is in range for
 * the provided timer.
 *
 * @param capture_mode the edge detection capture specification.  The
 * appropriate value is given by constants in the <msp430.h> header,
 * such as #CM_1.  The passed value is masked to eliminate extraneous
 * bits; if the resulting value does not identify a capture, the
 * function returns immediately with a value of -1.
 *
 * @param ccis the capture/compare input selection.  The appropriate
 * value is a constant from the <msp430.h> header, such as #CCIS_1.
 * Consult the MCU-specific datasheet to determine the functions of
 * the input alternatives for each timer/CC-block pair.
 *
 * @param count the number of capture events over which the delta is
 * measured.
 *
 * @return -1 (cast to unsigned int) if @p capture_mode is not valid
 * or the timer is unrecognized or stopped.  Otherwise the delta in
 * the counter of the timer over @p count captures. */
unsigned int uiBSP430timerCaptureDelta_ni (tBSP430periphHandle periph,
                                           int ccidx,
                                           unsigned int capture_mode,
                                           unsigned int ccis,
                                           unsigned int count);

/** Layout for Timer_A and Timer_B peripherals.
 */
typedef struct sBSP430hplTIMER {
  unsigned int ctl;             /**< Control register (TA0CTL) */
  union {
    unsigned int cctl[7];       /**< Capture/Compare Control registers (indexed) */
    struct {
      unsigned int cctl0;       /**< Tx#CCTL0 */
      unsigned int cctl1;       /**< Tx#CCTL1 */
      unsigned int cctl2;       /**< Tx#CCTL2 */
      unsigned int cctl3;       /**< Tx#CCTL3 */
      unsigned int cctl4;       /**< Tx#CCTL4 */
      unsigned int cctl5;       /**< TB#CCTL5 */
      unsigned int cctl6;       /**< TB#CCTL6 */
    };
  };
  unsigned int r;               /**< Raw timer counter (TA0R) */
  union {
    unsigned int ccr[7];        /**< Capture/Compare registers (indexed) */
    struct {
      unsigned int ccr0;        /**< Tx#CCR0 */
      unsigned int ccr1;        /**< Tx#CCR1 */
      unsigned int ccr2;        /**< Tx#CCR2 */
      unsigned int ccr3;        /**< Tx#CCR3 */
      unsigned int ccr4;        /**< Tx#CCR4 */
      unsigned int ccr5;        /**< TB#CCR5 */
      unsigned int ccr6;        /**< TB#CCR6 */
    };
  };
} sBSP430hplTIMER;

/** @cond DOXYGEN_INTERNAL
 *
 * What we're doing here is providing the base addresses for the
 * peripheral register maps in a manner that ignores the number of
 * CCs present in an instance. */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
/* Find the relevant tests with the following command over the
 * upstream include files in the msp430mcu package::

  for i in TA T0A T1A T2A T3A TB T0B T1B T2B T3B ; do
     grep -h BASEADDRESS_${i} * \
       | sort \
       | uniq ;
  done

 * Use the alternative with the most CCs as the default.  If the MCU
 * doesn't support it but a user references the handle, the name will
 * be unresolved at compile time, which is a fair error message.
 */
#if defined(__MSP430_HAS_T0A3__)
#define BSP430_PERIPH_TA0_BASEADDRESS_ __MSP430_BASEADDRESS_T0A3__
#else /* TA0 */
#define BSP430_PERIPH_TA0_BASEADDRESS_ __MSP430_BASEADDRESS_T0A5__
#endif /* TA0 */

#if defined(__MSP430_HAS_T1A2__)
#define BSP430_PERIPH_TA1_BASEADDRESS_ __MSP430_BASEADDRESS_T1A2__
#else /* TA1 */
#define BSP430_PERIPH_TA1_BASEADDRESS_ __MSP430_BASEADDRESS_T1A3__
#endif /* TA1 */

#if defined(__MSP430_HAS_T2A2__)
#define BSP430_PERIPH_TA2_BASEADDRESS_ __MSP430_BASEADDRESS_T2A2__
#else /* TA2 */
#define BSP430_PERIPH_TA2_BASEADDRESS_ __MSP430_BASEADDRESS_T2A3__
#endif /* TA2 */

#if defined(__MSP430_HAS_T3A2__)
#define BSP430_PERIPH_TA3_BASEADDRESS_ __MSP430_BASEADDRESS_T3A2__
#else /* TA3 */
#define BSP430_PERIPH_TA3_BASEADDRESS_ __MSP430_BASEADDRESS_T3A3__
#endif /* TA3 */

#if defined(__MSP430_HAS_T0B3__)
#define BSP430_PERIPH_TB0_BASEADDRESS_ __MSP430_BASEADDRESS_T0B3__
#else /* TB0 */
#define BSP430_PERIPH_TB0_BASEADDRESS_ __MSP430_BASEADDRESS_T0B7__
#endif /* TB0 */

#define BSP430_PERIPH_TB1_BASEADDRESS_ __MSP430_BASEADDRESS_T1B3__
#define BSP430_PERIPH_TB2_BASEADDRESS_ __MSP430_BASEADDRESS_T2B3__

#else /* MSP430XV2 */

/* Non-5xx MCU headers don't provide peripheral base addresses.
 * Fortunately, all the variants are at the same address in these
 * families (in 5xx there are a couple chips with different addresses
 * for TA0). */
#define BSP430_PERIPH_TA0_BASEADDRESS_ 0x0160
/* In some chips (e.g., msp430g2553) there are two Timer_As, and the
 * second is located at the same address as Timer_B in other chips in
 * the family.  There are no MCUs of this type that have a Timer_B. */
#define BSP430_PERIPH_TA1_BASEADDRESS_ 0x0180
#define BSP430_PERIPH_TB0_BASEADDRESS_ 0x0180
#endif /* MSP430XV2 */

/* The other thing we're doing here is adapting old-style vector
 * macros for legacy MCUs with outdated headers. */
#if defined(TIMERA0_VECTOR) && ! defined(TIMER0_A0_VECTOR)
#define TIMER0_A0_VECTOR TIMERA0_VECTOR
#endif /* T0A CC0 */
#if defined(TIMERA1_VECTOR) && ! defined(TIMER0_A1_VECTOR)
#define TIMER0_A1_VECTOR TIMERA1_VECTOR
#endif /* T0A CC1-2 TA */
#if defined(TIMERB0_VECTOR) && ! defined(TIMER0_B0_VECTOR)
#define TIMER0_B0_VECTOR TIMERB0_VECTOR
#endif /* T0B CC0 */
#if defined(TIMERB1_VECTOR) && ! defined(TIMER0_B1_VECTOR)
#define TIMER0_B1_VECTOR TIMERB1_VECTOR
#endif /* T0B CC1-2 TB */

/** @endcond */ /* DOXYGEN_INTERNAL */

/** Field value for variant stored in #sBSP430halTIMER field
 * hal_state.cflags when HPL reference is to an #sBSP430hplTIMER. */
#define BSP430_TIMER_HAL_HPL_VARIANT_TIMER 1

/** Bit set in sBSP430halTIMER::hal_state @c flags to indicate a timer
 * is known to be synchronous to the system clock.  In this case a
 * faster counter read approach (uiBSP430timerSyncCounterRead_ni())
 * can be used.
 *
 * @note */
#define BSP430_TIMER_FLAG_MCLKSYNC 0x01

/** Bit set in sBSP430halTIMER::hal_state @c flags to indicate a timer
 * is known to be significantly slower than the system clock.  In this
 * case a faster counter read approach
 * (iBSP430timerAsyncCounterRead_ni()) can be used with asynchronous
 * timers.
 *
 * @note The BSP430 infrastructure infers this flag only if the clock
 * is at least a factor of 64x slower than MCLK. */
#define BSP430_TIMER_FLAG_SLOW 0x02

/** Structure holding hardware abstraction layer state for Timer_A and
 * Timer_B. */
typedef struct sBSP430halTIMER {
  /** Common header used to extract the correct HPL pointer type from
   * the hpl union and to hold hints about the timer's
   * configuration. */
  sBSP430hplHALStatePrefix hal_state;

  /** The underlying timer peripheral register structure */
  volatile sBSP430hplTIMER * const hpl;

  /** The number of times the timer has wrapped.
   *
   * The value is maintained only if the corresponding HAL ISR is
   * enabled for the timer and the overflow interrupt is explicitly
   * enabled by the application.  It is also only accurate if
   * interrupts are enabled, as the value is incremented by the
   * interrupt handler itself.
   *
   * @note This field is not marked volatile because doing so costs
   * several extra instructions due to it being a multi-word value.
   * It should be read and written only when interrupts are
   * disabled. */
  unsigned long overflow_count;

  /** The callback chain to invoke when an overflow interrupt is
   * received.  @note This pointer, and the pointers for any
   * #sBSP430halISRVoidChainNode.next_ni fields in chain nodes accessed
   * through it, must be mutated only when interrupts are disabled. */
  const struct sBSP430halISRVoidChainNode * volatile overflow_cbchain_ni;

  /** The callback chain to invoke when a CCx interrupt is received.
   *
   * The chains are independent for each capture/compare block, but
   * the block index is passed into the chain so that a common handler
   * can be invoked if desired.  The chain for CC0 is accessed only if
   * the corresponding ISR is enabled (e.g.,
   * #configBSP430_HAL_TA0_CC0_ISR) */
  const struct sBSP430halISRIndexedChainNode * volatile * const cc_cbchain_ni;
} sBSP430halTIMER;

/** The timer internal state is protected. */
typedef struct sBSP430halTIMER * hBSP430halTIMER;

/** Infer the appropriate hints about timer configuration
 *
 * Some timer operations such as safe and valid reading of the counter
 * can be optimized by selecting between alternative techniques based
 * on configuration hints including #BSP430_TIMER_FLAG_MCLKSYNC and
 * #BSP430_TIMER_FLAG_SLOW.
 *
 * Invoking this function will determine the appropriate value of the
 * @c flags field of the timer's sBSP430hplHALStatePrefix::hal_state
 * structure:
 *
 * @li The function infers #BSP430_TIMER_FLAG_MCLKSYNC when the clock
 * source for the timer is equal to the clock source for MCLK, based
 * on xBSP430timerClockSource() and xBSP430clockMCLKSource().  Be
 * aware that #eBSP430clockSRC_DCOCLK is not considered synchronous
 * with #eBSP430clockSRC_DCOCLKDIV under this rule.
 *
 * @li The function infers #BSP430_TIMER_FLAG_SLOW when MCLK
 * frequency is at least 16 times faster than timer frequency.
 *
 * @param timer the timer for which flags are to be determined
 */
void vBSP430timerInferHints_ni (hBSP430halTIMER timer);

/** Return a reconstructed source for the timer.
 *
 * @note This should be used only in equality tests to determine
 * whether the source for a timer is the same as the source for a
 * system clock or another timer.  Do not interpret the value as
 * identifying a specific source.  For example, sometimes
 * #eBSP430clockSRC_XT2CLK will mean a crystal and sometimes it will
 * mean an external clock or VLOCLK, depending on the MCU and its
 * supporting hardware. */
eBSP430clockSource xBSP430timerClockSource (volatile sBSP430hplTIMER * hpl);

/** Provide the frequency of the timer source, if that can be determined.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return 0 if the timer is stopped; -1 (cast to unsigned long) if
 * the frequency cannot be determined; otherwise the frequency of the
 * source scaled by any relevant dividers. */
unsigned long ulBSP430timerFrequency_Hz_ni (tBSP430periphHandle periph);

/** Return the adjusted overflow counter for the timer and a 16-bit counter value.
 *
 * This is @link sBSP430halTIMER::overflow_count
 * timer->overflow_count@endlink, adjusted to account for any
 * registered but unhandled overflow event.  If @p ctr is greater than
 * 32767 it is assumed the unhandled overflow event occurred after the
 * counter was read and no adjustment is made.
 *
 * @param timer the timer for which the overflow counter is desired.
 *
 * @param ctr A recently obtained 16-bit counter value from the timer.
 * Interrupts must not have been enabled at any point since the
 * counter was read, and this function must be invoked with one half
 * of the timer's period.
 *
 * @return the adjusted overflow counter for @p timer */
unsigned long ulBSP430timerOverflowAdjusted_ni (hBSP430halTIMER timer,
                                                unsigned int ctr);

/** Read the timer counter assuming interrupts are disabled.
 *
 * @note This routine accounts for the possibility of a single pending
 * overflow event in the timer by adjusting the returned value to
 * account for that.  However, it does not clear the overflow event
 * because there may be overflow callbacks that must be invoked
 * through the interrupt handler, and it only detects a single pending
 * overflow event.  Times will be wrong if interrupts remain disabled
 * so long that a second overflow occurs before the first is processed
 * by the interrupt handler.
 *
 * @warning This routine uses uiBSP430timerSafeCounterRead_ni()
 * underneath.  For timers that are asynchronous to the CPU clock the
 * returned value may be incorrect unless
 * #configBSP430_TIMER_VALID_COUNTER_READ is enabled.
 *
 * @warning If #configBSP430_TIMER_VALID_COUNTER_READ is enabled this
 * call will hang if vBSP430timerSafeCounterInitialize_ni() has not
 * been invoked on the underlying timer or if the
 * #BSP430_TIMER_VALID_COUNTER_READ_CCIDX capture/compare register has
 * been reconfigured since that was done.  BSP430 will invoke this
 * method at configuration time for all timers that it configures
 * (including the @link bsp430/utility/uptime.h uptime
 * infrastructure@endlink and hBSP430timerAlarmInitialize()).
 *
 * @param timer The timer for which the count is desired.
 *
 * @param overflowp An optional pointer in which the high word of the
 * overflow counter is stored, supporting a 48-bit counter.
 *
 * @return A 32-bit unsigned count of the number of clock ticks
 * observed since the timer was last reset. */
unsigned long ulBSP430timerCounter_ni (hBSP430halTIMER timer,
                                       unsigned int * overflowp);

/** Read timer counter regardless of interrupt enable state.
 *
 * This wraps #ulBSP430timerCounter_ni with code that saves the
 * interrupt state, reads the timer, then restores the entry interrupt
 * state prior to returning the timer counts.
 *
 * @warning See warnings at ulBSP430timerCounter_ni().
 */
static BSP430_CORE_INLINE
unsigned long
ulBSP430timerCounter (hBSP430halTIMER timer,
                      unsigned int * overflowp)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  unsigned long rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = ulBSP430timerCounter_ni(timer, overflowp);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Read a timer capture register.
 *
 * Capture/compare registers may be set to record the time of an
 * external event.  The 16-bit captured value is held in
 * sBSP430hplTIMER::ccr, but for many cases a 32-bit counter value is
 * needed.  This function combines the 16-bit value with the result of
 * ulBSP430timerOverflowAdjusted_ni() to produce a 32-bit value under
 * the assumption that the combination is calculated less than 32768
 * ticks past the capture event.
 *
 * @param timer The timer for which the count is desired.
 *
 * @param ccidx The capture/compare register from which the low 16
 * bits are taken.
 *
 * @return A 32-bit unsigned count of the number of clock ticks
 * at the point where the capture register was set. */
unsigned long ulBSP430timerCaptureCounter_ni (hBSP430halTIMER timer,
                                              unsigned int ccidx);

/** Reset the timer counter.
 *
 * This clears both the overflow count and the timer internal counter.
 * If the timer is running it is stopped while the internal counter is
 * updated, then is restarted. */
void vBSP430timerResetCounter_ni (hBSP430halTIMER timer);

/** Set the timer counter.
 *
 * This sets both the overflow count and the timer internal counter.
 * If the timer is running it is stopped while the internal counter is
 * updated, then is restarted.
 *
 * @param timer the timer to be modified
 *
 * @param overflow the overflow value as would be returned though a
 * pointer assignment by vBSP430timerCounter_ni().
 *
 * @param counter the counter value that would be returned as a value
 * by vBSP430timerCounter_ni().
 */
void vBSP430timerSetCounter_ni (hBSP430halTIMER timer,
                                unsigned int overflow,
                                unsigned long counter);

/* Forward declaration */
struct sBSP430timerAlarm;

/** A handle to an alarm structure.
 *
 * @note The reference is to a @c const #sBSP430timerAlarm because
 * user code should never manipulate the alarm configuration
 * directly.
 *
 * @ingroup grp_timer_alarm */
typedef const struct sBSP430timerAlarm * hBSP430timerAlarm;

/** Callback for alarm events.
 *
 * This function will be invoked by the timer infrastructure in an
 * interrupt context when the alarm goes off.  The implementation is
 * permitted to invoke iBSP430timerAlarmSet_ni() to schedule a new
 * alarm.  If this is not done, the alarm will automatically be
 * disabled when the callback returns.
 *
 * @note Because @p cfg is a pointer to the underlying
 * #sBSP430timerAlarm structure, the techniques of @ref
 * callback_appinfo may be used to provide additional parameters for
 * use by the callback function.
 *
 * @param cfg The structure describing the alarm configuration.
 *
 * @return As with iBSP430halISRCallbackVoid_ni().
 *
 * @ingroup grp_timer_alarm */
typedef int (* iBSP430timerAlarmCallback_ni) (hBSP430timerAlarm alarm);

/** Bit set in sBSP430timerAlarm::flags if the alarm is currently set.
 *
 * @ingroup grp_timer_alarm */
#define BSP430_TIMER_ALARM_FLAG_SET 0x01

/** Bit set in sBSP430timerAlarm::flags if the alarm is enabled.
 *
 * @ingroup grp_timer_alarm */
#define BSP430_TIMER_ALARM_FLAG_ENABLED 0x02

/** A structure holding information related to timer-based alarms.
 *
 * @warning The contents of this structure must not be manipulated by
 * user code at any time.  The primary reason the internals are
 * exposed is so that alarm structures can be statically allocated.
 * Fields may be inspected, though only sBSP430timerAlarm::flags and
 * sBSP430timerAlarm::setting_tck are likely to be of great interest.
 *
 * @ingroup grp_timer_alarm */
typedef struct sBSP430timerAlarm {
  /** The timer registered as owner of this alarm structure. */
  hBSP430halTIMER timer;

  /** The capture/compare resource used by the alarm.
   *
   * @note This field is initialized by iBSP430timerAlarmInitialize()
   * and must not be manipulated by user code. */
  unsigned char ccidx;

  /** Assorted flags recording alarm state.  Bit values
   * #BSP430_TIMER_ALARM_FLAG_SET and #BSP430_TIMER_ALARM_FLAG_ENABLED
   * may be inspected; other bits are reserved for internal use.
   *
   * @note This field is maintained by the infrastructure and must not
   * be manipulated by user code. */
  unsigned char flags;

  /** The absolute time as determined by @a timer at which the alarm
   * fired (when the @a callback is invoked), or at which the next
   * alarm should fire (when returning from @a callback with
   * #BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT cleared).  This is the
   * scheduled time of the alarm, not the time at which the callback
   * is entered which is likely to be later due to processing
   * overhead.
   *
   * @note This field is maintained by iBSP430timerAlarmSet_ni() and
   * must not be manipulated by user code. */
  unsigned long setting_tck;

  /** The function invoked by the infrastructure when the alarm goes
   * off.  If this is a null pointer, the infrastructure will act as
   * though it was a function that did nothing but return
   * #BSP430_HAL_ISR_CALLBACK_EXIT_LPM.
   *
   * @note This field is initialized by iBSP430timerAlarmInitialize()
   * and must not be manipulated by user code. */
  iBSP430timerAlarmCallback_ni callback_ni;

  /** The callback chain node used when the alarm must be hooked into
   * a timer overflow chain.
   *
   * @note This field is initialized by iBSP430timerAlarmInitialize()
   * and maintained by iBSP430timerAlarmSetEnabled_ni(), and must not
   * be manipulated by user code. should not be manipulated by user
   * code. */
  sBSP430halISRVoidChainNode overflow_cb;

  /** The callback chain node used when the alarm must be hooked into
   * a capture/compare chain.
   *
   * @note This field is initialized by iBSP430timerAlarmInitialize()
   * and maintained by iBSP430timerAlarmSetEnabled_ni(), and must not
   * be manipulated by user code. should not be manipulated by user
   * code. */
  sBSP430halISRIndexedChainNode cc_cb;
} sBSP430timerAlarm;

/** Initialize an alarm structure.
 *
 * This routine configures the @p alarm structure so it is prepared to
 * operate using capture/compare register @p ccidx of the timer
 * peripheral identified by @p periph.  This involves setting various
 * fields inside the @p alarm structure; those settings should never
 * be manipulated by user code.
 *
 * Note that this simply initializes the structure; it does not set
 * any alarms, nor does it enable the alarm.  @p ccidx for @p periph
 * may be reused for another purpose, or re-initialized for a new
 * alarm role, as long as the alarm has first been disabled.
 *
 * @param alarm a pointer to the structure holding alarm configuration
 * data.
 *
 * @param periph the handle identifier, such as
 * #BSP430_UPTIME_TIMER_PERIPH_HANDLE.
 *
 * @param ccidx the capture/compare index within the timer.  It is the
 * user's responsibility to ensure that the selected timer supports
 * the selected capture/compare register.
 *
 * @warning If #configBSP430_TIMER_VALID_COUNTER_READ is enabled
 * attempts to configure an alarm on
 * #BSP430_TIMER_VALID_COUNTER_READ_CCIDX will be rejected.
 *
 * @param callback the callback function to be invoked when the alarm
 * goes off.  This may be a null pointer, in which case the
 * infrastructure will behave as though a registered callback did
 * nothing but return #BSP430_HAL_ISR_CALLBACK_EXIT_LPM.
 *
 * @return A non-null handle for the alarm.  A null handle will be
 * returned if initialization failed, e.g. because @p periph could not
 * be identified as a timer with a HAL supporting an alarm.
 *
 * @ingroup grp_timer_alarm */
hBSP430timerAlarm hBSP430timerAlarmInitialize (sBSP430timerAlarm * alarm,
                                               tBSP430periphHandle periph,
                                               int ccidx,
                                               iBSP430timerAlarmCallback_ni callback);

/** Enable or disable an alarm.
 *
 * The effect of this is to hook the alarm into the necessary
 * interrupt callbacks.  Enabling the alarm does not cause it to be
 * set; disabling does cause a set alarm to be cancelled.
 *
 * During the times an alarm is enabled its callbacks are linked into
 * the relevant interrupt callback chains.  This can introduce
 * overhead, so alarms should be explicitly disabled during periods
 * when they are not active.
 *
 * @warning Do not enable or disable alarms from inside interrupt
 * handlers or alarm callbacks.
 *
 * @param alarm the alarm that is being configured
 *
 * @param enablep nonzero if the alarm is to be enabled; zero if it is
 * to be disabled.
 *
 * @return 0 if setting was successful; -1 if an error occurs.
 *
 * @ingroup grp_timer_alarm */
int iBSP430timerAlarmSetEnabled_ni (hBSP430timerAlarm alarm,
                                    int enablep);

/** Enable an initialized alarm.
 *
 * A wrapper around iBSP430timerAlarmSetEnabled_ni() suitable for use
 * when interrupts are enabled.
 *
 * This hooks the alarm's callbacks into the timer infrastructure.  It
 * does not set the alarm.  Alarms must be enabled before they can be
 * set, and initialized before they can be enabled.
 *
 * @ingroup grp_timer_alarm */
static BSP430_CORE_INLINE
int iBSP430timerAlarmEnable (hBSP430timerAlarm alarm)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerAlarmSetEnabled_ni(alarm, 1);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Disable an enabled alarm.
 *
 * A wrapper around iBSP430timerAlarmSetEnabled_ni() suitable for use
 * when interrupts are enabled.
 *
 * This removes the alarm's callbacks from the timer infrastructure.
 * Any scheduled alarm will be canceled prior to disabling it.
 *
 * @ingroup grp_timer_alarm */
static BSP430_CORE_INLINE
int iBSP430timerAlarmDisable (hBSP430timerAlarm alarm)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerAlarmSetEnabled_ni(alarm, 0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Time limit for future alarms.
 *
 * If the requested time is less than this many ticks in the future,
 * iBSP430timerAlarmSet_ni() will not schedule the alarm and will
 * returned #BSP430_TIMER_ALARM_SET_NOW.  Events at least this many
 * ticks in the future will be scheduled.
 *
 * @defaulted
 * @ingroup grp_timer_alarm */
#ifndef BSP430_TIMER_ALARM_FUTURE_LIMIT
#define BSP430_TIMER_ALARM_FUTURE_LIMIT 3UL
#endif /* BSP430_TIMER_ALARM_FUTURE_LIMIT */

/** Value returned by iBSP430timerAlarmSet_ni() when the requested
 * time is too near for the scheduling to be reliable.  See
 * #BSP430_TIMER_ALARM_FUTURE_LIMIT.
 *
 * @ingroup grp_timer_alarm */
#define BSP430_TIMER_ALARM_SET_NOW 1

/** Value returned by iBSP430timerAlarmSet_ni() when the requested
 * time appears to be in the past.
 *
 * @ingroup grp_timer_alarm */
#define BSP430_TIMER_ALARM_SET_PAST 2

/** Value returned by iBSP430timerAlarmSet_ni() when the alarm was
 * already scheduled.
 *
 * @ingroup grp_timer_alarm */
#define BSP430_TIMER_ALARM_SET_ALREADY -2

/** Set the alarm to go off at the specified time.
 *
 * This function may be invoked in normal user code, or within an
 * alarm callback or other interrupt handler to reschedule the alarm.
 *
 * @note The alarm is set only if the return value is zero.
 *
 * @param alarm a pointer to an alarm structure initialized using
 * iBSP430timerAlarmInitialize().
 *
 * @param setting_tck the time at which the alarm should go off.  The
 * value should be no more than 2^31 ticks ahead of the current time
 * or it will be assumed to specify a past time.
 *
 * @return
 * @li Zero to indicate the alarm was successfully scheduled;
 * @li The positive value #BSP430_TIMER_ALARM_SET_NOW if @p
 * setting_tck is in the future but too near for the alarm
 * infrastructure to guarantee delivery of the alarm event;
 * @li The positive value #BSP430_TIMER_ALARM_SET_PAST if @p
 * setting_tck appears to be in the past;
 * @li The negative value #BSP430_TIMER_ALARM_SET_ALREADY if the alarm
 * was already scheduled;
 * @li other negative values indicating specific or generic errors.
 *
 * @ingroup grp_timer_alarm */
int iBSP430timerAlarmSet_ni (hBSP430timerAlarm alarm,
                             unsigned long setting_tck);

/** Forced an alarm at the specified time even if it is late.
 *
 * Like iBSP430timerAlarmSet_ni() except that a positive error
 * (#BSP430_TIMER_ALARM_SET_NOW or #BSP430_TIMER_ALARM_SET_PAST)
 * forces the alarm to be configured and the interrupt set to execute
 * as soon as possible.
 *
 * This is useful when:
 *
 * @li initially configuring an alarm for system timeslices, where the
 * underlying timer has already been started and what would normally
 * be the first tick may have already passed; or
 *
 * @li using a high-speed (MCLK-rate) timer when intervening activity
 * may delay the scheduling.
 *
 * @note The alarm is set only if the return value is non-negative.
 * For positive return values the alarm interrupt may already be
 * pending.
 *
 * @param alarm a pointer to an alarm structure initialized using
 * iBSP430timerAlarmInitialize().
 *
 * @param setting_tck the time at which the alarm should go off.
 *
 * @return
 * @li Zero to indicate the alarm was successfully scheduled
 */
int iBSP430timerAlarmSetForced_ni (hBSP430timerAlarm alarm,
                                   unsigned long setting_tck);

/** Wrapper to invoke iBSP430timerAlarmSet_ni() when interrupts are
 * enabled.
 *
 * @ingroup grp_timer_alarm */
static BSP430_CORE_INLINE
int iBSP430timerAlarmSet (hBSP430timerAlarm alarm,
                          unsigned long setting_tck)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerAlarmSet_ni(alarm, setting_tck);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Cancel a scheduled alarm event.
 *
 * This disconnects the scheduled alarm and inhibits any pending alarm
 * from being executed.  It may be executed from user code or from
 * within an alarm callback or other interrupt handler.
 *
 * @param alarm a pointer to an alarm structure initialized using
 * iBSP430timerAlarmInitialize().
 *
 * @return A non-negative value if the cancellation was effective
 * disabled.  The value is 1 if the alarm had been set, and zero if it
 * had not been set (e.g., the alarm had already fired).  A negative
 * value indicates an error such as an invalid @p alarm value, or the
 * alarm not being enabled.
 *
 * @ingroup grp_timer_alarm */
int iBSP430timerAlarmCancel_ni (hBSP430timerAlarm alarm);

/** Wrapper to invoke iBSP430timerAlarmCancel_ni() when interrupts are
 * enabled.
 *
 * @ingroup grp_timer_alarm */
static BSP430_CORE_INLINE
int iBSP430timerAlarmCancel (hBSP430timerAlarm alarm)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerAlarmCancel_ni(alarm);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/* Forward declaration */
struct sBSP430timerMuxAlarm;

/* Forward declaration */
struct sBSP430timerMuxSharedAlarm;

/** Callback used for multiplexed timers.
 *
 * This is invoked by the shared alarm when the time specified by
 * @link sBSP430timerMuxAlarm::setting_tck alarm->setting_tck@endlink
 * has been reached.  When this is invoked @p alarm has been removed
 * from the list of alarms associated with @shared.
 *
 * It is permitted to invoke iBSP430timerMuxAlarmSet_ni() from this
 * callback to re-associate @p alarm with @p shared.  Prior to doing
 * this the @p alarm->setting_tck value should be updated.
 *
 * @ingroup grp_timer_alarm
 */
typedef int (* iBSP430timerMuxAlarmCallback_ni) (struct sBSP430timerMuxSharedAlarm * shared,
                                                 struct sBSP430timerMuxAlarm * alarm);

/** Structure holding multiplexed alarm shared data.
 * @ingroup grp_timer_alarm */
typedef struct sBSP430timerMuxSharedAlarm {
  /** The underlying dedicated alarm that manages the multiplexed
   * alarms.
   *
   * Under normal circumstances this alarm remains enabled between
   * hBSP430timerMuxAlarmStartup() and iBSP430timerMuxAlarmShutdown().
   * The contents of this field should not be manipulated by user
   * code.  */
  sBSP430timerAlarm dedicated;

  /** The active multiplexed alarms, sorted by time with earliest
   * wakeup first. */
  struct sBSP430timerMuxAlarm * alarms;
} sBSP430timerMuxSharedAlarm;

/** Handle for the underlying shared alarm for multiplixed alarms.
 * @ingroup grp_timer_alarm */
typedef sBSP430timerMuxSharedAlarm * hBSP430timerMuxSharedAlarm;

/** Structure holding data for an individual multiplexed alarm.
 * Instances of the structure are adopted by a shared alarm when added
 * using iBSP430timerMuxAlarmAdd_ni(), and must not be mutated until
 * the corresponding callback is invoked or the alarm is cancelled.
 * @ingroup grp_timer_alarm */
typedef struct sBSP430timerMuxAlarm {
  /** The time, in absolute ticks of the associated @link
   * hBSP430timerMuxSharedAlarm shared alarm@endlink, at which the
   * alarm is due to fire.
   *
   * The value must be set by the user prior to invoking
   * iBSP430timerMuxAlarmAdd_ni(), and must not be modified until the
   * alarm is cancelled or has fired. */
  unsigned long setting_tck;

  /** The callback to be invoked when the alarm goes off. */
  iBSP430timerMuxAlarmCallback_ni callback_ni;

  /** A link to the next alarm in a chain.
   *
   * User code is only permitted to use this field when the structure
   * is not owned by a shared multiplex alarm. */
  struct sBSP430timerMuxAlarm * next;
} sBSP430timerMuxAlarm;

/** Handle for an individual multiplixed alarm.
 * @ingroup grp_timer_alarm */
typedef sBSP430timerMuxAlarm * hBSP430timerMuxAlarm;

/** Configure for a multiplexed alarm.
 *
 * hBSP430timerAlarmInitialize() is used to initialize the dedicated
 * timer capture/compare interrupt associated with @p periph and @p
 * ccidx, and the resulting alarm is enabled.  The list of shared
 * alarms is initialized to be empty.
 *
 * @see iBSP430timerMuxAlarmShutdown()
 *
 * @param shared a pointer to a structure that holds the underlying
 * dedicated alarm data along with a list of active alarms.
 *
 * @param periph the timer peripheral used to control the alarm.
 *
 * @param ccidx the capture/compare index within @p periph that is to be used for the shared alarms.
 *
 * @return a handle for the shared alarm, or a null pointer if an error is detected.
 *
 * @ingroup grp_timer_alarm */
hBSP430timerMuxSharedAlarm hBSP430timerMuxAlarmStartup (sBSP430timerMuxSharedAlarm * shared,
                                                        tBSP430periphHandle periph,
                                                        int ccidx);

/** Disable multiplexed alarms.
 *
 * The underlying shared alarm is cancelled and disabled.  User code
 * may inspect and manipulate the remaining alarms to process any
 * unexpired timers.
 *
 * @param shared a pointer to the structure used for multiplexed alarms
 *
 * @return as with iBSP430timerAlarmDisable_ni()
 *
 * @ingroup grp_timer_alarm */
int iBSP430timerMuxAlarmShutdown (sBSP430timerMuxSharedAlarm * shared);

/** Link a new alarm into the list managed by @p shared
 *
 * The user must have already initialized the @p alarm structure
 * including its callback and setting.  @p alarm is linked into the
 * list of alarms managed by @p shared, and the underlying timer is
 * configured to wake when the first alarm is due.
 *
 * The underlying dedicated alarm sets its wakeup using
 * iBSP430timerAlarmSetForced_ni() so that delays resulting from slow
 * processing will still result in the alarm being set even if it is
 * overdue.
 *
 * @param shared the shared alarm that manages multiplexed alarms.
 *
 * @param alarm information on the alarm to be set.  The structure
 * must not already be in an alarm list.  The contents pointed to by
 * this handle must not be manipulated by the user until the alarm
 * fires or is cancelled via iBSP430timerMuxAlarmRemove_ni().
 *
 * @return Normally the return value from
 * iBSP430timerAlarmSetForced_ni() when setting for the first
 * multiplexed alarm.  If a negative value appears, the multiplexed
 * alarm structure is in an undefined state.
 *
 * @ingroup grp_timer_alarm */
int iBSP430timerMuxAlarmAdd_ni (hBSP430timerMuxSharedAlarm shared,
                                hBSP430timerMuxAlarm alarm);

/** Remove an alarm from a shared list.
 *
 * The alarm is removed from the list.  If any alarms remain, @p
 * shared is updated to fire when the next alarm is due.  It is
 * guaranteed that the removed alarm will not fire after this function
 * has been invoked.
 *
 * @param shared the shared alarm that manages multiplexed alarms.
 *
 * @param alarm the alarm to be removed.
 *
 * @return Normally the return value from
 * iBSP430timerAlarmSetForced_ni() when setting for the next scheduled
 * multiplexed alarm.  Zero is returned if no alarms remain.  If a
 * negative value appears, the multiplexed alarm structure is in an
 * undefined state.
 *
 * @ingroup grp_timer_alarm */
int iBSP430timerMuxAlarmRemove_ni (hBSP430timerMuxSharedAlarm shared,
                                   hBSP430timerMuxAlarm alarm);

/** Bit set in sBSP430timerPulseCapture::flags_ni if the
 * sBSP430timerPulseCapture::start_tt timestamp corresponds to the
 * start of a pulse on a timer input. */
#define BSP430_TIMER_PULSECAP_START_VALID 0x01

/** Bit set in sBSP430timerPulseCapture::flags_ni if the
 * sBSP430timerPulseCapture::end_tt timestamp corresponds to the end
 * of a pulse on a timer input. */
#define BSP430_TIMER_PULSECAP_END_VALID 0x02

/** Bit set in sBSP430timerPulseCapture::flags_ni if any of these
 * conditions hold:
 *
 * @li the underlying timer recorded an overflow event (hardware #COV);
 * @li the underlying timer data was overwritten before the FLIH could extract it;
 * @li a new transition was detected after the pulse end before the state was reset via
 * vBSP430timerPulseCaptureClear (software).
 *
 * If this bit is set, neither counter values nor the
 * #BSP430_TIMER_PULSECAP_ACTIVE_HIGH bit are reliable. */
#define BSP430_TIMER_PULSECAP_OVERFLOW 0x04

/** Bit set in sBSP430timerPulseCapture::flags_ni if the captured start
 * time corresponded to a low-to-high transition.  The bit is cleared
 * if the start corresponded to a high-to-low transition.
 *
 * @note This is not an input; if an active-high pulse is desired, the
 * user must ensure the input signal is low prior to enabling the
 * capture, or may choise to reset the state in @a callback_ni if the
 * wrong transition was captured as the start of the pulse. */
#define BSP430_TIMER_PULSECAP_ACTIVE_HIGH 0x08

/** Bit set in sBSP430timerPulseCapture::flags_ni if the
 * sBSP430timerPulseCapture::callback_ni is to be invoked when a pulse
 * start is captured. */
#define BSP430_TIMER_PULSECAP_START_CALLBACK 0x100

/** Bit set in sBSP430timerPulseCapture::flags_ni if the
 * sBSP430timerPulseCapture::callback_ni is to be invoked when a pulse
 * end is captured. */
#define BSP430_TIMER_PULSECAP_END_CALLBACK 0x200

/** Bit set in sBSP430timerPulseCapture::flags_ni if the callback is
 * being invoked.  This is used for diagnostics and error checking. */
#define BSP430_TIMER_PULSECAP_CALLBACK_ACTIVE 0x0400

/** Bit set in sBSP430timerPulseCapture::flags_ni if the pulse capture
 * infrastructure is enabled (i.e., linked into the interrupt callback
 * chains). */
#define BSP430_TIMER_PULSECAP_ENABLED 0x1000

/** Bit set in sBSP430timerPulseCapture::flags_ni if the pulse capture
 * infrastructure is active (i.e., the interrupt is enabled). */
#define BSP430_TIMER_PULSECAP_ACTIVE 0x2000

/* Forward declaration */
struct sBSP430timerPulseCapture;

/** Callback invoked on state changes related to a pulse capture.
 *
 * @param state the state of the pulse capture.  The callback is permitted to mutate this state.
 *
 * @return a value conformant with @ref callback_retval that is used
 * as the return value from the interrupt callback handling pulse
 * capture events. */
typedef int (* iBSP430timerPulseCaptureCallback_ni) (struct sBSP430timerPulseCapture * state);

/** Structure containing data related to measuring the duration of a
 * pulse. */
typedef struct sBSP430timerPulseCapture {
  /** Structure to hook callback into timer interrupt chain.  This
   * must be the first field in the structure. */
  sBSP430halISRIndexedChainNode cb;

  /** Handle for the timer HAL used for pulse captures */
  hBSP430halTIMER hal;

  /** Capture/compare index on @a hal used for pulse captures. */
  int ccidx;

  /** Capture/compare index selector used for pulse captures.  This is
   * the #CCIS0|#CCIS1 setting. */
  unsigned int ccis;

  /** Callback invoked on overflow and optionally on start and end
   * captures.  This may be a null pointer if callbacks are not
   * required. */
  iBSP430timerPulseCaptureCallback_ni callback_ni;

  /** Flags indicating validity and configuration information.
   * @warning This field must be treated as @link enh_interrupt_ni not
   * interrupt-able@endlink while the pulse capture is enabled.  I.e.,
   * its contents should be inspected and mutated only while
   * interrupts are disabled. */
  volatile unsigned int flags_ni;

  /** The pulse start time from the underlying clock.  The content is
   * valid only if #BSP430_TIMER_PULSECAP_START_VALID is set in @a
   * flags_ni. */
  volatile unsigned long start_tt_ni;

  /** The pulse end time from the underlying clock.  The content is
   * valid only if #BSP430_TIMER_PULSECAP_END_VALID is set in @a
   * flags. */
  volatile unsigned long end_tt_ni;
} sBSP430timerPulseCapture;

/** Handle for a structure used to capture the width of a pulse */
typedef struct sBSP430timerPulseCapture * hBSP430timerPulseCapture;

/** Configure the @p pulsecap structure to capture pulse widths.
 *
 * Capture/compare register @p ccidx in @p periph is configured to
 * capture both rising and falling edges on input @p ccis.  The
 * capture interrupt is not enabled by this function.
 *
 * It is the user's responsibility to ensure that @p ccidx exists on
 * @p periph and that @p ccis is correct for the pulse signal.  The
 * user must also separately configure @p periph to count
 * continuously.
 *
 * @param pulsecap the structure holding the information about the
 * pulse capture timer.
 *
 * @param periph the timer that is to be used for capturing the pulse,
 * such as #BSP430_UPTIME_TIMER_PERIPH_HANDLE
 *
 * @param ccidx the capture/compare index within the timer.
 *
 * @param ccis the capture/compare input source on which the pulse
 * will arrive
 *
 * @param flags the initial flags to be used to control event
 * notification.  Only the #BSP430_TIMER_PULSECAP_START_CALLBACK and
 * #BSP430_TIMER_PULSECAP_END_CALLBACK fields are used.
 *
 * @param callback the callback to be invoked on pulse capture events.
 * This may be a NULL pointer.
 *
 * @return The pulse capture handle if successful.  A null handle will
 * be returned if initialization failed, e.g. because @p periph could
 * not be located. */
hBSP430timerPulseCapture
hBSP430timerPulseCaptureInitialize (hBSP430timerPulseCapture pulsecap,
                                    tBSP430periphHandle periph,
                                    int ccidx,
                                    unsigned int ccis,
                                    unsigned int flags,
                                    iBSP430timerPulseCaptureCallback_ni callback);

/** Enable or disable an initialized pulse capture structure.
 *
 * @warning This function must @b not be invoked from within an
 * iBSP430timerPulseCaptureCallback_ni() as it manipulates the
 * interrupt callback chains.
 *
 * @note If @p enablep is false, the capture will be deactivated
 * before being disabled.  If @p enablep is true the capture will @em
 * not be activated after being enabled.
 *
 * @param pulsecap a pulse capture structure initialized using
 * hBSP430timerPulseCaptureInitialize().
 *
 * @param enablep If true, the @p pulsecal controlling callback is
 * linked into the callback chain for the configured capture/compare
 * register of the pulse timer peripheral.  If false, the callback is
 * removed from that chain.
 *
 * @return 0 on success, or a negative error code.
 *
 * @see iBSP430timerPulseCaptureSetActive_ni()
 */
int iBSP430timerPulseCaptureSetEnabled_ni (hBSP430timerPulseCapture pulsecap,
                                           int enablep);

/** Activate or deactivate an enabled pulse capture structure.
 *
 * This does not inspect or modify the timer capture configuration; it
 * only affects the #CCIE and #CCIFG flags of the peripheral
 * interrupt.  If invoked when the capture infrastructure is not
 * enabled it returns an error code.
 *
 * It is permitted to invoke this function from within an
 * iBSP430timerPulseCaptureCallback_ni(), and in fact doing so is
 * preferable to setting #BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT.
 *
 * @param pulsecap a pulse capture structure initialized using
 * hBSP430timerPulseCaptureInitialize() and enabled by
 * iBSP430timerPulseCaptureSetEnabled_ni().
 *
 * @param activep If true, the underlying timer peripheral interrupt
 * is enabled; if false the interrupt is disabled.
 *
 * @return 0 on success, or a negative error code.
 *
 * @see iBSP430timerPulseCaptureSetEnabled_ni()
 */
int iBSP430timerPulseCaptureSetActive_ni (hBSP430timerPulseCapture pulsecap,
                                          int activep);

/** Clear state in @p pulsecal so the next transition begins a new
 * pulse.
 *
 * Bits #BSP430_TIMER_PULSECAP_START_VALID,
 * #BSP430_TIMER_PULSECAP_END_VALID, #BSP430_TIMER_PULSECAP_OVERFLOW,
 * and #BSP430_TIMER_PULSECAP_ACTIVE_HIGH are cleared.  All other bits
 * remain unchanged.
 *
 * @note This does not re-activate an inactive pulse capture.
 *
 * @param pulsecap the pulse capture structure.  The pointer must not
 * be null.
 */
static BSP430_CORE_INLINE
void
vBSP430timerPulseCaptureClear_ni (hBSP430timerPulseCapture pulsecap)
{
  pulsecap->flags_ni &= ~(BSP430_TIMER_PULSECAP_START_VALID
                          | BSP430_TIMER_PULSECAP_END_VALID
                          | BSP430_TIMER_PULSECAP_OVERFLOW
                          | BSP430_TIMER_PULSECAP_ACTIVE_HIGH);
}

/** Short-hand to disable a pulse capture using
 * iBSP430timerPulseCaptureSetEnabled_ni() even if interrupts are
 * enabled.
 *
 * @param pulsecap a pulse capture structure initialized via
 * hBSP430timerPulseCaptureInitialize().
 *
 * @return 0 if successful, or a negative error code */
static BSP430_CORE_INLINE
int
iBSP430timerPulseCaptureDisable (hBSP430timerPulseCapture pulsecap)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerPulseCaptureSetEnabled_ni(pulsecap, 0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Short-hand to enable a pulse capture using
 * iBSP430timerPulseCaptureSetEnabled_ni() even if interrupts are
 * enabled.
 *
 * @note This does not activate the capture.
 *
 * @param pulsecap a pulse capture structure initialized via
 * hBSP430timerPulseCaptureInitialize().
 *
 * @return 0 if successful, or a negative error code */
static BSP430_CORE_INLINE
int
iBSP430timerPulseCaptureEnable (hBSP430timerPulseCapture pulsecap)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerPulseCaptureSetEnabled_ni(pulsecap, 1);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Short-hand to re-activate an enabled pulse capture even if
 * interrupts are enabled.
 *
 * @note This operation will clear the pulse state using
 * vBSP430timerPulseCaptureClear_ni() prior to activating the
 * interrupt.
 *
 * @param pulsecap a pulse capture structure initialized via
 * hBSP430timerPulseCaptureInitialize() and enabled via
 * iBSP430timerPulseCaptureSetEnabled_ni().
 *
 * @return 0 if successful, or a negative error code
 *
 * @see iBSP430timerPulseCaptureSetActive_ni()
 */
static BSP430_CORE_INLINE
int
iBSP430timerPulseCaptureActivate (hBSP430timerPulseCapture pulsecap)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  vBSP430timerPulseCaptureClear_ni(pulsecap);
  rv = iBSP430timerPulseCaptureSetActive_ni(pulsecap, 1);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Short-hand to disable an active pulse capture even if interrupts
 * are enabled
 *
 * @param pulsecap a pulse capture structure initialized via
 * hBSP430timerPulseCaptureInitialize() and enabled via
 * iBSP430timerPulseCaptureSetEnabled_ni().
 *
 * @return 0 if successful, or a negative error code
 *
 * @see iBSP430timerPulseCaptureSetActive_ni() */
static BSP430_CORE_INLINE
int
iBSP430timerPulseCaptureDeactivate (hBSP430timerPulseCapture pulsecap)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerPulseCaptureSetActive_ni(pulsecap, 0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Invert the sense of a partial or full pulse capture.
 *
 * #BSP430_TIMER_PULSECAP_ACTIVE_HIGH is inverted in
 * sBSP430timerPulseCapture::flags_ni
 *
 * If the provided capture has a valid end timestamp it is copied into
 * sBSP430timerPulseCapture::start_tt_ni and
 * #BSP430_TIMER_PULSECAP_END_VALID is cleared.
 *
 * Otherwise, if the provided capture has a valid start timestamp it
 * is cleared.
 *
 * @param pulsecap the structure to be updated. */
static BSP430_CORE_INLINE
void vBSP430timerPulseCaptureInvertSense_ni (hBSP430timerPulseCapture pulsecap)
{
  unsigned int flags = pulsecap->flags_ni;
  flags ^= BSP430_TIMER_PULSECAP_ACTIVE_HIGH;
  if (BSP430_TIMER_PULSECAP_END_VALID & flags) {
    flags &= ~BSP430_TIMER_PULSECAP_END_VALID;
    flags |= BSP430_TIMER_PULSECAP_START_VALID;
    pulsecap->start_tt_ni = pulsecap->end_tt_ni;
  } else {
    flags &= ~BSP430_TIMER_PULSECAP_START_VALID;
  }
  pulsecap->flags_ni = flags;
}

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** Control inclusion of the @HAL interface to #BSP430_PERIPH_TA0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halTIMER structure reference is
 * available as #BSP430_HAL_TA0.
 *
 * It may also be obtained using
 * #hBSP430timerLookup(#BSP430_PERIPH_TA0).
 *
 * @cppflag
 * @affects #configBSP430_HPL_TA0 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_TA0
#define configBSP430_HAL_TA0 0
#endif /* configBSP430_HAL_TA0 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_TA0 - 0)
/* You do not need to know about this */
extern sBSP430halTIMER xBSP430hal_TA0_;
#endif /* configBSP430_HAL_TA0 */
/** @endcond */

/** sBSP430halTIMER HAL handle for #BSP430_PERIPH_TA0.
 *
 * This pointer may be used only if #configBSP430_HAL_TA0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA0 - 0)
#define BSP430_HAL_TA0 (&xBSP430hal_TA0_)
#endif /* configBSP430_HAL_TA0 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_TA1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halTIMER structure reference is
 * available as #BSP430_HAL_TA1.
 *
 * It may also be obtained using
 * #hBSP430timerLookup(#BSP430_PERIPH_TA1).
 *
 * @cppflag
 * @affects #configBSP430_HPL_TA1 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_TA1
#define configBSP430_HAL_TA1 0
#endif /* configBSP430_HAL_TA1 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_TA1 - 0)
/* You do not need to know about this */
extern sBSP430halTIMER xBSP430hal_TA1_;
#endif /* configBSP430_HAL_TA1 */
/** @endcond */

/** sBSP430halTIMER HAL handle for #BSP430_PERIPH_TA1.
 *
 * This pointer may be used only if #configBSP430_HAL_TA1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA1 - 0)
#define BSP430_HAL_TA1 (&xBSP430hal_TA1_)
#endif /* configBSP430_HAL_TA1 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_TA2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halTIMER structure reference is
 * available as #BSP430_HAL_TA2.
 *
 * It may also be obtained using
 * #hBSP430timerLookup(#BSP430_PERIPH_TA2).
 *
 * @cppflag
 * @affects #configBSP430_HPL_TA2 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_TA2
#define configBSP430_HAL_TA2 0
#endif /* configBSP430_HAL_TA2 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_TA2 - 0)
/* You do not need to know about this */
extern sBSP430halTIMER xBSP430hal_TA2_;
#endif /* configBSP430_HAL_TA2 */
/** @endcond */

/** sBSP430halTIMER HAL handle for #BSP430_PERIPH_TA2.
 *
 * This pointer may be used only if #configBSP430_HAL_TA2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA2 - 0)
#define BSP430_HAL_TA2 (&xBSP430hal_TA2_)
#endif /* configBSP430_HAL_TA2 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_TA3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halTIMER structure reference is
 * available as #BSP430_HAL_TA3.
 *
 * It may also be obtained using
 * #hBSP430timerLookup(#BSP430_PERIPH_TA3).
 *
 * @cppflag
 * @affects #configBSP430_HPL_TA3 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_TA3
#define configBSP430_HAL_TA3 0
#endif /* configBSP430_HAL_TA3 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_TA3 - 0)
/* You do not need to know about this */
extern sBSP430halTIMER xBSP430hal_TA3_;
#endif /* configBSP430_HAL_TA3 */
/** @endcond */

/** sBSP430halTIMER HAL handle for #BSP430_PERIPH_TA3.
 *
 * This pointer may be used only if #configBSP430_HAL_TA3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA3 - 0)
#define BSP430_HAL_TA3 (&xBSP430hal_TA3_)
#endif /* configBSP430_HAL_TA3 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_TB0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halTIMER structure reference is
 * available as #BSP430_HAL_TB0.
 *
 * It may also be obtained using
 * #hBSP430timerLookup(#BSP430_PERIPH_TB0).
 *
 * @cppflag
 * @affects #configBSP430_HPL_TB0 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_TB0
#define configBSP430_HAL_TB0 0
#endif /* configBSP430_HAL_TB0 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_TB0 - 0)
/* You do not need to know about this */
extern sBSP430halTIMER xBSP430hal_TB0_;
#endif /* configBSP430_HAL_TB0 */
/** @endcond */

/** sBSP430halTIMER HAL handle for #BSP430_PERIPH_TB0.
 *
 * This pointer may be used only if #configBSP430_HAL_TB0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TB0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TB0 - 0)
#define BSP430_HAL_TB0 (&xBSP430hal_TB0_)
#endif /* configBSP430_HAL_TB0 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_TB1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halTIMER structure reference is
 * available as #BSP430_HAL_TB1.
 *
 * It may also be obtained using
 * #hBSP430timerLookup(#BSP430_PERIPH_TB1).
 *
 * @cppflag
 * @affects #configBSP430_HPL_TB1 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_TB1
#define configBSP430_HAL_TB1 0
#endif /* configBSP430_HAL_TB1 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_TB1 - 0)
/* You do not need to know about this */
extern sBSP430halTIMER xBSP430hal_TB1_;
#endif /* configBSP430_HAL_TB1 */
/** @endcond */

/** sBSP430halTIMER HAL handle for #BSP430_PERIPH_TB1.
 *
 * This pointer may be used only if #configBSP430_HAL_TB1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TB1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TB1 - 0)
#define BSP430_HAL_TB1 (&xBSP430hal_TB1_)
#endif /* configBSP430_HAL_TB1 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_TB2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halTIMER structure reference is
 * available as #BSP430_HAL_TB2.
 *
 * It may also be obtained using
 * #hBSP430timerLookup(#BSP430_PERIPH_TB2).
 *
 * @cppflag
 * @affects #configBSP430_HPL_TB2 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_TB2
#define configBSP430_HAL_TB2 0
#endif /* configBSP430_HAL_TB2 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_TB2 - 0)
/* You do not need to know about this */
extern sBSP430halTIMER xBSP430hal_TB2_;
#endif /* configBSP430_HAL_TB2 */
/** @endcond */

/** sBSP430halTIMER HAL handle for #BSP430_PERIPH_TB2.
 *
 * This pointer may be used only if #configBSP430_HAL_TB2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TB2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TB2 - 0)
#define BSP430_HAL_TB2 (&xBSP430hal_TB2_)
#endif /* configBSP430_HAL_TB2 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** Control inclusion of the @HPL interface to #BSP430_PERIPH_TA0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplTIMER structure reference is
 * available as #BSP430_HPL_TA0.
 *
 * It may also be obtained using
 * #xBSP430hplLookupTIMER(#BSP430_PERIPH_TA0).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_TA0
#define configBSP430_HPL_TA0 (configBSP430_HAL_TA0 - 0)
#endif /* configBSP430_HPL_TA0 */

#if (configBSP430_HAL_TA0 - 0) && ! (configBSP430_HPL_TA0 - 0)
#warning configBSP430_HAL_TA0 requested without configBSP430_HPL_TA0
#endif /* HAL and not HPL */

/** Handle for the raw TA0 device.
 *
 * The handle may be used only if #configBSP430_HPL_TA0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA0 - 0)
#define BSP430_PERIPH_TA0 ((tBSP430periphHandle)(BSP430_PERIPH_TA0_BASEADDRESS_))
#endif /* configBSP430_HPL_TA0 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_TA1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplTIMER structure reference is
 * available as #BSP430_HPL_TA1.
 *
 * It may also be obtained using
 * #xBSP430hplLookupTIMER(#BSP430_PERIPH_TA1).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_TA1
#define configBSP430_HPL_TA1 (configBSP430_HAL_TA1 - 0)
#endif /* configBSP430_HPL_TA1 */

#if (configBSP430_HAL_TA1 - 0) && ! (configBSP430_HPL_TA1 - 0)
#warning configBSP430_HAL_TA1 requested without configBSP430_HPL_TA1
#endif /* HAL and not HPL */

/** Handle for the raw TA1 device.
 *
 * The handle may be used only if #configBSP430_HPL_TA1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA1 - 0)
#define BSP430_PERIPH_TA1 ((tBSP430periphHandle)(BSP430_PERIPH_TA1_BASEADDRESS_))
#endif /* configBSP430_HPL_TA1 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_TA2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplTIMER structure reference is
 * available as #BSP430_HPL_TA2.
 *
 * It may also be obtained using
 * #xBSP430hplLookupTIMER(#BSP430_PERIPH_TA2).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_TA2
#define configBSP430_HPL_TA2 (configBSP430_HAL_TA2 - 0)
#endif /* configBSP430_HPL_TA2 */

#if (configBSP430_HAL_TA2 - 0) && ! (configBSP430_HPL_TA2 - 0)
#warning configBSP430_HAL_TA2 requested without configBSP430_HPL_TA2
#endif /* HAL and not HPL */

/** Handle for the raw TA2 device.
 *
 * The handle may be used only if #configBSP430_HPL_TA2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA2 - 0)
#define BSP430_PERIPH_TA2 ((tBSP430periphHandle)(BSP430_PERIPH_TA2_BASEADDRESS_))
#endif /* configBSP430_HPL_TA2 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_TA3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplTIMER structure reference is
 * available as #BSP430_HPL_TA3.
 *
 * It may also be obtained using
 * #xBSP430hplLookupTIMER(#BSP430_PERIPH_TA3).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_TA3
#define configBSP430_HPL_TA3 (configBSP430_HAL_TA3 - 0)
#endif /* configBSP430_HPL_TA3 */

#if (configBSP430_HAL_TA3 - 0) && ! (configBSP430_HPL_TA3 - 0)
#warning configBSP430_HAL_TA3 requested without configBSP430_HPL_TA3
#endif /* HAL and not HPL */

/** Handle for the raw TA3 device.
 *
 * The handle may be used only if #configBSP430_HPL_TA3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA3 - 0)
#define BSP430_PERIPH_TA3 ((tBSP430periphHandle)(BSP430_PERIPH_TA3_BASEADDRESS_))
#endif /* configBSP430_HPL_TA3 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_TB0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplTIMER structure reference is
 * available as #BSP430_HPL_TB0.
 *
 * It may also be obtained using
 * #xBSP430hplLookupTIMER(#BSP430_PERIPH_TB0).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_TB0
#define configBSP430_HPL_TB0 (configBSP430_HAL_TB0 - 0)
#endif /* configBSP430_HPL_TB0 */

#if (configBSP430_HAL_TB0 - 0) && ! (configBSP430_HPL_TB0 - 0)
#warning configBSP430_HAL_TB0 requested without configBSP430_HPL_TB0
#endif /* HAL and not HPL */

/** Handle for the raw TB0 device.
 *
 * The handle may be used only if #configBSP430_HPL_TB0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB0 - 0)
#define BSP430_PERIPH_TB0 ((tBSP430periphHandle)(BSP430_PERIPH_TB0_BASEADDRESS_))
#endif /* configBSP430_HPL_TB0 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_TB1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplTIMER structure reference is
 * available as #BSP430_HPL_TB1.
 *
 * It may also be obtained using
 * #xBSP430hplLookupTIMER(#BSP430_PERIPH_TB1).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_TB1
#define configBSP430_HPL_TB1 (configBSP430_HAL_TB1 - 0)
#endif /* configBSP430_HPL_TB1 */

#if (configBSP430_HAL_TB1 - 0) && ! (configBSP430_HPL_TB1 - 0)
#warning configBSP430_HAL_TB1 requested without configBSP430_HPL_TB1
#endif /* HAL and not HPL */

/** Handle for the raw TB1 device.
 *
 * The handle may be used only if #configBSP430_HPL_TB1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB1 - 0)
#define BSP430_PERIPH_TB1 ((tBSP430periphHandle)(BSP430_PERIPH_TB1_BASEADDRESS_))
#endif /* configBSP430_HPL_TB1 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_TB2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplTIMER structure reference is
 * available as #BSP430_HPL_TB2.
 *
 * It may also be obtained using
 * #xBSP430hplLookupTIMER(#BSP430_PERIPH_TB2).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_TB2
#define configBSP430_HPL_TB2 (configBSP430_HAL_TB2 - 0)
#endif /* configBSP430_HPL_TB2 */

#if (configBSP430_HAL_TB2 - 0) && ! (configBSP430_HPL_TB2 - 0)
#warning configBSP430_HAL_TB2 requested without configBSP430_HPL_TB2
#endif /* HAL and not HPL */

/** Handle for the raw TB2 device.
 *
 * The handle may be used only if #configBSP430_HPL_TB2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB2 - 0)
#define BSP430_PERIPH_TB2 ((tBSP430periphHandle)(BSP430_PERIPH_TB2_BASEADDRESS_))
#endif /* configBSP430_HPL_TB2 */

/* END AUTOMATICALLY GENERATED CODE [periph_decl] */
/* !BSP430! end=periph_decl */

/* !BSP430! insert=hpl_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_decl] */
/** sBSP430hplTIMER HPL pointer for #BSP430_PERIPH_TA0.
 *
 * This pointer to a volatile structure overlaying the TA0
 * peripheral register map may be used only if
 * #configBSP430_HPL_TA0 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA0 - 0)
#define BSP430_HPL_TA0 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA0)
#endif /* configBSP430_HPL_TA0 */

/** sBSP430hplTIMER HPL pointer for #BSP430_PERIPH_TA1.
 *
 * This pointer to a volatile structure overlaying the TA1
 * peripheral register map may be used only if
 * #configBSP430_HPL_TA1 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA1 - 0)
#define BSP430_HPL_TA1 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA1)
#endif /* configBSP430_HPL_TA1 */

/** sBSP430hplTIMER HPL pointer for #BSP430_PERIPH_TA2.
 *
 * This pointer to a volatile structure overlaying the TA2
 * peripheral register map may be used only if
 * #configBSP430_HPL_TA2 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA2 - 0)
#define BSP430_HPL_TA2 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA2)
#endif /* configBSP430_HPL_TA2 */

/** sBSP430hplTIMER HPL pointer for #BSP430_PERIPH_TA3.
 *
 * This pointer to a volatile structure overlaying the TA3
 * peripheral register map may be used only if
 * #configBSP430_HPL_TA3 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA3 - 0)
#define BSP430_HPL_TA3 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA3)
#endif /* configBSP430_HPL_TA3 */

/** sBSP430hplTIMER HPL pointer for #BSP430_PERIPH_TB0.
 *
 * This pointer to a volatile structure overlaying the TB0
 * peripheral register map may be used only if
 * #configBSP430_HPL_TB0 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB0 - 0)
#define BSP430_HPL_TB0 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB0)
#endif /* configBSP430_HPL_TB0 */

/** sBSP430hplTIMER HPL pointer for #BSP430_PERIPH_TB1.
 *
 * This pointer to a volatile structure overlaying the TB1
 * peripheral register map may be used only if
 * #configBSP430_HPL_TB1 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB1 - 0)
#define BSP430_HPL_TB1 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB1)
#endif /* configBSP430_HPL_TB1 */

/** sBSP430hplTIMER HPL pointer for #BSP430_PERIPH_TB2.
 *
 * This pointer to a volatile structure overlaying the TB2
 * peripheral register map may be used only if
 * #configBSP430_HPL_TB2 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB2 - 0)
#define BSP430_HPL_TB2 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB2)
#endif /* configBSP430_HPL_TB2 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_timer_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_timer_isr_decl] */
/** Control inclusion of the secondary @HAL interrupt handler for #BSP430_PERIPH_TA0
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, or 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is not enabled.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TA0.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TA0 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA0_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA0_CC0_ISR - 0) && ! (configBSP430_HAL_TA0 - 0)
#warning configBSP430_HAL_TA0_CC0_ISR requested without configBSP430_HAL_TA0
#endif /* HAL_CC0_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_TA0
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_TA0 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TA0.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TA0 */
#ifndef configBSP430_HAL_TA0_ISR
#define configBSP430_HAL_TA0_ISR (configBSP430_HAL_TA0 - 0)
#endif /* configBSP430_HAL_TA0_ISR */

#if (configBSP430_HAL_TA0_ISR - 0) && ! (configBSP430_HAL_TA0 - 0)
#warning configBSP430_HAL_TA0_ISR requested without configBSP430_HAL_TA0
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the secondary @HAL interrupt handler for #BSP430_PERIPH_TA1
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, or 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is not enabled.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TA1.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TA1 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA1_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA1_CC0_ISR - 0) && ! (configBSP430_HAL_TA1 - 0)
#warning configBSP430_HAL_TA1_CC0_ISR requested without configBSP430_HAL_TA1
#endif /* HAL_CC0_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_TA1
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_TA1 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TA1.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TA1 */
#ifndef configBSP430_HAL_TA1_ISR
#define configBSP430_HAL_TA1_ISR (configBSP430_HAL_TA1 - 0)
#endif /* configBSP430_HAL_TA1_ISR */

#if (configBSP430_HAL_TA1_ISR - 0) && ! (configBSP430_HAL_TA1 - 0)
#warning configBSP430_HAL_TA1_ISR requested without configBSP430_HAL_TA1
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the secondary @HAL interrupt handler for #BSP430_PERIPH_TA2
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, or 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is not enabled.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TA2.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TA2 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA2_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA2_CC0_ISR - 0) && ! (configBSP430_HAL_TA2 - 0)
#warning configBSP430_HAL_TA2_CC0_ISR requested without configBSP430_HAL_TA2
#endif /* HAL_CC0_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_TA2
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_TA2 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TA2.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TA2 */
#ifndef configBSP430_HAL_TA2_ISR
#define configBSP430_HAL_TA2_ISR (configBSP430_HAL_TA2 - 0)
#endif /* configBSP430_HAL_TA2_ISR */

#if (configBSP430_HAL_TA2_ISR - 0) && ! (configBSP430_HAL_TA2 - 0)
#warning configBSP430_HAL_TA2_ISR requested without configBSP430_HAL_TA2
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the secondary @HAL interrupt handler for #BSP430_PERIPH_TA3
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, or 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is not enabled.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TA3.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TA3 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA3_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA3_CC0_ISR - 0) && ! (configBSP430_HAL_TA3 - 0)
#warning configBSP430_HAL_TA3_CC0_ISR requested without configBSP430_HAL_TA3
#endif /* HAL_CC0_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_TA3
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_TA3 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TA3.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TA3 */
#ifndef configBSP430_HAL_TA3_ISR
#define configBSP430_HAL_TA3_ISR (configBSP430_HAL_TA3 - 0)
#endif /* configBSP430_HAL_TA3_ISR */

#if (configBSP430_HAL_TA3_ISR - 0) && ! (configBSP430_HAL_TA3 - 0)
#warning configBSP430_HAL_TA3_ISR requested without configBSP430_HAL_TA3
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the secondary @HAL interrupt handler for #BSP430_PERIPH_TB0
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, or 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is not enabled.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TB0.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TB0 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB0_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB0_CC0_ISR - 0) && ! (configBSP430_HAL_TB0 - 0)
#warning configBSP430_HAL_TB0_CC0_ISR requested without configBSP430_HAL_TB0
#endif /* HAL_CC0_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_TB0
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_TB0 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TB0.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TB0 */
#ifndef configBSP430_HAL_TB0_ISR
#define configBSP430_HAL_TB0_ISR (configBSP430_HAL_TB0 - 0)
#endif /* configBSP430_HAL_TB0_ISR */

#if (configBSP430_HAL_TB0_ISR - 0) && ! (configBSP430_HAL_TB0 - 0)
#warning configBSP430_HAL_TB0_ISR requested without configBSP430_HAL_TB0
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the secondary @HAL interrupt handler for #BSP430_PERIPH_TB1
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, or 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is not enabled.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TB1.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TB1 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB1_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB1_CC0_ISR - 0) && ! (configBSP430_HAL_TB1 - 0)
#warning configBSP430_HAL_TB1_CC0_ISR requested without configBSP430_HAL_TB1
#endif /* HAL_CC0_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_TB1
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_TB1 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TB1.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TB1 */
#ifndef configBSP430_HAL_TB1_ISR
#define configBSP430_HAL_TB1_ISR (configBSP430_HAL_TB1 - 0)
#endif /* configBSP430_HAL_TB1_ISR */

#if (configBSP430_HAL_TB1_ISR - 0) && ! (configBSP430_HAL_TB1 - 0)
#warning configBSP430_HAL_TB1_ISR requested without configBSP430_HAL_TB1
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the secondary @HAL interrupt handler for #BSP430_PERIPH_TB2
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, or 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is not enabled.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TB2.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TB2 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB2_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB2_CC0_ISR - 0) && ! (configBSP430_HAL_TB2 - 0)
#warning configBSP430_HAL_TB2_CC0_ISR requested without configBSP430_HAL_TB2
#endif /* HAL_CC0_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_TB2
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_TB2 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_TB2.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_config_core
 * @dependency #configBSP430_HAL_TB2 */
#ifndef configBSP430_HAL_TB2_ISR
#define configBSP430_HAL_TB2_ISR (configBSP430_HAL_TB2 - 0)
#endif /* configBSP430_HAL_TB2_ISR */

#if (configBSP430_HAL_TB2_ISR - 0) && ! (configBSP430_HAL_TB2 - 0)
#warning configBSP430_HAL_TB2_ISR requested without configBSP430_HAL_TB2
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_timer_isr_decl] */
/* !BSP430! end=hal_timer_isr_decl */

/** Get the HPL handle for a specific timer.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return A typed pointer that can be used to manipulate the timer.
 * A null pointer is returned if the handle does not correspond to a
 * timer for which the HPL interface been enabled (e.g., with
 * #configBSP430_HPL_TA0).
 */
static BSP430_CORE_INLINE
volatile sBSP430hplTIMER * xBSP430hplLookupTIMER (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_demux] */
#if (configBSP430_HPL_TA0 - 0)
  if (BSP430_PERIPH_TA0 == periph) {
    return BSP430_HPL_TA0;
  }
#endif /* configBSP430_HPL_TA0 */

#if (configBSP430_HPL_TA1 - 0)
  if (BSP430_PERIPH_TA1 == periph) {
    return BSP430_HPL_TA1;
  }
#endif /* configBSP430_HPL_TA1 */

#if (configBSP430_HPL_TA2 - 0)
  if (BSP430_PERIPH_TA2 == periph) {
    return BSP430_HPL_TA2;
  }
#endif /* configBSP430_HPL_TA2 */

#if (configBSP430_HPL_TA3 - 0)
  if (BSP430_PERIPH_TA3 == periph) {
    return BSP430_HPL_TA3;
  }
#endif /* configBSP430_HPL_TA3 */

#if (configBSP430_HPL_TB0 - 0)
  if (BSP430_PERIPH_TB0 == periph) {
    return BSP430_HPL_TB0;
  }
#endif /* configBSP430_HPL_TB0 */

#if (configBSP430_HPL_TB1 - 0)
  if (BSP430_PERIPH_TB1 == periph) {
    return BSP430_HPL_TB1;
  }
#endif /* configBSP430_HPL_TB1 */

#if (configBSP430_HPL_TB2 - 0)
  if (BSP430_PERIPH_TB2 == periph) {
    return BSP430_HPL_TB2;
  }
#endif /* configBSP430_HPL_TB2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_demux] */
  /* !BSP430! end=periph_hpl_demux */
  return NULL;
}

/** Get the HAL handle for a specific timer.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return the HAL handle for the timer.  A null pointer is returned
 * if the handle does not correspond to a timer for which the HAL
 * interface has been enabled (e.g., with #configBSP430_HAL_TA0).
 */
static BSP430_CORE_INLINE
hBSP430halTIMER hBSP430timerLookup (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if (configBSP430_HAL_TA0 - 0)
  if (BSP430_PERIPH_TA0 == periph) {
    return BSP430_HAL_TA0;
  }
#endif /* configBSP430_HAL_TA0 */

#if (configBSP430_HAL_TA1 - 0)
  if (BSP430_PERIPH_TA1 == periph) {
    return BSP430_HAL_TA1;
  }
#endif /* configBSP430_HAL_TA1 */

#if (configBSP430_HAL_TA2 - 0)
  if (BSP430_PERIPH_TA2 == periph) {
    return BSP430_HAL_TA2;
  }
#endif /* configBSP430_HAL_TA2 */

#if (configBSP430_HAL_TA3 - 0)
  if (BSP430_PERIPH_TA3 == periph) {
    return BSP430_HAL_TA3;
  }
#endif /* configBSP430_HAL_TA3 */

#if (configBSP430_HAL_TB0 - 0)
  if (BSP430_PERIPH_TB0 == periph) {
    return BSP430_HAL_TB0;
  }
#endif /* configBSP430_HAL_TB0 */

#if (configBSP430_HAL_TB1 - 0)
  if (BSP430_PERIPH_TB1 == periph) {
    return BSP430_HAL_TB1;
  }
#endif /* configBSP430_HAL_TB1 */

#if (configBSP430_HAL_TB2 - 0)
  if (BSP430_PERIPH_TB2 == periph) {
    return BSP430_HAL_TB2;
  }
#endif /* configBSP430_HAL_TB2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}

/** Get a human-readable identifier for the timer
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return The name of the timer, e.g. "TA0".  If the peripheral is
 * not recognized as a timer, a null pointer is returned.
 */
const char * xBSP430timerName (tBSP430periphHandle periph);

/** Get the number of capture/compare registers supported by the timer.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return the number of CCs on the timer.  -1 is returned if the
 * timer does not exist or the resource was not configured. */
int iBSP430timerSupportedCCs (tBSP430periphHandle periph);

/** Inline function to read the counter of a timer synchronous to MCLK.
 *
 * This simply reads and returns the counter register of the
 * underlying peripheral.
 *
 * This call takes about 3 clock cycles on a CPUX MCU.
 *
 * @warning If the clock source for the timer is asynchronous to the
 * CPU clock there is a chance that the counter value read will be
 * corrupt.  Consider using uiBSP430timerAsyncCounterRead_ni() in that
 * situation.
 *
 * @param hpl reference to the underlying timer.
 *
 * @return the instantaneous counter value.
 *
 * @see uiBSP430timerAsyncCounterRead_ni()
 * @see uiBSP430timerLatchedCounterRead_ni()
 * @see uiBSP430timerSafeCounterRead_ni()
 */
static BSP430_CORE_INLINE_FORCED
unsigned int
uiBSP430timerSyncCounterRead_ni (volatile sBSP430hplTIMER * const hpl)
{
  return hpl->r;
}

/** Inline function to read the counter of a timer not synchronous to MCLK.
 *
 * This approach reads the counter repeatedly until the same value is
 * read twice.  The technique is appropriate when the counter is
 * asynchronous to MCLK, but it is safe to use only when the counter
 * update rate is significantly slower than MCLK, e.g. when the timer
 * is driven by LFXT1.
 *
 * This call takes about 9 clock cycles on a CPUX MCU if the first two
 * reads are equal.
 *
 * @warning The time taken by this function is only probabilistically
 * constant.
 *
 * @warning If the timer frequency is not at least a factor of 10
 * slower than MCLK, this routine may hang since two counter values
 * read consecutively may never be equal.
 *
 * @param hpl reference to the underlying timer.
 *
 * @return the instantaneous counter value.
 *
 * @see uiBSP430timerSyncCounterRead_ni()
 * @see uiBSP430timerLatchedCounterRead_ni()
 * @see uiBSP430timerSafeCounterRead_ni()
 */
static BSP430_CORE_INLINE_FORCED
unsigned int
uiBSP430timerAsyncCounterRead_ni (volatile sBSP430hplTIMER * const hpl)
{
  unsigned int v1;
  unsigned int v2;
  do {
    v1 = uiBSP430timerSyncCounterRead_ni(hpl);
    v2 = uiBSP430timerSyncCounterRead_ni(hpl);
  } while (v1 != v2);
  return v1;
}

/** Configure a timer CC to support uiBSP430timerLatchedCounterRead_ni().
 *
 * Latched counter reads rely on a pre-configured capture/compare
 * register.  This routine configures that register.
 *
 * @param hpl reference to the underlying timer
 *
 * @param ccidx the index of the capture/compare register to be used
 * for latched timer reads, often #BSP430_TIMER_VALID_COUNTER_READ_CCIDX
 */
static BSP430_CORE_INLINE_FORCED
void
vBSP430timerLatchedCounterInitialize_ni (volatile sBSP430hplTIMER * const hpl,
                                         int ccidx)
{
  /* Configure for capture, alternating between GND and VCC as
   * sources, triggering on both edges, asynchronously. */
  hpl->cctl[ccidx] = CAP | CCIS1 | CM_3;
}

/** Inline function to read a timer counter using a capture register.
 *
 * This approach uses a pre-configured capture/compare register to
 * latch the counter value, which is then returned.  It is generally
 * slower than uiBSP430timerSyncCounterRead_ni(), but is faster than
 * uiBSP430timerAsyncCounterRead_ni() and is also safe to use
 * regardless of the relationship between MCLK and the accessed timer.
 *
 * This call takes about 8 clock cycles on a CPUX MCU.
 *
 * @warning If the selected capture/compare register has not been
 * configured using vBSP430timerLatchedCounterInitialize_ni() the
 * return value will not represent the timer counter.
 *
 * @param hpl reference to the underlying timer
 *
 * @param ccidx the index of the capture/compare register to be used
 * for latched timer reads
 *
 * @return the instantaneous counter value.
 *
 * @see uiBSP430timerAsyncCounterRead_ni()
 * @see uiBSP430timerSyncCounterRead_ni()
 * @see uiBSP430timerLatchedCounterRead_ni()
 * @see uiBSP430timerSafeCounterRead_ni()
 */
static BSP430_CORE_INLINE_FORCED
unsigned int
uiBSP430timerLatchedCounterRead_ni (volatile sBSP430hplTIMER * const hpl,
                                    int ccidx)
{
  /* Latch the current counter into a capture register and return it. */
  hpl->cctl[ccidx] ^= CCIS0;
  return hpl->ccr[ccidx];
}

/** Configure the timer for safe reads.
 *
 * @note Here "safe" means that call will return a counter value
 * regardless of how the timer clock is related to MCLK.  When
 * #configBSP430_TIMER_VALID_COUNTER_READ is disabled this function has
 * no effect.
 *
 * This function must be invoked before any other function that might
 * attempt to invoke uiBSP430timerSafeCounterRead_ni() on the
 * peripheral.  This includes ulBSP430timerCounter_ni().
 *
 * @note It is safe to call this function multiple times, and in fact
 * it must be called again if you need to use
 * #BSP430_TIMER_VALID_COUNTER_READ_CCIDX for some other purpose.
 *
 * @param hpl reference to the underlying timer
 *
 * @warning If #BSP430_TIMER_VALID_COUNTER_READ_CCIDX does not exist on
 * @a periph and #BSP430_CORE_NDEBUG is false this call will hang.
 * Since this is a configuration error, the situation should be
 * detected during development.
 *
 * @dependency #configBSP430_TIMER_VALID_COUNTER_READ
 * @dependency #BSP430_CORE_NDEBUG
 */
static BSP430_CORE_INLINE
void
vBSP430timerSafeCounterInitialize_ni (volatile sBSP430hplTIMER * const hpl)
{
#if (configBSP430_TIMER_VALID_COUNTER_READ - 0)
#if ! (BSP430_CORE_NDEBUG - 0)
  tBSP430periphHandle periph = xBSP430periphFromHPL(hpl);
  while ((NULL == hpl)
         || (BSP430_TIMER_VALID_COUNTER_READ_CCIDX >= iBSP430timerSupportedCCs(periph))) {
    /* Spin to detect misconfiguration */
  }
#endif /* BSP430_CORE_NDEBUG */
  vBSP430timerLatchedCounterInitialize_ni(hpl, BSP430_TIMER_VALID_COUNTER_READ_CCIDX);
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */
}

/** Inline function to read the timer counter.
 *
 * @note Here "safe" means that call will return a counter value
 * regardless of how the timer clock is related to MCLK.  When
 * #configBSP430_TIMER_VALID_COUNTER_READ is disabled this function is
 * equivalent to uiBSP430timerSyncCounterRead_ni(); in that case, the
 * counter value may not be valid if the timer clock is asynchronous
 * to MCLK.  The remainder of this description assumes that
 * #configBSP430_TIMER_VALID_COUNTER_READ is enabled.
 *
 * This function uses uiBSP430timerLatchedCounterRead_ni() along with
 * #BSP430_TIMER_VALID_COUNTER_READ_CCIDX to read the timer.
 *
 * This call takes about 21 clock cycles on a CPUX MCU when
 * #configBSP430_TIMER_VALID_COUNTER_READ is enabled.
 *
 * When #BSP430_CORE_NDEBUG is false (default) this function validates
 * that the capture/compare register is properly configured to read
 * the counter.  When that requirement is not met, the CPU will spin
 * in place allowing the developer to see what has gone wrong, instead
 * of continuing to process invalid counter values.
 *
 * @param hpl reference to the underlying timer
 *
 * @return the instantaneous counter value.
 *
 * @dependency #configBSP430_TIMER_VALID_COUNTER_READ
 * @dependency #BSP430_CORE_NDEBUG
 *
 * @see uiBSP430timerAsyncCounterRead_ni()
 * @see uiBSP430timerSyncCounterRead_ni()
 * @see uiBSP430timerLatchedCounterRead_ni()
 */
static BSP430_CORE_INLINE_FORCED
unsigned int
uiBSP430timerSafeCounterRead_ni (volatile sBSP430hplTIMER * const hpl)
{
  unsigned int rv;
#if (configBSP430_TIMER_VALID_COUNTER_READ - 0)
#if ! (BSP430_CORE_NDEBUG - 0)
  /* Clear the flag; used only to prevent the following sanity check
   * from falsely failing. */
  hpl->cctl[BSP430_TIMER_VALID_COUNTER_READ_CCIDX] &= ~CCIFG;
#endif /* BSP430_CORE_NDEBUG */
  rv = uiBSP430timerLatchedCounterRead_ni(hpl, BSP430_TIMER_VALID_COUNTER_READ_CCIDX);
#if ! (BSP430_CORE_NDEBUG - 0)
  /* If the capture/compare register is not configured properly to
   * have done a capture, spin in place so this problem can be
   * detected during development. */
  while ((CCIS1 | CCIFG) != (hpl->cctl[BSP430_TIMER_VALID_COUNTER_READ_CCIDX] & (CCIS1 | CCIFG))) {
    /* Spin to detect misconfiguration */
  }
#endif /* NDEBUG */
#else /* configBSP430_TIMER_VALID_COUNTER_READ */
  /* Async could hang if the clock rates were too close, so just read
   * once and hope for the best. */
  rv = uiBSP430timerSyncCounterRead_ni(hpl);
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */
  return rv;
}

/** Select the best counter read solution based on a hint.
 *
 * When the application knows exactly how the timer is configured at
 * the point the counter is read, it can select the optimal counter
 * read function and use it directly for the least overhead.  When the
 * application or infrastructure can record information about how the
 * timer is configured but the counter is read in shared code the
 * hints may be used to get a better or faster answer than would be
 * obtained from uiBSP430timerSafeCounterRead_ni(), which is the only
 * solution when no information is available.
 *
 * If #BSP430_TIMER_FLAG_MCLKSYNC is set in @a flags,
 * uiBSP430timerSyncCounterRead_ni() is used. There's about an 8 cycle
 * overhead to select this case at runtime, resulting in a measurement
 * cost of 11 cycles.
 *
 * Otherwise, if #BSP430_TIMER_FLAG_SLOW is set in @a flags,
 * uiBSP430timerAsyncCounterRead_ni() is used.  There's about an 11
 * cycle overhead to select this case at runtime, resulting in a
 * measurement cost of 20 cycles.
 *
 * Otherwise, uiBSP430timerSafeCounterRead_ni() is used.
 *
 * @param hpl reference to the underlying timer
 *
 * @param flags bit mask including potentially
 * #BSP430_TIMER_FLAG_MCLKSYNC and #BSP430_TIMER_FLAG_SLOW.  This can
 * be obtained from the @c flags field of the corresponding
 * sBSP430halTIMER::hal_state field, if something updated that when
 * the timer was configured.
 *
 * @return the instantaneous counter value. */
static BSP430_CORE_INLINE_FORCED
unsigned int
uiBSP430timerBestCounterRead_ni (volatile sBSP430hplTIMER * const hpl,
                                 int flags)
{
  if (flags & BSP430_TIMER_FLAG_MCLKSYNC) {
    return uiBSP430timerSyncCounterRead_ni(hpl);
  }
  if (flags & BSP430_TIMER_FLAG_SLOW) {
    return uiBSP430timerAsyncCounterRead_ni(hpl);
  }
  return uiBSP430timerSafeCounterRead_ni(hpl);
}

#endif /* BSP430_MODULE_TIMER */

#endif /* BSP430_PERIPH_TIMER_H */
