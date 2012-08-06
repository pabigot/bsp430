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

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/periph/eusci.h>

#define SERIAL_HAL_HPL_A(_hal) (_hal)->hpl.euscia
#define SERIAL_HAL_HPL_B(_hal) (_hal)->hpl.euscib
#define HAL_HPL_FIELD(_hal,_fld) (*(BSP430_SERIAL_HAL_HPL_VARIANT_IS_EUSCIB(_hal) ? &(_hal)->hpl.euscib->_fld : &(_hal)->hpl.euscia->_fld))

#define SERIAL_HAL_WAKEUP_TRANSMIT_NI(_hal) do {                        \
    if (! (HAL_HPL_FIELD(_hal,ie) & UCTXIE)) {                          \
      HAL_HPL_FIELD(_hal,ie) |= (HAL_HPL_FIELD(_hal,ifg) & UCTXIFG);    \
    }                                                                   \
  } while (0)

#define SERIAL_HAL_RAW_TRANSMIT_NI(_hal, _c) do {       \
    while (! (HAL_HPL_FIELD(_hal,ifg) & UCTXIFG)) {     \
      ;                                                 \
    }                                                   \
    HAL_HPL_FIELD(_hal,txbuf) = _c;                     \
  } while (0)

#define SERIAL_HAL_FLUSH_NI(_hal) do {                  \
    while (HAL_HPL_FIELD(_hal,statw) & UCBUSY) {        \
      ;                                                 \
    }                                                   \
  } while (0)

#define SERIAL_HAL_RESET_NI(_hal) do {          \
    HAL_HPL_FIELD(_hal,ctlw0) = UCSWRST;        \
  } while (0)

#define SERIAL_HAL_HOLD_NI(_hal) do {       \
    HAL_HPL_FIELD(_hal,ctlw0) |= UCSWRST;       \
  } while (0)

#define SERIAL_HAL_RELEASE_NI(_hal) do {        \
    HAL_HPL_FIELD(_hal,ctlw0) &= ~UCSWRST;      \
    if ((_hal)->rx_callback) {                  \
      HAL_HPL_FIELD(_hal,ie) |= UCRXIE;         \
    }                                           \
  } while (0)

static
hBSP430halSERIAL
eusciaConfigure (hBSP430halSERIAL hal,
                 unsigned int ctlw0,
                 unsigned int brw,
                 unsigned int mctlw,
                 int set_mctl)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  HAL_HPL_FIELD(hal,ctlw0) = UCSWRST;
  do {
    /* Reject if the pins can't be configured */
    if ((NULL != hal)
        && (0 != iBSP430platformConfigurePeripheralPins_ni(xBSP430periphFromHPL(hal->hpl.any), 1))) {
      hal = NULL;
      break;
    }
    HAL_HPL_FIELD(hal, ctlw0) |= ctlw0;
    HAL_HPL_FIELD(hal, brw) = brw;
    if (set_mctl) {
      SERIAL_HAL_HPL_A(hal)->mctlw = mctlw;
    }

    /* Reset device statistics */
    hal->num_rx = hal->num_tx = 0;

    /* Release the USCI5 and enable the interrupts.  Interrupts are
     * disabled and cleared when UCSWRST is set. */
    SERIAL_HAL_RELEASE_NI(hal);
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return hal;
}

