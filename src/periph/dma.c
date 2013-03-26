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

#include <bsp430/platform.h>
#include <bsp430/periph/dma.h>

#if (BSP430_MODULE_DMA - 0)

#if (configBSP430_HAL_DMA - 0)

static const sBSP430halISRIndexedChainNode * ch_callback_DMA[BSP430_DMA_NUM_CHANNELS];

sBSP430halDMA xBSP430hal_DMA_ = {
  .hal_state = {
    .cflags =
#if (BSP430_MODULE_DMAX - 0)
    BSP430_DMA_HAL_HPL_VARIANT_DMAX
#else /* BSP430_MODULE_DMAX */
    BSP430_DMA_HAL_HPL_VARIANT_DMA
#endif /* BSP430_MODULE_DMAX */
  },
  .hpl = BSP430_HPL_DMA,
  .ch_cbchain_ni = ch_callback_DMA
};

#if (configBSP430_HAL_DMA_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(DMA_VECTOR)
isr_DMA (void)
{
  hBSP430halDMA dma = BSP430_HAL_DMA;
  unsigned int iv = DMAIV;
  int rv = 0;

  if (0 != iv) {
    int ch = (iv - 2) / 2;
    rv = iBSP430callbackInvokeISRIndexed_ni(ch + dma->ch_cbchain_ni, dma, ch, rv);
    /* Inhibit further interrupts on this channel if desired. */
    if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
      dma->hpl->ch[ch].ctl &= ~DMAIE;
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_DMA_ISR */

#endif /* BSP430_MODULE_DMA */

#endif /* configBSP430_HAL_DMA */
