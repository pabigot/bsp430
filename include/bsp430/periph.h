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
 * @brief Generic peripheral support for MSP430 MCUs.
 *
 * This file, included by peripheral-specific include files, supports
 * handles to peripherals which in turn are used in application code.
 * Use of a peripheral @c xx must be indicated by defining the
 * corresponding @c configBSP430_HPL_xx or @c configBSP430_HAL_xx in
 * the application @c bsp430_config.h file.
 *
 * Structures are defined in the peripheral-specific header for each
 * class of peripheral that may have multiple instances with the same
 * register set, to simplify access by converting the periphal address
 * to a pointer to such a structure.  These structures intentionally
 * use the GCC/ISO C11 extensions for unnamed struct/union fields.
 *
 * Access to the peripheral area through such pointers must be done
 * with care, as some structure fields are not valid for some variants
 * of the peripheral.
 *
 * In addition, callback function types, structures, and helper
 * functions exist to allow shared interrupts to delegate event
 * handling to application-specific code.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_H
#define BSP430_PERIPH_H

#include <bsp430/common.h>
#include <msp430.h>

/** An integral type used to uniquely identify a raw MCU peripheral.
 *
 * For MCU families where peripheral base addresses are provided in
 * the header files, those base addresses are used as the handle.  In
 * other cases arbitrary values may be used, so in general objects
 * with this type should be referenced only equality testing against
 * the constant handle identifier provided in the peripheral
 * header.
 *
 * The value zero is reserved and available as #BSP430_PERIPH_NONE to
 * indicate no peripheral.
 *
 * Odd values are reserved for non-standard "peripherals" for which
 * some identifier is necessary to allow use in functions like
 * #iBSP430platformConfigurePeripheralPins_ni, such as
 * #BSP430_PERIPH_LFXT1.
 *
 * Negative values indicate an error. */
typedef int tBSP430periphHandle;

/** HPL handle specifying that no peripheral has been identified. */
#define BSP430_PERIPH_NONE ((tBSP430periphHandle)0)

/** HPL handle identifying the XT1 crystal functionality.
 *
 * This is used by platform-independent clock peripherals to request
 * that the pins related to XIN and XOUT be configured to their
 * peripheral function. */
#define BSP430_PERIPH_LFXT1 ((tBSP430periphHandle)1)

/** HPL handle identifying the ability to expose clock signals.
 *
 * This is used by application code as a debug facility, invoking
 * #iBSP430platformConfigurePeripheralPins_ni to expose MCLK, SMCLK, and
 * ACLK on pins where they may be monitored.  Its availability is
 * conditional on platform support and
 * #configBSP430_PERIPH_EXPOSED_CLOCKS. */
#define BSP430_PERIPH_EXPOSED_CLOCKS ((tBSP430periphHandle)3)

/** @def BSP430_PERIPH_EXPOSED_CLOCKS_HELP
 *
 * A string constant that can be displayed by an application that uses
 * #BSP430_PERIPH_EXPOSED_CLOCKS to inform an observer where the
 * clocks may be found.  This should be defined in the
 * platform-specific header and made available via <bsp430/platform.h>.
 *
 * @nodefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_PERIPH_EXPOSED_CLOCKS_HELP "MCLK on P1.0; SMCLK on P1.1; ACLK on P1.2"
#endif /* BSP430_DOXYGEN */

/** @def configBSP430_PERIPH_EXPOSED_CLOCKS
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of
 * #BSP430_PERIPH_EXPOSED_CLOCKS in
 * #iBSP430platformConfigurePeripheralPins_ni.
 *
 * @defaulted  */
#ifndef configBSP430_PERIPH_EXPOSED_CLOCKS
#define configBSP430_PERIPH_EXPOSED_CLOCKS 0
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */


/** Reserved value for #BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT */
#define BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT_UNKNOWN 0

/** Mask used to isolate the HPL variant in the
 * sBSP430hplHALStatePrefix structure @a cflags field */
#define BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT_MASK_ 0x0F

/** Extract HPL variant from a HAL state instance.
 *
 * This is used to interpret the HPL pointer that is stored in the
 * state in situations where the HAL supports multiple types of
 * underlying peripheral. */
#define BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT(_p) (BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT_MASK_ & (_p)->hal_state.cflags)

/** Indication that an ISR is associated with a HAL instance
 *
 * This flag is set in a HAL instance state
 * sBSP430hplHALStatePrefix cflags field to denote that an ISR
 * implementation has been provided by the infrastructure.
 *
 * Where a peripheral supports multiple ISRs per instance:
 * <ul>
 * <li>A #sBSP430halTIMER instance specifies the CC0 ISR with this bit.
 * <li>A #sBSP430halUSCI instance specifies the RX ISR with this bit.
 * </ul>
 */