hBSP430halSERIAL
xBSP430eusciaOpenUART (hBSP430halSERIAL hal,
                       unsigned char ctl0_byte,
                       unsigned char ctl1_byte,
                       unsigned long baud)
{
  unsigned long brclk_Hz;
  unsigned long n;
  unsigned int ctlw0;
  unsigned int brw;
  unsigned int mctlw;
  unsigned int os16 = 0;
  unsigned int brf = 0;
  unsigned int brs;

  /* Reject unsupported HALs */
  if (NULL == hal) {
    return NULL;
  }

  /* Reject invalid baud rates */
  if ((0 == baud) || (BSP430_EUSCIA_UART_MAX_BAUD < baud)) {
    return NULL;
  }

  /* Force to UART async and wipe out the clock select fields */
  ctlw0 = (ctl0_byte << 8) | ctl1_byte;
  ctlw0 &= ~(UCMODE1 | UCMODE0 | UCSYNC | UCSSEL1 | UCSSEL0);

  /* Assume ACLK <= 20 kHz is VLOCLK and cannot be trusted.  Prefer
   * 32 kiHz ACLK for rates that are low enough.  Use SMCLK for
   * anything larger.  */
  brclk_Hz = usBSP430clockACLK_Hz_ni();
  if ((brclk_Hz > 20000) && (brclk_Hz >= (3 * baud))) {
    ctlw0 |= UCSSEL__ACLK;
  } else {
    ctlw0 |= UCSSEL__SMCLK;
    brclk_Hz = ulBSP430clockSMCLK_Hz_ni();
  }

#define BR_FRACTION_SHIFT 6
  /* The value for BRS is supposed to be a table lookup based on the
   * fractional part of f_brclk / baud.  Rather than replicate the
   * table, we simply preserve BR_FRACTION_SHIFT bits of the
   * fraction, then use that as the upper bits of the value of
   * BRS.  Seems to work, at least for 9600 baud. */
  n = (brclk_Hz << BR_FRACTION_SHIFT) / baud;
  brs = n & ((1 << BR_FRACTION_SHIFT) - 1);
  n >>= BR_FRACTION_SHIFT;
  brs <<= 8 - BR_FRACTION_SHIFT;
#undef BR_FRACTION_SHIFT
  brw = n;
  if (16 <= brw) {
    brw = brw / 16;
    os16 = UCOS16;
    brf = n - 16 * brw;
  }
  mctlw = (brf * UCBRF0) | (brs * UCBRS0) | os16;

  return eusciaConfigure(hal, ctlw0, brw, mctlw, 1);
}

int
iBSP430eusciaConfigureCallbacks (hBSP430halSERIAL hal,
                                 const struct sBSP430halISRCallbackVoid * rx_callback,
                                 const struct sBSP430halISRCallbackVoid * tx_callback)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc = 0;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  /* Finish any current activity, inhibiting the start of new activity
   * due to !GIE. */
  SERIAL_HAL_FLUSH_NI(hal);
  SERIAL_HAL_HOLD_NI(hal);
  if (hal->rx_callback || hal->tx_callback) {
    rc = -1;
  } else {
    hal->rx_callback = rx_callback;
    hal->tx_callback = tx_callback;
  }
  /* Release the EUSCIA and enable the interrupts.  Interrupts are
   * disabled and cleared when UCSWRST is set. */
  SERIAL_HAL_RELEASE_NI(hal);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

int
iBSP430eusciaClose (hBSP430halSERIAL hal)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  SERIAL_HAL_RESET_NI(hal);
  rc = iBSP430platformConfigurePeripheralPins_ni (xBSP430periphFromHPL(hal->hpl.any), 0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

void
vBSP430eusciaFlush_ni (hBSP430halSERIAL hal)
{
  SERIAL_HAL_FLUSH_NI(hal);
}

void
vBSP430eusciaWakeupTransmit_ni (hBSP430halSERIAL hal)
{
  SERIAL_HAL_WAKEUP_TRANSMIT_NI(hal);
}

int
iBSP430eusciaUARTrxByte_ni (hBSP430halSERIAL hal)
{
  if (hal->rx_callback) {
    return -1;
  }
  if (SERIAL_HAL_HPL_A(hal)->ifg & UCRXIFG) {
    return SERIAL_HAL_HPL_A(hal)->rxbuf;
  }
  return -1;
}

int
iBSP430eusciaUARTtxByte_ni (hBSP430halSERIAL hal, uint8_t c)
{
  if (hal->tx_callback) {
    return -1;
  }
  SERIAL_HAL_RAW_TRANSMIT_NI(hal, c);
  return c;
}

int
iBSP430eusciaUARTtxData_ni (hBSP430halSERIAL hal,
                            const uint8_t * data,
                            size_t len)
{
  const uint8_t * p = data;
  const uint8_t * edata = data + len;
  if (hal->tx_callback) {
    return -1;
  }
  while (p < edata) {
    SERIAL_HAL_RAW_TRANSMIT_NI(hal, *p++);
  }
  return p - data;
}

int
iBSP430eusciaUARTtxASCIIZ_ni (hBSP430halSERIAL hal, const char * str)
{
  const char * in_string = str;

  if (hal->tx_callback) {
    return -1;
  }
  while (*str) {
    SERIAL_HAL_RAW_TRANSMIT_NI(hal, *str);
    ++str;
  }
  return str - in_string;
}

/* Since the interrupt code is the same for all peripherals, on MCUs
 * with multiple USCI devices it is more space efficient to share it.
 * This does add an extra call/return for some minor cost in stack
 * space.
 *
 * Making the implementation function __c16__ ensures it's legitimate
 * to use portYIELD_FROM_ISR().
 *
 * Adding __always_inline__ supports maintainability by having a
 * single implementation but speed by forcing the implementation into
 * each handler.  It's a lot cleaner than defining the body as a
 * macro.  GCC will normally inline the code if there's only one call
 * point; there should be a configPORT_foo option to do so in other
 * cases. */
#if ((configBSP430_HAL_EUSCI_A0_ISR - 0)        \
     || (configBSP430_HAL_EUSCI_A1_ISR - 0)     \
     || (configBSP430_HAL_EUSCI_A2_ISR - 0))
