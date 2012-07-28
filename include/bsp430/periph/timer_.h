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
 * @brief Protected Timer_A/Timer_B HAL interface declarations.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_TIMER__H
#define BSP430_PERIPH_TIMER__H

#include <bsp430/periph/timer.h>

/** Structure holding hardware abstraction layer state for Timer_A and Timer_B.
 *
 * This structure is internal state, for access by applications only
 * when overriding BSP430 HAL capabilities. */
struct xBSP430timerState {
  /** The underlying timer peripheral register structure */
  volatile xBSP430periphTIMER * const timer;

  /** The number of times the timer has wrapped.
   *
   * The value is maintained only if the overflow interrupt is
   * enabled (TxIE in the timer control word).
   *
   * @note This field is not marked volatile because doing so costs
   * several extra instructions as it is not an atomic type.  It
   * should be read and written only when interrupts are
   * disabled. */
  unsigned long overflow_count;

  /** The callback chain to invoke when an overflow interrupt is
   * received. */
  const struct xBSP430periphISRCallbackVoid * overflow_callback;

  /** The callback chain to invoke when a CC0 interrupt is
   * received. */
  const struct xBSP430periphISRCallbackVoid * cc0_callback;

  /** The callback chain to invoke when a CCx interrupt is received.
   *
   * The chains are independent for each capture/compare block, but
   * the block index is passed into the chain so that a common
   * handler can be invoked if desired. */
  const struct xBSP430periphISRCallbackIndexed * cc_callback[1];
};

#endif /* BSP430_PERIPH_TIMER__H */

