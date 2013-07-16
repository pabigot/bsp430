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
 * @brief BSP430 interface to FreeRTOS
 *
 * More specifically, to the <a
 * href="http://github.com/pabigot/freertos-mspgcc">BSP430-enabled
 * FreeRTOS port supporting MSPGCC</a>.  Other toolchains aren't
 * intentionally excluded, they just aren't supported yet.
 *
 * This header is not included by any BSP430 code, nor should it be
 * directly included by application code.  It is included by the
 * freertos-mspgcc port of FreeRTOS via the @c portmacro.h header if
 * #configBSP430_RTOS_FREERTOS is defined to a true value by the
 * compilation environment.  Doing this makes basic BSP430
 * configuration data available within FreeRTOS application code.
 * Note that this inclusion occurs before FreeRTOS.h includes the
 * application-specific FreeRTOSConfig.h, allowing this file to set
 * defaults for the FreeRTOS port.
 *
 * The @c FreeRTOS.h header is similarly included by @c
 * bsp430/core.h when #configBSP430_RTOS_FREERTOS is defined to a
 * true value by the compilation environment.  This ensures that the
 * BSP430 infrastructure uses FreeRTOS-compatible mechanisms for
 * critical sections and interrupt management.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RTOS_FREERTOS_H
#define BSP430_RTOS_FREERTOS_H

#include <bsp430/core.h>
#include <bsp430/clock.h>

/** Mark that BSP430 will be running under FreeRTOS.
 *
 * Defining this macro to a true value externally to the system will:
 * <ul>
 *
 * <li> Cause the inclusion of bsp430/core.h to also include
 * "FreeRTOS.h", making available the standard FreeRTOS environment to
 * BSP430 library code and defining RTOS-related macros to use the
 * FreeRTOS implementation.
 *
 * <li> Cause the inclusion of "FreeRTOS.h" to include
 * <bsp430/core.h>, make standard BSP430 configuration data
 * available to the FreeRTOS application code.
 *
 * </ul> This cascading effect should be initiated by adding
 * <tt>-DconfigBSP430_RTOS_FREERTOS</tt> to the @c CPPFLAGS variable
 * during builds of FreeRTOS applications, including the BSP430
 * modules linked to the application.
 *
 * @defaulted */
#ifndef configBSP430_RTOS_FREERTOS
#define configBSP430_RTOS_FREERTOS 0
#endif /* configBSP430_RTOS_FREERTOS */

/** If defined to a true value, the FreeRTOS-BSP430 port.c file will
 * use the BSP430 timer infrastructure on TA0 for the task scheduler.
 * This requires that #configBSP430_HPL_TA0 be true and
 * #configBSP430_HAL_TA0_CC0_ISR be false.  Use of this feature is
 * compatible with #configBSP430_UPTIME, and allows the FreeRTOS
 * scheduler to co-exist with other users of TA0.
 *
 * @defaulted */
#ifndef configBSP430_FREERTOS_SCHEDULER
#define configBSP430_FREERTOS_SCHEDULER 1
#endif /* configBSP430_FREERTOS_SCHEDULER */

/** The FreeRTOS-BSP430 port needs to know the rate at which the
 * scheduler clock runs.  This is normally ACLK, but whether ACLK is
 * XT1CLK, VLOCLK, or some other frequency isn't really known.
 *
 * @todo Refine this
 *
 * @defaulted */
#ifndef portACLK_FREQUENCY_HZ
#define portACLK_FREQUENCY_HZ (BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? BSP430_CLOCK_NOMINAL_VLOCLK_HZ : BSP430_CLOCK_NOMINAL_XT1CLK_HZ)
#endif /* portACLK_FREQUENCY_HZ */

/** FreeRTOS scheduler requires use of TA0.
 *
 * @note If the setting here conflicts with a setting elsewhere, the
 * compiler should generate a warning. */
#define configBSP430_HPL_TA0 1

/** FreeRTOS scheduler defines its own TA0 CC0 ISR.
 *
 * @note If the setting here conflicts with a setting elsewhere, the
 * compiler should generate a warning.  */
#define configBSP430_HAL_TA0_CC0_ISR 0

/** Invoke to suspend the FreeRTOS scheduler.
 *
 * This does not suspend any tasks, or prevent the scheduler from
 * resuming the way vTaskEndScheduler does.  It merely inhibits the
 * task timeslice interrupt.  The intent is that this be invoked prior
 * to suspending the application in a low power mode that includes
 * #OSCOFF (e.g., #LPM4_bits), if it is determined that no task is
 * blocked on a tick-related event.
 *
 * @note This function is available only if
 * #configBSP430_FREERTOS_SCHEDULER is true. */
void vBSP430freertosSuspendScheduler (void);

/** Invoke to resume the FreeRTOS scheduler.
 *
 * This re-enables the task timeslice interrupt and schedules an
 * interrupt after completion of a full timeslice.
 *
 * The intent is that this be invoked after the application wakes up
 * from a low power mode in which #OSCOFF was set.
 *
 * @note This function is available only if
 #configBSP430_FREERTOS_SCHEDULER is true. */
void vBSP430freertosResumeScheduler (void);

#endif /* BSP430_RTOS_FREERTOS_H */
