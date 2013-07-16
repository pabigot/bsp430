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
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
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
#define BSP430_VERSION 20130715

/** Constant to specify no peripheral in preprocessor checks.
 *
 * Where a functional resource may be specified by either an
 * application or a platform, we need a way to identify it that
 * permits preprocessor checks that default the corresponding core
 * resource capability.  The peripheral handle values such as
 * #BSP430_PERIPH_TB0 cannot be used for this, since their expansion
 * includes cast operations that cannot be used in preprocessor
 * expressions.  Consequently there is a parallel integral-valued C
 * pre-processor identifier for each MCU peripheral.
 *
 * For example, if #BSP430_UPTIME_TIMER_PERIPH_CPPID is defined to
 * #BSP430_PERIPH_CPPID_TB0, then the following code ensures that
 * #configBSP430_HAL_TB0 and #configBSP430_HAL_TB0_ISR are both
 * defined to 1.
 *
 * @code
 * #if BSP430_UPTIME_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB0
 * #define configBSP430_HAL_TB0 1
 * #define configBSP430_HAL_TB0_ISR 1
 * #endif // BSP430_UPTIME_TIMER_PERIPH_CPPID
 * @endcode
 *
 * See <bsp430/periph/want_.h> for an infrastructure that uses these
 * constants to enable features without complex preprocessor
 * manipulation within user code.
 *
 * The value for #BSP430_PERIPH_CPPID_NONE indicates that no core
 * resource is associated with the functional resource.
 */
#define BSP430_PERIPH_CPPID_NONE 0

/* !BSP430! insert=periph_cppid */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_cppid] */

/** Constant to identify #BSP430_PERIPH_PORT1 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT1 1

/** Constant to identify #BSP430_PERIPH_PORT2 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT2 2

/** Constant to identify #BSP430_PERIPH_PORT3 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT3 3

/** Constant to identify #BSP430_PERIPH_PORT4 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT4 4

/** Constant to identify #BSP430_PERIPH_PORT5 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT5 5

/** Constant to identify #BSP430_PERIPH_PORT6 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT6 6

/** Constant to identify #BSP430_PERIPH_PORT7 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT7 7

/** Constant to identify #BSP430_PERIPH_PORT8 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT8 8

/** Constant to identify #BSP430_PERIPH_PORT9 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT9 9

/** Constant to identify #BSP430_PERIPH_PORT10 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT10 10

/** Constant to identify #BSP430_PERIPH_PORT11 in preprocessor checks */
#define BSP430_PERIPH_CPPID_PORT11 11

/** Constant to identify #BSP430_PERIPH_TA0 in preprocessor checks */
#define BSP430_PERIPH_CPPID_TA0 12

/** Constant to identify #BSP430_PERIPH_TA1 in preprocessor checks */
#define BSP430_PERIPH_CPPID_TA1 13

/** Constant to identify #BSP430_PERIPH_TA2 in preprocessor checks */
#define BSP430_PERIPH_CPPID_TA2 14

/** Constant to identify #BSP430_PERIPH_TA3 in preprocessor checks */
#define BSP430_PERIPH_CPPID_TA3 15

/** Constant to identify #BSP430_PERIPH_TB0 in preprocessor checks */
#define BSP430_PERIPH_CPPID_TB0 16

/** Constant to identify #BSP430_PERIPH_TB1 in preprocessor checks */
#define BSP430_PERIPH_CPPID_TB1 17

/** Constant to identify #BSP430_PERIPH_TB2 in preprocessor checks */
#define BSP430_PERIPH_CPPID_TB2 18

/** Constant to identify #BSP430_PERIPH_USCI_A0 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI_A0 19

/** Constant to identify #BSP430_PERIPH_USCI_A1 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI_A1 20

/** Constant to identify #BSP430_PERIPH_USCI_B0 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI_B0 21

/** Constant to identify #BSP430_PERIPH_USCI_B1 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI_B1 22

/** Constant to identify #BSP430_PERIPH_USCI5_A0 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI5_A0 23

/** Constant to identify #BSP430_PERIPH_USCI5_A1 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI5_A1 24

/** Constant to identify #BSP430_PERIPH_USCI5_A2 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI5_A2 25

/** Constant to identify #BSP430_PERIPH_USCI5_A3 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI5_A3 26

/** Constant to identify #BSP430_PERIPH_USCI5_B0 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI5_B0 27

/** Constant to identify #BSP430_PERIPH_USCI5_B1 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI5_B1 28

/** Constant to identify #BSP430_PERIPH_USCI5_B2 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI5_B2 29

/** Constant to identify #BSP430_PERIPH_USCI5_B3 in preprocessor checks */
#define BSP430_PERIPH_CPPID_USCI5_B3 30