static int
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
euscia_isr (hBSP430halSERIAL hal)
{
  int rv = 0;

  switch (SERIAL_HAL_HPL_A(hal)->iv) {
    default:
    case USCI_NONE:
      break;
    case USCI_UART_UCTXIFG: /* == USCI_SPI_UCTXIFG */
      rv = iBSP430callbackInvokeISRVoid_ni(&hal->tx_callback, hal, 0);
      if (rv & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN) {
        /* Found some data; send it out */
        ++hal->num_tx;
        SERIAL_HAL_HPL_A(hal)->txbuf = hal->tx_byte;
      } else {
        /* No data; mark transmission disabled */
        rv |= BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
      }
      /* If no more is expected, clear the interrupt but mark that the
       * function is ready so when the interrupt is next set it will
       * fire. */
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        SERIAL_HAL_HPL_A(hal)->ie &= ~UCTXIE;
        SERIAL_HAL_HPL_A(hal)->ifg |= UCTXIFG;
      }
      break;
    case USCI_UART_UCRXIFG: /* == USCI_SPI_UCRXIFG */
      hal->rx_byte = SERIAL_HAL_HPL_A(hal)->rxbuf;
      ++hal->num_rx;
      rv = iBSP430callbackInvokeISRVoid_ni(&hal->rx_callback, hal, 0);
      break;
  }
  return rv;
}
#endif /* EUSCIA ISR */

static struct sBSP430serialDispatch dispatch_ = {
  .openUART = xBSP430eusciaOpenUART,
  //  .openSPI = xBSP430eusciaOpenSPI,
  .configureCallbacks = iBSP430eusciaConfigureCallbacks,
  .close = iBSP430eusciaClose,
  .wakeupTransmit_ni = vBSP430eusciaWakeupTransmit_ni,
  .flush_ni = vBSP430eusciaFlush_ni,
  .uartRxByte_ni = iBSP430eusciaUARTrxByte_ni,
  .uartTxByte_ni = iBSP430eusciaUARTtxByte_ni,
  .uartTxData_ni = iBSP430eusciaUARTtxData_ni,
  .uartTxASCIIZ_ni = iBSP430eusciaUARTtxASCIIZ_ni
};

/* !BSP430! periph=euscia instance=EUSCI_A0,EUSCI_A1,EUSCI_A2 insert=hal_serial_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_serial_defn] */
#if configBSP430_HAL_EUSCI_A0 - 0
struct sBSP430halSERIAL xBSP430hal_EUSCI_A0_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIA
#if configBSP430_HAL_EUSCI_A0_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_EUSCI_A0_ISR */
  },
  .hpl = { .euscia = BSP430_HPL_EUSCI_A0 },
  .dispatch = &dispatch_
};
#endif /* configBSP430_HAL_EUSCI_A0 */

#if configBSP430_HAL_EUSCI_A1 - 0
struct sBSP430halSERIAL xBSP430hal_EUSCI_A1_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIA
#if configBSP430_HAL_EUSCI_A1_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_EUSCI_A1_ISR */
  },
  .hpl = { .euscia = BSP430_HPL_EUSCI_A1 },
  .dispatch = &dispatch_
};
#endif /* configBSP430_HAL_EUSCI_A1 */

#if configBSP430_HAL_EUSCI_A2 - 0
struct sBSP430halSERIAL xBSP430hal_EUSCI_A2_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIA
#if configBSP430_HAL_EUSCI_A2_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_EUSCI_A2_ISR */
  },
  .hpl = { .euscia = BSP430_HPL_EUSCI_A2 },
  .dispatch = &dispatch_
};
#endif /* configBSP430_HAL_EUSCI_A2 */

