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
 * @brief Support for chained callback registrations.
 *
 * Applications often need to perform specific operations when events
 * occur.  Encoding those operations directly into the event
 * infrastructure can introduce coupling, especially when the
 * infrastructure is a shared interrupt service routine.
 * 
 * This header provides function types and structures that allow
 * applications to hook into callback chains.  Function types reflect
 * common purposes: for example, notifying interested parties that a
 * particular event has occurred, and aggregating their overall
 * response.  Structures support linked lists of callback function
 * pointers.  Inline functions simplify the invocation of the callback
 * chains.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_CALLBACK_H
#define BSP430_UTILITY_CALLBACK_H

/* Forward declarations */
struct xBSP430callbackISRVoid;
struct xBSP430callbackISRIndexed;
									 
/** Mask for status register bits cleared in ISR top half.
 *
 * This is used to prevent corrupting the status register when the
 * return value of #iBSP430callbackISRVoid and
 * #iBSP430callbackISRIndexed includes bits that are not relevant to
 * status register bits. */
#define BSP430_CALLBACK_ISR_BIC_MASK 0x00F8

/** Indicate ISR top half should yield on return.
 *
 * In some cases, a chained ISR handler might perform an operation
 * that enables a higher-priority task.  This bit may be set in the
 * return value of #iBSP430callbackISRVoid and
 * #iBSP430callbackISRIndexed to indicate that the interrupt should
 * yield to that task when it returns. */
#define BSP430_CALLBACK_ISR_YIELD 0x1000

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
 * #BSP430_CALLBACK_ISR_YIELD.
 */
typedef int (* iBSP430callbackISRVoid) (const struct xBSP430callbackISRVoid * cb,
										void * context);

/** Callback for ISR chains where the event includes an index
 *
 * This type of callback is used for digital IO ports and
 * capture/compare events on timers.
 *
 * @param cb As with #iBSP430callbackISRVoid.
 *
 * @param context As with #iBSP430callbackISRVoid.
 *
 * @param idx The sub-entity to which the event applies, such as a
 * specific pin on a port or capture/compare block on a timer.
 *
 * @return As with #iBSP430callbackISRVoid.
 */
typedef int (* iBSP430callbackISRIndexed) (const struct xBSP430callbackISRIndexed * cb,
										   void * context,
										   int idx);
/** Structure used to record #iBSP430callbackISRVoid chains. */
struct xBSP430callbackISRVoid {
	/** The next callback in the chain.  Assign a null pointer to
	 * terminate the chain. */
	const struct xBSP430callbackISRVoid * next;

	/** The function to be invoked. */
	iBSP430callbackISRVoid callback;
};

/** Structure used to record #iBSP430callbackISRIndexed chains. */
struct xBSP430callbackISRIndexed {
	/** The next callback in the chain.  Assign a null pointer to
	 * terminate the chain. */
	const struct xBSP430callbackISRIndexed * next;

	/** The function to be invoked. */
	iBSP430callbackISRIndexed callback;
};

/** Execute a chain of #iBSP430callbackISRVoid callbacks.
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
iBSP430callbackInvokeISRVoid (const struct xBSP430callbackISRVoid * const * cbpp,
							  void * context,
							  int basis)
{
	while (*cbpp) {
		basis |= (*cbpp)->callback(*cbpp, context);
		cbpp = &(*cbpp)->next;
	}
	return basis;
}

/** Execute a chain of #iBSP430callbackISRIndexed callbacks.
 *
 * Same as #iBSP430callbackInvokeISRVoid, but providing an index to
 * the callback.
 *
 * @param cbpp As with #iBSP430callbackInvokeISRVoid
 * @param context As with #iBSP430callbackInvokeISRVoid
 * @param idx The index to be passed to each #iBSP430callbackISRIndexed handler
 * @param basis As with #iBSP430callbackInvokeISRVoid
 * @return As with #iBSP430callbackInvokeISRVoid */
static int
__inline__
iBSP430callbackInvokeISRIndexed (const struct xBSP430callbackISRIndexed * const * cbpp,
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

#endif /* BSP430_UTILITY_CALLBACK_H */