/** Constant to identify #BSP430_PERIPH_EUSCI_A0 in preprocessor checks */
#define BSP430_PERIPH_CPPID_EUSCI_A0 31

/** Constant to identify #BSP430_PERIPH_EUSCI_A1 in preprocessor checks */
#define BSP430_PERIPH_CPPID_EUSCI_A1 32

/** Constant to identify #BSP430_PERIPH_EUSCI_A2 in preprocessor checks */
#define BSP430_PERIPH_CPPID_EUSCI_A2 33

/** Constant to identify #BSP430_PERIPH_EUSCI_B0 in preprocessor checks */
#define BSP430_PERIPH_CPPID_EUSCI_B0 34
/* END AUTOMATICALLY GENERATED CODE [periph_cppid] */
/* !BSP430! end=periph_cppid */

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

#if (configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE - 0)
#include "bsp430_config.h"
#endif /* configBSP430_CORE_INCLUDE_BSP430_CONFIG_FILE */

/** Include runtime checks for critical errors.
 *
 * There may be situations where BSP430 can add checks that decrease
 * performance but detect situations that would otherwise lead to very
 * hard to diagnose bugs.  Like the classic @c NDEBUG macro used for
 * @c assert this macro may be defined to a true value to disable such
 * checks.
 *
 * This is used only in performance-critical code where a check
 * provides safety for a situation that otherwise would be extremely
 * difficult to diagnose.
 *
 * Seriously, you don't ever want to set this.
 */
#ifndef BSP430_CORE_NDEBUG
#define BSP430_CORE_NDEBUG 0
#endif /* BSP430_CORE_NDEBUG */

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
 * application intervention.  It also enables steps to prevent #SCG0
 * from being cleared in other situations, such as leaving low-power
 * mode.
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
 * Because of the severe impact of UCS7 and UCS10, this flag is
 * enabled by default on any platform that uses the UCS peripheral and
 * is not known to have the erratum fixed.  It is made generic in case
 * there are other cases where #SCG0 should left set throughout
 * application execution.
 *
 * @note If the application manipulates the status register directly,
 * the effect of this option may not be preserved.  See
 * #BSP430_CORE_LPM_EXIT_MASK for a value that may assist with
 * preserving the configuration.
 *
 * @cppflag
 * @defaulted  */
#ifndef configBSP430_CORE_DISABLE_FLL
#define configBSP430_CORE_DISABLE_FLL (defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__))
#endif /* configBSP430_CORE_DISABLE_FLL */

/** Define to true to have BSP430 ISRs clear #GIE when exiting LPM.
 *
 * #GIE must be set in the status register for maskable interrupts to
 * be processed.  In special cases applications may wish to have #GIE
 * cleared by interrupt handlers, so when control returns to the
 * program the effects of already-handled interrupts can be processed
 * before any new interrupts are handled.
 *
 * When this macro is defined to a true value, #GIE is added to
 * #BSP430_CORE_LPM_EXIT_MASK, meaning that it will be cleared along
 * with LPM bits when #BSP430_HAL_ISR_CALLBACK_EXIT_LPM is specified
 * in the return value of an @link iBSP430halISRCallbackVoid_ni interrupt
 * callback@endlink. */
#ifndef configBSP430_CORE_LPM_EXIT_CLEAR_GIE
#define configBSP430_CORE_LPM_EXIT_CLEAR_GIE 0
#endif /* configBSP430_CORE_LPM_EXIT_CLEAR_GIE */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_CORE_LPM_EXIT_CLEAR_GIE - 0)
#define BSP430_CORE_LPM_EXIT_CLEAR_GIE_ GIE
#else /* configBSP430_CORE_LPM_EXIT_CLEAR_GIE */
#define BSP430_CORE_LPM_EXIT_CLEAR_GIE_ 0
#endif /* configBSP430_CORE_LPM_EXIT_CLEAR_GIE */
/** @endcond */

