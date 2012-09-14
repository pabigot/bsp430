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
 * @brief A generic console print capability.
 *
 * cprintf() is like @c printf.  It disables interrupts while
 * operating to ensure that interleaved messages do not occur.  It is
 * "safe" for call from within interrupt handlers. cputs() is provided
 * where the complexity of printf is not required but atomic output is
 * desired.
 *
 * Other routines permit display of plain text (cputtext_ni()), single
 * characters (cputchar_ni()), and integers (cputi_ni(), cputu_ni(),
 * cputl_ni(), cputul_ni()) without incurring the stack overhead of
 * printf, which can be quite high (on the order of 100 bytes if
 * 64-bit integer support is included).  These assume that interrupts
 * are disabled when called.
 *
 * All these routines are safe to call even if the console was not
 * initialized, or its initialization failed, or it is temporarily
 * disabled: in that situation, they simply return immediately.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_CONSOLE_H
#define BSP430_UTILITY_CONSOLE_H

#include <bsp430/serial.h>
#include <bsp430/platform.h>
#include <stdarg.h>

/** @def configBSP430_CONSOLE
 *
 * Define to a true value to request that a serial handle be
 * identified to serve as a system console.
 *
 * See #configBSP430_CONSOLE_USE_DEFAULT_RESOURCE.
 *
 * @cppflag
 * @affects #BSP430_CONSOLE
 * @defaulted
 */
#ifndef configBSP430_CONSOLE
#define configBSP430_CONSOLE 0
#endif /* configBSP430_CONSOLE */

/** @def BSP430_CONSOLE
 *
 * Defined to a true value if #BSP430_CONSOLE_SERIAL_PERIPH_HANDLE has
 * been provided, making the console infrastructure available.
 *
 * @dependency #configBSP430_CONSOLE
 * @platformdefault
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_CONSOLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def configBSP430_CONSOLE_USE_DEFAULT_RESOURCE
 *
 * Define to a true value to use the default (platform-specific)
 * serial HAL handle.  This is true by default if
 * #configBSP430_CONSOLE is true.
 *
 * If you want to override the default, define this to a false value
 * and provide definitions for:
 * <ul>
 * <li>#BSP430_CONSOLE_SERIAL_PERIPH_HANDLE
 * </ul>
 *
 * You are also responsible for requesting the inclusion of the
 * corresponding device and its HAL interface in the application,
 * e.g. by setting #configBSP430_HAL_USCI5_A1.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_USE_DEFAULT_RESOURCE
#define configBSP430_CONSOLE_USE_DEFAULT_RESOURCE (configBSP430_CONSOLE - 0)
#endif /* configBSP430_CONSOLE_USE_DEFAULT_RESOURCE */

/** @def BSP430_CONSOLE_SERIAL_PERIPH_HANDLE
 *
 * The peripheral handle that should be used by platform-agnostic
 * programs to create the console, for example #BSP430_PERIPH_USCI5_A0.
 * A default is provided based on the platform or available serial
 * peripherals.
 *
 * If you override this, be sure to set
 * #configBSP430_CONSOLE_USE_DEFAULT_RESOURCE to false and to
 * explicitly request the HAL resource in your bsp430_config.h file
 * (e.g., enable #configBSP430_HAL_USCI5_A0).
 *
 * @dependency #BSP430_CONSOLE
 * @platformdefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_CONSOLE_BAUD_RATE
 *
 * The baud rate that should be used for the console.  This may be
 * overridden in a platform header or by an application configuration.
 *
 * @defaulted */
#ifndef BSP430_CONSOLE_BAUD_RATE
#define BSP430_CONSOLE_BAUD_RATE 9600
#endif /* BSP430_CONSOLE_BAUD_RATE */

/** @def BSP430_CONSOLE_RX_BUFFER_SIZE
 *
 * Define this to a non-zero power of 2 to allow the console
 * infrastructure to buffer input in an ISR.  The value must not
 * exceed 128.
 *
 * If this has a value of zero, character input is not interrupt
 * driven. cgetchar_ni() will return the most recently received
 * character, if any, and cpeekchar_ni() will not be available.
 * @defaulted */
