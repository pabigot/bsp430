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
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */
#ifndef BSP430_CORE_H
#define BSP430_CORE_H

#include <msp430.h>             /* MSP430 MCU-specific information */
#include <stdint.h>             /* Size-annotated integral types (uint8_t) */
#include <stddef.h>             /* NULL and size_t */

/** Version identifier for the BSP430 library
 *
 * A monotonically non-decreasing integer reflecting the version of
 * the BSP430 library that is being used.  The value represents a
 * development freeze date in the form YYYYMMDD as a decimal
 * number. */
#define BSP430_VERSION 20121002

/** @def configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE
 *
 * BSP430 depends heavily on configuration specified by preprocessor
 * tokens defined to true or false values.  Proper application
 * behavior requires that all object files be built using the same
 * configuration settings.
 *
 * All BSP430 implementation files include <bsp430/core.h>, either
 * directly or through other headers.  Defining
 * #configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE to a true value
 * (which is its default) will cause this header to include a file
 * named "bsp430_config.h", which is presumed to be in the compiler's
 * include path.  All application configuration may be placed into
 * this file.  It is recommended that this file end with the inclusion
 * of <bsp430/platform/bsp430_config.h> to reduce the amount of
 * generic configuration that must be supplied.
 *
 * If you provide configuration solely through external means (e.g.,
 * through @c -D arguments to the preprocessor) you can inhibit this
 * inclusion by defining #configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE to 0.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE
#define configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE 1
#endif /* configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE */

#if configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE - 0
#include "bsp430_config.h"
#endif /* configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE */

/** @def BSP430_CORE_FAMILY_IS_5XX
 *
 * Defined to a value that is true if the build target is an MCU in
 * the 5xx/FR5xx/6xx family.  These MCUs have incompatible HPL
 * structures compared with the same peripheral in earlier families;
 * where user or library code must take into account those
 * differences, this flag may be used in a prepropcessor condition to
 * detect the situation.
 *
 * In the future it may be necessary to distinguish membership in this
 * family from the CPU architecture, but for now the presence of the
 * @c __MSP430_HAS_MSP430XV2_CPU__ preprocessor symbol in <msp430.h>
 * is what controls the value of this flag.  */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define BSP430_CORE_FAMILY_IS_5XX 1
#else /* 5xx == CPUXv2*/
#define BSP430_CORE_FAMILY_IS_5XX 0
#endif /* 5xx == CPUXv2 */

/** @def configBSP430_CORE_DISABLE_FLL
 *
 * This macro may be defined to a true value to request that #SCG0 be
 * set after vBSP430platformInitialize_ni() configures the clocks,
 * preventing the FLL from changing the DCO configuration without
 * application intervention.  It may be referenced in other
 * situations, such as leaving low-power mode, to determine whether
 * the bit should remain set.
 *
 * The UCS peripheral has several errata which result in severe clock
 * instabilities when the FLL is allowed to run unmanaged.  These
 * include UCS7 ("DCO drifts when servicing short ISRs when in LPM0 or
 * exiting active from ISRs for short periods of time" and UCS10
 * ("Modulation causes shift in DCO frequency").  The latter is
 * documented in <a href="http://www.ti.com/lit/pdf/SLAA489">UCS10
 * Guidance</a>.  The UCS implementation of
 * #iBSP430ucsTrimDCOCLKDIV_ni() supports the UCS10 workaround.
 *
 * Stability in the presence of UCS7 and UCS10 may be further enhanced
 * by setting this option.  It is made generic in case there are other
 * cases where #SCG0 should left set throughout application execution.
 *
 * @note If the application manipulates the status register directly,
 * the effect of this option may not be preserved.
 *
 * @cppflag
 * @defaulted  */
#ifndef configBSP430_CORE_DISABLE_FLL
#define configBSP430_CORE_DISABLE_FLL 0
#endif /* configBSP430_CORE_DISABLE_FLL */

/** @def BSP430_CORE_LPM_EXIT_MASK
 *
 * The bits cleared in the stored status word to exit from low power
 * mode in an interrupt.
 *
 * This is either #LPM4_bits or (#LPM4_bits & ~#SCG0), depending on
 * #configBSP430_CORE_DISABLE_FLL.
 *
 * @dependency #configBSP430_CORE_DISABLE_FLL
 */
#ifndef BSP430_CORE_LPM_EXIT_MASK
#if configBSP430_CORE_DISABLE_FLL - 0
#define BSP430_CORE_LPM_EXIT_MASK (LPM4_bits & ~SCG0)
#else /* configBSP430_CORE_DISABLE_FLL */
#define BSP430_CORE_LPM_EXIT_MASK LPM4_bits
#endif /* configBSP430_CORE_DISABLE_FLL */
#endif /* BSP430_CORE_LPM_EXIT_MASK */

/** Bitmask isolating LPM-related bits recorded in status register.
 *
 * These are #SCG1, #SCG0, #OSCOFF, #CPUOFF, and (just in case) #GIE.
 * Other bits are eliminated from any LPM bit value before mutating a
 * status register setting in either #BSP430_CORE_LPM_ENTER_NI() or
 * #BSP430_CORE_LPM_EXIT_FROM_ISR(). */
#define BSP430_CORE_LPM_SR_MASK (SCG1 | SCG0 | OSCOFF | CPUOFF | GIE)