/* END AUTOMATICALLY GENERATED CODE [hal_serial_defn] */
/* !BSP430! end=hal_serial_defn */
/* !BSP430! uscifrom=eusci insert=hal_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_defn] */
#if configBSP430_HAL_EUSCI_A0_ISR - 0
static void
__attribute__((__interrupt__(USCI_A0_VECTOR)))
isr_EUSCI_A0 (void)
{
  int rv = euscia_isr(BSP430_HAL_EUSCI_A0);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_EUSCI_A0_ISR */

#if configBSP430_HAL_EUSCI_A1_ISR - 0
static void
__attribute__((__interrupt__(USCI_A1_VECTOR)))
isr_EUSCI_A1 (void)
{
  int rv = euscia_isr(BSP430_HAL_EUSCI_A1);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_EUSCI_A1_ISR */

#if configBSP430_HAL_EUSCI_A2_ISR - 0
static void
__attribute__((__interrupt__(USCI_A2_VECTOR)))
isr_EUSCI_A2 (void)
{
  int rv = euscia_isr(BSP430_HAL_EUSCI_A2);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_EUSCI_A2_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_defn] */
/* !BSP430! end=hal_isr_defn */

volatile sBSP430hplEUSCIA *
xBSP430hplLookupEUSCIA (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_demux] */
#if configBSP430_HPL_EUSCI_A0 - 0
  if (BSP430_PERIPH_EUSCI_A0 == periph) {
    return BSP430_HPL_EUSCI_A0;
  }
#endif /* configBSP430_HPL_EUSCI_A0 */

#if configBSP430_HPL_EUSCI_A1 - 0
  if (BSP430_PERIPH_EUSCI_A1 == periph) {
    return BSP430_HPL_EUSCI_A1;
  }
#endif /* configBSP430_HPL_EUSCI_A1 */

#if configBSP430_HPL_EUSCI_A2 - 0
  if (BSP430_PERIPH_EUSCI_A2 == periph) {
    return BSP430_HPL_EUSCI_A2;
  }
#endif /* configBSP430_HPL_EUSCI_A2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_demux] */
  /* !BSP430! end=periph_hpl_demux */
  return NULL;
}

/* !BSP430! periph=euscib instance=EUSCI_B0,EUSCI_B1,EUSCI_B2 insert=hal_serial_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_serial_defn] */
#if configBSP430_HAL_EUSCI_B0 - 0
struct sBSP430halSERIAL xBSP430hal_EUSCI_B0_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIB
#if configBSP430_HAL_EUSCI_B0_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_EUSCI_B0_ISR */
  },
  .hpl = { .euscib = BSP430_HPL_EUSCI_B0 },
  .dispatch = &dispatch_
};
#endif /* configBSP430_HAL_EUSCI_B0 */

#if configBSP430_HAL_EUSCI_B1 - 0
struct sBSP430halSERIAL xBSP430hal_EUSCI_B1_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIB
#if configBSP430_HAL_EUSCI_B1_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_EUSCI_B1_ISR */
  },
  .hpl = { .euscib = BSP430_HPL_EUSCI_B1 },
  .dispatch = &dispatch_
};
#endif /* configBSP430_HAL_EUSCI_B1 */

#if configBSP430_HAL_EUSCI_B2 - 0
struct sBSP430halSERIAL xBSP430hal_EUSCI_B2_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_EUSCIB
#if configBSP430_HAL_EUSCI_B2_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_EUSCI_B2_ISR */
  },
  .hpl = { .euscib = BSP430_HPL_EUSCI_B2 },
  .dispatch = &dispatch_
};
#endif /* configBSP430_HAL_EUSCI_B2 */