#ifndef BSP430_CONSOLE_RX_BUFFER_SIZE
#define BSP430_CONSOLE_RX_BUFFER_SIZE 0
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

/** Return a character that was input to the console.
 *
 * @return the next character that was input to the console, or -1 if
 * no characters are available.
 *
 * @note This function is available even if
 * #BSP430_CONSOLE_RX_BUFFER_SIZE is zero.
 */
int cgetchar_ni (void);

/** Peek at the next character input to the console
 *
 * Use this to determine whether there's any data ready to be read,
 * without actually consuming it yet.
 *
 * @return the value that would be returned by invoking cgetchar_ni(),
 * but without consuming any pending input.
 *
 * @warning This function is not available if
 * #BSP430_CONSOLE_RX_BUFFER_SIZE is zero.
 *
 * @dependency #BSP430_CONSOLE_RX_BUFFER_SIZE */
int cpeekchar_ni (void);

/** @def configBSP430_CONSOLE_PROVIDES_PUTCHAR
 *
 * If defined to a true value, the individual character display
 * function used internally to the console module will be made public
 * with the name @c putchar so that it will be used by @c printf(3)
 * when the C library depends on an external @c putchar.
 *
 * The "atomic" behavior promised by #cprintf is not guaranteed for @c
 * printf, @c puts, or any other library function that might invoke
 * this putchar implementation.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_PROVIDES_PUTCHAR
#define configBSP430_CONSOLE_PROVIDES_PUTCHAR 0
#endif /* configBSP430_CONSOLE_PROVIDES_PUTCHAR */

/** @def configBSP430_CONSOLE_USE_ONLCR
 *
 * If defined to true, the console display routines will always emit a
 * carriage return before a newline.  This provides compatibility with
 * standard terminal programs like minicom.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_USE_ONLCR
#define configBSP430_CONSOLE_USE_ONLCR 1
#endif /* configBSP430_CONSOLE_USE_ONLCR */

/** @def configBSP430_CONSOLE_LIBC_HAS_VUPRINTF
 *
 * Define to false if your libc does not provide vuprintf.
 * msp430-libc does provide this, and it is used to implement
 * cprintf.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_VUPRINTF
#define configBSP430_CONSOLE_LIBC_HAS_VUPRINTF 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_VUPRINTF */

/** @def configBSP430_CONSOLE_LIBC_HAS_ITOA
 *
 * Define to false if your libc does not provide itoa.  msp430-libc
 * does provide this, and it is used to implement cputi.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_ITOA
#define configBSP430_CONSOLE_LIBC_HAS_ITOA 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_ITOA */

/** @def configBSP430_CONSOLE_LIBC_HAS_UTOA
 *
 * Define to false if your libc does not provide utoa.  msp430-libc
 * does provide this, and it is used to implement cputu.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_UTOA
#define configBSP430_CONSOLE_LIBC_HAS_UTOA 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_UTOA */

/** @def configBSP430_CONSOLE_LIBC_HAS_LTOA
 *
 * Define to false if your libc does not provide ltoa.  msp430-libc
 * does provide this, and it is used to implement cputl.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_LTOA
#define configBSP430_CONSOLE_LIBC_HAS_LTOA 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_LTOA */

/** @def configBSP430_CONSOLE_LIBC_HAS_ULTOA
 *
 * Define to false if your libc does not provide ultoa.  msp430-libc
 * does provide this, and it is used to implement cputul.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_ULTOA
#define configBSP430_CONSOLE_LIBC_HAS_ULTOA 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_ULTOA */

/** Like printf(3), but to the console UART.
 *
 * Interrupts are disabled during the duration of the invocation.  On
 * exit, interruptibility state is restored (if entered with
 * interrupts disabled, they remain disabled).
 *
 * If xBSP430consoleInitialize() has not assigned a UART device, the
 * call is a no-op.
 *
 * @param format A printf(3) format string
 *
 * @return Number of characters printed if the console is enabled; 0
 * if it is disabled; a negative error code if an error is
 * encountered
 *
 * @dependency #BSP430_CONSOLE, #configBSP430_CONSOLE_LIBC_HAS_VUPRINTF */
