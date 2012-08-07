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
#include <bsp430/periph/usci.h>

/* !BSP430! periph=usci */
/* !BSP430! instance=USCI_A0,USCI_A1,USCI_B0,USCI_B1 */

#define SERIAL_HAL_HPL(_hal) (_hal)->hpl.usci
#define SERIAL_HAL_HPLAUX(_hal) (_hal)->hpl_aux.usci

#define WAKEUP_TRANSMIT_HAL_NI(_hal) do {               \
    if (! (*SERIAL_HAL_HPLAUX(_hal)->iep & SERIAL_HAL_HPLAUX(_hal)->tx_bit)) {            \
      *SERIAL_HAL_HPLAUX(_hal)->iep |= (*SERIAL_HAL_HPLAUX(_hal)->ifgp & SERIAL_HAL_HPLAUX(_hal)->tx_bit); \
    }                                                   \
  } while (0)

#define RAW_TRANSMIT_HAL_NI(_hal, _c) do {              \
    while (! (SERIAL_HAL_HPLAUX(_hal)->tx_bit & *SERIAL_HAL_HPLAUX(_hal)->ifgp)) {	\
      ;                                                 \
    }                                                   \
    SERIAL_HAL_HPL(_hal)->txbuf = _c;                   \
  } while (0)

#define RAW_RECEIVE_HAL_NI(_hal, _c) do {              \
    while (! (SERIAL_HAL_HPLAUX(_hal)->rx_bit & *SERIAL_HAL_HPLAUX(_hal)->ifgp)) {	\
      ;                                                 \
    }                                                   \
    _c = SERIAL_HAL_HPL(_hal)->rxbuf;                   \
  } while (0)


#define FLUSH_HAL_NI(_hal) do {                 \
    while (SERIAL_HAL_HPL(_hal)->stat & UCBUSY) {       \
      ;                                         \
    }                                           \
  } while (0)

struct sBSP430usciHPLAux {
  /** Pointer to the interrupt enable register for the peripheral */
  volatile unsigned char * const iep;

  /** Pointer to the interrupt flag register for the peripheral */
  volatile unsigned char * const ifgp;

  /** Bit within *iep and *ifgp used to denote an RX interrupt */
  unsigned char const rx_bit;

  /** Bit within *iep and *ifgp used to denote a TX interrupt */
  unsigned char const tx_bit;

  /** I2C own address register (USCI_Bx only) */
  volatile unsigned int * i2coap;

  /** I2C slave address register (USCI_Bx only) */
  volatile unsigned int * i2csap;
};

#define HAL_HPL_IS_USCI_A(hal_) (NULL == SERIAL_HAL_HPLAUX(hal_)->i2coap)
#define HAL_HPL_IS_USCI_B(hal_) (NULL != SERIAL_HAL_HPLAUX(hal_)->i2coap)

