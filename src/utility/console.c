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
 * @brief Implementation for the BSP430 console facility
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
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
    iBSP430uartTxByte_ni(uart, '\r');
  }
#endif /* configBSP430_CONSOLE_USE_ONLCR */
  return iBSP430uartTxByte_ni(uart, c);
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

#if BSP430_CONSOLE_RX_BUFFER_SIZE - 0
#if ((BSP430_CONSOLE_RX_BUFFER_SIZE) & ((BSP430_CONSOLE_RX_BUFFER_SIZE) - 1))
#error BSP430_CONSOLE_RX_BUFFER_SIZE is not a power of two
#endif /* validate BSP430_CONSOLE_RX_BUFFER_SIZE */
#if 128 < (BSP430_CONSOLE_RX_BUFFER_SIZE)
#error BSP430_CONSOLE_RX_BUFFER_SIZE is too large
#endif /* validate BSP430_CONSOLE_RX_BUFFER_SIZE */

typedef struct sConsoleRxBuffer {
  sBSP430halISRVoidChainNode cb_node;
  char buffer[BSP430_CONSOLE_RX_BUFFER_SIZE];
  unsigned char head;
  unsigned char tail;
} sConsoleRxBuffer;

static int
console_rx_isr_ni_ (const struct sBSP430halISRVoidChainNode * cb,
                    void * context)
{
  sConsoleRxBuffer * bufp = (sConsoleRxBuffer *)cb;
  sBSP430halSERIAL * hal = (sBSP430halSERIAL *) context;

  bufp->buffer[bufp->head] = hal->rx_byte;
  bufp->head = (bufp->head + 1) & ((sizeof(bufp->buffer) / sizeof(*bufp->buffer)) - 1);
  if (bufp->head == bufp->tail) {
    bufp->tail = (bufp->tail + 1) & ((sizeof(bufp->buffer) / sizeof(*bufp->buffer)) - 1);
  }
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

static sConsoleRxBuffer rx_buffer_ = {
  .cb_node = { .callback = console_rx_isr_ni },
};

static int
console_getchar_ (int do_pop)
{
  int rv = -1;
  if (rx_buffer_.head != rx_buffer_.tail) {
    rv = rx_buffer_.buffer[rx_buffer_.tail];
    if (do_pop) {
      rx_buffer_.tail = (rx_buffer_.tail + 1) & ((sizeof(rx_buffer_.buffer) / sizeof(*rx_buffer_.buffer)) - 1);
    }
  }
  return rv;
}

int
cpeekchar_ni (void)
{
  return console_getchar_(0);
}

int
cgetchar_ni (void)
{
  return console_getchar_(1);
}

#else /* BSP430_CONSOLE_RX_BUFFER_SIZE */

int
cgetchar_ni (void)
{
  int rv = -1;
  if (NULL != console_hal_) {
    rv = iBSP430uartRxByte_ni(console_hal_);
  }
  return rv;
}

#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

int
iBSP430consoleInitialize (void)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rv;
  hBSP430halSERIAL hal;

  if (console_hal_) {
    return 0;
  }

  hal = hBSP430serialLookup(BSP430_CONSOLE_SERIAL_PERIPH_HANDLE);
  if (NULL == hal) {
    return -1;
  }

  rv = -1;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {

#if BSP430_CONSOLE_RX_BUFFER_SIZE - 0
    /* Associate the callback before opening the device, so the
     * interrupts are enabled properly. */
    rx_buffer_.head = rx_buffer_.tail = 0;
    rx_buffer_.cb_node.next_ni = hal->rx_cbchain_ni;
    hal->rx_cbchain_ni = &rx_buffer_.cb_node;
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

    /* Attempt to configure and install the console */
    console_hal_ = hBSP430serialOpenUART(hal, 0, 0, BSP430_CONSOLE_BAUD_RATE);
    if (! console_hal_) {
      /* Open failed, revert the callback association. */
#if BSP430_CONSOLE_RX_BUFFER_SIZE - 0
      hal->rx_cbchain_ni = rx_buffer_.cb_node.next_ni;
      rx_buffer_.cb_node.next_ni = 0;
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */
      break;
    }
#if BSP430_PLATFORM_SPIN_FOR_JUMPER - 0
    vBSP430platformSpinForJumper_ni();
#endif /* BSP430_PLATFORM_SPIN_FOR_JUMPER */
    /* All is good */
    rv = 0;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

int
iBSP430consoleDeconfigure (void)
{
  int rv = 0;
  BSP430_CORE_INTERRUPT_STATE_T istate;

  if (! console_hal_) {
    return rv;
  }
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430serialClose(console_hal_);
#if BSP430_CONSOLE_RX_BUFFER_SIZE - 0
  console_hal_->rx_cbchain_ni = rx_buffer_.cb_node.next_ni;
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */
  console_hal_ = NULL;
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

#endif /* BSP430_CONSOLE */
