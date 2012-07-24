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
 * It is recommended that the default configuration using undivided
 * ACLK be retained, and that this functionality be enabled by
 * defining #configBSP430_UPTIME and invoking #vBSP430uptimeStart on
 * hardware initialization (as soon as the crystal driving ACLK has
 * been stabilized).
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
 * maintain a continuous system clock.
 *
 * @note #BSP430_UPTIME_TIMER_HAL_HANDLE must also be defined, and the clock
 * and its HAL ISR must be enabled (e.g., using
 * #configBSP430_PERIPH_TA0 and #configBSP430_HAL_TA0_ISR).
 */
#ifndef configBSP430_UPTIME
#define configBSP430_UPTIME 0
#endif /* configBSP430_UPTIME */

/** @def BSP430_UPTIME_TIMER_HAL_HANDLE
 *
 * Define to the handle of a HAL timer that can be used to maintain a
 * continuous system clock sourced from ACLK.
 */
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
 */
#ifndef BSP430_UPTIME_SSEL
#define BSP430_UPTIME_SSEL TASSEL_1
#endif /* BSP430_UPTIME_SSEL */

/** @def BSP430_UPTIME_DIVIDING_SHIFT
 *
 * Shift value dividing #BSP430_UPTIME_SSEL for the uptime clock.
 *
 */
#ifndef BSP430_UPTIME_DIVIDING_SHIFT
#define BSP430_UPTIME_DIVIDING_SHIFT 0
#endif /* BSP430_UPTIME_DIVIDING_SHIFT */

/** @def BSP430_UPTIME_CLOCK_HZ
 *
 * Number of uptime clocks per second. */
#ifndef BSP430_UPTIME_CLOCK_HZ
#define BSP430_UPTIME_CLOCK_HZ (BSP430_CLOCK_NOMINAL_ACLK_HZ >> BSP430_UPTIME_DIVIDING_SHIFT)
#endif /* BSP430_UPTIME_CLOCK_HZ */

/** Return system uptime in clock ticks.
 *
 * This function should be called with interrupts disabled. 
 */
static unsigned long
__inline__
ulBSP430uptimeFromISR (void)
{
	return ulBSP430timerCounterFromISR(BSP430_UPTIME_TIMER_HAL_HANDLE, 0);
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
void vBSP430uptimeStart (void);

/** Suspend the system uptime clock.
 *
 * Suspension means that the clocks are halted and interrupts from
 * them inhibited.  Counter values are not modified.
 * #vBSP430uptimeResume should be invoked to release the clocks to
 * continue counting.
 *
 * The system clock may be suspended prior to entering certain low
 * power modes.
 */
void vBSP430uptimeSuspend (void);

/** Resume the system uptime clock
 *
 * The clocks are re-enabled to continue counting from their last
 * recorded value.
 */
void vBSP430uptimeResume (void);

#endif /* BSP430_UTILITY_UPTIME_H */
