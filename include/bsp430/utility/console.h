/* Copyright 2012-2014, Peter A. Bigot
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
 * @brief A generic console output capability.
 *
 * This module collects various functions that format and generate
 * human-readable output on a serial console, or support reading data
 * from a serial console.
 *
 * @section h_utility_console_interrupts Interrupts and Console Operations
 *
 * In the original design of BSP430 console output was performed with
 * interrupts disabled under the belief that the potential of
 * interleaved output between different tasks and interrupts was
 * undesired.  In practice, interrupt handlers only invoke console
 * output when debugging, and this can be handled other ways.  The
 * impact of extended periods with interrupts disabled was
 * unacceptable in applications that require prompt response to
 * interrupts, such as radio packet reception and acknowledgement
 * transmission.
 *
 * Consequently all console output and input are now non-blocking and
 * disable interrupts only where this is necessary to prevent
 * corruption when both interrupt handlers and user code manipulate
 * the same data structures.
 *
 * The script <tt>${BSP430_ROOT}/maintainer/xni_console.sed</tt> may
 * be used to convert older sources to the new API where @link
 * suffix_ni <tt>_ni</tt>@endlink suffixes are no longer present.
 *
 * @section h_utility_console_capabilities Basic Capabilities
 *
 * Core routines natively supported are cputchar(), cputs(), and
 * cgetchar().  Extensions include cputchars(), cputtext(), and
 * cpeekchar().
 *
 * With library support through #BSP430_CONSOLE_USE_EMBTEXTF or <a
 * href="https://sourceforge.net/projects/mspgcc/files/msp430-libc/">msp430-libc</a>
 * in the MSPGCC toolchain full support for formatted output via
 * cprintf() and vcprintf() is possible.
 *
 * In addition optimized routines are provided to convert integers in
 * standard bases with minimal space overhead (cputi(), cputu(),
 * cputl(), cputul()).  The integer routines are more cumbersome but
 * necessary when the platform cannot accommodate the stack overhead
 * of cprintf() (on the order of 100 bytes if 64-bit integer support
 * is included).
 *
 * Compile-time options #BSP430_CONSOLE_RX_BUFFER_SIZE and
 * #BSP430_CONSOLE_TX_BUFFER_SIZE enable interrupt-driven input and
 * output, allowing the application to make progress while output is
 * produced and input buffered using interrupts.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_CONSOLE_H
#define BSP430_UTILITY_CONSOLE_H

#include <bsp430/serial.h>
#include <stdarg.h>

/** Define to a true value to request that a serial handle be
 * identified to serve as a system console.
 *
 * See #BSP430_CONSOLE_SERIAL_PERIPH_CPPID for information about which
 * serial peripheral will be used for this capability.
 *
 * @cppflag
 * @affects #BSP430_CONSOLE
 * @defaulted
 */
#ifndef configBSP430_CONSOLE
#define configBSP430_CONSOLE 0
#endif /* configBSP430_CONSOLE */

/** Defined to a true value if #BSP430_CONSOLE_SERIAL_PERIPH_CPPID has
 * been provided, making the console infrastructure available.
 *
 * @dependency #configBSP430_CONSOLE
 * @platformvalue
 */
#if defined(BSP430_DOXYGEN)
#define BSP430_CONSOLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** Define to the preprocessor-compatible identifier for the peripheral
 * that should be used by platform-agnostic programs to create the
 * console, for example #BSP430_PERIPH_CPPID_USCI5_A0.
 *
 * The define must appear in the @ref bsp430_config subsystem so that
 * functional resource requests are correctly propagated to the
 * underlying resource instances.  A default is provided based on the
 * platform or available serial peripherals.
 *
 * @dependency #BSP430_CONSOLE
 * @platformvalue
 * @affects #BSP430_CONSOLE_SERIAL_PERIPH_HANDLE */
#if defined(BSP430_DOXYGEN)
#define BSP430_CONSOLE_SERIAL_PERIPH_CPPID include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** The peripheral handle that should be used by platform-agnostic
 * programs to create the console.  This derives directly from
 * #BSP430_CONSOLE_SERIAL_PERIPH_CPPID, but is a serial peripheral
 * handle suitable for use in code.
 *
 * The corresponding HAL and ISR features are automatically enabled
 * for this peripheral.
 *
 * @defaulted
 * @platformvalue
 * @dependency #BSP430_CONSOLE_SERIAL_PERIPH_CPPID */
