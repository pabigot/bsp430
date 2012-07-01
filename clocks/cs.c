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

#include <bsp430/clocks/cs.h>
#include <stdint.h>

#if 5 < configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT
#error FR5xx CS module cannot express configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT
#endif /* configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT */

unsigned long
ulBSP430clockMCLK_Hz ()
{
	unsigned long freq_Hz = 0;
	
	switch (CSCTL1 & 0x86) {
	case 0x00:
	case 0x04:
		freq_Hz = 5333333UL;
		break;
	case 0x02:
		freq_Hz = 6666667UL;
		break;
	case 0x06:
		freq_Hz = 8000000UL;
		break;
	case 0x80:
	case 0x84:
		freq_Hz = 16000000UL;
		break;
	case 0x82:
		freq_Hz = 20000000UL;
		break;
	case 0x86:
		freq_Hz = 24000000UL;
		break;
	}
	return freq_Hz;
}

unsigned long
ulBSP430clockSMCLK_Hz ()
{
	return ulBSP430clockMCLK_Hz() >> configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT;
}

unsigned long
ulBSP430csConfigureMCLK (unsigned long ulFrequency_Hz)
{
	unsigned int csctl1 = 0x06;
	if ((5330000UL + 6670000UL) / 2 > ulFrequency_Hz) {
		csctl1 = 0;
	} else if ((6670000UL + 8000000UL) / 2 > ulFrequency_Hz) {
		csctl1 = 0x02;
	} else if ((8000000UL + 16000000UL) / 2 < ulFrequency_Hz) {
		csctl1 = 0x86;
		if ((16000000UL + 20000000UL) / 2 > ulFrequency_Hz) {
			csctl1 = 0x80;
		} else if ((20000000UL + 24000000UL) / 2 > ulFrequency_Hz) {
			csctl1 = 0x82;
		}
	}

	CSCTL0_H = 0xA5;
	CSCTL1 = csctl1;
	CSCTL2 = (CSCTL2 & 0x0700) | SELS__DCOCLK | SELM__DCOCLK;
	CSCTL3 = configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT << 4;
	CSCTL0_H = !0xA5;

	return ulBSP430clockMCLK_Hz();
}