int cprintf (const char * format, ...)
#if __GNUC__ - 0
__attribute__((__format__(printf, 1, 2)))
#endif /* __GNUC__ */
;

/** Like vprintf(3), but to the console UART.
 *
 * @param format A printf(3) format string
 * @param ap A stdarg reference to variable arguments to a calling function.
 * @return as with cprintf(). */
int vcprintf (const char * format, va_list ap);

/** Like puts(3) to the console UART
 *
 * As with #cprintf, interrupts are disabled for the duration of the
 * invocation.
 *
 * @note Any errors returned by the underlying UART implementation
 * while writing are ignored.
 *
 * @param s a string to be emitted to the console
 *
 * @return the number of characters written, including the newline
 */
int cputs (const char * s);

/** Like putchar(3) to the console UART
 *
 * @param c character to be output
 *
 * @return the character that was output */
int cputchar_ni (int c);

/** Like puts(3) to the console UART without trailing newline
 *
 * @note Any errors returned by the underlying UART implementation
 * while writing are ignored.
 *
 * @param s a string to be emitted to the console
 *
 * @return the number of characters written
 */
int cputtext_ni (const char * s);

/** Format an int using itoa and emit it to the console.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation here assumes that the radix is at least
 * 10.  Passing a smaller radix will likely result in stack
 * corruption.
 *
 * @return the number of characters emitted
 *
 * @dependency #BSP430_CONSOLE, #configBSP430_CONSOLE_LIBC_HAS_ITOA */
int cputi_ni (int n, int radix);

/** Format an int using utoa and emit it to the console.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation here assumes that the radix is at least
 * 10.  Passing a smaller radix will likely result in stack
 * corruption.
 *
 * @return the number of characters emitted
 *
 * @dependency #BSP430_CONSOLE, #configBSP430_CONSOLE_LIBC_HAS_UTOA */
int cputu_ni (unsigned int n, int radix);

/** Format an int using ltoa and emit it to the console.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation here assumes that the radix is at least
 * 10.  Passing a smaller radix will likely result in stack
 * corruption.
 *
 * @return the number of characters emitted
 *
 * @dependency #BSP430_CONSOLE, #configBSP430_CONSOLE_LIBC_HAS_LTOA */
int cputl_ni (long n, int radix);

/** Format an int using itoa and emit it to the console.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting

 * @warning The implementation here assumes that the radix is at least
 * 10.  Passing a smaller radix will likely result in stack
 * corruption.
 *
 * @return the number of characters emitted
 *
 * @dependency #BSP430_CONSOLE, #configBSP430_CONSOLE_LIBC_HAS_ULTOA */
int cputul_ni (unsigned long n, int radix);

/** Initialize and return the console serial HAL instance.
 *
 * This configures the platform-specified serial HAL instance
 * identified by #BSP430_CONSOLE_SERIAL_PERIPH_HANDLE as specified by
 * #BSP430_CONSOLE_BAUD_RATE.  If #BSP430_PLATFORM_SPIN_FOR_JUMPER is
 * true, it will invoke vBSP430platformSpinForJumper_ni().  Once the
 * console is configured and any required delays completed it will
 * return, allowing use of cprintf() and related functions.
 *
 * If this function is invoked multiple times without an intervening
 * call to iBSP430consoleDeconfigure(), the existing configuration is
 * unchanged.
 *
 * @return 0 if the console was successfully initialized, -1 if an
 * error occurred. */
int iBSP430consoleInitialize (void);

/** Deconfigure the console serial HAL instance
 *
 * This routine closes the HAL serial instance, decoupling it from any
 * callbacks and turning it off.  The instance may be re-enabled by
 * re-invoking iBSP430consoleInitialize().
 *
 * You might need this function if you change the rate of the clock on
 * which the console baud generator depends.
 *
 * @return 0 if the deconfiguration was successful, -1 if an error
 * occurred. */
int iBSP430consoleDeconfigure (void);

/** Return a reference to the console device.
 *
 * @return the serial HAL instance used for console interaction.  A
 * null pointer is returned if the console has not been successfully
 * initialized. */
hBSP430halSERIAL hBSP430console (void);

#endif /* BSP430_UTILITY_CONSOLE_H */
