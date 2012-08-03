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
#include <bsp430/periph/usci5.h>

/* !BSP430! periph=usci5 */
/* !BSP430! instance=USCI5_A0,USCI5_A1,USCI5_A2,USCI5_A3,USCI5_B0,USCI5_B1,USCI5_B2,USCI5_B3 */

/** Convert from a raw peripheral handle to the corresponding USCI5
 * device handle. */
static hBSP430halUSCI5 periphToDevice (tBSP430periphHandle periph);

#define SERIAL_HAL_HPL(_hal) ((_hal)->usci5)

#define SERIAL_HPL_WAKEUP_TRANSMIT_NI(_hpl) do {        \
    if (! (_hpl->ie & UCTXIE)) {                        \
      _hpl->ie |= (_hpl->ifg & UCTXIFG);                \
    }                                                   \
  } while (0)

#define SERIAL_HPL_RAW_TRANSMIT_NI(_hpl, _c) do {       \
    while (! (_hpl->ifg & UCTXIFG)) {                   \
      ;                                                 \
    }                                                   \
    _hpl->txbuf = _c;                                   \
  } while (0)

#define SERIAL_HPL_FLUSH_NI(_hpl) do {          \
    while (_hpl->stat & UCBUSY) {               \
      ;                                         \
    }                                           \
  } while (0)

#define SERIAL_HPL_RESET_NI(_hpl) do {          \
    (_hpl)->ctlw0 = UCSWRST;                    \
  } while (0)

#define SERIAL_HPL_SET_CLKSOURCE(_hpl, _cssel) do {     \
    (_hpl)->ctlw0 |= (_cssel);                          \
  } while (0)

#define SERIAL_HPL_HOLD_HPL_NI(_hpl) do {       \
    (_hpl)->ctlw0 |= UCSWRST;                   \
  } while (0)

#define SERIAL_HPL_RELEASE_HPL_NI(_hal,_hpl) do {       \
    (_hpl)->ctlw0 &= ~UCSWRST;                          \
    if ((_hal)->rx_callback) {                          \
      (_hpl)->ie |= UCRXIE;                             \
    }                                                   \
  } while (0)

static void
hplSetBaudRate (volatile struct sBSP430hplUSCI5 * hpl,
                unsigned long baud,
                unsigned long brclk_Hz)
{
  uint16_t br = (brclk_Hz / baud);
  uint16_t brs = (1 + 16 * (brclk_Hz - baud * br) / baud) / 2;

  hpl->brw = br;
  hpl->mctl = (0 * UCBRF0) | (brs * UCBRS0);
}

hBSP430halUSCI5
xBSP430usci5OpenUART (tBSP430periphHandle periph,
                      unsigned int control_word,
                      unsigned long baud)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  unsigned long brclk_Hz;
  hBSP430halUSCI5 hal = periphToDevice(periph);

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  /* Reject invalid baud rates */
  if ((0 == baud) || (1000000UL < baud)) {
    hal = NULL;
  }

  /* Reject if the pins can't be configured */
  if ((NULL != hal)
      && (0 != iBSP430platformConfigurePeripheralPins_ni((tBSP430periphHandle)(uintptr_t)(SERIAL_HAL_HPL(hal)), 1))) {
    hal = NULL;
  }

  if (NULL != hal) {
    /* Assume ACLK <= 20 kHz is VLOCLK and cannot be trusted.  Prefer
     * 32 kiHz ACLK for rates that are low enough.  Use SMCLK for
     * anything larger.  */
    brclk_Hz = usBSP430clockACLK_Hz_ni();
    SERIAL_HPL_RESET_NI(SERIAL_HAL_HPL(hal));
    if ((brclk_Hz > 20000) && (brclk_Hz >= (3 * baud))) {
      SERIAL_HPL_SET_CLKSOURCE(SERIAL_HAL_HPL(hal), UCSSEL__ACLK);
    } else {
      SERIAL_HPL_SET_CLKSOURCE(SERIAL_HAL_HPL(hal), UCSSEL__SMCLK);
      brclk_Hz = ulBSP430clockSMCLK_Hz_ni();
    }
    hplSetBaudRate(SERIAL_HAL_HPL(hal), baud, brclk_Hz);

    /* Reset device statistics */
    hal->num_rx = hal->num_tx = 0;

    /* Release the USCI5 and enable the interrupts.  Interrupts are
     * disabled and cleared when UCSWRST is set. */
    SERIAL_HPL_RELEASE_HPL_NI(hal, SERIAL_HAL_HPL(hal));
  }
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return hal;
}

