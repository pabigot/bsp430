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
#include <bsp430/periph/usci5.h>

/* !BSP430! periph=usci5 */
/* !BSP430! instance=USCI5_A0,USCI5_A1,USCI5_A2,USCI5_A3,USCI5_B0,USCI5_B1,USCI5_B2,USCI5_B3 */

#define SERIAL_HAL_HPL(_hal) ((_hal)->hpl.usci5)

#define SERIAL_HPL_WAKEUP_TRANSMIT_NI(_hpl) do {        \
    _hpl->ie |= UCTXIE;                                 \
  } while (0)

#define SERIAL_HPL_RAW_TRANSMIT_NI(_hpl, _c) do {       \
    while (! (_hpl->ifg & UCTXIFG)) {                   \
      ;                                                 \
    }                                                   \
    _hpl->txbuf = _c;                                   \
  } while (0)

#define SERIAL_HPL_RAW_RECEIVE_NI(_hpl, _c) do {        \
    while (! (_hpl->ifg & UCRXIFG)) {                   \
      ;                                                 \
    }                                                   \
    _c = _hpl->rxbuf;                                   \
  } while (0)

#define SERIAL_HPL_FLUSH_NI(_hpl) do {          \
    while (_hpl->stat & UCBUSY) {               \
      ;                                         \
    }                                           \
  } while (0)

#define SERIAL_HPL_RESET_NI(_hpl) do {          \
    (_hpl)->ctlw0 = UCSWRST;                    \
  } while (0)

#define SERIAL_HPL_HOLD_HPL_NI(_hpl) do {       \
    (_hpl)->ctlw0 |= UCSWRST;                   \
  } while (0)

