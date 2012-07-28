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
 * @brief Support for maintaining a system uptime counter.
 *
 * This module provides routines to initialize and query a long-term
 * clock, normally using #xBSP430timer_TA0 sourced by an undivided
 * ACLK.  The feature is enabled by adding the following to your
 * bsp430_config.h:
 *
 * @code

#define configBSP430_UPTIME 1
#define configBSP430_HAL_TA0 1

 * @endcode
 *
 * #vBSP430uptimeStart_ni must be invoked on hardware initialization
 * after configuring the system clocks.  This is done for you in
 * #vBSP430platformSetup_ni if #configBSP430_UPTIME is true.
 *
 * See #configBSP430_UPTIME_USE_DEFAULT_RESOURCE if you want to use
 * something other than #xBSP430timer_TA0 as the uptime clock source.
 *
 * @author Peter A. Bigot <bigotp@acm.org> @homepage
 * http://github.com/pabigot/freertos-mspgcc @date 2012 @copyright <a
 * href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_UPTIME_H
#define BSP430_UTILITY_UPTIME_H

#include <bsp430/periph/timer.h>
#include <bsp430/clock.h>

/** @def configBSP430_UPTIME
 *
 * Define to a true value to enable the uptime infrastructure to
 * maintain a continuous system clock.  Must be accompanied by the
 * required enabling of the HAL interrupt handler which monitors
 * counter overflows:
 *
 * @code

#define configBSP430_UPTIME 1
#define configBSP430_HAL_TA0 1

 * @endcode
 *
 * See #configBSP430_UPTIME_USE_DEFAULT_RESOURCE.
 * 
 * @defaulted 
 */
#ifndef configBSP430_UPTIME
#define configBSP430_UPTIME 0
#endif /* configBSP430_UPTIME */

/** @def configBSP430_UPTIME_USE_DEFAULT_RESOURCE
 *
 * In almost all cases, when #configBSP430_UPTIME is enabled
 * #xBSP430timer_TA0 will be used as #BSP430_UPTIME_TIMER_HAL_HANDLE.
 * Correct functioning requires that #configBSP430_PERIPH_TA0 and
 * #configBSP430_HAL_TA0_ISR be set as well.  Although these are
 * enabled by default when #configBSP430_HAL_TA0 is enabled, it is
 * easy to forget this step.
 *
 * While it is not possible within the uptime header to correct a
 * misconfigured system, if the selected peripheral can be identified
 * by the C preprocessor it is possible to detect the misconfiguration
 * and emit a warning.  Setting this option to a false value will
 * inhibit that check, and must be done if
 * #BSP430_UPTIME_TIMER_HAL_HANDLE is changed from its default
 * value. 
 * 
 * @defaulted */
#ifndef configBSP430_UPTIME_USE_DEFAULT_RESOURCE
#define configBSP430_UPTIME_USE_DEFAULT_RESOURCE 1
#endif /* configBSP430_UPTIME_USE_DEFAULT_RESOURCE */

#if configBSP430_UPTIME_USE_DEFAULT_RESOURCE - 0
#if defined(BSP430_UPTIME_TIMER_HAL_HANDLE)
#warning Override of BSP430_UPTIME_TIMER_HAL_HANDLE with true configBSP430_UPTIME_USE_DEFAULT_RESOURCE
#endif /* BSP430_UPTIME_TIMER_HAL_HANDLE */
#if !(configBSP430_PERIPH_TA0 - 0)
#warning configBSP430_UPTIME_USE_DEFAULT_RESOURCE but configBSP430_PERIPH_TA0 not enabled
#endif /* configBSP430_PERIPH_TA0 */
#if !(configBSP430_HAL_TA0_ISR - 0)
#warning configBSP430_UPTIME_USE_DEFAULT_RESOURCE but configBSP430_HAL_TA0_ISR not enabled
#endif /* configBSP430_HAL_TA0_ISR */
#endif /* configBSP430_UPTIME_USE_DEFAULT_RESOURCE */

/** @def BSP430_UPTIME_TIMER_HAL_HANDLE
 *
 * Define to the handle of a HAL timer that can be used to maintain a
 * continuous system clock sourced from ACLK.
 *
 * @warning If you set this, you must also set
 * #configBSP430_UPTIME_USE_DEFAULT_RESOURCE to be a false value.  You
 * must also ensure the corresponding peripheral
 * (e.g. #configBSP430_PERIPH_TA1) and HAL interrupt that will count
 * overflows (e.g. #configBSP430_HAL_TA1_ISR) are enabled, for example
 * by enabling #configBSP430_HAL_TA1, as the check for these
 * requirements cannot be enforced for non-default resources. 
 * 
 * @defaulted */
#ifndef BSP430_UPTIME_TIMER_HAL_HANDLE
#define BSP430_UPTIME_TIMER_HAL_HANDLE xBSP430timer_TA0
#endif /* BSP430_UPTIME_TIMER_HAL_HANDLE */

/** @def BSP430_UPTIME_SSEL
 *
 * Source selector for uptime clock.  This should be bits suitable for
 * storage into the TASSEL field of a timer control register.  (Note
 * that TASSEL and TBSSEL support the same values).
 *
 * @note If you select a source other than ACLK (@c TASSEL_1), update
 * #BSP430_UPTIME_CLOCK_HZ.
 * 
 * @defaulted */
#ifndef BSP430_UPTIME_SSEL
#define BSP430_UPTIME_SSEL TASSEL_1 /* == TASSEL__ACLK */
#endif /* BSP430_UPTIME_SSEL */

/** @def BSP430_UPTIME_DIVIDING_SHIFT
 *
 * Shift value dividing #BSP430_UPTIME_SSEL for the uptime clock.
 * 
 * @defaulted 
 */
#ifndef BSP430_UPTIME_DIVIDING_SHIFT
#define BSP430_UPTIME_DIVIDING_SHIFT 0
#endif /* BSP430_UPTIME_DIVIDING_SHIFT */

/** @def BSP430_UPTIME_CLOCK_HZ
 *
 * Number of uptime clocks per second.
 * 
 * @defaulted  */
#ifndef BSP430_UPTIME_CLOCK_HZ
#define BSP430_UPTIME_CLOCK_HZ (BSP430_CLOCK_NOMINAL_ACLK_HZ >> BSP430_UPTIME_DIVIDING_SHIFT)
#endif /* BSP430_UPTIME_CLOCK_HZ */

/** Return system uptime in clock ticks with disabled interrupts. */
static unsigned long
__inline__
ulBSP430uptime_ni (void)
{
  return ulBSP430timerCounter_ni(BSP430_UPTIME_TIMER_HAL_HANDLE, 0);
}

/** Return the system uptime in clock ticks. */
static unsigned long
__inline__
ulBSP430uptime (void)
{
  return ulBSP430timerCounter(BSP430_UPTIME_TIMER_HAL_HANDLE, 0);
}

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
 * recorded value.
 */
void vBSP430uptimeResume_ni (void);

#endif /* BSP430_UTILITY_UPTIME_H */