int
iBSP430usci5ConfigureCallbacks (hBSP430halUSCI5 hal,
                                const struct sBSP430halISRCallbackVoid * rx_callback,
                                const struct sBSP430halISRCallbackVoid * tx_callback)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc = 0;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  /* Finish any current activity, inhibiting the start of new activity
   * due to !GIE. */
  SERIAL_HPL_FLUSH_NI(SERIAL_HAL_HPL(hal));
  SERIAL_HPL_HOLD_HPL_NI(SERIAL_HAL_HPL(hal));
  if (hal->rx_callback || hal->tx_callback) {
    rc = -1;
  } else {
    hal->rx_callback = rx_callback;
    hal->tx_callback = tx_callback;
  }
  /* Release the USCI5 and enable the interrupts.  Interrupts are
   * disabled and cleared when UCSWRST is set. */
  SERIAL_HPL_RELEASE_HPL_NI(hal, SERIAL_HAL_HPL(hal));
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

int
iBSP430usci5Close (hBSP430halUSCI5 hal)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  SERIAL_HPL_FLUSH_NI(SERIAL_HAL_HPL(hal));
  SERIAL_HPL_RESET_NI(SERIAL_HAL_HPL(hal));
  rc = iBSP430platformConfigurePeripheralPins_ni ((tBSP430periphHandle)(uintptr_t)(SERIAL_HAL_HPL(hal)), 0);
  hal->flags = 0;
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return rc;
}

void
vBSP430usci5Flush_ni (hBSP430halUSCI5 hal)
{
  SERIAL_HPL_FLUSH_NI(SERIAL_HAL_HPL(hal));
}

void
vBSP430usci5WakeupTransmit_ni (hBSP430halUSCI5 hal)
{
  SERIAL_HPL_WAKEUP_TRANSMIT_NI(SERIAL_HAL_HPL(hal));
}

int
iBSP430usci5TransmitByte_ni (hBSP430halUSCI5 hal, int c)
{
  if (hal->tx_callback) {
    return -1;
  }
  SERIAL_HPL_RAW_TRANSMIT_NI(SERIAL_HAL_HPL(hal), c);
  return c;
}

int
iBSP430usci5TransmitData_ni (hBSP430halUSCI5 hal,
                             const uint8_t * data,
                             size_t len)
{
  const uint8_t * p = data;
  const uint8_t * edata = data + len;
  if (hal->tx_callback) {
    return -1;
  }
  while (p < edata) {
    SERIAL_HPL_RAW_TRANSMIT_NI(SERIAL_HAL_HPL(hal), *p++);
  }
  return p - data;
}

int
iBSP430usci5TransmitASCIIZ_ni (hBSP430halUSCI5 hal, const char * str)
{
  const char * in_string = str;

  if (hal->tx_callback) {
    return -1;
  }
  while (*str) {
    SERIAL_HPL_RAW_TRANSMIT_NI(SERIAL_HAL_HPL(hal), *str);
    ++str;
  }
  return str - in_string;
}

