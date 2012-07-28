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
 * @brief Common header included by all BSP430 leaf headers.
 *
 * This must be included by all BSP430 headers that do not include
 * another BSP430 header, so that it is available in all environments
 * where a BSP430 capability is referenced.  It is responsible for
 * including any RTOS-related header files that contain
 * application-specific configuration information that must be
 * consistent between headers and implementation files.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */
#ifndef BSP430_COMMON_H
#define BSP430_COMMON_H

#include <msp430.h>

/** Version identifier for the BSP430 library
 *
 * A monotonically non-decreasing integer reflecting the version of
 * the BSP430 library that is being used.  The value represents a
 * development freeze date in the form YYYYMMDD as a decimal
 * number. */
#define BSP430_VERSION 20120713

/** @def configBSP430_CONFIG_FILE
 *
 * BSP430 depends heavily on configuration specified by preprocessor
 * tokens defined to true or false values.  Proper application
 * behavior requires that all object files be built using the same
 * configuration settings.
 *
 * All BSP430 implementation files include <bsp430/common.h>, either
 * directly or through other headers.  Defining
 * #configBSP430_CONFIG_FILE to a true value (which is its default)
 * will cause this header to include a file named "bsp430_config.h".
 * All application configuration may be placed into this file.
 *
 * If you provide configuration through external means (e.g., through
 * @c -D arguments to the preprocessor) you can inhibit this inclusion
 * by defining #configBSP430_CONFIG_FILE to 0.
 *
 * @defaulted */
#ifndef configBSP430_CONFIG_FILE
#define configBSP430_CONFIG_FILE 1
#endif /* configBSP430_CONFIG_FILE */

#if configBSP430_CONFIG_FILE - 0
#include "bsp430_config.h"
#endif /* configBSP430_CONFIG_FILE */

/** @def BSP430_CORE_INTERRUPT_STATE_T
 *
 * A type that can be used to declare a variable that will hold
 * interrupt state stored by #BSP430_CORE_SAVE_INTERRUPT_STATE.
 *
 * @defaulted */
#ifndef BSP430_CORE_INTERRUPT_STATE_T
#define BSP430_CORE_INTERRUPT_STATE_T __istate_t
#endif /* BSP430_CORE_INTERRUPT_STATE_T */

/** @def BSP430_CORE_SAVE_INTERRUPT_STATE(_state)
 *
 * A function macro that will record whether interrupts are currently
 * enabled in the state parameter.  The parameter should subsequently
 * be passed to #BSP430_CORE_RESTORE_INTERRUPT_STATE.
 *
 * @param _state where the interrupt enable/disable state is stored.
 *
 * @defaulted */
#ifndef BSP430_CORE_SAVE_INTERRUPT_STATE
#define BSP430_CORE_SAVE_INTERRUPT_STATE(_state) do {	\
    (_state) = __get_interrupt_state();                 \
  } while (0)
#endif /* BSP430_CORE_SAVE_INTERRUPT_STATE */

/** @def BSP430_CORE_RESTORE_INTERRUPT_STATE(_state)
 *
 * A function macro that will enable or disable interrupts as recorded
 * in the provided state parameter.  The parameter value should have
 * been created using #BSP430_CORE_SAVE_INTERRUPT_STATE.
 *
 * @param _state where the interrupt enable/disable state is stored.
 *
 * @defaulted */
#ifndef BSP430_CORE_RESTORE_INTERRUPT_STATE
#define BSP430_CORE_RESTORE_INTERRUPT_STATE(_state) do {	\
    __set_interrupt_state(_state);                              \
  } while (0)
#endif /* BSP430_CORE_RESTORE_INTERRUPT_STATE */

/** @def BSP430_CORE_ENABLE_INTERRUPT()
 *
 * Set the status register #GIE bit so that interrupts are enabled.
 *
 * @defaulted */
#ifndef BSP430_CORE_ENABLE_INTERRUPT
#define BSP430_CORE_ENABLE_INTERRUPT() __enable_interrupt()
#endif /* BSP430_CORE_ENABLE_INTERRUPT */

/** @def BSP430_CORE_DISABLE_INTERRUPT()
 *
 * Clear the status register #GIE bit so that interrupts are disabled.
 *
 * @defaulted */
#ifndef BSP430_CORE_DISABLE_INTERRUPT
#define BSP430_CORE_DISABLE_INTERRUPT() __disable_interrupt()
#endif /* BSP430_CORE_DISABLE_INTERRUPT */

/* See <bsp430/rtos/freertos.h> */
#if configBSP430_RTOS_FREERTOS - 0
/* FreeRTOS defines application behavior in a shared header.  Read it
 * in here so everybody agrees on its contents. */

#include "FreeRTOS.h"

#define BSP430_RTOS_YIELD_FROM_ISR() portYIELD_FROM_ISR(1)

#endif /* configBSP430_RTOS_FREERTOS */

/** @def BSP430_RTOS_YIELD_FROM_ISR
 *
 * Instruct the RTOS environment to execute a context switch.  This
 * should only be invoked from with the function that is the top-half
 * of an interrupt service routine, and is usually invoked within
 * #BSP430_PERIPH_ISR_CALLBACK_TAIL when
 * #BSP430_PERIPH_ISR_CALLBACK_YIELD has been set.
 *
 * It is defined to a no-op in environments that do not support
 * context switches.
 *
 * @defaulted */
#ifndef BSP430_RTOS_YIELD_FROM_ISR
#define BSP430_RTOS_YIELD_FROM_ISR() do { } while (0)
#endif /* BSP430_RTOS_YIELD_FROM_ISR */

#endif /* BSP430_COMMON_H */