#define SERIAL_HPL_RELEASE_HPL_NI(_hal,_hpl) do {       \
    (_hpl)->ctlw0 &= ~UCSWRST;                          \
    if ((_hal)->rx_cbchain_ni) {                        \
      (_hpl)->ie |= UCRXIE;                             \
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

static
hBSP430halSERIAL
usci5Configure (hBSP430halSERIAL hal,
                unsigned char ctl0_byte,
                unsigned char ctl1_byte,
                unsigned int brw,
                int mctl)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  SERIAL_HAL_HPL(hal)->ctlw0 = UCSWRST;
  do {
    int periph_config = peripheralConfigFlag(ctl0_byte);

    /* Reject if the pins can't be configured */
    if (0 != iBSP430platformConfigurePeripheralPins_ni((tBSP430periphHandle)(uintptr_t)(SERIAL_HAL_HPL(hal)), periph_config, 1)) {
      hal = NULL;
      break;
    }
    SERIAL_HAL_HPL(hal)->ctlw0 |= (ctl0_byte << 8) | ctl1_byte;
    SERIAL_HAL_HPL(hal)->brw = brw;
    if (0 < mctl) {
      SERIAL_HAL_HPL(hal)->mctl = mctl;
    }

    /* Reset device statistics */
    hal->num_rx = hal->num_tx = 0;

    /* Attempt to release the device for use; if that failed, reset it
     * and return an error */
    if (0 != iBSP430usci5SetHold_ni(hal, 0)) {
      SERIAL_HAL_HPL(hal)->ctlw0 = UCSWRST;
      hal = NULL;
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return hal;
}

hBSP430halSERIAL
hBSP430usci5OpenUART (hBSP430halSERIAL hal,
                      unsigned char ctl0_byte,
                      unsigned char ctl1_byte,
                      unsigned long baud)
{
  unsigned long brclk_Hz = 0;
  uint16_t brw = 0;
  uint16_t brs = 0;

  /* Reject unsupported HALs */
  if (NULL == hal) {
    return NULL;
  }

  /* Reject invalid baud rates */
  if ((0 == baud) || (BSP430_USCI5_UART_MAX_BAUD < baud)) {
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

  return usci5Configure(hal, ctl0_byte, ctl1_byte, brw, (0 * UCBRF0) | (brs * UCBRS0));
}

hBSP430halSERIAL
hBSP430usci5OpenSPI (hBSP430halSERIAL hal,
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

  return usci5Configure(hal, ctl0_byte, ctl1_byte, prescaler, -1);
}

hBSP430halSERIAL
hBSP430usci5OpenI2C (hBSP430halSERIAL hal,
                     unsigned char ctl0_byte,
                     unsigned char ctl1_byte,
                     unsigned int prescaler)
{
  /* Reject unsupported HALs */
  if (NULL == hal) {
    return NULL;
  }
  /* Calculate default prescaler */
  if (0 == prescaler) {
    prescaler = uiBSP430serialSMCLKPrescaler(BSP430_SERIAL_I2C_BUS_SPEED_HZ);
    ctl1_byte |= UCSSEL1;
  }

  /* I2C is synchronous mode 3 */
  ctl0_byte |= UCMODE_3 | UCSYNC;

  return usci5Configure(hal, ctl0_byte, ctl1_byte, prescaler, -1);
}

int
iBSP430usci5SetHold_ni (hBSP430halSERIAL hal,
                        int holdp)
{
  int rc;
  int periph_config = peripheralConfigFlag(SERIAL_HAL_HPL(hal)->ctl0);

  SERIAL_HPL_FLUSH_NI(SERIAL_HAL_HPL(hal));
  if (holdp) {
    SERIAL_HAL_HPL(hal)->ctlw0 |= UCSWRST;
    rc = iBSP430platformConfigurePeripheralPins_ni(xBSP430periphFromHPL(hal->hpl.any), periph_config, 0);
  } else {
    rc = iBSP430platformConfigurePeripheralPins_ni(xBSP430periphFromHPL(hal->hpl.any), periph_config, 1);
    if (0 == rc) {
      SERIAL_HAL_HPL(hal)->ctlw0 &= ~UCSWRST;
      SERIAL_HPL_RELEASE_HPL_NI(hal, SERIAL_HAL_HPL(hal));
    }
  }
  return rc;
}

int
iBSP430usci5Close (hBSP430halSERIAL hal)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  SERIAL_HPL_FLUSH_NI(SERIAL_HAL_HPL(hal));
  SERIAL_HPL_RESET_NI(SERIAL_HAL_HPL(hal));
  rc = iBSP430platformConfigurePeripheralPins_ni((tBSP430periphHandle)(uintptr_t)(SERIAL_HAL_HPL(hal)),
                                                 peripheralConfigFlag(SERIAL_HAL_HPL(hal)->ctl0),
                                                 0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return rc;
}

void
vBSP430usci5Flush_ni (hBSP430halSERIAL hal)
{
  SERIAL_HPL_FLUSH_NI(SERIAL_HAL_HPL(hal));
}

void
vBSP430usci5WakeupTransmit_ni (hBSP430halSERIAL hal)
{
  SERIAL_HPL_WAKEUP_TRANSMIT_NI(SERIAL_HAL_HPL(hal));
}

int
iBSP430usci5UARTrxByte_ni (hBSP430halSERIAL hal)
{
  if (hal->rx_cbchain_ni) {
    return -1;
  }
  if (SERIAL_HAL_HPL(hal)->ifg & UCRXIFG) {
    ++hal->num_rx;
    return SERIAL_HAL_HPL(hal)->rxbuf;
  }
  return -1;
}

int
iBSP430usci5UARTtxByte_ni (hBSP430halSERIAL hal, uint8_t c)
{
  if (hal->tx_cbchain_ni) {
    return -1;
  }
  SERIAL_HPL_RAW_TRANSMIT_NI(SERIAL_HAL_HPL(hal), c);
  ++hal->num_tx;
  return c;
}

int
iBSP430usci5UARTtxData_ni (hBSP430halSERIAL hal,
                           const uint8_t * data,
                           size_t len)
{
  const uint8_t * p = data;
  const uint8_t * edata = data + len;
  if (hal->tx_cbchain_ni) {
    return -1;
  }
  while (p < edata) {
    SERIAL_HPL_RAW_TRANSMIT_NI(SERIAL_HAL_HPL(hal), *p++);
    ++hal->num_tx;
  }
  return p - data;
}

int
iBSP430usci5UARTtxASCIIZ_ni (hBSP430halSERIAL hal, const char * str)
{
  const char * in_string = str;

  if (hal->tx_cbchain_ni) {
    return -1;
  }
  while (*str) {
    SERIAL_HPL_RAW_TRANSMIT_NI(SERIAL_HAL_HPL(hal), *str);
    ++hal->num_tx;
    ++str;
  }
  return str - in_string;
}

int
iBSP430usci5SPITxRx_ni (hBSP430halSERIAL hal,
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
    SERIAL_HPL_RAW_TRANSMIT_NI(SERIAL_HAL_HPL(hal), txd);
    ++hal->num_tx;
    SERIAL_HPL_RAW_RECEIVE_NI(SERIAL_HAL_HPL(hal), *rxp);
    if (rx_data) {
      ++rxp;
    }
    ++i;
  }
  return i;
}

int
iBSP430usci5I2CsetAddresses_ni (hBSP430halSERIAL hal,
                                int own_address,
                                int slave_address)
{
  if (0 <= own_address) {
    SERIAL_HAL_HPL(hal)->i2coa = own_address;
  }
  if (0 <= slave_address) {
    SERIAL_HAL_HPL(hal)->i2csa = slave_address;
  }
  return 0;
}

int
iBSP430usci5I2CrxData_ni (hBSP430halSERIAL hal,
                          uint8_t * data,
                          size_t len)
{
  volatile struct sBSP430hplUSCI5 * hpl = SERIAL_HAL_HPL(hal);
  uint8_t * dp = data;
  const uint8_t * dpe = data + len;

  /* Set for receive */
  hpl->ctl1 &= ~UCTR;
  /* Delay for any in-progress stop to complete */
  do {
    if (hpl->ifg & (UCNACKIFG | UCALIFG)) {
      return -1;
    }
  } while (hpl->ctl1 & UCTXSTP);

  /* Issue a start */
  hpl->ctl1 |= UCTXSTT;
  while (dp < dpe) {
    if (dpe == (dp+1)) {
      /* This will be last character: wait for any in-progress start
       * to complete then issue stop */
      if (hpl->ctl1 & UCTXSTT) {
        do {
          if (hpl->ifg & (UCNACKIFG | UCALIFG)) {
            return -1;
          }
        } while (hpl->ctl1 & UCTXSTT);
      }
      hpl->ctl1 |= UCTXSTP;
    }
    do {
      if (hpl->ifg & (UCNACKIFG | UCALIFG)) {
        return -1;
      }
    } while (! (hpl->ifg & UCRXIFG));
    ++hal->num_rx;
    *dp++ = hpl->rxbuf;
  }
  return dp - data;
}

int
iBSP430usci5I2CtxData_ni (hBSP430halSERIAL hal,
                          const uint8_t * data,
                          size_t len)
{
  volatile struct sBSP430hplUSCI5 * hpl = SERIAL_HAL_HPL(hal);
  int i = 0;

  /* Delay for any in-progress stop to complete */
  do {
    if (hpl->ifg & (UCNACKIFG | UCALIFG)) {
      return -1;
    }
  } while (hpl->ctl1 & UCTXSTP);

  /* Issue a start for transmit */
  hpl->ctl1 |= UCTR | UCTXSTT;

  /* Send the data. */
  while (i < len) {
    do {
      if (hpl->ifg & (UCNACKIFG | UCALIFG)) {
        return -1;
      }
    } while (! (hpl->ifg & UCTXIFG));

    hpl->txbuf = data[i];
    ++i;
  }

  /* Wait for any queued data to be transmitted before we stop, lest
   * it get dropped. */
  do {
    if (hpl->ifg & (UCNACKIFG | UCALIFG)) {
      return -1;
    }
  } while (! (hpl->ifg & UCTXIFG));

  /* Send the stop. */
  hpl->ctl1 |= UCTXSTP;
  return i;
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
#if (20120406 < __MSPGCC__) && (__MSP430X__ - 0)
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usci5_isr (hBSP430halSERIAL hal)
{
  int did_tx;
  int rv = 0;

  switch (SERIAL_HAL_HPL(hal)->iv) {
    default:
    case USCI_NONE:
      break;
    case USCI_UCTXIFG:
      rv = iBSP430callbackInvokeISRVoid_ni(&hal->tx_cbchain_ni, hal, 0);
      did_tx = 0;
      if (rv & BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN) {
        /* Found some data; send it out */
        ++hal->num_tx;
        did_tx = 1;
        SERIAL_HAL_HPL(hal)->txbuf = hal->tx_byte;
      } else {
        /* No data; disable transmission interrupt */
        rv |= BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
      }
      /* If no more is expected, clear the interrupt so we don't wake
       * again.  Further, if we didn't transmit anything mark that the
       * function is ready so when the interrupt is next set it will
       * fire. */
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        SERIAL_HAL_HPL(hal)->ie &= ~UCTXIE;
        if (! did_tx) {
          SERIAL_HAL_HPL(hal)->ifg |= UCTXIFG;
        }
      }
      break;
    case USCI_UCRXIFG:
      hal->rx_byte = SERIAL_HAL_HPL(hal)->rxbuf;
      ++hal->num_rx;
      rv = iBSP430callbackInvokeISRVoid_ni(&hal->rx_cbchain_ni, hal, 0);
      break;
  }
  return rv;
}
#endif  /* HAL ISR */

#if BSP430_SERIAL - 0
static struct sBSP430serialDispatch dispatch_ = {
#if configBSP430_SERIAL_ENABLE_UART - 0
  .openUART = hBSP430usci5OpenUART,
  .uartRxByte_ni = iBSP430usci5UARTrxByte_ni,
  .uartTxByte_ni = iBSP430usci5UARTtxByte_ni,
  .uartTxData_ni = iBSP430usci5UARTtxData_ni,
  .uartTxASCIIZ_ni = iBSP430usci5UARTtxASCIIZ_ni,
#endif /* configBSP430_SERIAL_ENABLE_UART */
#if configBSP430_SERIAL_ENABLE_SPI - 0
  .openSPI = hBSP430usci5OpenSPI,
  .spiTxRx_ni = iBSP430usci5SPITxRx_ni,
#endif /* configBSP430_SERIAL_ENABLE_SPI */
#if configBSP430_SERIAL_ENABLE_I2C - 0
  .openI2C = hBSP430usci5OpenI2C,
  .i2cSetAddresses_ni = iBSP430usci5I2CsetAddresses_ni,
  .i2cRxData_ni = iBSP430usci5I2CrxData_ni,
  .i2cTxData_ni = iBSP430usci5I2CtxData_ni,
#endif /* configBSP430_SERIAL_ENABLE_I2C */
  .setHold_ni = iBSP430usci5SetHold_ni,
  .close = iBSP430usci5Close,
  .wakeupTransmit_ni = vBSP430usci5WakeupTransmit_ni,
  .flush_ni = vBSP430usci5Flush_ni,
};
#endif /* BSP430_SERIAL */

/* !BSP430! insert=hal_serial_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_serial_defn] */
#if configBSP430_HAL_USCI5_A0 - 0
struct sBSP430halSERIAL xBSP430hal_USCI5_A0_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI5
#if configBSP430_HAL_USCI5_A0_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI5_A0_ISR */
  },
  .hpl = { .usci5 = BSP430_HPL_USCI5_A0 },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_USCI5_A0 */

#if configBSP430_HAL_USCI5_A1 - 0
struct sBSP430halSERIAL xBSP430hal_USCI5_A1_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI5
#if configBSP430_HAL_USCI5_A1_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI5_A1_ISR */
  },
  .hpl = { .usci5 = BSP430_HPL_USCI5_A1 },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_USCI5_A1 */