/* Since the interrupt code is the same for all peripherals, on MCUs
 * with multiple USCI5 devices it is more space efficient to share it.
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
#if ((configBSP430_HAL_USCI5_A0_ISR - 0)        \
     || (configBSP430_HAL_USCI5_A1_ISR - 0)     \
     || (configBSP430_HAL_USCI5_A2_ISR - 0)     \
     || (configBSP430_HAL_USCI5_A3_ISR - 0)     \
     || (configBSP430_HAL_USCI5_B0_ISR - 0)     \
     || (configBSP430_HAL_USCI5_B1_ISR - 0)     \
     || (configBSP430_HAL_USCI5_B2_ISR - 0)     \
     || (configBSP430_HAL_USCI5_B3_ISR - 0)     \
     )
static int
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usci5_isr (hBSP430halUSCI5 device)
{
  int rv = 0;

  switch (device->usci5->iv) {
    default:
    case USCI_NONE:
      break;
    case USCI_UCTXIFG:
      rv = iBSP430callbackInvokeISRVoid_ni(&device->tx_callback, device, 0);
      if (rv & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN) {
        /* Found some data; send it out */
        ++device->num_tx;
        device->usci5->txbuf = device->tx_byte;
      } else {
        /* No data; disable transmission interrupt */
        rv |= BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
      }
      /* If no more is expected, clear the interrupt but mark that the
       * function is ready so when the interrupt is next set it will
       * fire. */
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        device->usci5->ie &= ~UCTXIE;
        device->usci5->ifg |= UCTXIFG;
      }
      break;
    case USCI_UCRXIFG:
      device->rx_byte = device->usci5->rxbuf;
      ++device->num_rx;
      rv = iBSP430callbackInvokeISRVoid_ni(&device->rx_callback, device, 0);
      break;
  }
  return rv;
}
#endif  /* HAL ISR */

/* !BSP430! insert=hal_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_defn] */
#if configBSP430_HAL_USCI5_A0 - 0
sBSP430halUSCI5 xBSP430hal_USCI5_A0_ = {
  .usci5 = BSP430_HPL_USCI5_A0
};
#endif /* configBSP430_HAL_USCI5_A0 */

#if configBSP430_HAL_USCI5_A1 - 0
sBSP430halUSCI5 xBSP430hal_USCI5_A1_ = {
  .usci5 = BSP430_HPL_USCI5_A1
};
#endif /* configBSP430_HAL_USCI5_A1 */

#if configBSP430_HAL_USCI5_A2 - 0
sBSP430halUSCI5 xBSP430hal_USCI5_A2_ = {
  .usci5 = BSP430_HPL_USCI5_A2
};
#endif /* configBSP430_HAL_USCI5_A2 */

#if configBSP430_HAL_USCI5_A3 - 0
sBSP430halUSCI5 xBSP430hal_USCI5_A3_ = {
  .usci5 = BSP430_HPL_USCI5_A3
};
#endif /* configBSP430_HAL_USCI5_A3 */

#if configBSP430_HAL_USCI5_B0 - 0
sBSP430halUSCI5 xBSP430hal_USCI5_B0_ = {
  .usci5 = BSP430_HPL_USCI5_B0
};
#endif /* configBSP430_HAL_USCI5_B0 */

#if configBSP430_HAL_USCI5_B1 - 0
sBSP430halUSCI5 xBSP430hal_USCI5_B1_ = {
  .usci5 = BSP430_HPL_USCI5_B1
};
#endif /* configBSP430_HAL_USCI5_B1 */

#if configBSP430_HAL_USCI5_B2 - 0
sBSP430halUSCI5 xBSP430hal_USCI5_B2_ = {
  .usci5 = BSP430_HPL_USCI5_B2
};
#endif /* configBSP430_HAL_USCI5_B2 */

#if configBSP430_HAL_USCI5_B3 - 0
sBSP430halUSCI5 xBSP430hal_USCI5_B3_ = {
  .usci5 = BSP430_HPL_USCI5_B3
};
#endif /* configBSP430_HAL_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [hal_defn] */
/* !BSP430! end=hal_defn */