static
hBSP430halSERIAL
usciConfigure (hBSP430halSERIAL hal,
               unsigned char ctl0_byte,
               unsigned char ctl1_byte,
               unsigned int brw,
               int mctl)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  SERIAL_HAL_HPL(hal)->ctl1 = UCSWRST;
  do {
    if (0 != iBSP430platformConfigurePeripheralPins_ni(xBSP430periphFromHPL(SERIAL_HAL_HPL(hal)), 1)) {
      hal = NULL;
      break;
    }
    SERIAL_HAL_HPL(hal)->ctl1 |= ctl1_byte;
    SERIAL_HAL_HPL(hal)->ctl0 = ctl0_byte;
    SERIAL_HAL_HPL(hal)->br0 = brw % 256;
    SERIAL_HAL_HPL(hal)->br1 = brw / 256;
    if (0 <= mctl) {
      SERIAL_HAL_HPL(hal)->mctl = mctl;
    }

    /* Mark the hal active */
    hal->num_rx = hal->num_tx = 0;

    /* Release the USCI and enable the interrupts.  Interrupts are
     * disabled and cleared when UCSWRST is set. */
    SERIAL_HAL_HPL(hal)->ctl1 &= ~UCSWRST;
    if (0 != hal->rx_callback) {
      *SERIAL_HAL_HPLAUX(hal)->iep |= SERIAL_HAL_HPLAUX(hal)->rx_bit;
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return hal;
}


hBSP430halSERIAL
hBSP430usciOpenUART (hBSP430halSERIAL hal,
                     unsigned char ctl0_byte,
                     unsigned char ctl1_byte,
                     unsigned long baud)
{
  unsigned long brclk_Hz = 0;
  uint16_t brw = 0;
  uint16_t brs = 0;

  /* Reject unsupported HALs */
  if ((NULL == hal)
      || (NULL == SERIAL_HAL_HPLAUX(hal))
      || HAL_HPL_IS_USCI_B(hal)) {
    return NULL;
  }
  /* Reject invalid baud rates */
  if ((0 == baud) || (BSP430_USCI_UART_MAX_BAUD < baud)) {
    return NULL;
  }

  /* Force to UART async and wipe out the clock select fields */
  ctl0_byte &= ~(UCMODE1 | UCMODE0 | UCSYNC);
  ctl1_byte &= ~(UCSSEL1 | UCSSEL0);

  /* Assume ACLK <= 20 kHz is VLOCLK and cannot be trusted.  Prefer
   * 32 kiHz ACLK for rates that are low enough.  Use SMCLK for
   * anything larger.  */
  brclk_Hz = usBSP430clockACLK_Hz_ni();
  if ((brclk_Hz > 20000) && (brclk_Hz >= (3 * baud))) {
    ctl1_byte |= UCSSEL_1;
  } else {
    ctl1_byte |= UCSSEL_2;
    brclk_Hz = ulBSP430clockSMCLK_Hz_ni();
  }

  brw = (brclk_Hz / baud);
  brs = (1 + 16 * (brclk_Hz - baud * brw) / baud) / 2;

  return usciConfigure(hal, ctl0_byte, ctl1_byte, brw, (0 * UCBRF0) | (brs * UCBRS0));
}

hBSP430halSERIAL
hBSP430usciOpenSPI (hBSP430halSERIAL hal,
                    unsigned char ctl0_byte,
                    unsigned char ctl1_byte,
                    unsigned int prescaler)
{
  /* Reject unsupported HALs */
  if (NULL == hal) {
    return NULL;
  }
  /* Reject invalid mode */
  if (UCMODE_3 == (ctl1_byte & (UCMODE0 | UCMODE1))) {
    return NULL;
  }
  /* Reject invalid prescaler */
  if (0 == prescaler) {
    return NULL;
  }

  /* SPI is synchronous */
  ctl0_byte |= UCSYNC;

  return usciConfigure(hal, ctl0_byte, ctl1_byte, prescaler, -1);
}

hBSP430halSERIAL
hBSP430usciOpenI2C (hBSP430halSERIAL hal,
                    unsigned char ctl0_byte,
                    unsigned char ctl1_byte,
                    unsigned int prescaler)
{
  /* Reject unsupported HALs */
  if ((NULL == hal)
      || (NULL == SERIAL_HAL_HPLAUX(hal))
      || HAL_HPL_IS_USCI_A(hal)) {
    return NULL;
  }
  /* Reject invalid prescaler */
  if (0 == prescaler) {
    return NULL;
  }

  /* I2C is synchronous mode 3 */
  ctl0_byte |= UCMODE_3 | UCSYNC;

  return usciConfigure(hal, ctl0_byte, ctl1_byte, prescaler, -1);
}

int
iBSP430usciConfigureCallbacks (hBSP430halSERIAL hal,
                               const struct sBSP430halISRCallbackVoid * rx_callback,
                               const struct sBSP430halISRCallbackVoid * tx_callback)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc = 0;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  SERIAL_HAL_HPL(hal)->ctl1 |= UCSWRST;
  if (hal->rx_callback || hal->tx_callback) {
    rc = -1;
  } else {
    hal->rx_callback = rx_callback;
    hal->tx_callback = tx_callback;
  }
  /* Release the USCI and enable the interrupts.  Interrupts are
   * disabled and cleared when UCSWRST is set. */
  SERIAL_HAL_HPL(hal)->ctl1 &= ~UCSWRST;
  if (hal->rx_callback) {
    *SERIAL_HAL_HPLAUX(hal)->iep |= SERIAL_HAL_HPLAUX(hal)->rx_bit;
  }
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

int
iBSP430usciSetHold (hBSP430halSERIAL hal,
                    int holdp)
{
  int rc;
  if (holdp) {
    SERIAL_HAL_HPL(hal)->ctl1 |= UCSWRST;
    rc = iBSP430platformConfigurePeripheralPins_ni (xBSP430periphFromHPL(hal->hpl.any), 0);
  } else {
    rc = iBSP430platformConfigurePeripheralPins_ni (xBSP430periphFromHPL(hal->hpl.any), 1);
    SERIAL_HAL_HPL(hal)->ctl1 &= ~UCSWRST;
  }
  return rc;
}

int
iBSP430usciClose (hBSP430halSERIAL hal)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  SERIAL_HAL_HPL(hal)->ctl1 = UCSWRST;
  rc = iBSP430platformConfigurePeripheralPins_ni ((tBSP430periphHandle)(uintptr_t)(SERIAL_HAL_HPL(hal)), 0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return rc;
}

void
vBSP430usciFlush_ni (hBSP430halSERIAL hal)
{
  FLUSH_HAL_NI(hal);
}

void
vBSP430usciWakeupTransmit_ni (hBSP430halSERIAL hal)
{
  WAKEUP_TRANSMIT_HAL_NI(hal);
}

int
iBSP430usciUARTrxByte_ni (hBSP430halSERIAL hal)
{
  if (hal->rx_callback) {
    return -1;
  }
  if (*SERIAL_HAL_HPLAUX(hal)->ifgp & SERIAL_HAL_HPLAUX(hal)->rx_bit) {
    return SERIAL_HAL_HPL(hal)->rxbuf;
  }
  return -1;
}
int
iBSP430usciUARTtxByte_ni (hBSP430halSERIAL hal, uint8_t c)
{
  if (hal->tx_callback) {
    return -1;
  }
  RAW_TRANSMIT_HAL_NI(hal, c);
  return c;
}

int
iBSP430usciUARTtxData_ni (hBSP430halSERIAL hal,
                          const uint8_t * data,
                          size_t len)
{
  const uint8_t * p = data;
  const uint8_t * edata = data + len;
  if (hal->tx_callback) {
    return -1;
  }
  while (p < edata) {
    RAW_TRANSMIT_HAL_NI(hal, *p++);
  }
  return p - data;
}

int
iBSP430usciUARTtxASCIIZ_ni (hBSP430halSERIAL hal, const char * str)
{
  const char * in_string = str;

  if (hal->tx_callback) {
    return -1;
  }
  while (*str) {
    RAW_TRANSMIT_HAL_NI(hal, *str);
    ++str;
  }
  return str - in_string;
}

int
iBSP430usciSPITxRx_ni (hBSP430halSERIAL hal,
                       const uint8_t * tx_data,
                       size_t tx_len,
                       size_t rx_len,
                       uint8_t * rx_data)
{
  size_t transaction_length = tx_len + rx_len;
  size_t i = 0;

  if (hal->tx_callback) {
    return -1;
  }
  while (i < transaction_length) {
    RAW_TRANSMIT_HAL_NI(hal, (i < tx_len) ? tx_data[i] : i);
    RAW_RECEIVE_HAL_NI(hal, *rx_data);
    ++rx_data;
    ++i;
  }
  return i;
}

int
iBSP430usciI2CsetAddresses_ni (hBSP430halSERIAL hal,
                               int own_address,
                               int slave_address)
{
  if (0 <= own_address) {
    *SERIAL_HAL_HPLAUX(hal)->i2coap = own_address;
  }
  if (0 <= slave_address) {
    *SERIAL_HAL_HPLAUX(hal)->i2csap = slave_address;
  }
  return 0;
}

int
iBSP430usciI2CrxData_ni (hBSP430halSERIAL hal,
                         uint8_t * data,
                         size_t len)
{
  volatile struct sBSP430hplUSCI * hpl = SERIAL_HAL_HPL(hal);
  struct sBSP430usciHPLAux * aux = SERIAL_HAL_HPLAUX(hal);
  uint8_t * dp = data;
  const uint8_t * dpe = data + len;
  int i = 0;

  /* Set for receive */
  hpl->ctl1 &= ~UCTR;
  /* Delay for any in-progress stop to complete */
  while (hpl->ctl1 & UCTXSTP) {
    if (hpl->stat & (UCNACKIFG | UCALIFG)) {
      return -1;
    }
  }
  /* Issue a start */
  hpl->ctl1 |= UCTXSTT;
  while (dp < dpe) {
    if (dpe == (dp+1)) {
      /* This will be last character: wait for any in-progress start
       * to complete then issue stop */
      while (hpl->ctl1 & UCTXSTT) {
        if (hpl->stat & (UCNACKIFG | UCALIFG)) {
          return -1;
        }
      }
      hpl->ctl1 |= UCTXSTP;
    }
    while (! (aux->rx_bit & *aux->ifgp)) {
      if (hpl->stat & (UCNACKIFG | UCALIFG)) {
        return -1;
      }
    }
    *dp++ = hpl->rxbuf;
  }
  return dp - data;
}

int
iBSP430usciI2CtxData_ni (hBSP430halSERIAL hal,
                         const uint8_t * data,
                         size_t len)
{
  volatile struct sBSP430hplUSCI * hpl = SERIAL_HAL_HPL(hal);
  struct sBSP430usciHPLAux * aux = SERIAL_HAL_HPLAUX(hal);
  int i = 0;

  /* Delay for any in-progress stop to complete */
  while (hpl->ctl1 & UCTXSTP) {
    if (hpl->stat & (UCNACKIFG | UCALIFG)) {
      return -1;
    }
  }
  /* Issue a start for transmit */
  hpl->ctl1 |= UCTR | UCTXSTT;
  while (i < len) {
    while (! (aux->tx_bit & *aux->ifgp)) {
      if (hpl->stat & (UCNACKIFG | UCALIFG)) {
        return -1;
      }
    }
    hpl->txbuf = data[i++];
  }
  /* Wait for any in-progress start to complete then issue stop */
  while (hpl->ctl1 & UCTXSTT) {
    if (hpl->stat & (UCNACKIFG | UCALIFG)) {
      return -1;
    }
  }
  hpl->ctl1 |= UCTXSTP;
  return i;
}

static struct sBSP430serialDispatch dispatch_ = {
  .openUART = hBSP430usciOpenUART,
  .openSPI = hBSP430usciOpenSPI,
  .openI2C = hBSP430usciOpenI2C,
  .configureCallbacks = iBSP430usciConfigureCallbacks,
  .setHold = iBSP430usciSetHold,
  .close = iBSP430usciClose,
  .wakeupTransmit_ni = vBSP430usciWakeupTransmit_ni,
  .flush_ni = vBSP430usciFlush_ni,
  .uartRxByte_ni = iBSP430usciUARTrxByte_ni,
  .uartTxByte_ni = iBSP430usciUARTtxByte_ni,
  .uartTxData_ni = iBSP430usciUARTtxData_ni,
  .uartTxASCIIZ_ni = iBSP430usciUARTtxASCIIZ_ni,
  .spiTxRx_ni = iBSP430usciSPITxRx_ni,
  .i2cSetAddresses_ni = iBSP430usciI2CsetAddresses_ni,
  .i2cRxData_ni = iBSP430usciI2CrxData_ni,
  .i2cTxData_ni = iBSP430usciI2CtxData_ni,
};

#if configBSP430_HAL_USCI_A0 - 0
static struct sBSP430usciHPLAux xBSP430hplaux_USCI_A0_ = {
  .iep = &IE2,
  .ifgp = &IFG2,
  .rx_bit = BIT0,
  .tx_bit = BIT1,
};
struct sBSP430halSERIAL xBSP430hal_USCI_A0_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI
#if configBSP430_HAL_USCIAB0RX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCIAB0RX_ISR */
#if configBSP430_HAL_USCIAB0TX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_USCIAB0TX_ISR */
  },
  .hpl = { .usci = BSP430_HPL_USCI_A0 },
  .hpl_aux = { .usci = &xBSP430hplaux_USCI_A0_ },
  .dispatch = &dispatch_,
};
#endif /* configBSP430_HAL_USCI_A0 */