#if configBSP430_HAL_USCI5_A2 - 0
struct sBSP430halSERIAL xBSP430hal_USCI5_A2_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI5
#if configBSP430_HAL_USCI5_A2_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI5_A2_ISR */
  },
  .hpl = { .usci5 = BSP430_HPL_USCI5_A2 },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_USCI5_A2 */

#if configBSP430_HAL_USCI5_A3 - 0
struct sBSP430halSERIAL xBSP430hal_USCI5_A3_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI5
#if configBSP430_HAL_USCI5_A3_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI5_A3_ISR */
  },
  .hpl = { .usci5 = BSP430_HPL_USCI5_A3 },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_USCI5_A3 */

#if configBSP430_HAL_USCI5_B0 - 0
struct sBSP430halSERIAL xBSP430hal_USCI5_B0_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI5
#if configBSP430_HAL_USCI5_B0_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI5_B0_ISR */
  },
  .hpl = { .usci5 = BSP430_HPL_USCI5_B0 },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_USCI5_B0 */

#if configBSP430_HAL_USCI5_B1 - 0
struct sBSP430halSERIAL xBSP430hal_USCI5_B1_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI5
#if configBSP430_HAL_USCI5_B1_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI5_B1_ISR */
  },
  .hpl = { .usci5 = BSP430_HPL_USCI5_B1 },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_USCI5_B1 */