/* !BSP430! uscifrom=usci5 insert=hal_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_defn] */
#if configBSP430_HAL_USCI5_A0_ISR - 0
static void
__attribute__((__interrupt__(USCI_A0_VECTOR)))
isr_USCI5_A0 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_A0);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_A0_ISR */

#if configBSP430_HAL_USCI5_A1_ISR - 0
static void
__attribute__((__interrupt__(USCI_A1_VECTOR)))
isr_USCI5_A1 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_A1);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_A1_ISR */

#if configBSP430_HAL_USCI5_A2_ISR - 0
static void
__attribute__((__interrupt__(USCI_A2_VECTOR)))
isr_USCI5_A2 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_A2);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_A2_ISR */

#if configBSP430_HAL_USCI5_A3_ISR - 0
static void
__attribute__((__interrupt__(USCI_A3_VECTOR)))
isr_USCI5_A3 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_A3);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_A3_ISR */

#if configBSP430_HAL_USCI5_B0_ISR - 0
static void
__attribute__((__interrupt__(USCI_B0_VECTOR)))
isr_USCI5_B0 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_B0);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_B0_ISR */

#if configBSP430_HAL_USCI5_B1_ISR - 0
static void
__attribute__((__interrupt__(USCI_B1_VECTOR)))
isr_USCI5_B1 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_B1);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_B1_ISR */

#if configBSP430_HAL_USCI5_B2_ISR - 0
static void
__attribute__((__interrupt__(USCI_B2_VECTOR)))
isr_USCI5_B2 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_B2);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_B2_ISR */

#if configBSP430_HAL_USCI5_B3_ISR - 0
static void
__attribute__((__interrupt__(USCI_B3_VECTOR)))
isr_USCI5_B3 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_B3);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_B3_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_defn] */
/* !BSP430! end=hal_isr_defn */

static hBSP430halUSCI5 periphToDevice (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if configBSP430_HAL_USCI5_A0 - 0
  if (BSP430_PERIPH_USCI5_A0 == periph) {
    return BSP430_HAL_USCI5_A0;
  }
#endif /* configBSP430_HAL_USCI5_A0 */

#if configBSP430_HAL_USCI5_A1 - 0
  if (BSP430_PERIPH_USCI5_A1 == periph) {
    return BSP430_HAL_USCI5_A1;
  }
#endif /* configBSP430_HAL_USCI5_A1 */

#if configBSP430_HAL_USCI5_A2 - 0
  if (BSP430_PERIPH_USCI5_A2 == periph) {
    return BSP430_HAL_USCI5_A2;
  }
#endif /* configBSP430_HAL_USCI5_A2 */

#if configBSP430_HAL_USCI5_A3 - 0
  if (BSP430_PERIPH_USCI5_A3 == periph) {
    return BSP430_HAL_USCI5_A3;
  }
#endif /* configBSP430_HAL_USCI5_A3 */

#if configBSP430_HAL_USCI5_B0 - 0
  if (BSP430_PERIPH_USCI5_B0 == periph) {
    return BSP430_HAL_USCI5_B0;
  }
#endif /* configBSP430_HAL_USCI5_B0 */

#if configBSP430_HAL_USCI5_B1 - 0
  if (BSP430_PERIPH_USCI5_B1 == periph) {
    return BSP430_HAL_USCI5_B1;
  }
#endif /* configBSP430_HAL_USCI5_B1 */

#if configBSP430_HAL_USCI5_B2 - 0
  if (BSP430_PERIPH_USCI5_B2 == periph) {
    return BSP430_HAL_USCI5_B2;
  }
#endif /* configBSP430_HAL_USCI5_B2 */

#if configBSP430_HAL_USCI5_B3 - 0
  if (BSP430_PERIPH_USCI5_B3 == periph) {
    return BSP430_HAL_USCI5_B3;
  }
#endif /* configBSP430_HAL_USCI5_B3 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}
