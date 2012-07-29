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
#include <bsp430/periph/usci_.h>

/* !BSP430! periph=usci */
/* !BSP430! instance=USCI_A0,USCI_A1,USCI_B0,USCI_B1 */

#define COM_PORT_ACTIVE  0x01

/** Convert from a raw peripheral handle to the corresponding USCI
 * device handle. */
static xBSP430usciHandle periphToDevice (xBSP430periphHandle periph);

#define WAKEUP_TRANSMIT_HAL_NI(_hal) do {               \
    if (! (*(_hal)->iep & (_hal)->tx_bit)) {            \
      *(_hal)->iep |= (*(_hal)->ifgp & (_hal)->tx_bit); \
    }                                                   \
  } while (0)

#define RAW_TRANSMIT_HAL_NI(_hal, _c) do {              \
    while (! ((_hal)->tx_bit & *(_hal)->ifgp)) {	\
      ;                                                 \
    }                                                   \
    _hal->usci->txbuf = _c;                             \
  } while (0)

#define FLUSH_HAL_NI(_hal) do {                 \
    while (_hal->usci->stat & UCBUSY) {         \
      ;                                         \
    }                                           \
  } while (0)

xBSP430usciHandle
xBSP430usciOpenUART (xBSP430periphHandle periph,
                     unsigned int control_word,
                     unsigned long baud)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  unsigned long brclk_Hz = 0;
  xBSP430usciHandle device = periphToDevice(periph);
  uint16_t br = 0;
  uint16_t brs = 0;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  /* Reject invalid baud rates */
  if ((0 == baud) || (1000000UL < baud)) {
    device = NULL;
  }

  /* Reject if the pins can't be configured */
  if ((NULL != device)
      && (0 != iBSP430platformConfigurePeripheralPins_ni((xBSP430periphHandle)(uintptr_t)(device->usci), 1))) {
    device = NULL;
  }

  if (NULL != device) {
    /* Assume ACLK <= 20 kHz is VLOCLK and cannot be trusted.  Prefer
     * 32 kiHz ACLK for rates that are low enough.  Use SMCLK for
     * anything larger.  */
    brclk_Hz = usBSP430clockACLK_Hz_ni();
    if ((brclk_Hz > 20000) && (brclk_Hz >= (3 * baud))) {
      device->usci->ctl1 = UCSWRST | UCSSEL_1;
    } else {
      device->usci->ctl1 = UCSWRST | UCSSEL_2;
      brclk_Hz = ulBSP430clockSMCLK_Hz_ni();
    }
    br = (brclk_Hz / baud);
    brs = (1 + 16 * (brclk_Hz - baud * br) / baud) / 2;

    device->usci->br0 = br % 256;
    device->usci->br1 = br / 256;
    device->usci->mctl = (0 * UCBRF0) | (brs * UCBRS0);

    /* Mark the device active */
    device->num_rx = device->num_tx = 0;
    device->flags |= COM_PORT_ACTIVE;

    /* Release the USCI and enable the interrupts.  Interrupts are
     * disabled and cleared when UCSWRST is set. */
    device->usci->ctl1 &= ~UCSWRST;
    if (0 != device->rx_callback) {
      *device->iep |= device->rx_bit;
    }
  }
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return device;
}

int
iBSP430usciConfigureCallbacks (xBSP430usciHandle device,
                               const struct xBSP430periphISRCallbackVoid * rx_callback,
                               const struct xBSP430periphISRCallbackVoid * tx_callback)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc = 0;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  device->usci->ctl1 |= UCSWRST;
  if (device->rx_callback || device->tx_callback) {
    rc = -1;
  } else {
    device->rx_callback = rx_callback;
    device->tx_callback = tx_callback;
  }
  /* Release the USCI and enable the interrupts.  Interrupts are
   * disabled and cleared when UCSWRST is set. */
  device->usci->ctl1 &= ~UCSWRST;
  if (device->rx_callback) {
    *device->iep |= device->rx_bit;
  }
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

