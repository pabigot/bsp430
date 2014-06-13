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
 * @brief Implementation for the BSP430 console facility
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/platform.h>
#include <bsp430/utility/console.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>

#if (BSP430_CONSOLE - 0)
/* Inhibit definition if required components were not provided. */

#if (BSP430_CONSOLE_USE_EMBTEXTF - 0)
#define HAVE_EMBTEXTF 1
#include <embtextf/uprintf.h>
#include <embtextf/xtoa.h>

#define itoa embtextf_itoa
#define utoa embtextf_utoa
#define ltoa embtextf_ltoa
#define ultoa embtextf_ultoa
#define vuprintf embtextf_vuprintf
#elif (BSP430_CORE_TOOLCHAIN_LIBC_MSP430_LIBC - 0)
/* msp430-libc natively incorporates the same interfaces provided by embtextf. */
#define HAVE_EMBTEXTF 1
#endif /* BSP430_CONSOLE_USE_EMBTEXTF */

static hBSP430halSERIAL console_hal_;

#if (BSP430_CONSOLE_RX_BUFFER_SIZE - 0)
#if 254 < (BSP430_CONSOLE_RX_BUFFER_SIZE)
#error BSP430_CONSOLE_RX_BUFFER_SIZE is too large
#endif /* validate BSP430_CONSOLE_RX_BUFFER_SIZE */

typedef struct sConsoleRxBuffer {
  sBSP430halISRVoidChainNode cb_node;
  char buffer[BSP430_CONSOLE_RX_BUFFER_SIZE];
  volatile unsigned char head;
  volatile unsigned char tail;
  iBSP430consoleRxCallback_ni callback_ni;
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
  if (NULL != bufp->callback_ni) {
    rv = bufp->callback_ni();
  } else {
    rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
  }
  rv |= BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN;
  return rv;
}

static sConsoleRxBuffer rx_buffer_ = {
  .cb_node = { .callback_ni = console_rx_isr_ni },
};

void
vBSP430consoleSetRxCallback_ni (iBSP430consoleRxCallback_ni cb)
{
  rx_buffer_.callback_ni = cb;
}

#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

#if (BSP430_CONSOLE_TX_BUFFER_SIZE - 0)
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
  .cb_node = { .callback_ni = console_tx_isr_ni },
};

static int
console_tx_queue (hBSP430halSERIAL uart, uint8_t c)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  sConsoleTxBuffer * bufp = &tx_buffer_;

  BSP430_CORE_DISABLE_INTERRUPT();
  while (1) {
    unsigned char head = bufp->head;
    unsigned char next_head = (head + 1) % (sizeof(bufp->buffer)/sizeof(*bufp->buffer));
    if (next_head == bufp->tail) {
      if (0 == bufp->wake_available) {
        bufp->wake_available = 1;
      }
      /* Sleep until the tx callback indicates space is available or
       * something else wakes us up.  Then immediately disable the
       * interrupts as that probably was not done during wakeup. */
      BSP430_CORE_LPM_ENTER_NI(LPM0_bits);
      BSP430_CORE_DISABLE_INTERRUPT();
      continue;
    }
    bufp->buffer[head] = c;
    bufp->head = next_head;
    if (head == bufp->tail) {
      vBSP430serialWakeupTransmit_rh(uart);
    }
    break;
  }
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return c;
}

static int (* uartTransmit) (hBSP430halSERIAL uart, uint8_t c);

#define UART_TRANSMIT(uart_, c_) uartTransmit(uart_, c_)

#else /* BSP430_CONSOLE_TX_BUFFER_SIZE */

#define UART_TRANSMIT(uart_, c_) iBSP430uartTxByte_rh(uart_, c_)

#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */

/* Optimized version used inline.  Assumes that the uart is not
 * null. */
static BSP430_CORE_INLINE
int
emit_char2 (int c, hBSP430halSERIAL uart)
{
#if (configBSP430_CONSOLE_USE_ONLCR - 0)
  if ('\n' == c) {
    UART_TRANSMIT(uart, '\r');
  }
#endif /* configBSP430_CONSOLE_USE_ONLCR */
  return UART_TRANSMIT(uart, c);
}

/* Base version used by cprintf.  This has to re-read the console_hal_
 * variable each time. */
static BSP430_CORE_INLINE
int
emit_char (int c)
{
  hBSP430halSERIAL uart = console_hal_;
  if (NULL == uart) {
    return -1;
  }
  return emit_char2(c, uart);
}

#if (configBSP430_CONSOLE_PROVIDES_PUTCHAR - 0)
int putchar (c)
{
  return emit_char(c);
}
#endif /* configBSP430_CONSOLE_PROVIDES_PUTCHAR */