#if configBSP430_HAL_USCI_A1 - 0
static struct sBSP430usciHPLAux xBSP430hplaux_USCI_A1_ = {
  .iep = &UC1IE,
  .ifgp = &UC1IFG,
  .rx_bit = BIT0,
  .tx_bit = BIT1,
};
struct sBSP430halSERIAL xBSP430hal_USCI_A1_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI
#if configBSP430_HAL_USCIAB1RX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCIAB0RX_ISR */
#if configBSP430_HAL_USCIAB1TX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_USCIAB0TX_ISR */
  },
  .hpl = { .usci = BSP430_HPL_USCI_A1 },
  .hpl_aux = { .usci = &xBSP430hplaux_USCI_A1_ },
  .dispatch = &dispatch_,
};
#endif /* configBSP430_HAL_USCI_A1 */

#if configBSP430_HAL_USCI_B0 - 0
static struct sBSP430usciHPLAux xBSP430hplaux_USCI_B0_ = {
  .iep = &IE2,
  .ifgp = &IFG2,
  .rx_bit = BIT2,
  .tx_bit = BIT3,
  .i2coap = &UCB0I2COA,
  .i2csap = &UCB0I2CSA,
};
struct sBSP430halSERIAL xBSP430hal_USCI_B0_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI
#if configBSP430_HAL_USCIAB0RX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCIAB0RX_ISR */
#if configBSP430_HAL_USCIAB0TX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_USCIAB0TX_ISR */
  },
  .hpl = { .usci = BSP430_HPL_USCI_B0 },
  .hpl_aux = { .usci = &xBSP430hplaux_USCI_B0_ },
  .dispatch = &dispatch_,
};
#endif /* configBSP430_HAL_USCI_B0 */