#if configBSP430_HAL_USCI5_B2 - 0
struct sBSP430halSERIAL xBSP430hal_USCI5_B2_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI5
#if configBSP430_HAL_USCI5_B2_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI5_B2_ISR */
  },
  .hpl = { .usci5 = BSP430_HPL_USCI5_B2 },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_USCI5_B2 */

#if configBSP430_HAL_USCI5_B3 - 0
struct sBSP430halSERIAL xBSP430hal_USCI5_B3_ = {
  .hal_state = {
    .cflags = BSP430_SERIAL_HAL_HPL_VARIANT_USCI5
#if configBSP430_HAL_USCI5_B3_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_USCI5_B3_ISR */
  },
  .hpl = { .usci5 = BSP430_HPL_USCI5_B3 },
#if BSP430_SERIAL - 0
  .dispatch = &dispatch_,
#endif /* BSP430_SERIAL */
};
#endif /* configBSP430_HAL_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [hal_serial_defn] */
/* !BSP430! end=hal_serial_defn */

/* !BSP430! uscifrom=usci5 insert=hal_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_defn] */
#if configBSP430_HAL_USCI5_A0_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCI_A0_VECTOR)
isr_USCI5_A0 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_A0);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_A0_ISR */

#if configBSP430_HAL_USCI5_A1_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCI_A1_VECTOR)
isr_USCI5_A1 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_A1);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_A1_ISR */

