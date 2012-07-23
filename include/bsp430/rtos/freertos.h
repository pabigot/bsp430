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
 * @brief BSP430 interface to FreeRTOS
 *
 * Primarily this enables use of the BSP430 timer infrastructure as
 * the FreeRTOS task scheduler.  The benefits of that include ability
 * to use TA0 as a system clock, and ability to temporarily inhibit
 * task timeslice interrupts to reduce power when no activity is
 * required.
 *
 * This header is not included by any BSP430 code, nor is it directly
 * included by application code.  It is included by the
 * freertos-mspgcc port of FreeRTOS when the @c portmacro.h header is
 * included if #configBSP430_RTOS_FREERTOS is defined to a true value
 * by the compilation environment.  The @c FreeRTOS.h header is
 * similarly included by @c bsp430/common.h with this same cue.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RTOS_FREERTOS_H
#define BSP430_RTOS_FREERTOS_H

/** Mark that BSP430 will be running under FreeRTOS.
 *
 * Defining this macro to a true value externally to the system will
 * cause the inclusion of bsp430/common.h to also include FreeRTOS.h,
 * making available the standard FreeRTOS environment.  This should be
 * done by adding <tt>-DconfigBSP430_RTOS_FREERTOS</tt> to the @c
 * CPPFLAGS variable during builds, since otherwise builds of BSP430
 * sources will not attempt to read configuration data provided in @c
 * FreeRTOS_Config.h.
 */
#ifndef configBSP430_RTOS_FREERTOS
#define configBSP430_RTOS_FREERTOS 0
#endif /* configBSP430_RTOS_FREERTOS */

/** Use the BSP430 timer infrastructure as the FreeRTOS scheduler.
 *
 */
#ifndef configBSP430_FREERTOS_SCHEDULER
#define configBSP430_FREERTOS_SCHEDULER 1
#endif /* configBSP430_FREERTOS_SCHEDULER */

/** FreeRTOS scheduler requires use of TA0 */
#define configBSP430_PERIPH_TA0 1
/** FreeRTOS scheduler defines its own TA0 CC0 ISR */
#define configBSP430_HAL_TA0_CC0_ISR 0

/** Invoke to suspend the FreeRTOS scheduler.
 *
 * This does not suspend any tasks, or prevent the scheduler from
 * resuming the way vTaskEndScheduler does.  It merely inhibits the
 * task timeslice interrupt.  The intent is that this be invoked prior
 * to suspending the application in a low power mode, if it is
 * determined that no task is blocked on a tick-related event. */
void vBSP430freertosSuspendScheduler (void);

/** Invoke to resume the FreeRTOS scheduler.
 *
 * This re-enables the task timeslice interrupt and schedules an
 * interrupt after completion of a full timeslice.
 *
 * The intent is that this be invoked after the application wakes up
 * from a low power mode. */
void vBSP430freertosResumeScheduler (void);

#endif /* BSP430_RTOS_FREERTOS_H */
