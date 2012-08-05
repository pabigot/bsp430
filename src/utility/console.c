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
 * @brief Implementation for the BSP430 console facility
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright @link http://www.opensource.org/licenses/BSD-3-Clause BSD 3-Clause @endlink
 */

#include <bsp430/utility/console.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <bsp430/platform.h>

#if BSP430_CONSOLE - 0

/* Inhibit definition if required components were not provided. */

static hBSP430halSERIAL console_hal_;

/* Optimized version used inline.  Assumes that the uart is not
 * null. */
static
__inline__
int
emit_char2_ni (int c, hBSP430halSERIAL uart)
{
#if configBSP430_CONSOLE_USE_ONLCR - 0
  if ('\n' == c) {
    iBSP430serialTransmitByte_ni(uart, '\r');
  }
#endif /* configBSP430_CONSOLE_USE_ONLCR */
  return iBSP430serialTransmitByte_ni(uart, c);
}

/* Base version used by cprintf.  This has to re-read the console_hal_
 * variable each time. */
static
__inline__
int
emit_char_ni (int c)
{
  hBSP430halSERIAL uart = console_hal_;
  if (NULL == uart) {
    return -1;
  }
  return emit_char2_ni(c, uart);
}

#if configBSP430_CONSOLE_PROVIDES_PUTCHAR - 0
int putchar (c)
{
  return emit_char_ni(c);
}
#endif /* configBSP430_CONSOLE_PROVIDES_PUTCHAR */

int
cputchar_ni (int c)
{
  return emit_char_ni(c);
}

/* Emit a NUL-terminated string of text, returning the number of
 * characters emitted. */
static int
emit_text_ni (const char * s,
              hBSP430halSERIAL uart)
{
  int rv = 0;
  if (uart) {
    while (s[rv]) {
      emit_char2_ni(s[rv++], uart);
      BSP430_CORE_WATCHDOG_CLEAR();
    }
  }
  return rv;
}

int
cputs (const char * s)
{
  int rv = 0;
  hBSP430halSERIAL uart = console_hal_;
  BSP430_CORE_INTERRUPT_STATE_T istate;

  if (! uart) {
    return 0;
  }
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = emit_text_ni(s, uart);
  emit_char2_ni('\n', uart);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return 1+rv;
}

int
cputtext_ni (const char * s)
{
  return emit_text_ni(s, console_hal_);
}

#if configBSP430_CONSOLE_LIBC_HAS_ITOA - 0
int
cputi_ni (int n, int radix)
{
  char buffer[sizeof("-32767")];
  return emit_text_ni(itoa(n, buffer, radix), console_hal_);
}
#endif /* configBSP430_CONSOLE_LIBC_HAS_ITOA */

#if configBSP430_CONSOLE_LIBC_HAS_UTOA - 0
int
cputu_ni (unsigned int n, int radix)
{
  char buffer[sizeof("65535")];
  return emit_text_ni(utoa(n, buffer, radix), console_hal_);
}
#endif /* configBSP430_CONSOLE_LIBC_HAS_UTOA */

#if configBSP430_CONSOLE_LIBC_HAS_LTOA - 0
int
cputl_ni (long n, int radix)
{
  char buffer[sizeof("-2147483647")];
  return emit_text_ni(ltoa(n, buffer, radix), console_hal_);
}
#endif /* configBSP430_CONSOLE_LIBC_HAS_LTOA */

#if configBSP430_CONSOLE_LIBC_HAS_ULTOA - 0
int
cputul_ni (unsigned long n, int radix)
{
  char buffer[sizeof("4294967295")];
  return emit_text_ni(ultoa(n, buffer, radix), console_hal_);
}
#endif /* configBSP430_CONSOLE_LIBC_HAS_ULTOA */

#if configBSP430_CONSOLE_LIBC_HAS_VUPRINTF - 0
int
#if __GNUC__ - 0
__attribute__((__format__(printf, 1, 2)))
#endif /* __GNUC__ */
cprintf (const char *fmt, ...)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  va_list argp;
  int rv;

  /* Fail fast if printing is disabled */
  if (! console_hal_) {
    return 0;
  }
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  va_start (argp, fmt);
  rv = vuprintf(emit_char_ni, fmt, argp);
  va_end (argp);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}
#endif /* configBSP430_CONSOLE_LIBC_HAS_VUPRINTF */

hBSP430halSERIAL
hBSP430console (void)
{
  return console_hal_;
}

int
iBSP430consoleInitialize (void)
{
  if (console_hal_) {
    return 0;
  }
  do {
    hBSP430halSERIAL hal = hBSP430serialLookup(BSP430_CONSOLE_SERIAL_PERIPH_HANDLE);
    if (NULL == hal) {
      break;
    }
    hal = hBSP430serialOpenUART(hal, 0, BSP430_CONSOLE_BAUD_RATE);
    if (NULL == hal) {
      break;
    }
#if BSP430_PLATFORM_SPIN_FOR_JUMPER - 0
    BSP430_CORE_INTERRUPT_STATE_T istate;
    BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
    BSP430_CORE_DISABLE_INTERRUPT();
    vBSP430platformSpinForJumper_ni();
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
#endif /* BSP430_PLATFORM_SPIN_FOR_JUMPER */
    console_hal_ = hal;
    return 0;
  } while (0);
  return -1;
}

#endif /* BSP430_CONSOLE */