#if defined(BSP430_DOXYGEN)
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE include <bsp430/platform.h>
#endif /* BSP430_CONSOLE_SERIAL_PERIPH_HANDLE */

/** The baud rate that should be used for the console.  This may be
 * overridden in a platform header or by an application configuration.
 *
 * @defaulted */
#ifndef BSP430_CONSOLE_BAUD_RATE
#define BSP430_CONSOLE_BAUD_RATE 9600
#endif /* BSP430_CONSOLE_BAUD_RATE */

/** Define this to the size of a buffer to be used for interrupt-driven
 * console input.  The value must not exceed 254, and buffer
 * management is most efficient if the value is a power of 2.
 *
 * If this has a value of zero, character input is not interrupt
 * driven.  cgetchar() will return the most recently received
 * character, if any, and cpeekchar() will not be available.
 *
 * @defaulted */
#ifndef BSP430_CONSOLE_RX_BUFFER_SIZE
#define BSP430_CONSOLE_RX_BUFFER_SIZE 0
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

/** Define this to the size of a buffer to be used for interrupt-driven
 * console output.  The value must not exceed 254, and buffer
 * management is most efficient if the value is a power of 2.
 *
 * If this has a value of zero, character output is not interrupt
 * driven.  cputchar() will block until the UART is ready to accept
 * a new character.
 *
 * @warning By enabling interrupt-driven output the console output
 * routines are no longer safe to call from within interrupt handlers.
 * See @ref h_utility_console_interrupts.
 *
 * @defaulted */
#ifndef BSP430_CONSOLE_TX_BUFFER_SIZE
#define BSP430_CONSOLE_TX_BUFFER_SIZE 0
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */

/** Define to indicate build infrastructure support for embtextf
 *
 * This flag should be defined to a true value by the build
 * infrastructure when the external @ref mp_external_embtextf library
 * is to supply enhanced formatting functions.  This enables:
 * @li cprintf(), vcprintf()
 * @li cputi(), cputu(), cputl(), cputul()
 *
 * @note When using the MSPGCC toolchain this capability is available
 * implicitly through msp430-libc.  #BSP430_CONSOLE_USE_EMBTEXTF
 * should be defined only when the external version of these functions
 * is to be used instead.
 */
#ifndef BSP430_CONSOLE_USE_EMBTEXTF
#define BSP430_CONSOLE_USE_EMBTEXTF 0
#endif /* BSP430_CONSOLE_USE_EMBTEXTF */

/** Return a character that was input to the console.
 *
 * @return the next character that was input to the console, or -1 if
 * no characters are available.
 *
 * @note This function is available even if
 * #BSP430_CONSOLE_RX_BUFFER_SIZE is zero.
 */
int cgetchar (void);


#if defined(BSP430_DOXYGEN) || (0 < BSP430_CONSOLE_RX_BUFFER_SIZE)

/** Callback for alarm events.
 *
 * This function will be invoked by the console infrastructure in an
 * interrupt context after received characters have been buffered.
 *
 * @return As with iBSP430halISRCallbackVoid_ni(). */
typedef int (* iBSP430consoleRxCallback_ni) (void);

/** Peek at the next character input to the console
 *
 * Use this to determine whether there's any data ready to be read,
 * without actually consuming it yet.
 *
 * @return the value that would be returned by invoking cgetchar(),
 * but without consuming any pending input.
 *
 * @warning This function is not available if
 * #BSP430_CONSOLE_RX_BUFFER_SIZE is zero.
 *
 * @dependency #BSP430_CONSOLE_RX_BUFFER_SIZE */
int cpeekchar (void);

