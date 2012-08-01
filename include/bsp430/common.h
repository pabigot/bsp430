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

#include <msp430.h>             /* MSP430 MCU-specific information */
#include <stdint.h>             /* Size-annotated integral types (uint8_t) */
#include <stddef.h>             /* NULL and size_t */

/** Version identifier for the BSP430 library
 *
 * A monotonically non-decreasing integer reflecting the version of
 * the BSP430 library that is being used.  The value represents a
 * development freeze date in the form YYYYMMDD as a decimal
 * number. */
#define BSP430_VERSION 20120713

/** @def configBSP430_COMMON_INCLUDE_BSP430_CONFIG_FILE
 *
 * BSP430 depends heavily on configuration specified by preprocessor
 * tokens defined to true or false values.  Proper application
 * behavior requires that all object files be built using the same
 * configuration settings.
 *
 * All BSP430 implementation files include <bsp430/common.h>, either
 * directly or through other headers.  Defining
 * #configBSP430_COMMON_INCLUDE_BSP430_CONFIG_FILE to a true value
 * (which is its default) will cause this header to include a file
 * named "bsp430_config.h", which is presumed to be in the compiler's
 * include path.  All application configuration may be placed into
 * this file.  It is recommended that this file end with the inclusion
 * of <bsp430/platform/bsp430_config.h> to reduce the amount of
 * generic configuration that must be supplied.
 *
 * If you provide configuration solely through external means (e.g.,
 * through @c -D arguments to the preprocessor) you can inhibit this
 * inclusion by defining #configBSP430_COMMON_INCLUDE_BSP430_CONFIG_FILE to 0.
 *
 * @defaulted */
#ifndef configBSP430_COMMON_INCLUDE_BSP430_CONFIG_FILE
#define configBSP430_COMMON_INCLUDE_BSP430_CONFIG_FILE 1
#endif /* configBSP430_COMMON_INCLUDE_BSP430_CONFIG_FILE */

#if configBSP430_COMMON_INCLUDE_BSP430_CONFIG_FILE - 0
#include "bsp430_config.h"
#endif /* configBSP430_COMMON_INCLUDE_BSP430_CONFIG_FILE */

/** @def configBSP430_CORE_SUPPORT_WATCHDOG
 *
 * Control use of the watchdog infrastructure by BSP430.
 *
 * If defined to a true value, the watchdog will not be inhibited by
 * vBSP430platformInitialize_ni(), and any code where BSP430 executes a
 * loop will invoke #BSP430_CORE_WATCHDOG_CLEAR() at a frequency of at
 * least once every 30 milliseconds.
 *
 * @warning The above is described intent and has not been rigorously
 * validated.
 *
 * @defaulted */
#ifndef configBSP430_CORE_SUPPORT_WATCHDOG
#define configBSP430_CORE_SUPPORT_WATCHDOG 0
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

/** @def BSP430_CORE_WATCHDOG_CLEAR
 *
 * A function macro which should expand to a toolchain-specific
 * statement that resets ("kicks") the watchdog timer.
 *
 * Be aware that the toolchain may provide an intrinsic with special
 * semantics, which should be understood before using its definition.
 * For example, in MSPGCC the __watchdog_clear() intrinsic references
 * a clear value managed with __set_watchdog_clear_value().  Invoking
 * __watchdog_clear() after manually halting the watchdog will restart
 * it in its last active configuration.
 *
 * @warning To reduce preprocessor conditionals in the core library,
 * this macro is unconditionally bound to a no-op if
 * #configBSP430_CORE_SUPPORT_WATCHDOG is false, even if it has been
 * previously defined to a different value.
 *
 * @defaulted */
#if configBSP430_CORE_SUPPORT_WATCHDOG - 0
#ifndef BSP430_CORE_WATCHDOG_CLEAR
#if 20120406 < __MSPGCC__
#define BSP430_CORE_WATCHDOG_CLEAR() __watchdog_clear()
#endif /* MSPGCC */
#endif /* BSP430_CORE_WATCHDOG_CLEAR */
#else /* configBSP430_CORE_SUPPORT_WATCHDOG */
#undef BSP430_CORE_WATCHDOG_CLEAR
#define BSP430_CORE_WATCHDOG_CLEAR() do { } while (0)
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

/** @def BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES
 *
 * The maximum number of delay cycles that can be executed without
 * obliging BSP430 to execute #BSP430_CORE_WATCHDOG_CLEAR().
 *
 * If you know that <bsp430/clock.h> is available, you could define
 * this in terms of #BSP430_CLOCK_NOMINAL_MCLK_HZ.
 */
#ifndef BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES
#define BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES 10000U
#endif /* BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES */

/** @def BSP430_CORE_DELAY_CYCLES
 *
 * A function macro which requests a delay for a specific number of
 * MCLK cycles.
 *
 * This macro could be used to abstract between the spelling used by
 * different toolchains for the __delay_cycles() intrinsic, but its
 * primary purpose is to succinctly support watchdog by allowing the
 * loop to be broken or not depending on compile time capabilities.
 *
 * In most cases, this feature will be invoked with a duration that is
 * far less than #BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES, but if not it
 * will function.  The duration of the delay will be slightly extended
 * as a result of the machinery supporting the watchdog.  For real
 * applications, if you want to delay so long you should probably be
 * using a timer.
 *
 * @note If the invocation of this macro is itself in a loop, you
 * should invoke #BSP430_CORE_WATCHDOG_CLEAR() explicitly before or
 * after the delay, to make it clear that the loop is watchdog-safe
 * and in case somebody changes the constants so that the expansion of
 * BSP430_CORE_DELAY_CYCLES in the loop doesn't involve a watchdog
 * clear.
 *
 * @param _duration_mclk the number of MCLK cycles for which the delay
 * is desired.  This must be a compile-time integer constant
 * compatible with unsigned long.  #configBSP430_CORE_SUPPORT_WATCHDOG
 * is enabled and the constant exceeds
 * #BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES, a watchdog clear will be
 * issued after every such period.
 *
 * @dependency #configBSP430_CORE_SUPPORT_WATCHDOG */
#if configBSP430_CORE_SUPPORT_WATCHDOG - 0
#define BSP430_CORE_DELAY_CYCLES(_duration_mclk) do {                   \
    if ((_duration_mclk) > BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES) {     \
      unsigned int _watchdog_iterations = (_duration_mclk) / BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES; \
      while (0 < _watchdog_iterations--) {                              \
        __delay_cycles(BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES);          \
        BSP430_CORE_WATCHDOG_CLEAR();                                   \
      }                                                                 \
    }                                                                   \
    __delay_cycles((_duration_mclk) % BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES); \
  } while (0)
#else /* configBSP430_CORE_SUPPORT_WATCHDOG */
#define BSP430_CORE_DELAY_CYCLES(_duration_mclk) __delay_cycles(_duration_mclk)
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

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
 * #BSP430_PERIPH_ISR_CALLBACK_TAIL_NI when
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