/** @def BSP430_CORE_LPM_EXIT_MASK
 *
 * The bits cleared in the stored status word to exit from low power
 * mode in an interrupt.
 *
 * This starts as either #LPM4_bits or (#LPM4_bits & ~#SCG0),
 * depending on #configBSP430_CORE_DISABLE_FLL.  #GIE is added if
 * #configBSP430_CORE_LPM_EXIT_CLEAR_GIE is true.
 *
 * @dependency #configBSP430_CORE_DISABLE_FLL
 */
#ifndef BSP430_CORE_LPM_EXIT_MASK
#if (configBSP430_CORE_DISABLE_FLL - 0)
#define BSP430_CORE_LPM_EXIT_MASK (BSP430_CORE_LPM_EXIT_CLEAR_GIE_ | (LPM4_bits & ~SCG0))
#else /* configBSP430_CORE_DISABLE_FLL */
#define BSP430_CORE_LPM_EXIT_MASK (BSP430_CORE_LPM_EXIT_CLEAR_GIE_ | LPM4_bits)
#endif /* configBSP430_CORE_DISABLE_FLL */
#endif /* BSP430_CORE_LPM_EXIT_MASK */

/** Bitmask isolating LPM-related bits recorded in status register.
 *
 * These are #SCG1, #SCG0, #OSCOFF, #CPUOFF, and (just in case) #GIE.
 * Other bits are eliminated from any LPM bit value before mutating a
 * status register setting in either #BSP430_CORE_LPM_ENTER() or
 * #BSP430_CORE_LPM_EXIT_FROM_ISR(). */
#define BSP430_CORE_LPM_SR_MASK (SCG1 | SCG0 | OSCOFF | CPUOFF | GIE)

/** Bit indicating that LPMx.5 should be entered.
 *
 * Where an integer value represents a low power mode configuration,
 * this bit may be set along with #LPM3_bits or #LPM4_bits to specify
 * that the LPMx.5 mode feature supported by the Power Management
 * Module should also apply.
 *
 * @note #BSP430_CORE_LPM_ENTER() does not pay attention to this
 * bit.  Use it and #BSP430_MODULE_PMM to determine whether it is
 * necessary to invoke #BSP430_PMM_ENTER_LPMXp5_NI() prior to invoking
 * #BSP430_CORE_LPM_ENTER(). */
#define BSP430_CORE_LPM_LPMXp5 0x0100

/** Defined to a true value if GCC is being used */
#define BSP430_CORE_TOOLCHAIN_GCC (1 < __GNUC__)

/** Defined to a true value if the Texas Instruments compiler is being
 * used.
 *
 * The TI compiler is part of Code Composer Studio, but itself has no
 * clear dependencies on CCS and internally uses a different version
 * series.  Since BSP430 has no IDE infrastructure, the toolchain is
 * identified by the compiler (TI), not the IDE (CCS).
 *
 * @warning Although this definition is present and used, the TI C/C++
 * compiler is not fully supported.  At this time the issue is
 * weaknesses in libc (itoa and printf).
 */
#ifdef __TI_COMPILER_VERSION__
/* TI's compiler isn't GCC even if somebody added --gcc to the command
 * line, which causes __GNUC__ to be defined (apparently as 3). */
#undef BSP430_CORE_TOOLCHAIN_GCC
#define BSP430_CORE_TOOLCHAIN_TI 1
#endif /* __TI_COMPILER_VERSION__ */

#if (BSP430_CORE_TOOLCHAIN_TI - 0)
#define __read_stack_register() _get_SP_register()
#define __read_status_register() _get_SR_register()
#define __bis_status_register(v_) _bis_SR_register(v_)
#define __bic_status_register(v_) _bic_SR_register(v_)
#define __bic_status_register_on_exit(v_) _bic_SR_register_on_exit(v_)
#define __nop() _nop()
#endif /* BSP430_CORE_TOOLCHAIN_TI */

/** Mark a function to be inlined.
 *
 * Most toolchains support this feature, but the spelling of the
 * request varies.
 *
 * The toolchain is free to ignore the request, which is after all
 * only the developer's expert opinion.  When optimizing for size
 * toolchains are likely to ignore this if more than one call site is
 * in the translation unit.
 *
 * @see #BSP430_CORE_INLINE_FORCED
 */