#if configBSP430_HAL_USCI5_A2_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCI_A2_VECTOR)
isr_USCI5_A2 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_A2);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_A2_ISR */

#if configBSP430_HAL_USCI5_A3_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCI_A3_VECTOR)
isr_USCI5_A3 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_A3);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_A3_ISR */

#if configBSP430_HAL_USCI5_B0_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCI_B0_VECTOR)
isr_USCI5_B0 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_B0);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_B0_ISR */

#if configBSP430_HAL_USCI5_B1_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCI_B1_VECTOR)
isr_USCI5_B1 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_B1);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_B1_ISR */

#if configBSP430_HAL_USCI5_B2_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCI_B2_VECTOR)
isr_USCI5_B2 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_B2);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_B2_ISR */

#if configBSP430_HAL_USCI5_B3_ISR - 0
BSP430_CORE_DECLARE_INTERRUPT(USCI_B3_VECTOR)
isr_USCI5_B3 (void)
{
  int rv = usci5_isr(BSP430_HAL_USCI5_B3);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_USCI5_B3_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_defn] */
/* !BSP430! end=hal_isr_defn */

const char *
xBSP430usci5Name (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_name_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_name_demux] */
#if configBSP430_HPL_USCI5_A0 - 0
  if (BSP430_PERIPH_USCI5_A0 == periph) {
    return "USCI5_A0";
  }
#endif /* configBSP430_HPL_USCI5_A0 */

#if configBSP430_HPL_USCI5_A1 - 0
  if (BSP430_PERIPH_USCI5_A1 == periph) {
    return "USCI5_A1";
  }
#endif /* configBSP430_HPL_USCI5_A1 */

#if configBSP430_HPL_USCI5_A2 - 0
  if (BSP430_PERIPH_USCI5_A2 == periph) {
    return "USCI5_A2";
  }
#endif /* configBSP430_HPL_USCI5_A2 */

#if configBSP430_HPL_USCI5_A3 - 0
  if (BSP430_PERIPH_USCI5_A3 == periph) {
    return "USCI5_A3";
  }
#endif /* configBSP430_HPL_USCI5_A3 */

#if configBSP430_HPL_USCI5_B0 - 0
  if (BSP430_PERIPH_USCI5_B0 == periph) {
    return "USCI5_B0";
  }
#endif /* configBSP430_HPL_USCI5_B0 */

#if configBSP430_HPL_USCI5_B1 - 0
  if (BSP430_PERIPH_USCI5_B1 == periph) {
    return "USCI5_B1";
  }
#endif /* configBSP430_HPL_USCI5_B1 */

#if configBSP430_HPL_USCI5_B2 - 0
  if (BSP430_PERIPH_USCI5_B2 == periph) {
    return "USCI5_B2";
  }
#endif /* configBSP430_HPL_USCI5_B2 */

#if configBSP430_HPL_USCI5_B3 - 0
  if (BSP430_PERIPH_USCI5_B3 == periph) {
    return "USCI5_B3";
  }
#endif /* configBSP430_HPL_USCI5_B3 */

  /* END AUTOMATICALLY GENERATED CODE [periph_name_demux] */
  /* !BSP430! end=periph_name_demux */
  return NULL;
}
