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

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/serial.h>
#include <bsp430/periph/usci.h>

/* !BSP430! periph=usci */
/* !BSP430! instance=USCI_A0,USCI_A1,USCI_B0,USCI_B1 */

#define SERIAL_HAL_HPL(_hal) (_hal)->hpl.usci
#define SERIAL_HAL_HPLAUX(_hal) (_hal)->hpl_aux.usci

#define WAKEUP_TRANSMIT_HAL_NI(_hal) do {                               \
    *SERIAL_HAL_HPLAUX(_hal)->iep |= SERIAL_HAL_HPLAUX(_hal)->tx_bit;   \
  } while (0)

#define RAW_TRANSMIT_HAL_NI(_hal, _c) do {                              \
    while (! (SERIAL_HAL_HPLAUX(_hal)->tx_bit & *SERIAL_HAL_HPLAUX(_hal)->ifgp)) { \
      ;                                                                 \
    }                                                                   \
    SERIAL_HAL_HPL(_hal)->txbuf = _c;                                   \
    ++(_hal)->num_tx;                                                   \
  } while (0)

#define RAW_RECEIVE_HAL_NI(_hal, _c) do {                               \
    while (! (SERIAL_HAL_HPLAUX(_hal)->rx_bit & *SERIAL_HAL_HPLAUX(_hal)->ifgp)) { \
      ;                                                                 \
    }                                                                   \
    _c = SERIAL_HAL_HPL(_hal)->rxbuf;                                   \
    ++(_hal)->num_rx;                                                   \
  } while (0)

#define FLUSH_HAL_NI(_hal) do {                         \
    while (SERIAL_HAL_HPL(_hal)->stat & UCBUSY) {       \
      ;                                                 \
    }                                                   \
  } while (0)

/** Inspect bits in CTL0 to determine the appropriate peripheral
 * configuration. */
