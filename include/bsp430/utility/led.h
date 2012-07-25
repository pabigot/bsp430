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
 * @brief Support for platform-independent LED manipulation.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_LED_H
#define BSP430_UTILITY_LED_H

#include "FreeRTOS.h"

/** Call this once to initialize the hardware for all LEDs.
 *
 * The common implementation uses the LEDs defined in #pxBSP430leds.
 *
 * @note Unless #configBSP430_LED_USE_COMMON is defined to a true
 * value, the application or platform must define this function. */
void vBSP430ledInit_ni (void);

/** Invoke to change the state of a given LED.
 *
 * @param ucLED The index to the LED of interest.  The call does
 * should simply return if ucLED does not specify a valid LED.
 *
 * @param xValue How to set the LED.  If positive, the LED is turned
 * on.  If zero, the LED is turned off.  If negative, the LED state is
 * inverted.
 *
 * @note If #configBSP430_LED_USE_COMMON is defined to a true value,
 * the application or platform need not define this function, but must
 * provide the data structures described for
 * #configBSP430_LED_USE_COMMON. */
void vBSP430ledSet (unsigned char ucLED,
					signed portBASE_TYPE xValue);

/** @def configBSP430_LED_USE_COMMON
 *
 * If the development board has LEDs that can be expressed using
 * #xBSP430led (i.e., controlled through 8-bit digital I/O ports with
 * a single selection register) then the common implementation of the
 * LED interface can be used.  In that case, define this to a true
 * value, and provide within a platform file or the application
 * definitions of the #pxBSP430leds and #ucBSP430leds variables.
 *
 * Since most platforms can benefit from the shared implementation,
 * those that do not should override the default value. */
#ifndef configBSP430_LED_USE_COMMON
#define configBSP430_LED_USE_COMMON 1
#endif /* configBSP430_LED_USE_COMMON */

/** Structure used to define the LED interface.
 *
 * The assumption is that all MSP430 board LEDs are controlled through
 * GPIO pins.  The address of the PxOUT register is provided
 * explicitly; the PxSEL and PxDIR registers are inferred by comparing
 * that address with the known addresses of the PxOUT registers in the
 * initialization loop. */
typedef struct xBSP430_LED_DEFN {
	/** Address of the PxOUT register used to control the LED */
	volatile unsigned char* pucPxOUT;
	/** The bit mask for the LED (not the bit position) */
	unsigned char ucBIT;
	/** Unused at the moment but makes alignment requirements
	 * explicit.  If we need to support a platform that wires its LEDs
	 * to be on when low, this is where that would be communicated to
	 * the infrastructure. */
	unsigned char ucFLAGS;
} xBSP430led;

/** Platform should define the LED configuration available to it. */
extern const xBSP430led pxBSP430leds[];

/** Platform should define the length of the pxBSP430leds
 * configuration array.  We'll assume there can't be more than 255
 * LEDs. */
extern const unsigned char ucBSP430leds;


#endif /* BSP430_UTILITY_LED_H */
