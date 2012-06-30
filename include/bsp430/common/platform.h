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

/** 
 *
 */

#ifndef BSP430_COMMON_PLATFORM_H
#define BSP430_COMMON_PLATFORM_H

#include <msp430.h>
#include "FreeRTOS.h"

/** Basic configuration for the platform.
 *
 * This routine will:
 * @li Disable the watchdog
 * @li Enable the platform crystal, if available
 * @li Configure the clocks based on configCPU_CLOCK_HZ
 * @li Configure TimerA0
 * @li Set up a 9600 baud console
 */
void vBSP430platformSetup ();

/** Configure the pins associated with a given peripheral.
 *
 * @param devid Device for which pins should be configured
 * 
 * @param enablep TRUE to enable for peripheral use; FALSE to disable
 * (return to general purpose IO)
 *
 * @return 0 if configuration was successful, -1 if the device was not
 * recognized.
 */
int vBSP430platformConfigurePeripheralPins (int devid, int enablep);

#endif /* BSP430_COMMON_PLATFORM_H */