static BSP430_CORE_INLINE
int
peripheralConfigFlag (unsigned char ctl0)
{
  if (0 == (ctl0 & UCSYNC)) {
    return BSP430_PERIPHCFG_SERIAL_UART;
  }
  switch (ctl0 & (UCMODE1 | UCMODE0)) {
    case 0:
      return BSP430_PERIPHCFG_SERIAL_SPI3;
    case UCMODE0:
    case UCMODE1:
      return BSP430_PERIPHCFG_SERIAL_SPI4;
    case UCMODE0 | UCMODE1:
      return BSP430_PERIPHCFG_SERIAL_I2C;
  }
  return 0;
}

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
    int periph_config = peripheralConfigFlag(ctl0_byte);

    if (0 != iBSP430platformConfigurePeripheralPins_ni(xBSP430periphFromHPL(SERIAL_HAL_HPL(hal)), periph_config, 1)) {
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

    /* Release the device for use */
    vBSP430usciSetReset_ni(hal, 0);
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
   * 32 KiHz ACLK for rates that are low enough.  Use SMCLK for
   * anything larger.  */
  brclk_Hz = ulBSP430clockACLK_Hz_ni();
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
  if (UCMODE_3 == (ctl0_byte & (UCMODE0 | UCMODE1))) {
    return NULL;
  }
  /* Calculate default prescaler */
  if (0 == prescaler) {
    prescaler = uiBSP430serialSMCLKPrescaler(BSP430_SERIAL_SPI_BUS_SPEED_HZ);
    ctl1_byte |= UCSSEL1;
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
  /* Calculate default prescaler */
  if (0 == prescaler) {
    prescaler = uiBSP430serialSMCLKPrescaler(BSP430_SERIAL_I2C_BUS_SPEED_HZ);
    ctl1_byte |= UCSSEL1;
  }

  /* I2C is synchronous mode 3 */
  ctl0_byte |= UCMODE_3 | UCSYNC;

  return usciConfigure(hal, ctl0_byte, ctl1_byte, prescaler, -1);
}

void
vBSP430usciSetReset_ni (hBSP430halSERIAL hal,
                        int resetp)
{
  if (resetp) {
    SERIAL_HAL_HPL(hal)->ctl1 |= UCSWRST;
  } else {
    /* Release the USCI and enable the interrupts.  Interrupts are
     * disabled and cleared when UCSWRST is set. */
    SERIAL_HAL_HPL(hal)->ctl1 &= ~UCSWRST;
    if (hal->rx_cbchain_ni) {
      *SERIAL_HAL_HPLAUX(hal)->iep |= SERIAL_HAL_HPLAUX(hal)->rx_bit;
    }
  }
}

int
iBSP430usciSetHold_ni (hBSP430halSERIAL hal,
                       int holdp)
{
  int rc;
  int periph_config = peripheralConfigFlag(SERIAL_HAL_HPL(hal)->ctl0);

  if (holdp) {
    FLUSH_HAL_NI(hal);
    SERIAL_HAL_HPL(hal)->ctl1 |= UCSWRST;
    rc = iBSP430platformConfigurePeripheralPins_ni(xBSP430periphFromHPL(hal->hpl.any), periph_config, 0);
  } else {
    rc = iBSP430platformConfigurePeripheralPins_ni(xBSP430periphFromHPL(hal->hpl.any), periph_config, 1);
    if (0 == rc) {
      /* Release the USCI and enable the interrupts.  Interrupts are
       * disabled and cleared when UCSWRST is set. */
      SERIAL_HAL_HPL(hal)->ctl1 &= ~UCSWRST;
      if (hal->rx_cbchain_ni) {
        *SERIAL_HAL_HPLAUX(hal)->iep |= SERIAL_HAL_HPLAUX(hal)->rx_bit;
      }
    }
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
  rc = iBSP430platformConfigurePeripheralPins_ni((tBSP430periphHandle)(uintptr_t)(SERIAL_HAL_HPL(hal)),
                                                 peripheralConfigFlag(SERIAL_HAL_HPL(hal)->ctl0),
                                                 0);
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
  if (hal->rx_cbchain_ni) {
    return -1;
  }
  if (*SERIAL_HAL_HPLAUX(hal)->ifgp & SERIAL_HAL_HPLAUX(hal)->rx_bit) {
    ++hal->num_rx;
    return SERIAL_HAL_HPL(hal)->rxbuf;
  }
  return -1;
}
int
iBSP430usciUARTtxByte_ni (hBSP430halSERIAL hal, uint8_t c)
{
  if (hal->tx_cbchain_ni) {
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
  if (hal->tx_cbchain_ni) {
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

  if (hal->tx_cbchain_ni) {
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
  uint8_t * rxp;
  uint8_t rx_dummy;
  size_t i = 0;

  if (hal->tx_cbchain_ni) {
    return -1;
  }
  rxp = rx_data;
  if (NULL == rx_data)  {
    rxp = &rx_dummy;
  }
  while (i < transaction_length) {
    uint8_t txd = (i < tx_len) ? tx_data[i] : BSP430_SERIAL_SPI_READ_TX_BYTE(i-tx_len);
    RAW_TRANSMIT_HAL_NI(hal, txd);
    RAW_RECEIVE_HAL_NI(hal, *rxp);
    if (rx_data) {
      ++rxp;
    }
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

/** Check for standard I2C transaction-aborting errors; if present,
 * return from the containing function with a negative error code. */
#define I2C_ERRCHECK_RETURN() do {                       \
    unsigned int stat = hpl->stat;                       \
    if (stat & (UCNACKIFG | UCALIFG)) {                  \
      return -(BSP430_I2C_ERRFLAG_PROTOCOL | stat);      \
    }                                                    \
  } while (0)

/** @def I2C_ERRCHECK_SPIN_WHILE_COND
 *
 * Loop repeatedly (at least once) until @a c_ evaluates to false.
 *
 * Within the loop, an I2C transaction-aborting error will result in
 * the containing function returning a negative error code
 * <tt>-(#BSP430_I2C_ERRFLAG_PROTOCOL | code)</tt> for some code.
 *
 * If #BSP430_I2C_SPIN_LIMIT is positive and @a c_ remains true while
 * the loop executes #BSP430_I2C_SPIN_LIMIT times, the containing
 * function will return the negative error code
 * #BSP430_I2C_ERRFLAG_SPINLIMIT.
 *
 * If #BSP430_I2C_SPIN_LIMIT is zero or negative, the loop will
 * execute an unbounded number of times until @a c_ becomes false.
 */
#if defined(BSP430_DOXYGEN) || (0 < BSP430_I2C_SPIN_LIMIT)
#define I2C_ERRCHECK_SPIN_WHILE_COND(c_) do {                   \
    unsigned int limit = (unsigned int)BSP430_I2C_SPIN_LIMIT;   \
    while (1) {                                                 \
      I2C_ERRCHECK_RETURN();                                    \
      if (! (c_)) {                                             \
        break;                                                  \
      }                                                         \
      if (0 == --limit) {                                       \
        return -BSP430_I2C_ERRFLAG_SPINLIMIT;                   \
      }                                                         \
    }                                                           \
  } while (0)
#else
#define I2C_ERRCHECK_SPIN_WHILE_COND(c_) do {                   \
    I2C_ERRCHECK_RETURN();                                      \
  } while (c_);
#endif

int
iBSP430usciI2CrxData_ni (hBSP430halSERIAL hal,
                         uint8_t * data,
                         size_t len)
{
  volatile struct sBSP430hplUSCI * hpl = SERIAL_HAL_HPL(hal);
  struct sBSP430usciHPLAux * aux = SERIAL_HAL_HPLAUX(hal);
  uint8_t * dp = data;
  const uint8_t * dpe = data + len;

  /* Check for errors while waiting for any in-progress activity to
   * complete */
  I2C_ERRCHECK_SPIN_WHILE_COND(hpl->stat & UCBUSY);

  /* Set for receive */
  hpl->ctl1 &= ~UCTR;

  /* Issue a start */
  hpl->ctl1 |= UCTXSTT;
  while (dp < dpe) {
    if (dpe == (dp+1)) {
      /* This will be last character: wait for any in-progress start
       * to complete then issue stop to be transmitted with next
       * receive. */
      if (hpl->ctl1 & UCTXSTT) {
        I2C_ERRCHECK_SPIN_WHILE_COND(hpl->ctl1 & UCTXSTT);
      }
      hpl->ctl1 |= UCTXSTP;
    }
    I2C_ERRCHECK_SPIN_WHILE_COND(! (aux->rx_bit & *aux->ifgp));
    *dp++ = hpl->rxbuf;
    ++hal->num_rx;
  }

  /* Wait for STP transmission to complete */
  I2C_ERRCHECK_SPIN_WHILE_COND(hpl->ctl1 & UCTXSTP);

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

  /* Check for errors while waiting for any in-progress activity to
   * complete */
  I2C_ERRCHECK_SPIN_WHILE_COND(hpl->stat & UCBUSY);

  /* Issue a start for transmit */
  hpl->ctl1 |= UCTR | UCTXSTT;

  /* Send the data. */
  while (i < len) {
    I2C_ERRCHECK_SPIN_WHILE_COND(! (aux->tx_bit & *aux->ifgp));
    hpl->txbuf = data[i++];
    ++hal->num_tx;
  }

  /* Wait for any queued data to be transmitted before we stop, lest
   * it get dropped. */
  I2C_ERRCHECK_SPIN_WHILE_COND(! (aux->tx_bit & *aux->ifgp));

  /* Send the stop and wait for it to complete. */
  hpl->ctl1 |= UCTXSTP;
  I2C_ERRCHECK_SPIN_WHILE_COND(hpl->ctl1 & UCTXSTP);

  return i;
}

#if BSP430_SERIAL - 0
static struct sBSP430serialDispatch dispatch_ = {
#if configBSP430_SERIAL_ENABLE_UART - 0
  .openUART = hBSP430usciOpenUART,
  .uartRxByte_ni = iBSP430usciUARTrxByte_ni,
  .uartTxByte_ni = iBSP430usciUARTtxByte_ni,
  .uartTxData_ni = iBSP430usciUARTtxData_ni,
  .uartTxASCIIZ_ni = iBSP430usciUARTtxASCIIZ_ni,
#endif /* configBSP430_SERIAL_ENABLE_UART */
#if configBSP430_SERIAL_ENABLE_SPI - 0
  .openSPI = hBSP430usciOpenSPI,
  .spiTxRx_ni = iBSP430usciSPITxRx_ni,
#endif /* configBSP430_SERIAL_ENABLE_SPI */
#if configBSP430_SERIAL_ENABLE_I2C - 0
  .openI2C = hBSP430usciOpenI2C,
  .i2cSetAddresses_ni = iBSP430usciI2CsetAddresses_ni,
  .i2cRxData_ni = iBSP430usciI2CrxData_ni,
  .i2cTxData_ni = iBSP430usciI2CtxData_ni,
#endif /* configBSP430_SERIAL_ENABLE_I2C */
  .setReset_ni = vBSP430usciSetReset_ni,
  .setHold_ni = iBSP430usciSetHold_ni,
  .close = iBSP430usciClose,
  .wakeupTransmit_ni = vBSP430usciWakeupTransmit_ni,
  .flush_ni = vBSP430usciFlush_ni,
};
#endif /* BSP430_SERIAL */

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
#if configBSP430_HAL_USCI_AB0RX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI_AB0RX_ISR */
#if configBSP430_HAL_USCI_AB0TX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_USCI_AB0TX_ISR */
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
#if configBSP430_HAL_USCI_AB1RX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI_AB0RX_ISR */
#if configBSP430_HAL_USCI_AB1TX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_USCI_AB0TX_ISR */
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
#if configBSP430_HAL_USCI_AB0RX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI_AB0RX_ISR */
#if configBSP430_HAL_USCI_AB0TX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_USCI_AB0TX_ISR */
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
#if configBSP430_HAL_USCI_AB1RX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI_AB0RX_ISR */
#if configBSP430_HAL_USCI_AB1TX_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_USCI_AB0TX_ISR */
  },
  .hpl = { .usci = BSP430_HPL_USCI_B1 },
  .hpl_aux = { .usci = &xBSP430hplaux_USCI_B1_ },
  .dispatch = &dispatch_,
};
#endif /* configBSP430_HAL_USCI_B1 */

#if ((configBSP430_HAL_USCI_AB0RX_ISR - 0) || (configBSP430_HAL_USCI_AB1RX_ISR - 0))
static int
#if (20120406 < __MSPGCC__) && (__MSP430X__ - 0)
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usciabrx_isr (hBSP430halSERIAL hal)
{
  hal->rx_byte = SERIAL_HAL_HPL(hal)->rxbuf;
  ++hal->num_rx;
  return iBSP430callbackInvokeISRVoid_ni(&hal->rx_cbchain_ni, hal, 0);
}

#if configBSP430_HAL_USCI_AB0RX_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCIAB0RX_VECTOR)
isr_USCI_AB0RX (void)
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
#endif /* HAL USCI_AB0RX ISR */

#if configBSP430_HAL_USCI_AB1RX_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCIAB1RX_VECTOR)
isr_USCI_AB1RX (void)
{
  hBSP430halSERIAL usci = NULL;
  int rv = 0;

  if (0) {
  }
#if configBSP430_HAL_USCI_A1 - 0
  else if (SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_A1)->rx_bit & *(SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_A1)->ifgp)) {
    usci = BSP430_HAL_USCI_A1;
  }
#endif /* configBSP430_HAL_USCI_A1 */
#if configBSP430_HAL_USCI_B1 - 0
  else if (SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_B1)->rx_bit & *(SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_B1)->ifgp)) {
    usci = BSP430_HAL_USCI_B1;
  }