#define BSP430_PERIPH_HAL_STATE_CFLAGS_ISR 0x80

/** Indication that secondary ISR is associated with a HAL instance
 *
 * This flag is set in a HAL instance state
 * sBSP430hplHALStatePrefix cflags field to denote that a secondary
 * ISR implementation has been provided by the infrastructure.
 *
 * Where a peripheral supports multiple ISRs per instance:
 * <ul>
 * <li>A #sBSP430halTIMER instance specifies the CC1-CC6 and overflow ISR with this bit.
 * <li>A #sBSP430halUSCI instance specifies the TX ISR with this bit.
 * </ul>
 */
#define BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2 0x40

/** Common prefix for HAL state structures.
 *
 * Each state structure for a HAL instance begins with a field of this
 * type named @c hal_state.
 */
typedef struct sBSP430hplHALStatePrefix {
  /** Immutable flags recording information about the HAL interface.
   *
   * Information conveyed includes a selector for the underlying HPL
   * structure type, and whether the HAL layer includes ISR support.
   * See #BSP430_PERIPH_HAL_STATE_CFLAGS_VARIANT. */
  const unsigned char cflags;
  /** HAL-specific flags recording peripheral state
   *
   * This primarily exists for alignment, though at some point there
   * may be flags that are worth having at this level, such as a
   * resource-in-use marker. */
  unsigned char flags;
} sBSP430hplHALStatePrefix;

/** Get the peripheral handle corresponding to an HPL handle
 *
 * The Hardware Presentation Layer handle is a typed pointer to a
 * structure of registers used to control the peripheral.  The
 * peripheral handle is an integral value.  This routine translates
 * from the HPL to the integral handle; the other direction is done by
 * peripheral-specific functions like xBSP430periphLookupTIMER().
 *
 * @param hpl Hardware presentation layer handle.  This is generally a
 * pointer to a volatile structure; the absence of the volatile
 * qualifier would require that it be explicitly cast when used as
 * this argument.
 *
 * @return The peripheral handle.  The result is undefined if what is
 * passed is not a pointer to the HPL structure for an enabled
 * peripheral. */
static tBSP430periphHandle
__inline__
xBSP430periphFromHPL (volatile void * hpl)
{
  return (tBSP430periphHandle)(uintptr_t)hpl;
}

/* Forward declarations */
struct sBSP430halISRCallbackVoid;
struct sBSP430halISRCallbackIndexed;

/** Mask for status register bits cleared in ISR top half.
 *
 * This is used to prevent corrupting the status register when the
 * return value of #iBSP430halISRCallbackVoid and
 * #iBSP430halISRCallbackIndexed includes bits that are not relevant to
 * status register bits. */
#define BSP430_HAL_ISR_CALLBACK_BIC_MASK 0x00F8

/** Indicate ISR top half should yield on return.
 *
 * In some cases, a chained ISR handler might perform an operation
 * that enables a higher-priority task.  This bit may be set in the
 * return value of #iBSP430halISRCallbackVoid and
 * #iBSP430halISRCallbackIndexed to indicate that the interrupt should
 * yield to that task when it returns. */
#define BSP430_HAL_ISR_CALLBACK_YIELD 0x1000

/** Indicate ISR top half should disable the corresponding interrupt on return.
 *
 * In some cases, the handler can determine that the interrupt is no
 * longer needed, but cannot itself disable the interrupt enable bit.
 * This bit may be set in the return value of
 * #iBSP430halISRCallbackVoid and #iBSP430halISRCallbackIndexed
 * to indicate that the top-half should clear the corresponding IE bit
 * before returning returns. */
#define BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT 0x2000

/** Indicate that no further ISR callbacks should be invoked.
 *
 * In some cases, a handler can determine that no subsequent handler in
 * the chain should be invoked.  For example, a USCI interrupt is
 * raised when there is space in the transmission buffer, and the
 * callback chain is invoked.  The first handler in the chain that can
 * provide data for transmission should store it in the context and
 * return this flag.  Subsequent handlers will not be invoked, and the
 * ISR itself will be informed whether data has been made available.
 *
 * (The value of this flag is specifically selected to be a value
 * supported by the constant generator, to optimize the callback
 * loop.) */
#define BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN 0x0001

/** Callback for ISR chains that require no special arguments.
 *
 * @param cb A reference to the callback structure.  In most cases,
 * this will be ignored; however, callback-specific state can be
 * passed this way by providing a callback structure that is the first
 * member in a structure that holds that state.
 *
 * @param context A reference to the hardware abstraction state handle
 * for the interrupt that occurred, such as a timer.
 *
 * @return An integral value used by the ISR top half to wake up from
 * low power modes and otherwise affect subsequent execution.  It
 * should be a combination of bits like #LPM4_bits and
 * #BSP430_HAL_ISR_CALLBACK_YIELD.
 */
