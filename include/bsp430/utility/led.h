/* Copyright (c) 2012, Peter A. Bigot
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
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_LED_H
#define BSP430_UTILITY_LED_H

/** @def configBSP430_LED
 *
 * Define to a true value to indicate intent to use the LED interface.
 * Most platforms should support LEDs directly, bypassing any HAL/HPL
 * interfaces.
 *
 * @cppflag
 * @affects #BSP430_LED
 * @defaulted
 */
#ifndef configBSP430_LED
#define configBSP430_LED 1
#endif /* configBSP430_LED */

/** @def BSP430_LED
 *
 * Indicate that the LED interface is available on the platform.  This
 * is normally set by <bsp430/platform.h> when #configBSP430_LED is
 * true.  If it happens to be false, the LED interface functions are
 * replaced by macros that do nothing.
 *
 * This flag is defined only if #configBSP430_LED is true.
 *
 * @dependency #configBSP430_LED
 * @platformdefault */
#if defined(BSP430_DOXYGEN) || defined(configBSP430_LED)
#ifndef BSP430_LED
#if configBSP430_LED - 0
#define BSP430_LED 1
#else /* configBSP430_LED */
#define BSP430_LED 0
#endif /* configBSP430_LED */
#endif /* BSP430_LED */
#endif /* BSP430_DOXYGEN */

/** Call to initialize the hardware for all LEDs.
 *
 * The common implementation uses the LEDs defined in #xBSP430hal_.
 *
 * @note Unless #configBSP430_LED_USE_COMMON is defined to a true
 * value, the application or platform must define this function. */
#if defined(BSP430_DOXYGEN) || (configBSP430_LED - 0)
void vBSP430ledInitialize_ni (void);
#else /* BSP430_LED */
#define vBSP430ledInitialize_ni() do {} while (0)
#endif /* BSP430_LED */

/** Invoke to change the state of a given LED.
 *
 * @param led_idx the index to the LED of interest.  The call does
 * should simply return if ucLED does not specify a valid LED.
 *
 * @param value how to set the LED.  If positive, the LED is turned
 * on.  If zero, the LED is turned off.  If negative, the LED state is
 * inverted.
 *
 * @note If #configBSP430_LED_USE_COMMON is defined to a true value,
 * the application or platform need not define this function, but must
 * provide the data structures described for
 * #configBSP430_LED_USE_COMMON. */
#if defined(BSP430_DOXYGEN) || (configBSP430_LED - 0)
void vBSP430ledSet (int led_idx,
                    int value);
#else /* BSP430_LED */
#define vBSP430ledSet(led_idx, value) do { (void)(led_idx); (void)(value); } while (0)
#endif /* BSP430_LED */

/** @def configBSP430_LED_USE_COMMON
 *
 * If the development board has LEDs that can be expressed using
 * #sBSP430halLED (i.e., controlled through 8-bit digital I/O ports with
 * a single selection register) then the common implementation of the
 * LED interface can be used.  In that case, define this to a true
 * value, and provide within a platform file or the application
 * definitions of the #xBSP430hal_ and #nBSP430led variables.
 *
 * Since most platforms can benefit from the shared implementation,
 * those that do not should override the default value.
 *
 * @cppflag
 * @defaulted */
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
typedef struct sBSP430halLED {
  /** Address of the PxOUT register used to control the LED */
  volatile unsigned char * const outp;
  /** The bit mask for the LED (not the bit position) */
  unsigned char const bit;
  /** Unused at the moment but makes alignment requirements
   * explicit.  If we need to support a platform that wires its LEDs
   * to be on when low, this is where that would be communicated to
   * the infrastructure. */
  unsigned char const flags;
} sBSP430halLED;

#if configBSP430_LED_USE_COMMON - 0

/** Platform should define the LED configuration available to it. */
extern const sBSP430halLED xBSP430hal_[];

/** Platform should define the length of the xBSP430hal_
 * configuration array.  We'll assume there can't be more than 255
 * LEDs. */
extern const unsigned char nBSP430led;

#endif /* configBSP430_LED_USE_COMMON */

#endif /* BSP430_UTILITY_LED_H */