#if configBSP430_HAL_USCI_B1 - 0
static struct sBSP430usciHPLAux xBSP430hplaux_USCI_B0_ = {
  .iep = &UC1IE,
  .ifgp = &UC1IFG,
  .rx_bit = BIT2,
  .tx_bit = BIT3,
  .i2coap = &UCB1I2COA,
  .i2csap = &UCB1I2CSA,
};
struct sBSP430halSERIAL xBSP430hal_USCI_B1_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI
#if configBSP430_HAL_USCIAB1RX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCIAB0RX_ISR */
#if configBSP430_HAL_USCIAB1TX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_USCIAB0TX_ISR */
  },
  .hpl = { .usci = BSP430_HPL_USCI_B1 },
  .hpl_aux = { .usci = &xBSP430hplaux_USCI_B1_ },
  .dispatch = &dispatch_,
};
#endif /* configBSP430_HAL_USCI_B1 */

#if ((configBSP430_HAL_USCIAB0RX_ISR - 0) || (configBSP430_HAL_USCIAB1RX_ISR - 0))
static int
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usciabrx_isr (hBSP430halSERIAL hal)
{
  hal->rx_byte = SERIAL_HAL_HPL(hal)->rxbuf;
  ++hal->num_rx;
  return iBSP430callbackInvokeISRVoid_ni(&hal->rx_callback, hal, 0);
}

