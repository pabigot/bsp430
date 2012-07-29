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
#include <bsp430/periph/euscia_.h>
#include <stddef.h>

/* !BSP430! periph=euscia */
/* !BSP430! instance=EUSCI_A0,EUSCI_A1,EUSCI_A2 */

#define COM_PORT_ACTIVE  0x01

/** Convert from a raw peripheral handle to the corresponding USCI
 * device handle. */
static xBSP430eusciaHandle periphToDevice (xBSP430periphHandle periph);

#define WAKEUP_TRANSMIT_HPL_NI(_hpl) do {       \
    if (! (_hpl->ie & UCTXIE)) {                \
      _hpl->ie |= (_hpl->ifg & UCTXIFG);        \
    }                                           \
  } while (0)

#define RAW_TRANSMIT_HPL_NI(_hpl, _c) do {      \
    while (! (_hpl->ifg & UCTXIFG)) {           \
      ;                                         \
    }                                           \
    _hpl->txbuf = _c;                           \
  } while (0)

#define FLUSH_HPL_NI(_hpl) do {                 \
    while (_hpl->statw & UCBUSY) {              \
      ;                                         \
    }                                           \
  } while (0)

xBSP430eusciaHandle
xBSP430eusciaOpenUART (xBSP430periphHandle periph,
                       unsigned int control_word,
                       unsigned long baud)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  unsigned long brclk_Hz;
  xBSP430eusciaHandle device = periphToDevice(periph);
  unsigned long n;
  uint16_t br;
  uint16_t os16 = 0;
  uint16_t brf = 0;
  uint16_t brs;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  /* Reject invalid baud rates */
  if ((0 == baud) || (1000000UL < baud)) {
    device = NULL;
  }

  /* Reject if the pins can't be configured */
  if ((NULL != device)
      && (0 != iBSP430platformConfigurePeripheralPins_ni((xBSP430periphHandle)(device->euscia), 1))) {
    device = NULL;
  }

  if (NULL != device) {
    /* Assume ACLK <= 20 kHz is VLOCLK and cannot be trusted.  Prefer
     * 32 kiHz ACLK for rates that are low enough.  Use SMCLK for
     * anything larger.  */
    brclk_Hz = usBSP430clockACLK_Hz_ni();
    if ((brclk_Hz > 20000) && (brclk_Hz >= (3 * baud))) {
      device->euscia->ctlw0 = UCSWRST | UCSSEL__ACLK;
    } else {
      device->euscia->ctlw0 = UCSWRST | UCSSEL__SMCLK;
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
    br = n;
    if (16 <= br) {
      br = br / 16;
      os16 = UCOS16;
      brf = n - 16 * br;
    }
    device->euscia->brw = br;
    device->euscia->mctlw = (brf * UCBRF0) | (brs * UCBRS0) | os16;

    /* Mark the device active */
    device->num_rx = device->num_tx = 0;
    device->flags |= COM_PORT_ACTIVE;

    /* Release the USCI and enable the interrupts.  Interrupts are
     * disabled and cleared when UCSWRST is set. */
    device->euscia->ctlw0 &= ~UCSWRST;
    if (device->rx_callback) {
      device->euscia->ie |= UCRXIE;
    }
  }
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return device;
}

int
iBSP430eusciaConfigureCallbacks (xBSP430eusciaHandle device,
                                 const struct xBSP430periphISRCallbackVoid * rx_callback,
                                 const struct xBSP430periphISRCallbackVoid * tx_callback)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc = 0;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  /* Finish any current activity, inhibiting the start of new activity
   * due to !GIE. */
  FLUSH_HPL_NI(device->euscia);
  device->euscia->ctlw0 |= UCSWRST;
  if (device->rx_callback || device->tx_callback) {
    rc = -1;
  } else {
    device->rx_callback = rx_callback;
    device->tx_callback = tx_callback;
  }
  /* Release the USCI and enable the interrupts.  Interrupts are
   * disabled and cleared when UCSWRST is set. */
  device->euscia->ctlw0 &= ~UCSWRST;
  if (device->rx_callback) {
    device->euscia->ie |= UCRXIE;
  }
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

int
iBSP430eusciaClose (xBSP430eusciaHandle device)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  device->euscia->ctlw0 = UCSWRST;
  rc = iBSP430platformConfigurePeripheralPins_ni ((xBSP430periphHandle)(device->euscia), 0);
  device->flags = 0;
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

void
vBSP430eusciaFlush_ni (xBSP430eusciaHandle device)
{
  FLUSH_HPL_NI(device->euscia);
}

void
vBSP430eusciaWakeupTransmit_ni (xBSP430eusciaHandle device)
{
  WAKEUP_TRANSMIT_HPL_NI(device->euscia);
}

int
iBSP430eusciaTransmitByte_ni (xBSP430eusciaHandle device, int c)
{
  if (device->tx_callback) {
    return -1;
  }
  RAW_TRANSMIT_HPL_NI(device->euscia, c);
  return c;
}

int
iBSP430eusciaTransmitData_ni (xBSP430eusciaHandle device,
                              const uint8_t * data,
                              size_t len)
{
  const uint8_t * p = data;
  const uint8_t * edata = data + len;
  if (device->tx_callback) {
    return -1;
  }
  while (p < edata) {
    RAW_TRANSMIT_HPL_NI(device->euscia, *p++);
  }
  return p - data;
}