/* END AUTOMATICALLY GENERATED CODE [hal_serial_defn] */
/* !BSP430! end=hal_serial_defn */
/* !BSP430! uscifrom=eusci insert=hal_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_defn] */
#if configBSP430_HAL_EUSCI_B0_ISR - 0
static void
__attribute__((__interrupt__(USCI_B0_VECTOR)))
isr_EUSCI_B0 (void)
{
  int rv = euscib_isr(BSP430_HAL_EUSCI_B0);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_EUSCI_B0_ISR */

#if configBSP430_HAL_EUSCI_B1_ISR - 0
static void
__attribute__((__interrupt__(USCI_B1_VECTOR)))
isr_EUSCI_B1 (void)
{
  int rv = euscib_isr(BSP430_HAL_EUSCI_B1);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_EUSCI_B1_ISR */

#if configBSP430_HAL_EUSCI_B2_ISR - 0
static void
__attribute__((__interrupt__(USCI_B2_VECTOR)))
isr_EUSCI_B2 (void)
{
  int rv = euscib_isr(BSP430_HAL_EUSCI_B2);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_EUSCI_B2_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_defn] */
/* !BSP430! end=hal_isr_defn */

volatile sBSP430hplEUSCIB *
xBSP430hplLookupEUSCIB (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_demux] */
#if configBSP430_HPL_EUSCI_B0 - 0
  if (BSP430_PERIPH_EUSCI_B0 == periph) {
    return BSP430_HPL_EUSCI_B0;
  }
#endif /* configBSP430_HPL_EUSCI_B0 */

#if configBSP430_HPL_EUSCI_B1 - 0
  if (BSP430_PERIPH_EUSCI_B1 == periph) {
    return BSP430_HPL_EUSCI_B1;
  }
#endif /* configBSP430_HPL_EUSCI_B1 */

#if configBSP430_HPL_EUSCI_B2 - 0
  if (BSP430_PERIPH_EUSCI_B2 == periph) {
    return BSP430_HPL_EUSCI_B2;
  }
#endif /* configBSP430_HPL_EUSCI_B2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_demux] */
  /* !BSP430! end=periph_hpl_demux */
  return NULL;
}

/* !BSP430! periph=eusci instance=EUSCI_A0,EUSCI_A1,EUSCI_A2,EUSCI_B0,EUSCI_B1,EUSCI_B2 */

hBSP430halSERIAL
hBSP430eusciLookup (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if configBSP430_HAL_EUSCI_A0 - 0
  if (BSP430_PERIPH_EUSCI_A0 == periph) {
    return BSP430_HAL_EUSCI_A0;
  }
#endif /* configBSP430_HAL_EUSCI_A0 */

#if configBSP430_HAL_EUSCI_A1 - 0
  if (BSP430_PERIPH_EUSCI_A1 == periph) {
    return BSP430_HAL_EUSCI_A1;
  }
#endif /* configBSP430_HAL_EUSCI_A1 */

#if configBSP430_HAL_EUSCI_A2 - 0
  if (BSP430_PERIPH_EUSCI_A2 == periph) {
    return BSP430_HAL_EUSCI_A2;
  }
#endif /* configBSP430_HAL_EUSCI_A2 */

#if configBSP430_HAL_EUSCI_B0 - 0
  if (BSP430_PERIPH_EUSCI_B0 == periph) {
    return BSP430_HAL_EUSCI_B0;
  }
#endif /* configBSP430_HAL_EUSCI_B0 */

#if configBSP430_HAL_EUSCI_B1 - 0
  if (BSP430_PERIPH_EUSCI_B1 == periph) {
    return BSP430_HAL_EUSCI_B1;
  }
#endif /* configBSP430_HAL_EUSCI_B1 */

#if configBSP430_HAL_EUSCI_B2 - 0
  if (BSP430_PERIPH_EUSCI_B2 == periph) {
    return BSP430_HAL_EUSCI_B2;
  }
#endif /* configBSP430_HAL_EUSCI_B2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}

const char *
xBSP430eusciName (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_name_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_name_demux] */
#if configBSP430_HPL_EUSCI_A0 - 0
  if (BSP430_PERIPH_EUSCI_A0 == periph) {
    return "EUSCI_A0";
  }
#endif /* configBSP430_HPL_EUSCI_A0 */

#if configBSP430_HPL_EUSCI_A1 - 0
  if (BSP430_PERIPH_EUSCI_A1 == periph) {
    return "EUSCI_A1";
  }
#endif /* configBSP430_HPL_EUSCI_A1 */

#if configBSP430_HPL_EUSCI_A2 - 0
  if (BSP430_PERIPH_EUSCI_A2 == periph) {
    return "EUSCI_A2";
  }
#endif /* configBSP430_HPL_EUSCI_A2 */

#if configBSP430_HPL_EUSCI_B0 - 0
  if (BSP430_PERIPH_EUSCI_B0 == periph) {
    return "EUSCI_B0";
  }
#endif /* configBSP430_HPL_EUSCI_B0 */

#if configBSP430_HPL_EUSCI_B1 - 0
  if (BSP430_PERIPH_EUSCI_B1 == periph) {
    return "EUSCI_B1";
  }
#endif /* configBSP430_HPL_EUSCI_B1 */

#if configBSP430_HPL_EUSCI_B2 - 0
  if (BSP430_PERIPH_EUSCI_B2 == periph) {
    return "EUSCI_B2";
  }
#endif /* configBSP430_HPL_EUSCI_B2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_name_demux] */
  /* !BSP430! end=periph_name_demux */
  return NULL;
}