/** Bit indicating that LPMx.5 should be entered.
 *
 * Where an integer value represents a low power mode configuration,
 * this bit may be set along with #LPM3_bits or #LPM4_bits to specify
 * that the LPMx.5 mode feature supported by the Power Management
 * Module should also apply.
 *
 * @note #BSP430_CORE_LPM_ENTER_NI() does not pay attention to this
 * bit.  Use it and #BSP430_MODULE_PMM to determine whether it is
 * necessary to invoke #BSP430_PMM_ENTER_LPMXp5_NI() prior to invoking
 * #BSP430_CORE_LPM_ENTER_NI(). */
#define BSP430_CORE_LPM_LPMXp5 0x0100

/** Defined to a true value if GCC is being used */
#if 1 < __GNUC__
#define BSP430_CORE_TOOLCHAIN_GCC 1
#endif /* __GNUC__ */

/** Defined to a true value if Code Composer Studio is being used.
 *
 * @warning Although this definition is present and used, Code
 * Composer Studio cannot be used to build BSP430 because it does not
 * support ISO C11 anonymous struct/union fields. */
#ifdef __TI_COMPILER_VERSION__
#define BSP430_CORE_TOOLCHAIN_CCS 1
#endif /* __TI_COMPILER_VERSION__ */

/** Mark a function to be inlined.
 *
 * The spelling of this varies among toolchains.
 */
#if defined(BSP430_DOXYGEN) || (BSP430_CORE_TOOLCHAIN_GCC - 0)
#define BSP430_CORE_INLINE __inline__
#elif BSP430_CORE_TOOLCHAIN_CCS - 0
#define BSP430_CORE_INLINE __inline
#else /* TOOLCHAIN */
#define BSP430_CORE_INLINE inline
#endif /* TOOLCHAIN */

/** Enter a low-power mode
 *
 * This sets the status register bits in accordance to the bits
 * specified in @p lpm_bits_.
 *
 * @param lpm_bits_ bits to be set in the status register.  The value
 * is masked by #BSP430_CORE_LPM_SR_MASK before being written.
 */
#define BSP430_CORE_LPM_ENTER_NI(lpm_bits_) __bis_status_register(BSP430_CORE_LPM_SR_MASK & (lpm_bits_))

/** Exit low-power mode on return from ISR
 *
 * This clears the status register bits provided in @p lpm_bits from
 * the saved status register value stored in the stack by the
 * interrupt invocation, so that when the interrupt returns the change
 * will take effect.
 *
 * @warning This macro is only usable within the top-half of an
 * interrupt service routine, meaning the function that is registered
 * in the MCU interrupt table.  If you are using the HAL interrupt
 * capabilities of BSP430, you won't use this macro.
 *
 * @param lpm_bits_ bits to be cleared in the stored status register.
 * The value is masked by #BSP430_CORE_LPM_SR_MASK before being
 * written.
 */
#define BSP430_CORE_LPM_EXIT_FROM_ISR(lpm_bits_) __bic_status_register_on_exit(BSP430_CORE_LPM_SR_MASK & (lpm_bits_))

/** @def configBSP430_CORE_SUPPORT_WATCHDOG
 *
 * Control use of the watchdog infrastructure by BSP430.
 *
 * If defined to a true value, the function macros
 * #BSP430_CORE_WATCHDOG_CLEAR() and #BSP430_CORE_DELAY_CYCLES() will
 * reset the watchdog in accordance with their descriptions.  If
 * false, those macros will not manipulate the watchdog register.
 *
 * When watchdog support is enabled, all BSP430 functions that might
 * execute for more than 30 milliseconds with an MCLK frequency of at
 * least 1 MHz will invoke #BSP430_CORE_WATCHDOG_CLEAR() at a
 * frequency of at least once every 30 milliseconds.
 *
 * See #BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG, which defaults to true
 * if and only if #configBSP430_CORE_SUPPORT_WATCHDOG is false.
 *
 * @warning The above is described intent and has not been rigorously
 * validated.
 *
 * @cppflag
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
 * It is safe to use this macro regardless of whether
 * #configBSP430_CORE_SUPPORT_WATCHDOG is true.  When the watchdog is
 * not supported, it evalutes to an empty statement.
 *
 * @see #BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG
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
#if defined(BSP430_DOXYGEN) || (BSP430_CORE_TOOLCHAIN_GCC - 0)
#define BSP430_CORE_INTERRUPT_STATE_T __istate_t
#else /* TOOLCHAIN */
#define BSP430_CORE_INTERRUPT_STATE_T unsigned int
#endif /* TOOLCHAIN */
#endif /* BSP430_CORE_INTERRUPT_STATE_T */

/** @def BSP430_CORE_SAVE_INTERRUPT_STATE(_state)
 *
 * A function macro that will record whether interrupts are currently
 * enabled in the state parameter.  The parameter should subsequently
 * be passed to #BSP430_CORE_RESTORE_INTERRUPT_STATE.
 *
 * The canonical code sequence for this is:
 * @code
 * BSP430_CORE_INTERRUPT_STATE_T istate;
 *
 * BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
 * BSP430_CORE_DISABLE_INTERRUPT();
 * do {
 *   // stuff with interrupts disabled
 * } while (0);
 * BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
 * @endcode
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
 * #BSP430_HAL_ISR_CALLBACK_TAIL_NI when
 * #BSP430_HAL_ISR_CALLBACK_YIELD has been set.
 *
 * It is defined to a no-op in environments that do not support
 * context switches.
 *
 * @defaulted */
#ifndef BSP430_RTOS_YIELD_FROM_ISR
#define BSP430_RTOS_YIELD_FROM_ISR() do { } while (0)
#endif /* BSP430_RTOS_YIELD_FROM_ISR */

#endif /* BSP430_CORE_H */
