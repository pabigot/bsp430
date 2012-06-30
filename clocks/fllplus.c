/*
  
Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the software nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#include <bsp430/clocks/fllplus.h>

unsigned char ucBSP430fllplusConfigure( const xFLLPLUSDefn * pxConfig )
{
	unsigned char ucReturnValue;
	unsigned portBASE_TYPE uxStableLoopsLeft = 100;
	
	portENTER_CRITICAL();

	FLL_CTL0 = pxConfig->ucFLL_CTL0;
	FLL_CTL1 = pxConfig->ucFLL_CTL1;
	do {
		IFG1 &= ~OFIFG;
		__delay_cycles(20000);
		--uxStableLoopsLeft;
	} while ((IFG1 & OFIFG) && (0 < uxStableLoopsLeft));
	ucReturnValue = !(IFG1 & OFIFG);
	SCFI0 = pxConfig->ucSCFI0;
	SCFQCTL = pxConfig->ucSCFQCTL;

	portEXIT_CRITICAL();
	return ucReturnValue;
}