int
iBSP430usciClose (xBSP430usciHandle device)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  device->usci->ctl1 = UCSWRST;
  rc = iBSP430platformConfigurePeripheralPins_ni ((xBSP430periphHandle)(uintptr_t)(device->usci), 0);
  device->flags = 0;
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return rc;
}

void
vBSP430usciFlush_ni (xBSP430usciHandle device)
{
  FLUSH_HAL_NI(device);
}

void
vBSP430usciWakeupTransmit_ni (xBSP430usciHandle device)
{
  WAKEUP_TRANSMIT_HAL_NI(device);
}

int
iBSP430usciTransmitByte_ni (xBSP430usciHandle device, int c)
{
  if (device->tx_callback) {
    return -1;
  }
  RAW_TRANSMIT_HAL_NI(device, c);
  return c;
}

int
iBSP430usciTransmitData_ni (xBSP430usciHandle device,
                            const uint8_t * data,
                            size_t len)
{
  const uint8_t * p = data;
  const uint8_t * edata = data + len;
  if (device->tx_callback) {
    return -1;
  }
  while (p < edata) {
    RAW_TRANSMIT_HAL_NI(device, *p++);
  }
  return p - data;
}

int
iBSP430usciTransmitASCIIZ_ni (xBSP430usciHandle device, const char * str)
{
  const char * in_string = str;

  if (device->tx_callback) {
    return -1;
  }
  while (*str) {
    RAW_TRANSMIT_HAL_NI(device, *str);
    ++str;
  }
  return str - in_string;
}

#if configBSP430_HAL_USCI_A0 - 0
static struct xBSP430usciState state_USCI_A0_ = {
  .usci = (xBSP430periphUSCI *)_BSP430_PERIPH_USCI_A0_BASEADDRESS,
  .iep = &IE2,
  .ifgp = &IFG2,
  .rx_bit = BIT0,
  .tx_bit = BIT1,
};

xBSP430usciHandle const xBSP430usci_USCI_A0 = &state_USCI_A0_;
#endif /* configBSP430_HAL_USCI_A0 */

#if configBSP430_HAL_USCI_A1 - 0
static struct xBSP430usciState state_USCI_A1_ = {
  .usci = (xBSP430periphUSCI *)_BSP430_PERIPH_USCI_A1_BASEADDRESS
  .iep = &UC1IE,
  .ifgp = &UC1IFG,
  .rx_bit = BIT0,
  .tx_bit = BIT1,
};

xBSP430usciHandle const xBSP430usci_USCI_A1 = &state_USCI_A1_;
#endif /* configBSP430_HAL_USCI_A1 */

#if configBSP430_HAL_USCI_B0 - 0
static struct xBSP430usciState state_USCI_B0_ = {
  .usci = (xBSP430periphUSCI *)_BSP430_PERIPH_USCI_B0_BASEADDRESS
  .iep = &IE2,
  .ifgp = &IFG2,
  .rx_bit = BIT2,
  .tx_bit = BIT3,
};

xBSP430usciHandle const xBSP430usci_USCI_B0 = &state_USCI_B0_;
#endif /* configBSP430_HAL_USCI_B0 */

#if configBSP430_HAL_USCI_B1 - 0
static struct xBSP430usciState state_USCI_B1_ = {
  .usci = (xBSP430periphUSCI *)_BSP430_PERIPH_USCI_B1_BASEADDRESS
  .iep = &UC1IE,
  .ifgp = &UC1IFG,
  .rx_bit = BIT2,
  .tx_bit = BIT3,
};

xBSP430usciHandle const xBSP430usci_USCI_B1 = &state_USCI_B1_;
#endif /* configBSP430_HAL_USCI_B1 */

#if ((configBSP430_HAL_USCIAB0RX_ISR - 0) || (configBSP430_HAL_USCIAB1RX_ISR - 0))
static int
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usciabrx_isr (xBSP430usciHandle device)
{
  device->rx_byte = device->usci->rxbuf;
  ++device->num_rx;
  return iBSP430callbackInvokeISRVoid_ni(&device->rx_callback, device, 0);
}

