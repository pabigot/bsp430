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

#include <bsp430/periph/timer.h>

/* !BSP430! periph=timer */
/* !BSP430! instance=T0A,T1A,T2A,T3A,T0B,T1B,T2B */

volatile xBSP430periphTIMER *
xBSP430periphLookupTIMER (xBSP430periphHandle periph)
{
/* !BSP430! insert=periph_ba_hpl_demux */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_ba_hpl_demux] */
#if configBSP430_PERIPH_T0A - 0
	if (BSP430_PERIPH_T0A == periph) {
		return xBSP430periph_T0A;
	}
#endif /* configBSP430_PERIPH_T0A */

#if configBSP430_PERIPH_T1A - 0
	if (BSP430_PERIPH_T1A == periph) {
		return xBSP430periph_T1A;
	}
#endif /* configBSP430_PERIPH_T1A */

#if configBSP430_PERIPH_T2A - 0
	if (BSP430_PERIPH_T2A == periph) {
		return xBSP430periph_T2A;
	}
#endif /* configBSP430_PERIPH_T2A */

#if configBSP430_PERIPH_T3A - 0
	if (BSP430_PERIPH_T3A == periph) {
		return xBSP430periph_T3A;
	}
#endif /* configBSP430_PERIPH_T3A */

#if configBSP430_PERIPH_T0B - 0
	if (BSP430_PERIPH_T0B == periph) {
		return xBSP430periph_T0B;
	}
#endif /* configBSP430_PERIPH_T0B */

#if configBSP430_PERIPH_T1B - 0
	if (BSP430_PERIPH_T1B == periph) {
		return xBSP430periph_T1B;
	}
#endif /* configBSP430_PERIPH_T1B */

#if configBSP430_PERIPH_T2B - 0
	if (BSP430_PERIPH_T2B == periph) {
		return xBSP430periph_T2B;
	}
#endif /* configBSP430_PERIPH_T2B */

/* END AUTOMATICALLY GENERATED CODE [periph_ba_hpl_demux] */
/* !BSP430! end=periph_ba_hpl_demux */
	return NULL;
}
