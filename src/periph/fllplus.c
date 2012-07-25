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

#include <bsp430/periph/fllplus.h>

unsigned char
ucBSP430fllplusConfigure (const xBSP430fllplusConfig * pxConfig)
{
	unsigned char ucReturnValue;
	unsigned int uxStableLoopsLeft = 100;
	
	BSP430_ENTER_CRITICAL();

	FLL_CTL0 = pxConfig->ucFLL_CTL0;
	FLL_CTL1 = pxConfig->ucFLL_CTL1;
	do {
		BSP430_CLOCK_LFXT1_CLEAR_FAULT();
		loop_limit -= loop_delta;
		__delay_cycles(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);

	} while ((BSP430_CLOCK_LFXT1_IS_FAULTED()) && (0 != loop_limit));
	ucReturnValue = ! BSP430_CLOCK_LFXT1_IS_FAULTED();
	SCFI0 = pxConfig->ucSCFI0;
	SCFQCTL = pxConfig->ucSCFQCTL;

	BSP430_EXIT_CRITICAL();
	return ucReturnValue;
}

unsigned long
ulBSP430clockMCLK_Hz ()
{
}

unsigned long
ulBSP430clockSMCLK_Hz ()
{
}

unsigned short
usBSP430clockACLK_Hz ()
{
	return (FLL_CTL0 & LFOF) ? 12000U : 32768U;
}

int
iBSP430clockConfigureXT1 (int enablep,
						  int loop_limit)
{
	int loop_delta;
	int rc;
	
	rc = iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_XT1, enablep);
	if ((0 != rc) || (! enablep)) {
		return rc;
	}
	loop_delta = (0 < loop_limit) ? 1 : 0;

	FLL_CTL0 = (FLL_CTL0 & ~(OSCCAP0 | OSCCAP1 | XTS_FLL)) | BSP430_FLLPLUS_XCAPxPF;
	do {
		FLL_CTL0 &= ~LFOF;
		loop_limit -= loop_delta;
		__delay_cycles(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
	} while ((FLL_CTL0 & LFOF) && (0 != loop_limit));

	rc = !(FLL_CTL0 & LFOF);
	if (! rc) {
		(void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_XT1, 0);
	}
	return rc;
}