#if defined(BSP430_DOXYGEN) || (BSP430_CORE_TOOLCHAIN_GCC - 0)
#define BSP430_CORE_INLINE __inline__
#elif BSP430_CORE_TOOLCHAIN_TI - 0
#define BSP430_CORE_INLINE __inline
#else /* TOOLCHAIN */
#define BSP430_CORE_INLINE inline
#endif /* TOOLCHAIN */

/** Insist that a function be inlined.
 *
 * Use this when #BSP430_CORE_INLINE is being ignored.  Not all
 * toolchains will support this; on those it should be treated as
 * #BSP430_CORE_INLINE.
 */
#if defined(BSP430_DOXYGEN) || (BSP430_CORE_TOOLCHAIN_GCC - 0)
/* GCC wants both directives */
#define BSP430_CORE_INLINE_FORCED BSP430_CORE_INLINE __attribute__((__always_inline__))
#else /* TOOLCHAIN */
#define BSP430_CORE_INLINE_FORCED BSP430_CORE_INLINE
#endif /* TOOLCHAIN */

/** @def BSP430_CORE_PACKED_STRUCT
 *
 * Declare a packed structure in a toolchain-specific manner.
 *
 * @param nm_ name of the structure to be declared
 *
 * This expands to @c struct @p nm_ annotated with toolchain-specific
 * directives to ensure the structure contents have no padding.  It is
 * used for binary messages that mix types which might normally
 * require padding to maintain MCU-standard alignment. */
#if defined(BSP430_DOXYGEN) || (BSP430_CORE_TOOLCHAIN_GCC - 0)
#define BSP430_CORE_PACKED_STRUCT(nm_) struct __attribute__((__packed__)) nm_
#endif /* TOOLCHAIN */

/** Mark a function as an interrupt handler.
 *
 * The technique required varies among toolchains.
 *
 * @param iv_ the TI header constant identifying the interrupt, e.g @c
 * USCI_B2_VECTOR
 */
#if (BSP430_CORE_TOOLCHAIN_TI - 0)
#define BSP430_CORE_DECLARE_INTERRUPT(iv_) _BSP430_CORE_TOOLCHAIN_TI_PRAGMA(vector=##iv_) __interrupt void
/* Helper function required to make the _Pragma argument a single
 * string literal */
#define _BSP430_CORE_TOOLCHAIN_TI_PRAGMA(x_) _Pragma(#x_)
#else  /* TOOLCHAIN */
#define BSP430_CORE_DECLARE_INTERRUPT(iv_) static void __attribute__((__interrupt__(iv_)))
#endif /* TOOLCHAIN */

/** Enter a low-power mode
 *
 * This sets the status register bits in accordance to the bits
 * specified in @p lpm_bits_.
 *
 * This macro is normally intended to be invoked when interrupts are
 * enabled. It does not add #GIE to @p lpm_bits.  If invoked when
 * interrupts are disabled and #GIE is not in @p lpm_bits it is
 * unlikely to ever return.
 *
 * Interrupts normally remain enabled after #BSP430_CORE_LPM_ENTER()
 * completes.
 *
 * @note If #configBSP430_CORE_LPM_EXIT_CLEAR_GIE is enabled
 * interrupts will be disabled during wakeup and remain disabled when
 * #BSP430_CORE_LPM_ENTER() completes.
 *
 * @param lpm_bits_ bits to be set in the status register.  The value
 * is masked by #BSP430_CORE_LPM_SR_MASK before being written.
 */
#define BSP430_CORE_LPM_ENTER(lpm_bits_) __bis_status_register(BSP430_CORE_LPM_SR_MASK & (lpm_bits_))

/** Enter a low-power mode while interrupts are disabled
 *
 * This invokes #BSP430_CORE_LPM_ENTER(@p lpm_bits | #GIE).  It exists
 * to explicitly note that entering LPM when interrupts are disabled
 * should normally set #GIE so that the application can be woken.
 *
 * Interrupts normally remain enabled after
 * #BSP430_CORE_LPM_ENTER_NI() completes.  Using
 * #BSP430_CORE_DISABLE_INTERRUPT() immediately after
 * #BSP430_CORE_LPM_ENTER_NI() will disable interrupts again, though
 * there is a window where additional interrupts may be processed
 * before this can happen.
 *
 * @note If #configBSP430_CORE_LPM_EXIT_CLEAR_GIE is enabled
 * interrupts will be disabled during wakeup and remain disabled when
 * #BSP430_CORE_LPM_ENTER_NI() completes, eliminating any possible
 * window for additional interrupts to be processed before the CPU
 * becomes active.
 *
 * @param lpm_bits_ as with #BSP430_CORE_LPM_ENTER()
 */
