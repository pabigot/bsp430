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

#include <bsp430/periph/timer_.h>

/* !BSP430! periph=timer */
/* !BSP430! instance=TA0,TA1,TA2,TA3,TB0,TB1,TB2 */

/* !BSP430! insert=periph_ba_hpl_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_ba_hpl_defn] */
#if configBSP430_PERIPH_TA0 - 0
volatile xBSP430periphTIMER * const xBSP430periph_TA0 = (volatile xBSP430periphTIMER *)_BSP430_PERIPH_TA0_BASEADDRESS;
#endif /* configBSP430_PERIPH_TA0 */

#if configBSP430_PERIPH_TA1 - 0
volatile xBSP430periphTIMER * const xBSP430periph_TA1 = (volatile xBSP430periphTIMER *)_BSP430_PERIPH_TA1_BASEADDRESS;
#endif /* configBSP430_PERIPH_TA1 */

#if configBSP430_PERIPH_TA2 - 0
volatile xBSP430periphTIMER * const xBSP430periph_TA2 = (volatile xBSP430periphTIMER *)_BSP430_PERIPH_TA2_BASEADDRESS;
#endif /* configBSP430_PERIPH_TA2 */

#if configBSP430_PERIPH_TA3 - 0
volatile xBSP430periphTIMER * const xBSP430periph_TA3 = (volatile xBSP430periphTIMER *)_BSP430_PERIPH_TA3_BASEADDRESS;
#endif /* configBSP430_PERIPH_TA3 */

#if configBSP430_PERIPH_TB0 - 0
volatile xBSP430periphTIMER * const xBSP430periph_TB0 = (volatile xBSP430periphTIMER *)_BSP430_PERIPH_TB0_BASEADDRESS;
#endif /* configBSP430_PERIPH_TB0 */

#if configBSP430_PERIPH_TB1 - 0
volatile xBSP430periphTIMER * const xBSP430periph_TB1 = (volatile xBSP430periphTIMER *)_BSP430_PERIPH_TB1_BASEADDRESS;
#endif /* configBSP430_PERIPH_TB1 */

#if configBSP430_PERIPH_TB2 - 0
volatile xBSP430periphTIMER * const xBSP430periph_TB2 = (volatile xBSP430periphTIMER *)_BSP430_PERIPH_TB2_BASEADDRESS;
#endif /* configBSP430_PERIPH_TB2 */

/* END AUTOMATICALLY GENERATED CODE [periph_ba_hpl_defn] */
/* !BSP430! end=periph_ba_hpl_defn */

/* !BSP430! insert=hal_ba_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_ba_defn] */
#if configBSP430_PERIPH_TA0 - 0
static struct xBSP430timerState state_TA0_ = {
	.timer = (xBSP430periphTIMER *)_BSP430_PERIPH_TA0_BASEADDRESS
};

xBSP430timerHandle const xBSP430timer_TA0 = &state_TA0_;
#endif /* configBSP430_PERIPH_TA0 */

#if configBSP430_PERIPH_TA1 - 0
static struct xBSP430timerState state_TA1_ = {
	.timer = (xBSP430periphTIMER *)_BSP430_PERIPH_TA1_BASEADDRESS
};

xBSP430timerHandle const xBSP430timer_TA1 = &state_TA1_;
#endif /* configBSP430_PERIPH_TA1 */

#if configBSP430_PERIPH_TA2 - 0
static struct xBSP430timerState state_TA2_ = {
	.timer = (xBSP430periphTIMER *)_BSP430_PERIPH_TA2_BASEADDRESS
};

xBSP430timerHandle const xBSP430timer_TA2 = &state_TA2_;
#endif /* configBSP430_PERIPH_TA2 */

#if configBSP430_PERIPH_TA3 - 0
static struct xBSP430timerState state_TA3_ = {
	.timer = (xBSP430periphTIMER *)_BSP430_PERIPH_TA3_BASEADDRESS
};

xBSP430timerHandle const xBSP430timer_TA3 = &state_TA3_;
#endif /* configBSP430_PERIPH_TA3 */

#if configBSP430_PERIPH_TB0 - 0
static struct xBSP430timerState state_TB0_ = {
	.timer = (xBSP430periphTIMER *)_BSP430_PERIPH_TB0_BASEADDRESS
};

xBSP430timerHandle const xBSP430timer_TB0 = &state_TB0_;
#endif /* configBSP430_PERIPH_TB0 */

#if configBSP430_PERIPH_TB1 - 0
static struct xBSP430timerState state_TB1_ = {
	.timer = (xBSP430periphTIMER *)_BSP430_PERIPH_TB1_BASEADDRESS
};

xBSP430timerHandle const xBSP430timer_TB1 = &state_TB1_;
#endif /* configBSP430_PERIPH_TB1 */

#if configBSP430_PERIPH_TB2 - 0
static struct xBSP430timerState state_TB2_ = {
	.timer = (xBSP430periphTIMER *)_BSP430_PERIPH_TB2_BASEADDRESS
};

xBSP430timerHandle const xBSP430timer_TB2 = &state_TB2_;
#endif /* configBSP430_PERIPH_TB2 */

/* END AUTOMATICALLY GENERATED CODE [hal_ba_defn] */
/* !BSP430! end=hal_ba_defn */

volatile xBSP430periphTIMER *
xBSP430periphLookupTIMER (xBSP430periphHandle periph)
{
/* !BSP430! insert=periph_ba_hpl_demux */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_ba_hpl_demux] */
#if configBSP430_PERIPH_TA0 - 0
	if (BSP430_PERIPH_TA0 == periph) {
		return xBSP430periph_TA0;
	}
#endif /* configBSP430_PERIPH_TA0 */

#if configBSP430_PERIPH_TA1 - 0
	if (BSP430_PERIPH_TA1 == periph) {
		return xBSP430periph_TA1;
	}
#endif /* configBSP430_PERIPH_TA1 */

#if configBSP430_PERIPH_TA2 - 0
	if (BSP430_PERIPH_TA2 == periph) {
		return xBSP430periph_TA2;
	}
#endif /* configBSP430_PERIPH_TA2 */

#if configBSP430_PERIPH_TA3 - 0
	if (BSP430_PERIPH_TA3 == periph) {
		return xBSP430periph_TA3;
	}
#endif /* configBSP430_PERIPH_TA3 */

#if configBSP430_PERIPH_TB0 - 0
	if (BSP430_PERIPH_TB0 == periph) {
		return xBSP430periph_TB0;
	}
#endif /* configBSP430_PERIPH_TB0 */

#if configBSP430_PERIPH_TB1 - 0
	if (BSP430_PERIPH_TB1 == periph) {
		return xBSP430periph_TB1;
	}
#endif /* configBSP430_PERIPH_TB1 */

#if configBSP430_PERIPH_TB2 - 0
	if (BSP430_PERIPH_TB2 == periph) {
		return xBSP430periph_TB2;
	}
#endif /* configBSP430_PERIPH_TB2 */

/* END AUTOMATICALLY GENERATED CODE [periph_ba_hpl_demux] */
/* !BSP430! end=periph_ba_hpl_demux */
	return NULL;
}