#endif /* configBSP430_HAL_USCI_B1 */
  if (usci) {
    rv = usciabrx_isr(usci);
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* HAL USCI_AB1RX ISR */

#endif  /* HAL USCI_ABxRX ISR */

#if ((configBSP430_HAL_USCI_AB0TX_ISR - 0) || (configBSP430_HAL_USCI_AB1TX_ISR - 0))
static int
#if (20120406 < __MSPGCC__) && (__MSP430X__ - 0)
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usciabtx_isr (hBSP430halSERIAL hal)
{
  int rv = iBSP430callbackInvokeISRVoid_ni(&hal->tx_cbchain_ni, hal, 0);
  int did_tx = 0;
  if (rv & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN) {
    /* Found some data; send it out */
    ++hal->num_tx;
    SERIAL_HAL_HPL(hal)->txbuf = hal->tx_byte;
    did_tx = 1;
  } else {
    /* No data; mark transmission disabled */
    rv |= BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
  }
  /* If no more is expected, clear the interrupt so we don't wake
   * again.  Further, if we didn't transmit anything mark that the
   * function is ready so when the interrupt is next set it will
   * fire. */
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    *SERIAL_HAL_HPLAUX(hal)->iep &= ~SERIAL_HAL_HPLAUX(hal)->tx_bit;
    if (! did_tx) {
      *SERIAL_HAL_HPLAUX(hal)->ifgp |= SERIAL_HAL_HPLAUX(hal)->tx_bit;
    }
  }
  return rv;
}

#if configBSP430_HAL_USCI_AB0TX_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCIAB0TX_VECTOR)
isr_USCI_AB0TX (void)
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
#endif /* HAL USCI_AB0TX ISR */

#if configBSP430_HAL_USCI_AB1TX_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCIAB1TX_VECTOR)
isr_USCI_AB1TX (void)
{
  int rv = 0;
  hBSP430halSERIAL usci = NULL;

  if (0) {
  }
#if configBSP430_HAL_USCI_A1 - 0
  else if (SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_A1)->tx_bit & *(SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_A1)->ifgp)) {
    usci = BSP430_HAL_USCI_A1;
  }
#endif /* configBSP430_HAL_USCI_A1 */
#if configBSP430_HAL_USCI_B1 - 0
  else if (SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_B1)->tx_bit & *(SERIAL_HAL_HPLAUX(BSP430_HAL_USCI_B1)->ifgp)) {
    usci = BSP430_HAL_USCI_B1;
  }
#endif /* configBSP430_HAL_USCI_B1 */
  if (usci) {
    rv = usciabtx_isr(usci);
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* HAL USCI_AB1TX ISR */

#endif  /* HAL USCI_ABxTX ISR */

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