#define BSP430_CORE_LPM_ENTER_NI(lpm_bits_) BSP430_CORE_LPM_ENTER(GIE | (lpm_bits_))

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
#if (configBSP430_CORE_SUPPORT_WATCHDOG - 0)
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
 * @param duration_mclk_ the number of MCLK cycles for which the delay
 * is desired.  This must be a compile-time integer constant
 * compatible with unsigned long.  #configBSP430_CORE_SUPPORT_WATCHDOG
 * is enabled and the constant exceeds
 * #BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES, a watchdog clear will be
 * issued after every such period.
 *
 * @dependency #configBSP430_CORE_SUPPORT_WATCHDOG */
#if (configBSP430_CORE_SUPPORT_WATCHDOG - 0)
#define BSP430_CORE_DELAY_CYCLES(duration_mclk_) do {                   \
    if ((duration_mclk_) > BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES) {     \
      unsigned int _watchdog_iterations = (duration_mclk_) / BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES; \
      while (0 < _watchdog_iterations--) {                              \
        __delay_cycles(BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES);          \
        BSP430_CORE_WATCHDOG_CLEAR();                                   \
      }                                                                 \
    }                                                                   \
    __delay_cycles((duration_mclk_) % BSP430_CORE_WATCHDOG_MAX_DELAY_CYCLES); \
  } while (0)
#else /* configBSP430_CORE_SUPPORT_WATCHDOG */
#define BSP430_CORE_DELAY_CYCLES(duration_mclk_) __delay_cycles(duration_mclk_)
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

/** @def BSP430_CORE_SAVE_INTERRUPT_STATE(state_)
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
 * @param state_ where the interrupt enable/disable state is stored.
 *
 * @defaulted */
#ifndef BSP430_CORE_SAVE_INTERRUPT_STATE
#define BSP430_CORE_SAVE_INTERRUPT_STATE(state_) do {   \
    (state_) = __get_interrupt_state();                 \
  } while (0)
#endif /* BSP430_CORE_SAVE_INTERRUPT_STATE */

/** Declare a variable holding interrupt state and initialize it with current state.
 *
 * This macro supports a simpler canonical sequence:
 * @code
 *   {
 *     BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
 *     ...
 *     BSP430_CORE_DISABLE_INTERRUPT();
 *     do {
 *       // stuff with interrupts disabled
 *     } while (0);
 *     BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
 *     ...
 *   }
 * @endcode
 *
 * This approach results from the experience that in a large number of
 * cases the declaration of the variable into which the state is
 * stored is immediately followed by the command that stores the
 * current state into that variable.
 *
 * @param var_ where the interrupt enable/disable state is stored.
 *
 * @defaulted */
#ifndef BSP430_CORE_SAVED_INTERRUPT_STATE
#define BSP430_CORE_SAVED_INTERRUPT_STATE(var_)             \
  BSP430_CORE_INTERRUPT_STATE_T var_ = __get_interrupt_state()
#endif /* BSP430_CORE_SAVED_INTERRUPT_STATE */

/** @def BSP430_CORE_RESTORE_INTERRUPT_STATE(state_)
 *
 * A function macro that will enable or disable interrupts as recorded
 * in the provided state parameter.  The parameter value should have
 * been created using #BSP430_CORE_SAVE_INTERRUPT_STATE.
 *
 * @param state_ where the interrupt enable/disable state is stored.
 *
 * @defaulted */
#ifndef BSP430_CORE_RESTORE_INTERRUPT_STATE
#define BSP430_CORE_RESTORE_INTERRUPT_STATE(state_) do {        \
    __set_interrupt_state(state_);                              \
  } while (0)
#endif /* BSP430_CORE_RESTORE_INTERRUPT_STATE */

/** @def BSP430_CORE_ENABLE_INTERRUPT()
 *
 * Set the status register #GIE bit so that interrupts are enabled.
 *
 * @note BSP430 will insert a @c nop after the @c eint when
 * #BSP430_CORE_FAMILY_IS_5XX is true, on the assumption that MSP430
 * erratum CPU42 is present and has not been worked around by the
 * compiler.
 *
 * @defaulted */