#if configBSP430_HAL_USCIAB0RX_ISR - 0
static void
__attribute__((__interrupt__(USCIAB0RX_VECTOR)))
isr_USCIAB0RX (void)
{
  hBSP430halSERIAL usci = NULL;
  int rv = 0;

  if (0) {
  }
#if configBSP430_HAL_USCI_A0 - 0
  else if (SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_A0)->rx_bit & *(SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_A0)->ifgp)) {
    usci = BSP430_HAL_USCI_A0;
  }
#endif /* configBSP430_HAL_USCI_A0 */
#if configBSP430_HAL_USCI_B0 - 0
  else if (SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_B0)->rx_bit & *(SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_B0)->ifgp)) {
    usci = BSP430_HAL_USCI_B0;
  }
#endif /* configBSP430_HAL_USCI_B0 */
  if (usci) {
    rv = usciabrx_isr(usci);
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* HAL USCIAB0RX ISR */

#endif  /* HAL USCIABxRX ISR */

#if ((configBSP430_HAL_USCIAB0TX_ISR - 0) || (configBSP430_HAL_USCIAB1TX_ISR - 0))
static int
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usciabtx_isr (hBSP430halSERIAL hal)
{
  int rv = iBSP430callbackInvokeISRVoid_ni(&hal->tx_callback, hal, 0);
  if (rv & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN) {
    /* Found some data; send it out */
    ++hal->num_tx;
    SERIAL_HAL_HPL(hal)->txbuf = hal->tx_byte;
  } else {
    /* No data; mark transmission disabled */
    rv |= BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
  }
  /* If no more is expected, clear the interrupt but mark that the
   * function is ready so when the interrupt is next set it will
   * fire. */
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    *SERIAL_HAL_HPLAUX(hal)->iep &= ~SERIAL_HAL_HPLAUX(hal)->tx_bit;
    *SERIAL_HAL_HPLAUX(hal)->ifgp |= SERIAL_HAL_HPLAUX(hal)->tx_bit;
  }
  return rv;
}