/** Register a callback for console RX events.
 *
 * If a non-null callback is registered with the console, it will be
 * invoked after each character received on the console is stored in
 * the receive buffer.  The return value of the callback influences
 * how the console RX interrupt handler manages LPM wakeup and other
 * behavior; see @ref callback_retval.
 *
 * If no callback is registered, the infrastructure will act as though
 * a registered callback did nothing but return
 * #BSP430_HAL_ISR_CALLBACK_EXIT_LPM.
 *
 * @note The infrastructure will add
 * #BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN to any value returned by this
 * callback, including the value returned by a null (default)
 * callback.
 *
 * @warning This function is not available if
 * #BSP430_CONSOLE_RX_BUFFER_SIZE is zero.
 *
 * @param cb the callback to be invoked on received characters, or a
 * null pointer to cause the application to wake from LPM when
 * characters are received.
 *
 * @dependency #BSP430_CONSOLE_RX_BUFFER_SIZE
 */
void vBSP430consoleSetRxCallback_ni (iBSP430consoleRxCallback_ni cb);

#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

/** If defined to a true value, the individual character display
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

/** If defined to true, the console display routines will always emit a
 * carriage return before a newline.  This provides compatibility with
 * standard terminal programs like minicom.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_CONSOLE_USE_ONLCR
#define configBSP430_CONSOLE_USE_ONLCR 1
#endif /* configBSP430_CONSOLE_USE_ONLCR */

/** Like puts(3) to the console UART
 *
 * As with #cprintf, interrupts are disabled for the duration of the
 * invocation (except see @ref h_utility_console_interrupts).
 *
 * @note Any errors returned by the underlying UART implementation
 * while writing are ignored.
 *
 * @param s a string to be emitted to the console
 *
 * @return the number of characters written, including the newline
 *
 * @consoleoutput
 */
int cputs (const char * s);

/** Like putchar(3) to the console UART
 *
 * @param c character to be output
 *
 * @return the character that was output
 *
 * @consoleoutput */
int cputchar (int c);

/** Like puts(3) to the console UART without trailing newline
 *
 * @note Any errors returned by the underlying UART implementation
 * while writing are ignored.
 *
 * @param s a NUL-terminated string to be emitted to the console
 *
 * @return the number of characters written
 *
 * @consoleoutput */
int cputtext (const char * s);

/** Like puts(3) to the console UART without trailing newline and
 * with explicit length instead of a terminating NUL.
 *
 * @note Any errors returned by the underlying UART implementation
 * while writing are ignored.
 *
 * @param cp first of a series of characters to be emitted to the
 * console
 *
 * @param len number of characters to emit
 *
 * @return the number of characters written
 *
 * @consoleoutput */
int cputchars (const char * cp, size_t len);

#if defined(BSP430_DOXYGEN) || (BSP430_CONSOLE_USE_EMBTEXTF - 0) || (__MSPGCC__ - 0)

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
 * @dependency #BSP430_CONSOLE, #BSP430_CONSOLE_USE_EMBTEXTF
 *
 * @consoleoutput */
int cprintf (const char * format, ...)
#if (__GNUC__ - 0)
__attribute__((__format__(printf, 1, 2)))
#endif /* __GNUC__ */
;

/** Like vprintf(3), but to the console UART.
 *
 * @param format A printf(3) format string
 * @param ap A stdarg reference to variable arguments to a calling function.
 * @return as with cprintf().
 *
 * @dependency #BSP430_CONSOLE, #BSP430_CONSOLE_USE_EMBTEXTF
 *
 * @consoleoutput */
int vcprintf (const char * format, va_list ap);

/** Format an int using itoa and emit it to the console.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation assumes that @p radix is at least 10.
 * Passing a smaller radix will likely result in stack corruption.
 *
 * @return the number of characters emitted
 *
 * @dependency #BSP430_CONSOLE, #BSP430_CONSOLE_USE_EMBTEXTF */
int cputi (int n, int radix);

/** Format an int using utoa and emit it to the console.
 *
 * @consoleoutput
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation assumes that the radix is at least 10.
 * Passing a smaller radix will likely result in stack corruption.
 *
 * @return the number of characters emitted
 *
 * @dependency #BSP430_CONSOLE, #BSP430_CONSOLE_USE_EMBTEXTF
 *
 * @consoleoutput */
int cputu (unsigned int n, int radix);