#ifndef BSP430_CORE_ENABLE_INTERRUPT
#if (BSP430_CORE_FAMILY_IS_5XX - 0)
#define BSP430_CORE_ENABLE_INTERRUPT() do {     \
    __enable_interrupt();                       \
    __nop();                                    \
  } while (0)
#else /* BSP430_CORE_FAMILY_IS_5XX */
#define BSP430_CORE_ENABLE_INTERRUPT() __enable_interrupt()
#endif /* BSP430_CORE_FAMILY_IS_5XX */
#endif /* BSP430_CORE_ENABLE_INTERRUPT */

/** @def BSP430_CORE_DISABLE_INTERRUPT()
 *
 * Clear the status register #GIE bit so that interrupts are disabled.
 *
 * @defaulted */
#ifndef BSP430_CORE_DISABLE_INTERRUPT
#define BSP430_CORE_DISABLE_INTERRUPT() __disable_interrupt()
#endif /* BSP430_CORE_DISABLE_INTERRUPT */

/** Generic convert from microseconds to ticks at some frequency.
 *
 * @note Calculations are done using 32-bit operations.  Fastest code
 * when parameters are compile-time constants.
 *
 * @note The result is rounded down.
 *
 * @see #BSP430_CORE_TICKS_TO_US, #BSP430_CORE_MS_TO_TICKS */
#define BSP430_CORE_US_TO_TICKS(us_, hz_) (((us_) * (unsigned long)(hz_)) / 1000000UL)

/** Generic convert from ticks at some frequency to microseconds.
 *
 * @note Calculations are done using 32-bit operations.  Fastest code
 * when parameters are compile-time constants.
 *
 * @note The result is rounded down.
 *
 * @see #BSP430_CORE_US_TO_TICKS, #BSP430_CORE_TICKS_TO_MS */
#define BSP430_CORE_TICKS_TO_US(ticks_, hz_) ((1000000UL * (ticks_)) / (hz_))

/** Generic convert from milliseconds to ticks at some frequency.
 *
 * @note Calculations are done using 32-bit operations.  Fastest code
 * when parameters are compile-time constants.
 *
 * @note The result is rounded down.
 *
 * @see #BSP430_CORE_TICKS_TO_MS, #BSP430_CORE_US_TO_TICKS */
#define BSP430_CORE_MS_TO_TICKS(ms_, hz_) (((ms_) * (unsigned long)(hz_)) / 1000UL)

/** Generic convert from ticks at some frequency to milliseconds.
 *
 * @note Calculations are done using 32-bit operations.  Fastest code
 * when parameters are compile-time constants.
 *
 * @note The result is rounded down.
 *
 * @see #BSP430_CORE_MS_TO_TICKS, #BSP430_CORE_TICKS_TO_US */
#define BSP430_CORE_TICKS_TO_MS(ticks_, hz_) ((1000UL * (ticks_)) / (hz_))

/** Utility to convert a 16-bit word between big-endian and little-endian.
 *
 * Some toolchains may provide an intrinsic that is faster than basic C code.
 */
#if (BSP430_CORE_TOOLCHAIN_GCC - 0)
#include <byteswap.h>
#define BSP430_CORE_SWAP_16(w_) bswap_16(w_)
#else
#define BSP430_CORE_SWAP_16(w_) ((  ((w_) & 0xFF00) >> 8)       \
                                 | (((w_) & 0x00FF) << 8) )
#endif

/** Utility to convert a 32-bit word between big-endian and little-endian.
 *
 * Some toolchains may provide an intrinsic that is faster than basic C code.
 */
#if (BSP430_CORE_TOOLCHAIN_GCC - 0)
#include <byteswap.h>
#define BSP430_CORE_SWAP_32(lw_) bswap_32(lw_)
#else
#define BSP430_CORE_SWAP_32(lw_) ((  ((lw_) & 0xFF000000) >> 24)     \
                                  | (((lw_) & 0x00FF0000) >> 8)      \
                                  | (((lw_) & 0x0000FF00) << 8)      \
                                  | (((lw_) & 0x000000FF) << 24) )
#endif

/* See <bsp430/rtos/freertos.h> */
#if (configBSP430_RTOS_FREERTOS - 0)
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