typedef int (* iBSP430halISRCallbackVoid) (const struct sBSP430halISRCallbackVoid * cb,
    void * context);

/** Callback for ISR chains where the event includes an index
 *
 * This type of callback is used for digital IO ports and
 * capture/compare events on timers.
 *
 * @param cb As with #iBSP430halISRCallbackVoid.
 *
 * @param context As with #iBSP430halISRCallbackVoid.
 *
 * @param idx The sub-entity to which the event applies, such as a
 * specific pin on a port or capture/compare block on a timer.
 *
 * @return As with #iBSP430halISRCallbackVoid.
 */
typedef int (* iBSP430halISRCallbackIndexed) (const struct sBSP430halISRCallbackIndexed * cb,
    void * context,
    int idx);

/** Structure used to record #iBSP430halISRCallbackVoid chains. */
typedef struct sBSP430halISRCallbackVoid {
  /** The next callback in the chain.  Assign a null pointer to
   * terminate the chain. */
  const struct sBSP430halISRCallbackVoid * next;

  /** The function to be invoked. */
  iBSP430halISRCallbackVoid callback;
} sBSP430halISRCallbackVoid;

/** Structure used to record #iBSP430halISRCallbackIndexed chains. */
typedef struct sBSP430halISRCallbackIndexed {
  /** The next callback in the chain.  Assign a null pointer to
   * terminate the chain. */
  const struct sBSP430halISRCallbackIndexed * next;

  /** The function to be invoked. */
  iBSP430halISRCallbackIndexed callback;
} sBSP430halISRCallbackIndexed;

/** Execute a chain of #iBSP430halISRCallbackVoid callbacks.
 *
 * The return value of the callback is expected to be a bitmask
 * indicating bits to be cleared to wake from low power modes and bits
 * indicating other activities to be performed by the ISR top half.
 * The return value of individual callbacks is combined using bitwise
 * or with the @a basis parameter.
 *
 * @param cbpp Pointer to the start of the chain
 * @param context The context to be passed into each callback
 * @param basis The basis for the return value
 * @return The bitwise OR of the @a basis value and the return values
 * of each callback */
static int
__inline__
iBSP430callbackInvokeISRVoid_ni (const struct sBSP430halISRCallbackVoid * const * cbpp,
                                 void * context,
                                 int basis)
{
  while (*cbpp && ! (basis & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN)) {
    basis |= (*cbpp)->callback(*cbpp, context);
    cbpp = &(*cbpp)->next;
  }
  return basis;
}

/** Execute a chain of #iBSP430halISRCallbackIndexed callbacks.
 *
 * Same as #iBSP430callbackInvokeISRVoid_ni, but providing an index to
 * the callback.
 *
 * @param cbpp As with #iBSP430callbackInvokeISRVoid_ni
 * @param context As with #iBSP430callbackInvokeISRVoid_ni
 * @param idx The index to be passed to each #iBSP430halISRCallbackIndexed handler
 * @param basis As with #iBSP430callbackInvokeISRVoid_ni
 * @return As with #iBSP430callbackInvokeISRVoid_ni */
static int
__inline__
iBSP430callbackInvokeISRIndexed_ni (const struct sBSP430halISRCallbackIndexed * const * cbpp,
                                    void * context,
                                    int idx,
                                    int basis)
{
  while (*cbpp && ! (basis & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN)) {
    basis |= (*cbpp)->callback(*cbpp, context, idx);
    cbpp = &(*cbpp)->next;
  }
  return basis;
}

/** Execute code in ISR top-half based on callback return flags.
 *
 * Clear the requested bits in the status register, and if necessary
 * yield control to a higher priority task.
 *
 * @param _return_flags An expression denoting a return value from a
 * chain of callbacks, producing bits including (for example) @c
 * LPM_bits and/or #BSP430_HAL_ISR_CALLBACK_YIELD. */
#define BSP430_HAL_ISR_CALLBACK_TAIL_NI(_return_flags) do {             \
    int return_flags_ = (_return_flags);                                \
    __bic_status_register_on_exit((return_flags_) & BSP430_HAL_ISR_CALLBACK_BIC_MASK); \
    if ((return_flags_) & BSP430_HAL_ISR_CALLBACK_YIELD) {           \
      BSP430_RTOS_YIELD_FROM_ISR();                                     \
    }                                                                   \
  } while (0)

#endif /* BSP430_PERIPH_H */
