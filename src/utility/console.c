/* Copyright 2012-2013, Peter A. Bigot
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
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/platform.h>
#include <bsp430/utility/console.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#if BSP430_CONSOLE - 0

/* Inhibit definition if required components were not provided. */

static hBSP430halSERIAL console_hal_;

#if BSP430_CONSOLE_RX_BUFFER_SIZE - 0
#if 254 < (BSP430_CONSOLE_RX_BUFFER_SIZE)
#error BSP430_CONSOLE_RX_BUFFER_SIZE is too large
#endif /* validate BSP430_CONSOLE_RX_BUFFER_SIZE */

typedef struct sConsoleRxBuffer {
  sBSP430halISRVoidChainNode cb_node;
  char buffer[BSP430_CONSOLE_RX_BUFFER_SIZE];
  volatile unsigned char head;
  volatile unsigned char tail;
  iBSP430consoleRxCallback_ni callback;
} sConsoleRxBuffer;

static int
console_rx_isr_ni (const struct sBSP430halISRVoidChainNode * cb,
                   void * context)
{
  sConsoleRxBuffer * bufp = (sConsoleRxBuffer *)cb;
  sBSP430halSERIAL * hal = (sBSP430halSERIAL *) context;
  unsigned char head = bufp->head;
  int rv;

  bufp->buffer[head] = hal->rx_byte;
  head = (head + 1) % (sizeof(bufp->buffer) / sizeof(*bufp->buffer));
  if (head == bufp->tail) {
    bufp->tail = (bufp->tail + 1) % (sizeof(bufp->buffer) / sizeof(*bufp->buffer));
  }
  bufp->head = head;
  if (NULL != bufp->callback) {
    rv = bufp->callback();
  } else {
    rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
  }
  rv |= BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN;
  return rv;
}

static sConsoleRxBuffer rx_buffer_ = {
  .cb_node = { .callback = console_rx_isr_ni },
};

void
vBSP430consoleSetRxCallback_ni (iBSP430consoleRxCallback_ni cb)
{
  rx_buffer_.callback = cb;
}

#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

#if BSP430_CONSOLE_TX_BUFFER_SIZE - 0
#if 254 < (BSP430_CONSOLE_TX_BUFFER_SIZE)
#error BSP430_CONSOLE_TX_BUFFER_SIZE is too large
#endif /* validate BSP430_CONSOLE_TX_BUFFER_SIZE */

typedef struct sConsoleTxBuffer {
  sBSP430halISRVoidChainNode cb_node;
  char buffer[BSP430_CONSOLE_TX_BUFFER_SIZE];
  volatile unsigned char head;
  volatile unsigned char tail;
  volatile int wake_available;
} sConsoleTxBuffer;

/* Calculate the number of bytes available in the buffer given the
 * head and tail indexes. */
#define TX_BUFFER_AVAILABLE_(bp_,h_,t_) (((h_) >= (t_))                 \
                                         ? (sizeof((bp_)->buffer) + (t_) - (h_) - 1) \
                                         : ((t_) - (h_) - 1))

static int
console_tx_isr_ni (const struct sBSP430halISRVoidChainNode * cb,
                   void * context)
{
  sConsoleTxBuffer * bufp = (sConsoleTxBuffer *)cb;
  sBSP430halSERIAL * hal = (sBSP430halSERIAL *) context;
  unsigned char tail = bufp->tail;
  unsigned char head = bufp->head;
  int wake_available;
  int rv = 0;

  /* If there's data available here, store it and mark that we have
   * done so. */
  if (head != tail) {
    hal->tx_byte = bufp->buffer[tail];
    rv |= BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN;
    bufp->tail = tail = (tail + 1) % (sizeof(bufp->buffer) / sizeof(*bufp->buffer));
  }
  wake_available = bufp->wake_available;
  if (head == tail) {
    /* Ran out of data.  Turn off the interrupt infrastructure. */
    rv |= BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
    /* If somebody wants to know when there's space available, well,
     * there's never going to be any more space than the whole
     * buffer. */
    if (0 != wake_available) {
      bufp->wake_available = 0;
      rv |= BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
    }
  } else if (0 < wake_available) {
    int available = TX_BUFFER_AVAILABLE_(bufp, head, tail);

    if (available >= wake_available) {
      bufp->wake_available = 0;
      rv |= BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
    }
  }
  return rv;
}

static sConsoleTxBuffer tx_buffer_ = {
  .cb_node = { .callback = console_tx_isr_ni },
};

int
console_tx_queue_ni (hBSP430halSERIAL uart, uint8_t c)
{
  sConsoleTxBuffer * bufp = &tx_buffer_;

  while (1) {
    unsigned char head = bufp->head;
    unsigned char next_head = (head + 1) % (sizeof(bufp->buffer)/sizeof(*bufp->buffer));
    if (next_head == bufp->tail) {
      if (0 == bufp->wake_available) {
        bufp->wake_available = 1;
      }
      BSP430_CORE_LPM_ENTER_NI(LPM0_bits | GIE);
      BSP430_CORE_DISABLE_INTERRUPT();
      continue;
    }
    bufp->buffer[head] = c;
    bufp->head = next_head;
    if (head == bufp->tail) {
      vBSP430serialWakeupTransmit_ni(uart);
    }
    break;
  }
  return c;
}

static int (* uartTransmit_ni) (hBSP430halSERIAL uart, uint8_t c);

#define UART_TRANSMIT(uart_, c_) uartTransmit_ni(uart_, c_)

#else /* BSP430_CONSOLE_TX_BUFFER_SIZE */

#define UART_TRANSMIT(uart_, c_) iBSP430uartTxByte_ni(uart_, c_)

#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */

