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

/** TimerA0
 *
 * This module supports a standard configuration of TimerA0 on an
 * MSP430 device.  This timer is also normally used to manage the
 * timeslice clock.
 *
 * TA0 is configured to source from ACLK, presumed to be running at 32
 * kiHz.  CCR0 and its corresponding interrupt are used in the port.c
 * file to manage scheduler timeslices.
 *
 * The remaining TA0 CCRs are available for application use.
 * Applications hook into the infrastructure invoking
 * timerA0SetIFGHandler.
 */

#ifndef _BSP430_TIMERA0_H_
#define _BSP430_TIMERA0_H_

#include "platform.h"

typedef void (* isrHandler) ();

/** Assign a handler to be invoked from the TA0 top-half ISR.
 *
 * @param taiv the TA0IV value identifying the event for which the
 * handler should be invoked. Valid values are TA0IV_TA0CCR1 through
 * the highest CCR supported on the MCU.
 *
 * @param handler the handler to be invoked upon a TA0IV event.  Pass
 * a null pointer to de-register a handler.
 *
 * @return the handler previously registered for the event. */
isrHandler xBSP430timerA0SetIFGHandler (unsigned int taiv,
										isrHandler handler);

/** Configure TimerA0 and reset the global counters. */
void vBSP430timerA0Configure ();

/** Return the number of ticks since last configure.
 *
 * At 32 kiHz, the 32-bit return value covers about 36 hours. */ 
unsigned long ulBSP430timerA0Ticks ();

/** Return the number of roll-overs since last configure.
 * 
 * In combination with ulBSP430timerA0Ticks() this provides a 48-bit
 * uptime counter.  At 32 kiHz this is somewhere around 272 years, so
 * its probably good enough. */
unsigned long ulBSP430timerA0Rollovers ();

#endif /* _BSP430_TIMERA0_H_ */
