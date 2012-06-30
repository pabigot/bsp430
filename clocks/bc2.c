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

#include <bsp430/clocks/bc2.h>

unsigned char
ucBSP430bc2Configure (unsigned char ucDCOCTL,
					  unsigned char ucBCSCTL1,
					  unsigned char ucBCSCTL2,
					  unsigned char ucBCSCTL3)
{
	unsigned char ucCrystalOK = pdFALSE;
	
	portENTER_CRITICAL();
	
	BCSCTL3 = ucBCSCTL3;
	if ( ! ( BCSCTL3 & LFXT1S1 ) ) {
		portBASE_TYPE uxStableLoopsLeft = 10;

		/* See whether the crystal is populated and functional.
		 * Assume that if fault is not detected in 100msec, the
		 * crystal is stable.  Retry for up to one second.  Reset the
		 * DCO to its PUC values for the purpose of the delay loop. */
		DCOCTL = 0;
		BCSCTL1 = 0x87;
		DCOCTL = 0x60;
		do {
			IFG1 &= ~OFIFG;
			__delay_cycles(100000UL);
			--uxStableLoopsLeft;
		} while ( ( IFG1 & OFIFG ) && ( 0 < uxStableLoopsLeft ) );
		ucCrystalOK = ! ( IFG1 & OFIFG );
		if (! ucCrystalOK) {
			/* No functional crystal; fall back to drive ACLK from VLOCLK */
			BCSCTL3 |= LFXT1S_2;
		}
	}
	/* Select lowest DCOx and MODx prior to configuring */
	DCOCTL = 0;
	/* Set range */
	BCSCTL1 = ucBCSCTL1;
	/* Set DCO step and modulation */
	DCOCTL = ucDCOCTL;
	/* Set clock dividers */
	BCSCTL2 = ucBCSCTL2;

	portEXIT_CRITICAL();
	return ucCrystalOK;
}
