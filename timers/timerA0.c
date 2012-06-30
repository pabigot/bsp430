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

#include <bsp430/timers/timerA0.h>
#include <stdint.h>

volatile static isrHandler isr_handlers [
#if defined(__MSP430_HAS_TA2__)
										2
#elif defined(__MSP430_HAS_TA3__) || defined(__MSP430_HAS_T0A3__)
										3
#elif defined(__MSP430_HAS_TA5__) || defined(__MSP430_HAS_T0A5__)
										5
#else
#error Unable to determine CCR count for TA0
#endif
										];

/* Count of rollover events */
static uint32_t prvRollovers;

void vBSP430timerA0Configure ()
{
	__istate_t istate = __get_interrupt_state();
	__disable_interrupt();
	
	/* Ensure the timer is stopped before configuring. */
	TA0CTL = 0;
	__delay_cycles(50);

	prvRollovers = 0;
	
	/* Count continuously using ACLK clearing the initial counter, and
	 * interrupt on rollover. */
	TA0CTL = TASSEL__ACLK | MC__CONTINOUS | TACLR | TAIE;
	
	__set_interrupt_state(istate);
}

unsigned long ulBSP430timerA0Rollovers ()
{
	__istate_t istate = __get_interrupt_state();
	unsigned long rv;
	
	__disable_interrupt();
	rv = prvRollovers;
	__set_interrupt_state(istate);
	
	return rv;
}

unsigned long ulBSP430timerA0Ticks ()
{
	__istate_t istate = __get_interrupt_state();
	uint32_t rv;
	uint16_t ta0ra;
	uint16_t ta0rb;
	uint16_t ta0ctl;

	__disable_interrupt();
	/* What we're doing here is making sure we have a consistent read
	 * of the timer register and that we know whether there's a
	 * pending rollover that would affect the return value. */
	ta0ra = TA0R;
	do {
		ta0ctl = TA0CTL;
		ta0rb = TA0R;
		if (ta0ra == ta0rb) {
			break;
		}
		ta0ctl = TA0CTL;
		ta0ra = TA0R;
	} while (ta0ra != ta0rb);

	if (ta0ctl & TAIFG) {
		++prvRollovers;
		TA0CTL &= ~TAIFG;
	}
	rv = (prvRollovers << 16) + ta0ra;
	__set_interrupt_state(istate);
	return rv;
}

__attribute__( ( __interrupt__( TIMER0_A1_VECTOR ) ) )
static void prvT0AISR ( void )
{
	unsigned int taiv = TA0IV;
	if (TA0IV_TA0IFG == taiv) {
		++prvRollovers;
	} else if (TA0IV_NONE != taiv) {
	}
}