/* Optimized version used inline.  Assumes that the uart is not
 * null. */
static
BSP430_CORE_INLINE
int
emit_char2_ni (int c, hBSP430halSERIAL uart)
{
#if configBSP430_CONSOLE_USE_ONLCR - 0
  if ('\n' == c) {
    UART_TRANSMIT(uart, '\r');
  }
#endif /* configBSP430_CONSOLE_USE_ONLCR */
  return UART_TRANSMIT(uart, c);
}

/* Base version used by cprintf.  This has to re-read the console_hal_
 * variable each time. */
static
BSP430_CORE_INLINE
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

/* Emit a NUL-terminated string of text, returning the number of
 * characters emitted. */
static int
emit_chars_ni (const char * cp,
               size_t len,
               hBSP430halSERIAL uart)
{
  int rv = 0;

  if (uart) {
    while (rv < len) {
      emit_char2_ni(cp[rv++], uart);
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
cputtext (const char * cp)
{
  int rv = 0;
  hBSP430halSERIAL uart = console_hal_;
  BSP430_CORE_INTERRUPT_STATE_T istate;

  if (! uart) {
    return 0;
  }
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = emit_text_ni(cp, uart);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

int
cputtext_ni (const char * s)
{
  return emit_text_ni(s, console_hal_);
}

int
cputchars (const char * cp,
           size_t len)
{
  int rv = 0;
  hBSP430halSERIAL uart = console_hal_;
  BSP430_CORE_INTERRUPT_STATE_T istate;

  if (! uart) {
    return 0;
  }
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = emit_chars_ni(cp, len, uart);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

int
cputchars_ni (const char * cp,
              size_t len)
{
  return emit_chars_ni(cp, len, console_hal_);
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
  int rv;
  va_list argp;
  va_start(argp, fmt);
  rv = vcprintf(fmt, argp);
  va_end(argp);
  return rv;
}

int
vcprintf (const char * fmt, va_list ap)
{
  int rv;
  BSP430_CORE_INTERRUPT_STATE_T istate;

  /* Fail fast if printing is disabled */
  if (! console_hal_) {
    return 0;
  }
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = vuprintf(emit_char_ni, fmt, ap);
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
iBSP430consoleTransmitUseInterrupts_ni (int enablep)
{
#if BSP430_CONSOLE_TX_BUFFER_SIZE - 0
  if (enablep) {
    if (uartTransmit_ni != console_tx_queue_ni) {
      uartTransmit_ni = console_tx_queue_ni;
      vBSP430serialFlush_ni(console_hal_);
      iBSP430serialSetHold_ni(console_hal_, 1);
      BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, console_hal_->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
      iBSP430serialSetHold_ni(console_hal_, 0);
      if (tx_buffer_.head != tx_buffer_.tail) {
        vBSP430serialWakeupTransmit_ni(console_hal_);
      }
    }
  } else {
    if (uartTransmit_ni != iBSP430uartTxByte_ni) {
      uartTransmit_ni = iBSP430uartTxByte_ni;
      /* This flushes any character currently in the UART; it does not
       * flush anything left in the transmission buffer. */
      vBSP430serialFlush_ni(console_hal_);
      iBSP430serialSetHold_ni(console_hal_, 1);
      BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, console_hal_->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
      iBSP430serialSetHold_ni(console_hal_, 0);
    }
  }
  return 0;
#else /* BSP430_CONSOLE_TX_BUFFER_SIZE */
  return enablep ? -1 : 0;
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */
}

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
    BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, hal->rx_cbchain_ni, rx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

#if BSP430_CONSOLE_TX_BUFFER_SIZE - 0
    uartTransmit_ni = console_tx_queue_ni;
    tx_buffer_.wake_available = 0;
    tx_buffer_.head = tx_buffer_.tail = 0;
    BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, hal->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */

    /* Attempt to configure and install the console */
    console_hal_ = hBSP430serialOpenUART(hal, 0, 0, BSP430_CONSOLE_BAUD_RATE);
    if (! console_hal_) {
      /* Open failed, revert the callback association. */
#if BSP430_CONSOLE_RX_BUFFER_SIZE - 0
      BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, hal->rx_cbchain_ni, rx_buffer_.cb_node, next_ni);
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
  BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, console_hal_->rx_cbchain_ni, rx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */
#if BSP430_CONSOLE_TX_BUFFER_SIZE - 0
  BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, console_hal_->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */
  console_hal_ = NULL;
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

int
iBSP430consoleWaitForTxSpace_ni (int want_available)
{
  int rv = 0;
#if BSP430_CONSOLE_TX_BUFFER_SIZE - 0
  if (((int)sizeof(tx_buffer_.buffer) - 1) < want_available) {
    return -1;
  }
  while (1) {
    int available;
    unsigned char head = tx_buffer_.head;
    unsigned char tail = tx_buffer_.tail;

    if (0 > want_available) {
      if (head == tail) {
        break;
      }
    } else {
      available = TX_BUFFER_AVAILABLE_(&tx_buffer_, head, tail);
      if (available >= want_available) {
        break;
      }
    }
    if (0 == tx_buffer_.wake_available) {
      tx_buffer_.wake_available = want_available;
    }
    rv = 1;
    BSP430_CORE_LPM_ENTER_NI(LPM0_bits | GIE);
    BSP430_CORE_DISABLE_INTERRUPT();
  }
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */
  return rv;
}

int
iBSP430consoleFlush (void)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rv;

  if (! console_hal_) {
    return 0;
  }
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = iBSP430consoleWaitForTxSpace_ni(-1);
  vBSP430serialFlush_ni(console_hal_);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

#endif /* BSP430_CONSOLE */