#if configBSP430_HAL_USCIAB0TX_ISR - 0
static void
__attribute__((__interrupt__(USCIAB0TX_VECTOR)))
isr_USCIAB0TX (void)
{
  int rv = 0;
  hBSP430halSERIAL usci = NULL;

  if (0) {
  }
#if configBSP430_HAL_USCI_A0 - 0
  else if (SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_A0)->tx_bit & *(SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_A0)->ifgp)) {
    usci = BSP430_HAL_USCI_A0;
  }
#endif /* configBSP430_HAL_USCI_A0 */
#if configBSP430_HAL_USCI_B0 - 0
  else if (SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_B0)->tx_bit & *(SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_B0)->ifgp)) {
    usci = BSP430_HAL_USCI_B0;
  }
#endif /* configBSP430_HAL_USCI_B0 */
  if (usci) {
    rv = usciabtx_isr(usci);
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* HAL USCIAB0TX ISR */

#endif  /* HAL USCIABxTX ISR */

volatile sBSP430hplUSCI *
xBSP430hplLookupUSCI (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_demux] */
#if configBSP430_HPL_USCI_A0 - 0
  if (BSP430_PERIPH_USCI_A0 == periph) {
    return BSP430_HPL_USCI_A0;
  }
#endif /* configBSP430_HPL_USCI_A0 */

#if configBSP430_HPL_USCI_A1 - 0
  if (BSP430_PERIPH_USCI_A1 == periph) {
    return BSP430_HPL_USCI_A1;
  }
#endif /* configBSP430_HPL_USCI_A1 */

#if configBSP430_HPL_USCI_B0 - 0
  if (BSP430_PERIPH_USCI_B0 == periph) {
    return BSP430_HPL_USCI_B0;
  }
#endif /* configBSP430_HPL_USCI_B0 */

#if configBSP430_HPL_USCI_B1 - 0
  if (BSP430_PERIPH_USCI_B1 == periph) {
    return BSP430_HPL_USCI_B1;
  }
#endif /* configBSP430_HPL_USCI_B1 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_demux] */
  /* !BSP430! end=periph_hpl_demux */
  return NULL;
}

hBSP430halSERIAL
hBSP430usciLookup (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if configBSP430_HAL_USCI_A0 - 0
  if (BSP430_PERIPH_USCI_A0 == periph) {
    return BSP430_HAL_USCI_A0;
  }
#endif /* configBSP430_HAL_USCI_A0 */

#if configBSP430_HAL_USCI_A1 - 0
  if (BSP430_PERIPH_USCI_A1 == periph) {
    return BSP430_HAL_USCI_A1;
  }
#endif /* configBSP430_HAL_USCI_A1 */

#if configBSP430_HAL_USCI_B0 - 0
  if (BSP430_PERIPH_USCI_B0 == periph) {
    return BSP430_HAL_USCI_B0;
  }
#endif /* configBSP430_HAL_USCI_B0 */

#if configBSP430_HAL_USCI_B1 - 0
  if (BSP430_PERIPH_USCI_B1 == periph) {
    return BSP430_HAL_USCI_B1;
  }
#endif /* configBSP430_HAL_USCI_B1 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}

const char *
xBSP430usciName (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_name_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_name_demux] */
#if configBSP430_HPL_USCI_A0 - 0
  if (BSP430_PERIPH_USCI_A0 == periph) {
    return "USCI_A0";
  }
#endif /* configBSP430_HPL_USCI_A0 */

#if configBSP430_HPL_USCI_A1 - 0
  if (BSP430_PERIPH_USCI_A1 == periph) {
    return "USCI_A1";
  }
#endif /* configBSP430_HPL_USCI_A1 */

#if configBSP430_HPL_USCI_B0 - 0
  if (BSP430_PERIPH_USCI_B0 == periph) {
    return "USCI_B0";
  }
#endif /* configBSP430_HPL_USCI_B0 */

#if configBSP430_HPL_USCI_B1 - 0
  if (BSP430_PERIPH_USCI_B1 == periph) {
    return "USCI_B1";
  }
#endif /* configBSP430_HPL_USCI_B1 */

  /* END AUTOMATICALLY GENERATED CODE [periph_name_demux] */
  /* !BSP430! end=periph_name_demux */
  return NULL;
}
