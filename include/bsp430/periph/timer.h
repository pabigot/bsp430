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
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 *
 * @defgroup grp_timer_alarm Alarm-related Timer Functionality
 *
 * @brief bsp430/periph/timer.h data structures and functions supporting alarms
 *
 * BSP430 provides an infrastructure that uses the capture/compare
 * interrupt callbacks of the timer peripheral to support a
 * configurable alarm system.
 *
 * Each alarm is configured at initialization to reference a specific
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
 * #BSP430_UPTIME_TIMER_PERIPH_HANDLE | @em automatic         | #configBSP430_UPTIME_USE_DEFAULT_CC0_ISR
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE | #configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL | #configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR
 * #BSP430_PERIPH_TA3                 | #configBSP430_HAL_TA3 | #configBSP430_HAL_TA3_CC0_ISR
 *
 * @note It is the responsibility of the application to configure the
 * underlying timer to continuous mode with the appropriate clock
 * source, any desired dividers, and enabling the interrupt that
 * maintains the 32-bit overflow counter timer HAL infrastructure.
 * For example: @code

  alarmHAL_ = hBSP430timerLookup(ALARM_TIMER_PERIPH_HANDLE);
  if (alarmHAL_) {
    alarmHAL_->hpl->ctl = 0;
    vBSP430timerResetCounter_ni(alarmHAL_);
    alarmHAL_->hpl->ctl = APP_TASSEL | MC_2 | TACLR | TAIE;
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
 * sBSP430timerAlarm::callback is invoked.  The callback may invoke
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
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
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
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_TIMER_H
#define BSP430_PERIPH_TIMER_H

/* !BSP430! periph=timer */
/* !BSP430! instance=TA0,TA1,TA2,TA3,TB0,TB1,TB2 */

#include <bsp430/periph.h>

/** @def BSP430_MODULE_TIMER_A
 *
 * Defined on inclusion of <bsp430/periph/timer.h>.  The value
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

/** @def BSP430_MODULE_TIMER_B
 *
 * Defined on inclusion of <bsp430/periph/timer.h>.  The value
 * evaluates to true if the target MCU supports the Timer_B Module,
 * and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_TIMER_B (defined(__MSP430_HAS_TB3__)      \
                               || defined(__MSP430_HAS_TB7__)   \
                               || defined(__MSP430_HAS_T0B3__)  \
                               || defined(__MSP430_HAS_T0B7__))


/** @def BSP430_MODULE_TIMER
 *
 * Defined on inclusion of <bsp430/periph/timer.h>.  The value
 * evaluates to true if the target MCU supports the Timer_A or
 * Timer_B, and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_TIMER (BSP430_MODULE_TIMER_A || BSP430_MODULE_TIMER_B)

/* Surprisingly, not every MCU has a Timer_A.  Some have only a Basic
 * Timer (BT). */
#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_TIMER - 0)

/** @def configBSP430_TIMER_CCACLK
 *
 * Define to true to indicate that the application or infrastructure
 * would like to use a timer that supports using ACLK as a
 * capture/compare input signal.  The timer is identified by
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE, and that macro is defined only
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
 * #if BSP430_UPTIME_TIMER_PERIPH_HANDLE == BSP430_TIMER_CCACLK_PERIPH_HANDLE
 * #error Cannot use both
 * #endif // uptime == ccaclk
 * @endcode
 *
 * @cppflag
 * @affects #BSP430_TIMER_CCACLK
 * @defaulted
 * @ingroup grp_timer_ccaclk */
#ifndef configBSP430_TIMER_CCACLK
#define configBSP430_TIMER_CCACLK 0
#endif /* configBSP430_TIMER_CCACLK */

/** @def configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE
 *
 * Define to a true value to use the default (platform-specific)
 * CCACLK timer.  This is true by default if
 * #configBSP430_TIMER_CCACLK is true.  If it remains true, the HPL
 * resources corresponding to #BSP430_TIMER_CCACLK_PERIPH_HANDLE and
 * #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE will also be enabled
 * by default.
 *
 * If you want to override the default, define this to a false value
 * and provide definitions for:
 * <ul>
 * <li>#BSP430_TIMER_CCACLK_PERIPH_HANDLE
 * <li>#BSP430_TIMER_CCACLK_CC_INDEX
 * <li>#BSP430_TIMER_CCACLK_CCIS
 * <li>#BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
 * <li>#BSP430_TIMER_CCACLK_CLK_PORT_BIT
 * </ul>
 *
 * Also be sure you enable the HAL or HPL resources for the timer and
 * the port.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_timer_ccaclk */
#ifndef configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE
#define configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE (configBSP430_TIMER_CCACLK - 0)
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE */

/** @def configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL
 *
 * Define to a true value to automatically request the HAL support for
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE when the latter is defaulted.
 * When false, only the HPL support for the peripheral is enabled.  As
 * is standard for BSP430 timers, enabling the HAL does not enable the
 * ISR for CC0; see #configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_timer_ccaclk */
#ifndef configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL
#define configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL 0
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL */

/** @def configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL
 *
 * Define to a true value to automatically request the HAL support for
 * #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE when the latter is
 * defaulted.  When false, only the HPL support for the peripheral is
 * enabled.
 *
 * You would want to use the port HAL if the source for the clock
 * being timed was external to the microcontroller.  See @ref
 * ex_sensors_hh10d for an example.
 *
 * @cppflag
 * @defaulted
 * @ingroup grp_timer_ccaclk */
#ifndef configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL
#define configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL 0
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL */

/** @def configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR
 *
 * Normally, when the HAL timer infrastructure associated with
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE is enabled through
 * #configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL, the standard HAL
 * ISR is enabled but the companion CC0 interrupt HAL ISR is left
 * unmanaged by BSP430 (see #configBSP430_HAL_TA1_CC0_ISR).
 *
 * Set this flag to 1 if you want BSP430 to provide an implementation for
 * the CC0 interrupt for the HAL timer that underlies the
 * implementation, thereby allowing BSP430 to dispatch CC0 events to
 * callbacks registered in #sBSP430halTIMER.
 *
 * Set this flag to 0 if you intend to implement your own CC0 ISR or
 * will not be using CC0 on #BSP430_TIMER_CCACLK_PERIPH_HANDLE.
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK
 *
 * Defined to a true value if ACLK-triggered timer captures are
 * supported using the platform-specific values of:
 *
 * <ul>
 * <li>#BSP430_TIMER_CCACLK_PERIPH_HANDLE
 * <li>#BSP430_TIMER_CCACLK_CC_INDEX
 * <li>#BSP430_TIMER_CCACLK_CCIS
 * </ul>
 *
 * The values above should not be referenced if this macro is
 * undefined or has a false value.
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
 * @platformdefault
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_PERIPH_HANDLE
 *
 * The peripheral handle for a timer that is capable of using ACLK as
 * a capture/compare input.  Preferably this would not be the same as
 * #BSP430_UPTIME_TIMER_PERIPH_HANDLE.  The intended use of this timer
 * is to measure pulses of some fast clock (SMCLK or an external
 * clock) against ACLK.
 *
 * See #configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE if you intend
 * to override the platform default.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @platformdefault
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_CC_INDEX
 *
 * The capture/compare block index within
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE that can use ACLK as an input
 * signal.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @platformdefault
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CC_INDEX include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_CCIS
 *
 * The capture/compare block input signal bits to be selected in the
 * control word for CC block #BSP430_TIMER_CCACLK_CC_INDEX of
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE to use ACLK as an input signal.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @platformdefault
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CCIS include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
 *
 * The BSP430 port peripheral on which the external clock source for
 * BSP430_TIMER_CCACLK_PERIPH_HANDLE can be found.
 *
 * @note If #configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE is true,
 * the corresponding HAL interface will default to enabled.
 *
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_CLK_PORT_BIT
 *
 * The pin on BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE to which the
 * external clock source for BSP430_TIMER_CCACLK_PERIPH_HANDLE can be
 * provided.
 *
 * @ingroup grp_timer_ccaclk */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

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
 * @return -1 if @p capture_mode is not valid or the timer is
 * unrecognized or stopped.  Otherwise the delta in the counter of the
 * timer over @p count captures.
 * */
unsigned int uiBSP430timerCaptureDelta_ni (tBSP430periphHandle periph,
                                           int ccidx,
                                           unsigned int capture_mode,
                                           unsigned int ccis,
                                           unsigned int count);

/** Layout for Timer_A and Timer_B peripherals.
 */
typedef struct sBSP430hplTIMER {
  unsigned int ctl;			/**< Control register (TA0CTL) */
  union {
    unsigned int cctl[7];		/**< Capture/Compare Control registers (indexed) */
    struct {
      unsigned int cctl0;	/**< Tx#CCTL0 */
      unsigned int cctl1;	/**< Tx#CCTL1 */
      unsigned int cctl2;	/**< Tx#CCTL2 */
      unsigned int cctl3;	/**< Tx#CCTL3 */
      unsigned int cctl4;	/**< Tx#CCTL4 */
      unsigned int cctl5; /**< TB#CCTL5 */
      unsigned int cctl6;	/**< TB#CCTL6 */
    };
  };
  unsigned int r;				/**< Raw timer counter (TA0R) */
  union {
    unsigned int ccr[7];	/**< Capture/Compare registers (indexed) */
    struct {
      unsigned int ccr0;	/**< Tx#CCR0 */
      unsigned int ccr1;	/**< Tx#CCR1 */
      unsigned int ccr2;	/**< Tx#CCR2 */
      unsigned int ccr3;	/**< Tx#CCR3 */
      unsigned int ccr4;	/**< Tx#CCR4 */
      unsigned int ccr5;	/**< TB#CCR5 */
      unsigned int ccr6;	/**< TB#CCR6 */
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
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Field value for variant stored in sBSP430halTIMER.hal_state.cflags
 * when HPL reference is to an #sBSP430hplTIMER. */
#define BSP430_TIMER_HAL_HPL_VARIANT_TIMER 1

/** Structure holding hardware abstraction layer state for Timer_A and
 * Timer_B. */
typedef struct sBSP430halTIMER {
  /** Common header used to extract the correct HPL pointer type from
   * the hpl union. */
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

/** Provide the frequency of the timer source, if that can be determined.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return 0 if the timer is stopped; -1 if the frequency cannot be
 * determined, a positive value for a known source. */
unsigned long ulBSP430timerFrequency_Hz_ni (tBSP430periphHandle periph);

/** Read the timer counter assuming interrupts are disabled.
 *
 * @note This routine accounts for the possibility of a single
 * as-yet-unhandled overflow event in the timer, but the counter will
 * generally be wrong if interrupts are disabled most of the time
 * resulting in lost overflow events.
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
 */
static unsigned long
BSP430_CORE_INLINE
ulBSP430timerCounter (hBSP430halTIMER timer,
                      unsigned int * overflowp)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  unsigned long rv;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = ulBSP430timerCounter_ni(timer, overflowp);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Read the timer counter.
 *
 * @param timer The timer for which the count is desired.
 *
 * @param overflowp An optional pointer in which the high word of the
 * overflow counter is stored, supporting a 48-bit counter.
 *
 * @return A 32-bit unsigned count of the number of clock ticks
 * observed since the timer was last reset. */
unsigned long ulBSP430timerCounter (hBSP430halTIMER timer,
                                    unsigned int * overflowp);

/** Reset the timer counter.
 *
 * This clears both the overflow count and the timer internal counter.
 * It does not start or stop the timer. */
void vBSP430timerResetCounter_ni (hBSP430halTIMER timer);

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
 * @return As with iBSP430halISRCallbackVoid().
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
struct sBSP430timerAlarm {
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
  iBSP430timerAlarmCallback_ni callback;

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
};

/** Initialize an alarm structure.
 *
 * This routine configures the @p alarm structure so it is prepared to
 * operate using capture/compare register @p ccidx of the timer
 * peripheral identified by @p periph.  This involves setting various
 * fields inside the @p alarm structure; those settings should never
 * be manipulated by user code.
 *
 * Note that this simply initializes the structure; it does not set
 * any alarms, nor does it enable the alarm.
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
 * @param callback the callback function to be invoked when the alarm
 * goes off.  This may be a null pointer, in which case the alarm will
 * be disabled and the system will return from low power mode if
 * necessary.
 *
 * @return A non-null handle for the alarm.  A null handle will be
 * returned if initialization failed, e.g. because @p periph could not
 * be identified as a timer with a HAL supporting an alarm.
 *
 * @ingroup grp_timer_alarm */
hBSP430timerAlarm hBSP430timerAlarmInitialize (struct sBSP430timerAlarm * alarm,
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
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rv;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
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
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rv;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerAlarmSetEnabled_ni(alarm, 0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Time limit for past alarms.
 *
 * If the requested time is less than this many ticks in the past,
 * iBSP430timerAlarmSet_ni() will not schedule the alarm and will
 * return #BSP430_TIMER_ALARM_SET_PAST.  Times more than this number
 * of ticks in the past are assumed to be valid future times that were
 * subject to 32-bit overflow.
 *
 * @defaulted
 * @ingroup grp_timer_alarm */
#ifndef BSP430_TIMER_ALARM_PAST_LIMIT
#define BSP430_TIMER_ALARM_PAST_LIMIT 65536UL
#endif /* BSP430_TIMER_ALARM_PAST_LIMIT */

/** Time limit for future alarms.
 *
 * If the requested time is less than this many ticks in the future,
 * iBSP430timerAlarmSet_ni() will not schedule the alarm and will
 * returned #BSP430_TIMER_ALARM_SET_NOW.  This more than this limit in
 * the future will be scheduled.
 *
 * @defaulted
 * @ingroup grp_timer_alarm */
#ifndef BSP430_TIMER_ALARM_FUTURE_LIMIT
#define BSP430_TIMER_ALARM_FUTURE_LIMIT 5UL
#endif /* BSP430_TIMER_ALARM_FUTURE_LIMIT */

/** Value returned by iBSP430timerAlarmSet_ni() when the requested
 * time is too near for the scheduling to be reliable.  See
 * #BSP430_TIMER_ALARM_FUTURE_LIMIT.
 *
 * @ingroup grp_timer_alarm */
#define BSP430_TIMER_ALARM_SET_NOW 1

/** Value returned by iBSP430timerAlarmSet_ni() when the requested
 * time appears to have recently passed.  See
 * #BSP430_TIMER_ALARM_PAST_LIMIT.
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
 * @warning The default value for BSP430_TIMER_ALARM_PAST_LIMIT
 * assumes the interrupt will be processed within a single overflow of
 * the underlying 16-bit timer peripheral.  When a busy system uses
 * very high-speed timers, e.g. an undivided @c SMCLK, application
 * code should take into account that the
 * sBSP430timerAlarm::setting_tck value may be so far in the past that
 * new settings are mis-interpreted as being in the far future.
 *
 * @param alarm a pointer to an alarm structure initialized using
 * iBSP430timerAlarmInitialize().
 *
 * @param setting_tck the time at which the alarm should go off.
 *
 * @return
 * @li Zero to indicate the alarm was successfully scheduled;
 * @li The positive value #BSP430_TIMER_ALARM_SET_NOW if @p
 * setting_tck is too near for the alarm infrastructure to guarantee
 * delivery of the alarm event;
 * @li The positive value #BSP430_TIMER_ALARM_SET_PAST if @p
 * setting_tck appears to be in the recent past;
 * @li The negative value #BSP430_TIMER_ALARM_SET_ALREADY if the alarm
 * was already scheduled;
 * @li other negative values indicating errors.
 *
 * @ingroup grp_timer_alarm */
int iBSP430timerAlarmSet_ni (hBSP430timerAlarm alarm,
                             unsigned long setting_tck);

/** Wrapper to invoke iBSP430timerAlarmSet_ni() when interrupts are
 * enabled.
 *
 * @ingroup grp_timer_alarm */
static BSP430_CORE_INLINE
int iBSP430timerAlarmSet (hBSP430timerAlarm alarm,
                          unsigned long setting_tck)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rv;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
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
 * @return zero if the alarm was disabled.  A negative value indicates
 * an error, such as that the alarm was not set when this was called.
 * This may indicate that the alarm had already fired.
 *
 * @ingroup grp_timer_alarm */
int iBSP430timerAlarmCancel_ni (hBSP430timerAlarm alarm);

/** Wrapper to invoke iBSP430timerCancel_ni() when interrupts are
 * enabled.
 *
 * @ingroup grp_timer_alarm */
static BSP430_CORE_INLINE
int iBSP430timerAlarmCancel (hBSP430timerAlarm alarm)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rv;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430timerAlarmCancel_ni(alarm);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** @def configBSP430_HAL_TA0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TA0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TA0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TA0
#define configBSP430_HAL_TA0 0
#endif /* configBSP430_HAL_TA0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TA0 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TA0_;
#endif /* configBSP430_HAL_TA0 */
/** @endcond */

/** BSP430 HAL handle for TA0.
 *
 * The handle may be used only if #configBSP430_HAL_TA0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA0 - 0)
#define BSP430_HAL_TA0 (&xBSP430hal_TA0_)
#endif /* configBSP430_HAL_TA0 */

/** @def configBSP430_HAL_TA1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TA1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TA1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TA1
#define configBSP430_HAL_TA1 0
#endif /* configBSP430_HAL_TA1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TA1 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TA1_;
#endif /* configBSP430_HAL_TA1 */
/** @endcond */

/** BSP430 HAL handle for TA1.
 *
 * The handle may be used only if #configBSP430_HAL_TA1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA1 - 0)
#define BSP430_HAL_TA1 (&xBSP430hal_TA1_)
#endif /* configBSP430_HAL_TA1 */

/** @def configBSP430_HAL_TA2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA2 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TA2 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TA2 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TA2
#define configBSP430_HAL_TA2 0
#endif /* configBSP430_HAL_TA2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TA2 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TA2_;
#endif /* configBSP430_HAL_TA2 */
/** @endcond */

/** BSP430 HAL handle for TA2.
 *
 * The handle may be used only if #configBSP430_HAL_TA2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA2 - 0)
#define BSP430_HAL_TA2 (&xBSP430hal_TA2_)
#endif /* configBSP430_HAL_TA2 */

/** @def configBSP430_HAL_TA3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA3 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TA3 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TA3 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TA3
#define configBSP430_HAL_TA3 0
#endif /* configBSP430_HAL_TA3 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TA3 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TA3_;
#endif /* configBSP430_HAL_TA3 */
/** @endcond */

/** BSP430 HAL handle for TA3.
 *
 * The handle may be used only if #configBSP430_HAL_TA3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA3 - 0)
#define BSP430_HAL_TA3 (&xBSP430hal_TA3_)
#endif /* configBSP430_HAL_TA3 */

/** @def configBSP430_HAL_TB0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TB0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TB0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TB0
#define configBSP430_HAL_TB0 0
#endif /* configBSP430_HAL_TB0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TB0 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TB0_;
#endif /* configBSP430_HAL_TB0 */
/** @endcond */

/** BSP430 HAL handle for TB0.
 *
 * The handle may be used only if #configBSP430_HAL_TB0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TB0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TB0 - 0)
#define BSP430_HAL_TB0 (&xBSP430hal_TB0_)
#endif /* configBSP430_HAL_TB0 */

/** @def configBSP430_HAL_TB1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TB1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TB1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TB1
#define configBSP430_HAL_TB1 0
#endif /* configBSP430_HAL_TB1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TB1 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TB1_;
#endif /* configBSP430_HAL_TB1 */
/** @endcond */

/** BSP430 HAL handle for TB1.
 *
 * The handle may be used only if #configBSP430_HAL_TB1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TB1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TB1 - 0)
#define BSP430_HAL_TB1 (&xBSP430hal_TB1_)
#endif /* configBSP430_HAL_TB1 */

/** @def configBSP430_HAL_TB2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB2 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TB2 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TB2 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TB2
#define configBSP430_HAL_TB2 0
#endif /* configBSP430_HAL_TB2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TB2 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TB2_;
#endif /* configBSP430_HAL_TB2 */
/** @endcond */

/** BSP430 HAL handle for TB2.
 *
 * The handle may be used only if #configBSP430_HAL_TB2
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
/** @def configBSP430_HPL_TA0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TA0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_TA1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TA1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_TA2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TA2 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_TA3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA3 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TA3 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_TB0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TB0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_TB1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TB1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_TB2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TB2 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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
/** HPL pointer for TA0.
 *
 * Typed pointer to a volatile structure overlaying the TA0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TA0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA0 - 0)
#define BSP430_HPL_TA0 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA0)
#endif /* configBSP430_HPL_TA0 */

/** HPL pointer for TA1.
 *
 * Typed pointer to a volatile structure overlaying the TA1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TA1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA1 - 0)
#define BSP430_HPL_TA1 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA1)
#endif /* configBSP430_HPL_TA1 */

/** HPL pointer for TA2.
 *
 * Typed pointer to a volatile structure overlaying the TA2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TA2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA2 - 0)
#define BSP430_HPL_TA2 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA2)
#endif /* configBSP430_HPL_TA2 */

/** HPL pointer for TA3.
 *
 * Typed pointer to a volatile structure overlaying the TA3
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TA3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA3 - 0)
#define BSP430_HPL_TA3 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA3)
#endif /* configBSP430_HPL_TA3 */

/** HPL pointer for TB0.
 *
 * Typed pointer to a volatile structure overlaying the TB0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TB0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB0 - 0)
#define BSP430_HPL_TB0 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB0)
#endif /* configBSP430_HPL_TB0 */

/** HPL pointer for TB1.
 *
 * Typed pointer to a volatile structure overlaying the TB1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TB1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB1 - 0)
#define BSP430_HPL_TB1 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB1)
#endif /* configBSP430_HPL_TB1 */

/** HPL pointer for TB2.
 *
 * Typed pointer to a volatile structure overlaying the TB2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TB2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB2 - 0)
#define BSP430_HPL_TB2 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB2)
#endif /* configBSP430_HPL_TB2 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_timer_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_timer_isr_decl] */
/** @def configBSP430_HAL_TA0_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA0, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TA0_ISR, enabling #configBSP430_HAL_TA0
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TA0 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA0_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA0_CC0_ISR - 0) && ! (configBSP430_HAL_TA0 - 0)
#warning configBSP430_HAL_TA0_CC0_ISR requested without configBSP430_HAL_TA0
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TA0_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TA0 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA0 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TA0 */
#ifndef configBSP430_HAL_TA0_ISR
#define configBSP430_HAL_TA0_ISR (configBSP430_HAL_TA0 - 0)
#endif /* configBSP430_HAL_TA0_ISR */

#if (configBSP430_HAL_TA0_ISR - 0) && ! (configBSP430_HAL_TA0 - 0)
#warning configBSP430_HAL_TA0_ISR requested without configBSP430_HAL_TA0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TA1_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA1, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TA1_ISR, enabling #configBSP430_HAL_TA1
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TA1 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA1_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA1_CC0_ISR - 0) && ! (configBSP430_HAL_TA1 - 0)
#warning configBSP430_HAL_TA1_CC0_ISR requested without configBSP430_HAL_TA1
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TA1_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TA1 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA1 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TA1 */
#ifndef configBSP430_HAL_TA1_ISR
#define configBSP430_HAL_TA1_ISR (configBSP430_HAL_TA1 - 0)
#endif /* configBSP430_HAL_TA1_ISR */

#if (configBSP430_HAL_TA1_ISR - 0) && ! (configBSP430_HAL_TA1 - 0)
#warning configBSP430_HAL_TA1_ISR requested without configBSP430_HAL_TA1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TA2_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA2, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TA2_ISR, enabling #configBSP430_HAL_TA2
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TA2 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA2_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA2_CC0_ISR - 0) && ! (configBSP430_HAL_TA2 - 0)
#warning configBSP430_HAL_TA2_CC0_ISR requested without configBSP430_HAL_TA2
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TA2_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TA2 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA2 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TA2 */
#ifndef configBSP430_HAL_TA2_ISR
#define configBSP430_HAL_TA2_ISR (configBSP430_HAL_TA2 - 0)
#endif /* configBSP430_HAL_TA2_ISR */

#if (configBSP430_HAL_TA2_ISR - 0) && ! (configBSP430_HAL_TA2 - 0)
#warning configBSP430_HAL_TA2_ISR requested without configBSP430_HAL_TA2
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TA3_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA3, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TA3_ISR, enabling #configBSP430_HAL_TA3
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TA3 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA3_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA3_CC0_ISR - 0) && ! (configBSP430_HAL_TA3 - 0)
#warning configBSP430_HAL_TA3_CC0_ISR requested without configBSP430_HAL_TA3
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TA3_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TA3 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA3 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TA3 */
#ifndef configBSP430_HAL_TA3_ISR
#define configBSP430_HAL_TA3_ISR (configBSP430_HAL_TA3 - 0)
#endif /* configBSP430_HAL_TA3_ISR */

#if (configBSP430_HAL_TA3_ISR - 0) && ! (configBSP430_HAL_TA3 - 0)
#warning configBSP430_HAL_TA3_ISR requested without configBSP430_HAL_TA3
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TB0_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB0, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TB0_ISR, enabling #configBSP430_HAL_TB0
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TB0 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB0_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB0_CC0_ISR - 0) && ! (configBSP430_HAL_TB0 - 0)
#warning configBSP430_HAL_TB0_CC0_ISR requested without configBSP430_HAL_TB0
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TB0_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TB0 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB0 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TB0 */
#ifndef configBSP430_HAL_TB0_ISR
#define configBSP430_HAL_TB0_ISR (configBSP430_HAL_TB0 - 0)
#endif /* configBSP430_HAL_TB0_ISR */

#if (configBSP430_HAL_TB0_ISR - 0) && ! (configBSP430_HAL_TB0 - 0)
#warning configBSP430_HAL_TB0_ISR requested without configBSP430_HAL_TB0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TB1_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB1, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TB1_ISR, enabling #configBSP430_HAL_TB1
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TB1 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB1_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB1_CC0_ISR - 0) && ! (configBSP430_HAL_TB1 - 0)
#warning configBSP430_HAL_TB1_CC0_ISR requested without configBSP430_HAL_TB1
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TB1_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TB1 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB1 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TB1 */
#ifndef configBSP430_HAL_TB1_ISR
#define configBSP430_HAL_TB1_ISR (configBSP430_HAL_TB1 - 0)
#endif /* configBSP430_HAL_TB1_ISR */

#if (configBSP430_HAL_TB1_ISR - 0) && ! (configBSP430_HAL_TB1 - 0)
#warning configBSP430_HAL_TB1_ISR requested without configBSP430_HAL_TB1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TB2_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB2, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TB2_ISR, enabling #configBSP430_HAL_TB2
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TB2 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB2_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB2_CC0_ISR - 0) && ! (configBSP430_HAL_TB2 - 0)
#warning configBSP430_HAL_TB2_CC0_ISR requested without configBSP430_HAL_TB2
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TB2_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TB2 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB2 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
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
#if configBSP430_HPL_TA0 - 0
  if (BSP430_PERIPH_TA0 == periph) {
    return BSP430_HPL_TA0;
  }
#endif /* configBSP430_HPL_TA0 */

#if configBSP430_HPL_TA1 - 0
  if (BSP430_PERIPH_TA1 == periph) {
    return BSP430_HPL_TA1;
  }
#endif /* configBSP430_HPL_TA1 */

#if configBSP430_HPL_TA2 - 0
  if (BSP430_PERIPH_TA2 == periph) {
    return BSP430_HPL_TA2;
  }
#endif /* configBSP430_HPL_TA2 */

#if configBSP430_HPL_TA3 - 0
  if (BSP430_PERIPH_TA3 == periph) {
    return BSP430_HPL_TA3;
  }
#endif /* configBSP430_HPL_TA3 */

#if configBSP430_HPL_TB0 - 0
  if (BSP430_PERIPH_TB0 == periph) {
    return BSP430_HPL_TB0;
  }
#endif /* configBSP430_HPL_TB0 */

#if configBSP430_HPL_TB1 - 0
  if (BSP430_PERIPH_TB1 == periph) {
    return BSP430_HPL_TB1;
  }
#endif /* configBSP430_HPL_TB1 */

#if configBSP430_HPL_TB2 - 0
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
#if configBSP430_HAL_TA0 - 0
  if (BSP430_PERIPH_TA0 == periph) {
    return BSP430_HAL_TA0;
  }
#endif /* configBSP430_HAL_TA0 */

#if configBSP430_HAL_TA1 - 0
  if (BSP430_PERIPH_TA1 == periph) {
    return BSP430_HAL_TA1;
  }
#endif /* configBSP430_HAL_TA1 */

#if configBSP430_HAL_TA2 - 0
  if (BSP430_PERIPH_TA2 == periph) {
    return BSP430_HAL_TA2;
  }
#endif /* configBSP430_HAL_TA2 */

#if configBSP430_HAL_TA3 - 0
  if (BSP430_PERIPH_TA3 == periph) {
    return BSP430_HAL_TA3;
  }
#endif /* configBSP430_HAL_TA3 */

#if configBSP430_HAL_TB0 - 0
  if (BSP430_PERIPH_TB0 == periph) {
    return BSP430_HAL_TB0;
  }
#endif /* configBSP430_HAL_TB0 */

#if configBSP430_HAL_TB1 - 0
  if (BSP430_PERIPH_TB1 == periph) {
    return BSP430_HAL_TB1;
  }
#endif /* configBSP430_HAL_TB1 */

#if configBSP430_HAL_TB2 - 0
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

#endif /* BSP430_MODULE_TIMER */

#endif /* BSP430_PERIPH_TIMER_H */
