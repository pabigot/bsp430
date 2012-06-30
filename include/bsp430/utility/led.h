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

/** @file
 *
 * Support for platform-independent LED manipulation
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_LED_H
#define BSP430_UTILITY_LED_H

#include "FreeRTOS.h"

/** Structure used to define the LED interface.
 *
 * The assumption is that all MSP430 board LEDs are controlled through
 * GPIO pins.  The address of the PxOUT register is provided
 * explicitly; the PxSEL and PxDIR registers are inferred by comparing
 * that address with the known addresses of the PxOUT registers in the
 * initialization loop. */

typedef struct xLED_DEFN {
	/** Address of the PxOUT register used to control the LED */
	volatile unsigned char* pucPxOUT;
	/** The bit mask for the LED (not the bit position) */
	unsigned char ucBIT;
	/** Unused at the moment but makes alignment requirements
	 * explicit.  If we need to support a platform that wires its LEDs
	 * to be on when low, this is where that would be communicated to
	 * the infrastructure. */
	unsigned char ucFLAGS;
} xLEDDefn;

/** Platform should define the LED configuration available to it. */
extern const xLEDDefn pxLEDDefn[];

/** Platform should define the length of the LED configuration array.
 * We'll assume there can't be more than 255 LEDs. */
extern const unsigned char ucLEDDefnCount;

/** Call this once to initialize the hardware for all LEDs defined in
 * the pxLEDDefn array. */
void vBSP430ledInit (void);

/** Invoke to change the state of a given LED.
 *
 * @param ucLED The index to the LED of interest.  The call does
 * nothing if ucLED is not between zero and ucLEDDefnCount-1,
 * inclusive.
 *
 * @param xValue How to set the LED.  If positive, the LED is turned
 * on.  If zero, the LED is turned off.  If negative, the LED state is
 * inverted.
 */
void vBSP430ledSet (unsigned char uxLED,
					signed portBASE_TYPE xValue);

#endif /* BSP430_UTILITY_LED_H */