#if configBSP430_HAL_USCIAB0RX_ISR - 0
static void
__attribute__((__interrupt__(USCIAB0RX_VECTOR)))
isr_USCIAB0RX (void)
{
  xBSP430usciHandle usci = NULL;
  int rv = 0;
  
  if (0) {
  }
#if configBSP430_HAL_USCI_A0_ISR - 0
  else if (xBSP430usci_USCI_A0->rx_bit & *(xBSP430usci_USCI_A0->ifgp)) {
    usci = xBSP430usci_USCI_A0;
  }
#endif /* configBSP430_HAL_USCI_A0_ISR */
#if configBSP430_HAL_USCI_B0_ISR - 0
  else if (xBSP430usci_USCI_B0->rx_bit & *(xBSP430usci_USCI_B0->ifgp)) {
    usci = xBSP430usci_USCI_B0;
  }
#endif /* configBSP430_HAL_USCI_B0_ISR */
  if (usci) {
    rv = usciabrx_isr(usci);
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* HAL USCIAB0RX ISR */

#endif  /* HAL USCIABxRX ISR */

#if ((configBSP430_HAL_USCIAB0TX_ISR - 0) || (configBSP430_HAL_USCIAB1TX_ISR - 0))
static int
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usciabtx_isr (xBSP430usciHandle device)
{
  int rv = iBSP430callbackInvokeISRVoid_ni(&device->tx_callback, device, 0);
  if (rv & BSP430_PERIPH_ISR_CALLBACK_BREAK_CHAIN) {
    /* Found some data; send it out */
    ++device->num_tx;
    device->usci->txbuf = device->tx_byte;
  } else {
    /* No data; mark transmission disabled */
    rv |= BSP430_PERIPH_ISR_CALLBACK_DISABLE_INTERRUPT;
  }
  /* If no more is expected, clear the interrupt but mark that the
   * function is ready so when the interrupt is next set it will
   * fire. */
  if (rv & BSP430_PERIPH_ISR_CALLBACK_DISABLE_INTERRUPT) {
    *device->iep &= ~device->tx_bit;
    *device->ifgp |= device->tx_bit;
  }
  return rv;
}

#if configBSP430_HAL_USCIAB0TX_ISR - 0
static void
__attribute__((__interrupt__(USCIAB0TX_VECTOR)))
isr_USCIAB0TX (void)
{
  int rv = 0;
  xBSP430usciHandle usci = NULL;

  if (0) {
  }
#if configBSP430_HAL_USCI_A0_ISR - 0
  else if (xBSP430usci_USCI_A0->tx_bit & *(xBSP430usci_USCI_A0->ifgp)) {
    usci = xBSP430usci_USCI_A0;
  }
#endif /* configBSP430_HAL_USCI_A0_ISR */
#if configBSP430_HAL_USCI_B0_ISR - 0
  else if (xBSP430usci_USCI_B0->tx_bit & *(xBSP430usci_USCI_B0->ifgp)) {
    usci = xBSP430usci_USCI_B0;
  }
#endif /* configBSP430_HAL_USCI_B0_ISR */
  if (usci) {
    rv = usciabtx_isr(usci);
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL(rv);
}
#endif /* HAL USCIAB0TX ISR */

#endif  /* HAL USCIABxTX ISR */

static xBSP430usciHandle periphToDevice (xBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if configBSP430_HAL_USCI_A0 - 0
  if (BSP430_PERIPH_USCI_A0 == periph) {
    return xBSP430usci_USCI_A0;
  }
#endif /* configBSP430_PERIPH_USCI_A0 */

#if configBSP430_HAL_USCI_A1 - 0
  if (BSP430_PERIPH_USCI_A1 == periph) {
    return xBSP430usci_USCI_A1;
  }
#endif /* configBSP430_PERIPH_USCI_A1 */

#if configBSP430_HAL_USCI_B0 - 0
  if (BSP430_PERIPH_USCI_B0 == periph) {
    return xBSP430usci_USCI_B0;
  }
#endif /* configBSP430_PERIPH_USCI_B0 */

#if configBSP430_HAL_USCI_B1 - 0
  if (BSP430_PERIPH_USCI_B1 == periph) {
    return xBSP430usci_USCI_B1;
  }
#endif /* configBSP430_PERIPH_USCI_B1 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}