int
cputchar (int c)
{
  return emit_char(c);
}

/* Emit a NUL-terminated string of text, returning the number of
 * characters emitted. */
static int
emit_text (const char * s,
           hBSP430halSERIAL uart)
{
  int rv = 0;
  if (uart) {
    while (s[rv]) {
      emit_char2(s[rv++], uart);
      BSP430_CORE_WATCHDOG_CLEAR();
    }
  }
  return rv;
}

/* Emit a NUL-terminated string of text, returning the number of
 * characters emitted. */
static int
emit_chars (const char * cp,
            size_t len,
            hBSP430halSERIAL uart)
{
  int rv = 0;

  if (uart) {
    while (rv < len) {
      emit_char2(cp[rv++], uart);
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

  if (! uart) {
    return 0;
  }
  rv = emit_text(s, uart);
  emit_char2('\n', uart);
  return 1+rv;
}

int
cputtext (const char * cp)
{
  hBSP430halSERIAL uart = console_hal_;

  if (! uart) {
    return 0;
  }
  return emit_text(cp, uart);
}

int
cputchars (const char * cp,
           size_t len)
{
  hBSP430halSERIAL uart = console_hal_;

  if (! uart) {
    return 0;
  }
  return emit_chars(cp, len, uart);
}

#if HAVE_EMBTEXTF
int
cputi (int n, int radix)
{
  char buffer[sizeof("-32767")];
  return emit_text(itoa(n, buffer, radix), console_hal_);
}

int
cputu (unsigned int n, int radix)
{
  char buffer[sizeof("65535")];
  return emit_text(utoa(n, buffer, radix), console_hal_);
}

int
cputl (long n, int radix)
{
  char buffer[sizeof("-2147483647")];
  return emit_text(ltoa(n, buffer, radix), console_hal_);
}

int
cputul (unsigned long n, int radix)
{
  char buffer[sizeof("4294967295")];
  return emit_text(ultoa(n, buffer, radix), console_hal_);
}

int
vcprintf (const char * fmt, va_list ap)
{
  /* Fail fast if printing is disabled */
  if (! console_hal_) {
    return 0;
  }
  return vuprintf(emit_char, fmt, ap);
}

#elif (BSP430_CORE_TOOLCHAIN_LIBC_NEWLIB - 0)

int
vcprintf (const char * fmt, va_list ap)
{
  /* Fail fast if printing is disabled */
  if (! console_hal_) {
    return 0;
  }
  /* Delegate to newlib standard output */
  return vprintf(fmt, ap);
}

#endif /* HAVE_EMBTEXTF */

#if ((BSP430_CONSOLE_USE_EMBTEXTF - 0)                  \
     || (BSP430_CORE_TOOLCHAIN_LIBC_MSP430_LIBC - 0)    \
     || (BSP430_CORE_TOOLCHAIN_LIBC_NEWLIB - 0))

int
#if (__GNUC__ - 0)
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

#endif /* cprintf */

hBSP430halSERIAL
hBSP430console (void)
{
  return console_hal_;
}

#if (BSP430_CONSOLE_RX_BUFFER_SIZE - 0)

static int
console_getchar_ (int do_pop)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv = -1;

  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    if (rx_buffer_.head != rx_buffer_.tail) {
      rv = rx_buffer_.buffer[rx_buffer_.tail];
      if (do_pop) {
        rx_buffer_.tail = (rx_buffer_.tail + 1) & ((sizeof(rx_buffer_.buffer) / sizeof(*rx_buffer_.buffer)) - 1);
      }
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

int
cpeekchar (void)
{
  return console_getchar_(0);
}

int
cgetchar (void)
{
  return console_getchar_(1);
}

#else /* BSP430_CONSOLE_RX_BUFFER_SIZE */

int
cgetchar (void)
{
  int rv = -1;
  if (NULL != console_hal_) {
    rv = iBSP430uartRxByte_rh(console_hal_);
  }
  return rv;
}

#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

int
iBSP430consoleTransmitUseInterrupts_ni (int enablep)
{
#if (BSP430_CONSOLE_TX_BUFFER_SIZE - 0)
  if (enablep) {
    if (uartTransmit != console_tx_queue) {
      uartTransmit = console_tx_queue;
      vBSP430serialFlush_ni(console_hal_);
      iBSP430serialSetHold_rh(console_hal_, 1);
      BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, console_hal_->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
      iBSP430serialSetHold_rh(console_hal_, 0);
      if (tx_buffer_.head != tx_buffer_.tail) {
        vBSP430serialWakeupTransmit_rh(console_hal_);
      }
    }
  } else {
    if (uartTransmit != iBSP430uartTxByte_rh) {
      uartTransmit = iBSP430uartTxByte_rh;
      /* This flushes any character currently in the UART; it does not
       * flush anything left in the transmission buffer. */
      vBSP430serialFlush_ni(console_hal_);
      iBSP430serialSetHold_rh(console_hal_, 1);
      BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, console_hal_->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
      iBSP430serialSetHold_rh(console_hal_, 0);
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
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
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
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
#if (BSP430_CONSOLE_RX_BUFFER_SIZE - 0)
    /* Associate the callback before opening the device, so the
     * interrupts are enabled properly. */
    rx_buffer_.head = rx_buffer_.tail = 0;
    BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, hal->rx_cbchain_ni, rx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

#if (BSP430_CONSOLE_TX_BUFFER_SIZE - 0)
    uartTransmit = console_tx_queue;
    tx_buffer_.wake_available = 0;
    tx_buffer_.head = tx_buffer_.tail = 0;
    BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode, hal->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */

    /* Attempt to configure and install the console */
    console_hal_ = hBSP430serialOpenUART(hal, 0, 0, BSP430_CONSOLE_BAUD_RATE);
#if (BSP430_SERIAL_ENABLE_RESOURCE - 0)
    /* Fail if found peripheral but could not get control of its
     * resource */
    if ((NULL != console_hal_)
        && (0 != iBSP430resourceClaim_ni(&console_hal_->resource, &console_hal_, eBSP430resourceWait_NONE, NULL))) {
      console_hal_ = NULL;
    }
#endif /* BSP430_SERIAL_ENABLE_RESOURCE */

    if (! console_hal_) {
      /* Open failed, revert the callback association. */
#if (BSP430_CONSOLE_RX_BUFFER_SIZE - 0)
      BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, hal->rx_cbchain_ni, rx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */
      break;
    }
#if (BSP430_PLATFORM_SPIN_FOR_JUMPER - 0)
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
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv = 0;

  if (! console_hal_) {
    return rv;
  }
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = iBSP430serialClose(console_hal_);
#if (BSP430_CONSOLE_RX_BUFFER_SIZE - 0)
    BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, console_hal_->rx_cbchain_ni, rx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */
#if (BSP430_CONSOLE_TX_BUFFER_SIZE - 0)
    BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode, console_hal_->tx_cbchain_ni, tx_buffer_.cb_node, next_ni);
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */
#if (BSP430_SERIAL_ENABLE_RESOURCE - 0)
    (void)iBSP430resourceRelease_ni(&console_hal_->resource, &console_hal_);
#endif /* BSP430_SERIAL_ENABLE_RESOURCE */
    console_hal_ = NULL;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

int
iBSP430consoleWaitForTxSpace_ni (int want_available)
{
  int rv = 0;
#if (BSP430_CONSOLE_TX_BUFFER_SIZE - 0)
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
    /* Sleep until the tx callback indicates space is available or
     * something else wakes us up.  Then immediately disable the
     * interrupts as that probably was not done during wakeup. */
    BSP430_CORE_LPM_ENTER_NI(LPM0_bits);
    BSP430_CORE_DISABLE_INTERRUPT();
  }
#endif /* BSP430_CONSOLE_TX_BUFFER_SIZE */
  return rv;
}

int
iBSP430consoleFlush (void)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  if (! console_hal_) {
    return 0;
  }
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = iBSP430consoleWaitForTxSpace_ni(-1);
    vBSP430serialFlush_ni(console_hal_);
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

void
vBSP430consoleDisplayOctets (const uint8_t * dp,
                             size_t len)
{
  const uint8_t * const edp = dp + len;
  while (dp < edp) {
    cprintf("%02x", *dp++);
    if (dp < edp) {
      cputchar(' ');
    }
  }
}

void
vBSP430consoleDisplayMemory (const uint8_t * dp,
                             size_t len,
                             unsigned long base)
{
  const uint8_t * const edp = dp + len;
  const uint8_t * adp = dp;

  while (dp < edp) {
    if (0 == (base & 0x0F)) {
      if (adp < dp) {
        cputtext("  ");
        while (adp < dp) {
          cputchar(isprint(*adp) ? *adp : '.');
          ++adp;
        }
      }
      adp = dp;
      cprintf("\n%08lx ", base);
    } else if (0 == (base & 0x07)) {
      cputchar(' ');
    }
    cprintf(" %02x", *dp++);
    ++base;
  }
  if (adp < dp) {
    while (base & 0x0F) {
      if (0 == (base & 0x07)) {
        cputchar(' ');
      }
      cprintf("   ");
      ++base;
    }
    cputtext("  ");
    while (adp < dp) {
      cputchar(isprint(*adp) ? *adp : '.');
      ++adp;
    }
  }
  cputchar('\n');
}

#endif /* BSP430_CONSOLE */
