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
 * @brief A generic console print capability.
 *
 * #cprintf is like @c printf.  It disables interrupts while operating
 * to ensure that interleaved messages do not occur.  It is "safe" for
 * call from within interrupt handlers.
 *
 * A companion #cputs is provided where the complexity of printf is
 * not required but atomic output is desired.
 *
 * Other routines permit display of plain text, single characters, and
 * integers without incurring the stack overhead of printf, which can
 * be quite high (on the order of 100 bytes if 64-bit integer support
 * is included).  These assume that interrupts are disabled when
 * called.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_CONSOLE_H
#define BSP430_UTILITY_CONSOLE_H

#include <bsp430/uart.h>

/** @def BSP430_CONSOLE_UART_PERIPH_HANDLE
 *
 * The HPL peripheral handle that should be used by platform-agnostic
 * programs to create the console.  Suggested possibilities are
 * #BSP430_PERIPH_USCI_A0, #BSP430_PERIPH_USCI5_A1 (sic), and
 * #BSP430_PERIPH_EUSCI_A0.  A potential default is often made
 * available by including <bsp430/platform/bsp430_config.h>.  If you
 * provide one yourself, remember to enable the HAL for the
 * peripheral.
 *
 * If you want to disable the console for an application but are
 * getting a default assigned anyway, define this to
 * #BSP430_PERIPH_NONE.
 *
 * @nodefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_CONSOLE_UART_PERIPH_HANDLE no default value
#endif /* BSP430_DOXYGEN */

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
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_VUPRINTF
#define configBSP430_CONSOLE_LIBC_HAS_VUPRINTF 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_VUPRINTF */

/** @def configBSP430_CONSOLE_LIBC_HAS_ITOA
 *
 * Define to false if your libc does not provide itoa.  msp430-libc
 * does provide this, and it is used to implement cputi.
 *
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_ITOA
#define configBSP430_CONSOLE_LIBC_HAS_ITOA 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_ITOA */

/** @def configBSP430_CONSOLE_LIBC_HAS_UTOA
 *
 * Define to false if your libc does not provide utoa.  msp430-libc
 * does provide this, and it is used to implement cputu.
 *
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_UTOA
#define configBSP430_CONSOLE_LIBC_HAS_UTOA 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_UTOA */

/** @def configBSP430_CONSOLE_LIBC_HAS_LTOA
 *
 * Define to false if your libc does not provide ltoa.  msp430-libc
 * does provide this, and it is used to implement cputl.
 *
 * @defaulted */
#ifndef configBSP430_CONSOLE_LIBC_HAS_LTOA
#define configBSP430_CONSOLE_LIBC_HAS_LTOA 1
#endif /* configBSP430_CONSOLE_LIBC_HAS_LTOA */

/** @def configBSP430_CONSOLE_LIBC_HAS_ULTOA
 *
 * Define to false if your libc does not provide ultoa.  msp430-libc
 * does provide this, and it is used to implement cputul.
 *
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
 * If #xBSP430consoleConfigure has not assigned a UART device, the
 * call is a no-op.
 *
 * @param format A printf(3) format string
 *
 * @return Number of characters printed if the console is enabled; 0
 * if it is disabled; a negative error code if an error is
 * encountered */
int
cprintf (const char * format, ...)
#if __GNUC__ - 0
__attribute__((__format__(printf, 1, 2)))
#endif /* __GNUC__ */
;

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
 * @note This function is unimplemented if
 * #configBSP430_CONSOLE_LIBC_HAS_ITOA is false.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation here assumes that the radix is at least
 * 10.  Passing a smaller radix will likely result in stack
 * corruption.
 *
 * @return the number of characters emitted */
int cputi_ni (int n, int radix);

/** Format an int using utoa and emit it to the console.
 *
 * @note This function is unimplemented if
 * #configBSP430_CONSOLE_LIBC_HAS_UTOA is false.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation here assumes that the radix is at least
 * 10.  Passing a smaller radix will likely result in stack
 * corruption.
 *
 * @return the number of characters emitted */
int cputu_ni (unsigned int n, int radix);

/** Format an int using ltoa and emit it to the console.
 *
 * @note This function is unimplemented if
 * #configBSP430_CONSOLE_LIBC_HAS_LTOA is false.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation here assumes that the radix is at least
 * 10.  Passing a smaller radix will likely result in stack
 * corruption.
 *
 * @return the number of characters emitted */
int cputl_ni (long n, int radix);

/** Format an int using itoa and emit it to the console.
 *
 * @note This function is unimplemented if
 * #configBSP430_CONSOLE_LIBC_HAS_ULTOA is false.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting

 * @warning The implementation here assumes that the radix is at least
 * 10.  Passing a smaller radix will likely result in stack
 * corruption.
 *
 * @return the number of characters emitted */
int cputul_ni (unsigned long n, int radix);


/** Configure a console device.
 *
 * @note Any errors returned by the underlying UART implementation
 * while writing are ignored.
 *
 * @note Although the interface permits changing the UART associated
 * with the console, nothing is done to prevent catastrophe if the
 * previous device is being actively used by a function in this
 * module.
 *
 * @param uart a serial device to be used for #cprintf.  Pass a null
 * handle to disable the console print infrastructure.  Attempts to
 * re-initialize with a new device do not produce a diagnostic; the
 * new device is used in subsequent operations.
 *
 * @return 1 if a valid console was passed; 0 if the console facility
 * was disabled by passing a null handle; -1 if an error occurred */
int iBSP430consoleConfigure (xBSP430uartHandle uart);

#endif /* BSP430_UTILITY_CONSOLE_H */
