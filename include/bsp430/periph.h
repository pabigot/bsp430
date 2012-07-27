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
 * Use of a given peripheral must be indicated by defining the
 * corresponding @c configBSP430_PERIPH_xx in the application @c
 * FreeRTOSConfig.h file.
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
 * #BSP430_PERIPH_XT1.
 *
 * Negative values indicate an error. */
typedef int xBSP430periphHandle;

/** HPL handle specifying that no peripheral has been identified. */
#define BSP430_PERIPH_NONE ((xBSP430periphHandle)0)

/** HPL handle identifying the XT1 crystal functionality.
 *
 * This is used by platform-independent clock peripherals to request
 * that the pins related to XIN and XOUT be configured to their
 * peripheral function. */
#define BSP430_PERIPH_XT1 ((xBSP430periphHandle)1)

/** HPL handle identifying the ability to expose clock signals.
 *
 * This is used by application code as a debug facility, invoking
 * #iBSP430platformConfigurePeripheralPins_ni to expose MCLK, SMCLK, and
 * ACLK on pins where they may be monitored.  Its availability is
 * conditional on platform support and
 * #configBSP430_PERIPH_EXPOSED_CLOCKS. */
#define BSP430_PERIPH_EXPOSED_CLOCKS ((xBSP430periphHandle)3)

/** @def configBSP430_PERIPH_EXPOSED_CLOCKS
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of
 * #BSP430_PERIPH_EXPOSED_CLOCKS in
 * #iBSP430platformConfigurePeripheralPins_ni. */
#ifndef configBSP430_PERIPH_EXPOSED_CLOCKS
#define configBSP430_PERIPH_EXPOSED_CLOCKS 0
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */

/* Forward declarations */
struct xBSP430periphISRCallbackVoid;
struct xBSP430periphISRCallbackIndexed;
									 
/** Mask for status register bits cleared in ISR top half.
 *
 * This is used to prevent corrupting the status register when the
 * return value of #iBSP430periphISRCallbackVoid and
 * #iBSP430periphISRCallbackIndexed includes bits that are not relevant to
 * status register bits. */
#define BSP430_PERIPH_ISR_CALLBACK_BIC_MASK 0x00F8

/** Indicate ISR top half should yield on return.
 *
 * In some cases, a chained ISR handler might perform an operation
 * that enables a higher-priority task.  This bit may be set in the
 * return value of #iBSP430periphISRCallbackVoid and
 * #iBSP430periphISRCallbackIndexed to indicate that the interrupt should
 * yield to that task when it returns. */
#define BSP430_PERIPH_ISR_CALLBACK_YIELD 0x1000

/** Indicate ISR top half should disable the corresponding interrupt on return.
 *
 * In some cases, the handler can determine that the interrupt is no
 * longer needed, but cannot itself disable the interrupt enable bit.
 * This bit may be set in the return value of
 * #iBSP430periphISRCallbackVoid and #iBSP430periphISRCallbackIndexed
 * to indicate that the top-half should clear the corresponding IE bit
 * before returning returns. */
#define BSP430_PERIPH_ISR_DISABLE_INTERRUPT 0x2000

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
 * should be a combination of bits like LPM4_bits and
 * #BSP430_PERIPH_ISR_CALLBACK_YIELD.
 */
typedef int (* iBSP430periphISRCallbackVoid) (const struct xBSP430periphISRCallbackVoid * cb,
											  void * context);

/** Callback for ISR chains where the event includes an index
 *
 * This type of callback is used for digital IO ports and
 * capture/compare events on timers.
 *
 * @param cb As with #iBSP430periphISRCallbackVoid.
 *
 * @param context As with #iBSP430periphISRCallbackVoid.
 *
 * @param idx The sub-entity to which the event applies, such as a
 * specific pin on a port or capture/compare block on a timer.
 *
 * @return As with #iBSP430periphISRCallbackVoid.
 */
typedef int (* iBSP430periphISRCallbackIndexed) (const struct xBSP430periphISRCallbackIndexed * cb,
												 void * context,
												 int idx);
/** Structure used to record #iBSP430periphISRCallbackVoid chains. */
struct xBSP430periphISRCallbackVoid {
	/** The next callback in the chain.  Assign a null pointer to
	 * terminate the chain. */
	const struct xBSP430periphISRCallbackVoid * next;

	/** The function to be invoked. */
	iBSP430periphISRCallbackVoid callback;
};

/** Structure used to record #iBSP430periphISRCallbackIndexed chains. */
struct xBSP430periphISRCallbackIndexed {
	/** The next callback in the chain.  Assign a null pointer to
	 * terminate the chain. */
	const struct xBSP430periphISRCallbackIndexed * next;

	/** The function to be invoked. */
	iBSP430periphISRCallbackIndexed callback;
};

/** Execute a chain of #iBSP430periphISRCallbackVoid callbacks.
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
iBSP430callbackInvokeISRVoid_ni (const struct xBSP430periphISRCallbackVoid * const * cbpp,
							  void * context,
							  int basis)
{
	while (*cbpp) {
		basis |= (*cbpp)->callback(*cbpp, context);
		cbpp = &(*cbpp)->next;
	}
	return basis;
}

/** Execute a chain of #iBSP430periphISRCallbackIndexed callbacks.
 *
 * Same as #iBSP430callbackInvokeISRVoid_ni, but providing an index to
 * the callback.
 *
 * @param cbpp As with #iBSP430callbackInvokeISRVoid_ni
 * @param context As with #iBSP430callbackInvokeISRVoid_ni
 * @param idx The index to be passed to each #iBSP430periphISRCallbackIndexed handler
 * @param basis As with #iBSP430callbackInvokeISRVoid_ni
 * @return As with #iBSP430callbackInvokeISRVoid_ni */
static int
__inline__
iBSP430callbackInvokeISRIndexed_ni (const struct xBSP430periphISRCallbackIndexed * const * cbpp,
								 void * context,
								 int idx,
								 int basis)
{
	while (*cbpp) {
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
 * LPM_bits and/or #BSP430_PERIPH_ISR_CALLBACK_YIELD. */
#define BSP430_PERIPH_ISR_CALLBACK_TAIL(_return_flags) do {				\
		int return_flags_ = (_return_flags);							\
		__bic_status_register_on_exit((return_flags_) & BSP430_PERIPH_ISR_CALLBACK_BIC_MASK); \
		portYIELD_FROM_ISR(((return_flags_) & BSP430_PERIPH_ISR_CALLBACK_YIELD) ? pdTRUE : pdFALSE); \
	} while (0)

#endif /* BSP430_PERIPH_H */
