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
 * @brief Generic peripheral support for MSP430 MCUs.
 *
 * This file provides generic support for referencing peripherals and
 * distinct capabilities symbolicly through a unique integer
 * identifier.  It also defines structures that are used for interrupt
 * callbacks across peripherals.
 *
 * \section h_periph__handles Peripheral Handles
 *
 * A peripheral (instance) handle, such as #BSP430_PERIPH_PORT1, is a
 * constant that identifies the peripheral instance and distinguishes
 * it from other instances of the same peripheral (e.g.,
 * #BSP430_PERIPH_PORT2) and from different peripherals
 * (e.g. #BSP430_PERIPH_USCI_A0).  For internal simplicity the value
 * of a peripheral is often related to the physical address of its
 * corresponding control registers within the standard 16-bit address
 * space of the MSP430, but this fact should never be used by a BSP430
 * application.
 *
 * Other denotable capabilities are technically not peripherals but
 * require similar configuration support, such as #BSP430_PERIPH_LFXT1
 * and #BSP430_PERIPH_EXPOSED_CLOCKS, are also given handles so that
 * they can be used in routines like
 * iBSP430platformConfigurePeripheral_ni().
 *
 * \section h_periph__callbacks Callback Infrastructure
 *
 *  NOT YET FINISHED
 *
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
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_H
#define BSP430_PERIPH_H

#include <bsp430/core.h>
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
#define BSP430_PERIPH_LFXT1 ((tBSP430periphHandle)0x4001)

/** @def configBSP430_PERIPH_EXPOSED_CLOCKS
 *
 * Define to a true value in @c bsp430_config.h to enable use of
 * #BSP430_PERIPH_EXPOSED_CLOCKS in
 * #iBSP430platformConfigurePeripheralPins_ni.
 *
 * @cppflag
 * @affects #BSP430_PERIPH_EXPOSED_CLOCKS
 * @defaulted  */
#ifndef configBSP430_PERIPH_EXPOSED_CLOCKS
#define configBSP430_PERIPH_EXPOSED_CLOCKS 0
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */

/** HPL handle identifying the ability to expose clock signals.
 *
 * This is used by application code as a debug facility, invoking
 * #iBSP430platformConfigurePeripheralPins_ni to expose MCLK, SMCLK, and
 * ACLK on pins where they may be monitored.  Its availability is
 * conditional on platform support and
 * #configBSP430_PERIPH_EXPOSED_CLOCKS. */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
#define BSP430_PERIPH_EXPOSED_CLOCKS ((tBSP430periphHandle)0x4003)
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */

/** HPL handle identifying the platform primary button.
 *
 * @dependency #BSP430_PLATFORM_BUTTON0
 */
#define BSP430_PERIPH_BUTTON0 ((tBSP430periphHandle)0x4101)

/** HPL handle identifying the platform secondary button.
 * @dependency @link #BSP430_PLATFORM_BUTTON0 BSP430_PLATFORM_BUTTON1 @endlink
 */
#define BSP430_PERIPH_BUTTON1 ((tBSP430periphHandle)0x4103)

/** HPL handle identifying the platform tertiary button.
 * @dependency @link #BSP430_PLATFORM_BUTTON0 BSP430_PLATFORM_BUTTON2 @endlink
 */
#define BSP430_PERIPH_BUTTON2 ((tBSP430periphHandle)0x4105)

/** HPL handle identifying the platform quaternary button.
 * @dependency @link #BSP430_PLATFORM_BUTTON0 BSP430_PLATFORM_BUTTON3 @endlink
 */
#define BSP430_PERIPH_BUTTON3 ((tBSP430periphHandle)0x4107)

/** Value indicating the default peripheral configuration should be used. */
#define BSP430_PERIPHCFG_DEFAULT 0

/** Value indicating peripheral should be configured in UART mode.
 *
 * Only the RX and TX signals will be configured by
 * iBSP430platformConfigurePeripheralPins_ni(). */
#define BSP430_PERIPHCFG_SERIAL_UART 1

/** Value indicating peripheral should be configured in I2C mode.
 *
 * Only the SDA and SCL signals will be configured by
 * iBSP430platformConfigurePeripheralPins_ni().
 */
#define BSP430_PERIPHCFG_SERIAL_I2C 2

/** Value indicating peripheral should be configured in SPI 3-pin
 * mode.
 *
 * Only the MOSI (SIMO), MISO (SOMI), and CLK pins will be configured
 * by iBSP430platformConfigurePeripheralPins_ni().  STE will remain in
 * its last-configured mode. */
#define BSP430_PERIPHCFG_SERIAL_SPI3 3

/** Value indicating peripheral should be configured in SPI 4-pin
 * mode.
 *
 * The MOSI (SIMO), MISO (SOMI), CLK, and STE pins will be configured
 * by iBSP430platformConfigurePeripheralPins_ni(). */
#define BSP430_PERIPHCFG_SERIAL_SPI4 4

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
 * <li>A #sBSP430halTIMER instance specifies the CC1-CC6 and overflow ISR with this bit.
 * <li>A #sBSP430halSERIAL instance based on #sBSP430hplUSCI specifies the RX ISR with this bit.
 * </ul>
 *
 * @note As a general rule, when the HAL interface for a peripheral
 * instance is enabled, the corresponding primary ISR is also enabled.
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
 * <li>A #sBSP430halTIMER instance specifies the CC0 ISR with this bit.
 * <li>A #sBSP430halSERIAL instance based on #sBSP430hplUSCI specifies the TX ISR with this bit.
 * </ul>
 *
 * @note As a general rule the secondary ISR for a HAL interface
 * is not implicitly enabled when the HAL interface is enabled.
 * An exception is made for #sBSP430halSERIAL where the TX and RX
 * ISRs are both enabled.
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
 * peripheral-specific functions like xBSP430hplLookupTIMER().
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
struct sBSP430halISRVoidChainNode;
struct sBSP430halISRIndexedChainNode;

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

/** Indicate that LPM mode should be exited
 *
 * This flag may be marked in the return value of
 * #iBSP430halISRCallbackVoid and #iBSP430halISRCallbackIndexed
 * callbacks to indicate that the top-half should clear the standard
 * LPM bits prior to exit, causing the MCU to return to active mode.
 *
 * Use of this flag is preferred to use of #LPM4_bits or a similar
 * value because #configBSP430_CORE_DISABLE_FLL can influence the
 * selection of the bits to be cleared.
 *
 * (The value of this flag is specifically selected to be a value
 * supported by the constant generator, to optimize the callback
 * loop.) */
#define BSP430_HAL_ISR_CALLBACK_EXIT_LPM 0x0002

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
typedef int (* iBSP430halISRCallbackVoid) (const struct sBSP430halISRVoidChainNode * cb,
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
typedef int (* iBSP430halISRCallbackIndexed) (const struct sBSP430halISRIndexedChainNode * cb,
                                              void * context,
                                              int idx);

/** Structure used to record #iBSP430halISRCallbackVoid chains. */
typedef struct sBSP430halISRVoidChainNode {
  /** The next callback in the chain.  Assign a null pointer to
   * terminate the chain.  @note This field must only be mutated when
   * interrupts are disabled, or if the node is not within a callback
   * chain installed in a HAL instance. */
  const struct sBSP430halISRVoidChainNode * next_ni;

  /** The function to be invoked. */
  iBSP430halISRCallbackVoid callback;
} sBSP430halISRVoidChainNode;

/** Structure used to record #iBSP430halISRCallbackIndexed chains. */
typedef struct sBSP430halISRIndexedChainNode {
  /** The next callback in the chain.  Assign a null pointer to
   * terminate the chain.  @note This field must only be mutated when
   * interrupts are disabled, or if the node is not within a callback
   * chain installed in a HAL instance. */
  const struct sBSP430halISRIndexedChainNode * next_ni;

  /** The function to be invoked. */
  iBSP430halISRCallbackIndexed callback;
} sBSP430halISRIndexedChainNode;

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
iBSP430callbackInvokeISRVoid_ni (const struct sBSP430halISRVoidChainNode * const * cbpp,
                                 void * context,
                                 int basis)
{
  while (*cbpp && ! (basis & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN)) {
    basis |= (*cbpp)->callback(*cbpp, context);
    cbpp = &(*cbpp)->next_ni;
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
iBSP430callbackInvokeISRIndexed_ni (const struct sBSP430halISRIndexedChainNode * const * cbpp,
                                    void * context,
                                    int idx,
                                    int basis)
{
  while (*cbpp && ! (basis & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN)) {
    basis |= (*cbpp)->callback(*cbpp, context, idx);
    cbpp = &(*cbpp)->next_ni;
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
#define BSP430_HAL_ISR_CALLBACK_TAIL_NI(_return_flags) do {     \
    int return_flags_ = (_return_flags);                        \
    if (return_flags_ & BSP430_HAL_ISR_CALLBACK_EXIT_LPM) {     \
      return_flags_ |= BSP430_CORE_LPM_EXIT_MASK;               \
    }                                                           \
    BSP430_CORE_LPM_EXIT_FROM_ISR(return_flags_);               \
    if (return_flags_ & BSP430_HAL_ISR_CALLBACK_YIELD) {        \
      BSP430_RTOS_YIELD_FROM_ISR();                             \
    }                                                           \
  } while (0)

#endif /* BSP430_PERIPH_H */