int
iBSP430eusciaTransmitASCIIZ_ni (xBSP430eusciaHandle device, const char * str)
{
  const char * in_string = str;

  if (device->tx_callback) {
    return -1;
  }
  while (*str) {
    RAW_TRANSMIT_HPL_NI(device->euscia, *str);
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
euscia_isr (xBSP430eusciaHandle device)
{
  int rv = 0;

  switch (device->euscia->iv) {
    default:
    case USCI_NONE:
      break;
    case USCI_UART_UCTXIFG: /* == USCI_SPI_UCTXIFG */
      rv = iBSP430callbackInvokeISRVoid_ni(&device->tx_callback, device, 0);
      if (rv & BSP430_PERIPH_ISR_CALLBACK_BREAK_CHAIN) {
        /* Found some data; send it out */
        ++device->num_tx;
        device->euscia->txbuf = device->tx_byte;
      } else {
        /* No data; mark transmission disabled */
        rv |= BSP430_PERIPH_ISR_CALLBACK_DISABLE_INTERRUPT;
      }
      /* If no more is expected, clear the interrupt but mark that the
       * function is ready so when the interrupt is next set it will
       * fire. */
      if (rv & BSP430_PERIPH_ISR_CALLBACK_DISABLE_INTERRUPT) {
        device->euscia->ie &= ~UCTXIE;
        device->euscia->ifg |= UCTXIFG;
      }
      break;
    case USCI_UART_UCRXIFG: /* == USCI_SPI_UCRXIFG */
      device->rx_byte = device->euscia->rxbuf;
      ++device->num_rx;
      rv = iBSP430callbackInvokeISRVoid_ni(&device->rx_callback, device, 0);
      break;
  }
  return rv;
}
#endif /* EUSCIA ISR */

/* !BSP430! insert=hal_ba_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_ba_defn] */
#if configBSP430_HAL_EUSCI_A0 - 0
static struct xBSP430eusciaState state_EUSCI_A0_ = {
  .euscia = (xBSP430periphEUSCIA *)_BSP430_PERIPH_EUSCI_A0_BASEADDRESS
};

xBSP430eusciaHandle const xBSP430euscia_EUSCI_A0 = &state_EUSCI_A0_;
#endif /* configBSP430_HAL_EUSCI_A0 */

#if configBSP430_HAL_EUSCI_A1 - 0
static struct xBSP430eusciaState state_EUSCI_A1_ = {
  .euscia = (xBSP430periphEUSCIA *)_BSP430_PERIPH_EUSCI_A1_BASEADDRESS
};

xBSP430eusciaHandle const xBSP430euscia_EUSCI_A1 = &state_EUSCI_A1_;
#endif /* configBSP430_HAL_EUSCI_A1 */

#if configBSP430_HAL_EUSCI_A2 - 0
static struct xBSP430eusciaState state_EUSCI_A2_ = {
  .euscia = (xBSP430periphEUSCIA *)_BSP430_PERIPH_EUSCI_A2_BASEADDRESS
};

xBSP430eusciaHandle const xBSP430euscia_EUSCI_A2 = &state_EUSCI_A2_;
#endif /* configBSP430_HAL_EUSCI_A2 */

/* END AUTOMATICALLY GENERATED CODE [hal_ba_defn] */
/* !BSP430! end=hal_ba_defn */

/* !BSP430! uscifrom=eusci insert=hal_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_defn] */
#if configBSP430_HAL_EUSCI_A0_ISR - 0
static void
__attribute__((__interrupt__(USCI_A0_VECTOR)))
isr_EUSCI_A0 (void)
{
  int rv = euscia_isr(xBSP430euscia_EUSCI_A0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* configBSP430_HAL_EUSCI_A0_ISR */

#if configBSP430_HAL_EUSCI_A1_ISR - 0
static void
__attribute__((__interrupt__(USCI_A1_VECTOR)))
isr_EUSCI_A1 (void)
{
  int rv = euscia_isr(xBSP430euscia_EUSCI_A1);
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* configBSP430_HAL_EUSCI_A1_ISR */

#if configBSP430_HAL_EUSCI_A2_ISR - 0
static void
__attribute__((__interrupt__(USCI_A2_VECTOR)))
isr_EUSCI_A2 (void)
{
  int rv = euscia_isr(xBSP430euscia_EUSCI_A2);
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* configBSP430_HAL_EUSCI_A2_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_defn] */
/* !BSP430! end=hal_isr_defn */

static xBSP430eusciaHandle periphToDevice (xBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if configBSP430_HAL_EUSCI_A0 - 0
  if (BSP430_PERIPH_EUSCI_A0 == periph) {
    return xBSP430euscia_EUSCI_A0;
  }
#endif /* configBSP430_PERIPH_EUSCI_A0 */

#if configBSP430_HAL_EUSCI_A1 - 0
  if (BSP430_PERIPH_EUSCI_A1 == periph) {
    return xBSP430euscia_EUSCI_A1;
  }
#endif /* configBSP430_PERIPH_EUSCI_A1 */

#if configBSP430_HAL_EUSCI_A2 - 0
  if (BSP430_PERIPH_EUSCI_A2 == periph) {
    return xBSP430euscia_EUSCI_A2;
  }
#endif /* configBSP430_PERIPH_EUSCI_A2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}