/** Format an int using ltoa and emit it to the console.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation assumes that the radix is at least 10.
 * Passing a smaller radix will likely result in stack corruption.
 *
 * @return the number of characters emitted
 *
 * @dependency #BSP430_CONSOLE, #BSP430_CONSOLE_USE_EMBTEXTF
 *
 * @consoleoutput */
int cputl (long n, int radix);

/** Format an int using itoa and emit it to the console.
 *
 * @param n the integer value to be formatted
 * @param radix the radix to use when formatting
 *
 * @warning The implementation assumes that the radix is at least 10.
 * Passing a smaller radix will likely result in stack corruption.
 *
 * @return the number of characters emitted
 *
 * @dependency #BSP430_CONSOLE, #BSP430_CONSOLE_USE_EMBTEXTF
 *
 * @consoleoutput */
int cputul (unsigned long n, int radix);

#endif /* BSP430_CONSOLE_USE_EMBTEXTF */

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
 * @note If #BSP430_SERIAL_ENABLE_RESOURCE is non-zero the resource
 * associated with the console peripheral will be claimed by the
 * console infrastructure and held until iBSP430consoleDeconfigure()
 * is invoked.  Failure to obtain the resource will result in an error
 * during initialization.
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

/** Control whether console output uses interrupt-driven transmission.
 *
 * When #BSP430_CONSOLE_TX_BUFFER_SIZE is configured to a positive
 * value, it is normally improper to use the console output routines
 * from within interrupt handlers and in other cases where interrupts
 * are disabled, since the routines might enable interrupts to allow
 * the transmission buffer to drain.  See @ref
 * h_utility_console_interrupts.  This routine can be used at runtime
 * to disable the interrupt-based transmission, thus allowing use of
 * direct, busy-waiting console output.
 *
 * @note You probably want to invoke vBSP430consoleFlush() prior to
 * disabling interrupt-driven transmission.  If you don't, whatever
 * was unflushed in the buffer will be displayed once the transmission
 * is re-enabled.
 *
 * @param enablep nonzero if interrupt-drive transmission is to be
 * used; zero to disable the transmit interrupt on the console UART
 * and use direct UART writes instead.
 *
 * @return 0 if the configuration was accepted.  -1 if @p enable is
 * nonzero but the application was not configured with
 * interrupt-driven transmission enabled.
 */
int iBSP430consoleTransmitUseInterrupts_ni (int enablep);

/** Potentially block until space is available in console transmit buffer.
 *
 * This function causes the caller to block until the interrupt-driven
 * console transmit buffer has drained to the point where at least @p
 * want_available bytes are available.
 *
 * If the console does not use interrupt-driven transmission, this
 * function immediately returns zero.
 *
 * @note The return value of this function indicates whether it was
 * necessary to enable interrupts in order to achieve the desired
 * available space.  In such a case an application may need to
 * re-check other conditions to ensure there is no pending work prior
 * to entering low power mode.
 *
 * @param want_available the number of bytes that are requested.  A
 * negative number requires that the transmit buffer be empty (i.e.,
 * flushed).
 *
 * @return
 * @li Zero if @p want_available bytes were available on entry without
 * suspending;
 * @li A positive number if the bytes are now available, but the
 * function had to suspend (enabling interrupts) in order to obtain
 * that space;
 * @li -1 if @p want_available is larger than
 * #BSP430_CONSOLE_TX_BUFFER_SIZE-1, which is the maximum number of
 * bytes that can be made available.
 *
 * @consoleoutput */
int iBSP430consoleWaitForTxSpace_ni (int want_available);

/** Flush any pending data in the console transmit buffer.
 *
 * This is simply a wrapper around calls to
 * iBSP430consoleWaitForTxSpace_ni() and vBSP430serialFlush_ni(), with
 * interrupts disabled if required.  On return, all queued output will
 * have been transmitted.
 *
 * If the console does not use interrupt driven transmission, this
 * function will simply spin until the last character has been
 * transmitted by the UART.
 *
 * @return 0 if the console was flushed without suspending; a positive
 * number of the application had to suspend to permit interrupt-driven
 * transmission to complete.
 *
 * @consoleoutput */
int iBSP430consoleFlush (void);

#endif /* BSP430_UTILITY_CONSOLE_H */
