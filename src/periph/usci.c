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

/** Convert from a raw peripheral handle to the corresponding USCI
 * device handle. */
hBSP430halSERIAL xBSP430usciPeriphToHal (tBSP430periphHandle periph);

#define SERIAL_HAL_HPL(_hal) BSP430_SERIAL_HAL_GET_HPL_USCI(_hal)
#define SERIAL_HAL_HPLAUX(_hal) BSP430_SERIAL_HAL_GET_HPLAUX_USCI(_hal)

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
};

hBSP430halSERIAL
xBSP430usciOpenUART (hBSP430halSERIAL hal,
                     unsigned char ctl0_byte,
                     unsigned char ctl1_byte,
                     unsigned long baud)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  unsigned long brclk_Hz = 0;
  uint16_t br = 0;
  uint16_t brs = 0;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  /* Reject invalid baud rates */
  if ((0 == baud) || (1000000UL < baud)) {
    hal = NULL;
  }

  /* Reject if the pins can't be configured */
  if ((NULL != hal)
      && (0 != iBSP430platformConfigurePeripheralPins_ni(xBSP430periphFromHPL(SERIAL_HAL_HPL(hal)), 1))) {
    hal = NULL;
  }

  if (NULL != hal) {
    /* Assume ACLK <= 20 kHz is VLOCLK and cannot be trusted.  Prefer
     * 32 kiHz ACLK for rates that are low enough.  Use SMCLK for
     * anything larger.  */
    brclk_Hz = usBSP430clockACLK_Hz_ni();
    ctl0_byte &= ~(UCMODE1 | UCMODE0 | UCSYNC);
    ctl1_byte &= ~(UCSSEL1 | UCSSEL0);
    ctl1_byte |= UCSWRST;
    if ((brclk_Hz > 20000) && (brclk_Hz >= (3 * baud))) {
      ctl1_byte |= UCSSEL_1;
    } else {
      ctl1_byte |= UCSSEL_2;
      brclk_Hz = ulBSP430clockSMCLK_Hz_ni();
    }
    SERIAL_HAL_HPL(hal)->ctl1 = ctl1_byte;
    SERIAL_HAL_HPL(hal)->ctl0 = ctl0_byte;    
    br = (brclk_Hz / baud);
    brs = (1 + 16 * (brclk_Hz - baud * br) / baud) / 2;

    SERIAL_HAL_HPL(hal)->br0 = br % 256;
    SERIAL_HAL_HPL(hal)->br1 = br / 256;
    SERIAL_HAL_HPL(hal)->mctl = (0 * UCBRF0) | (brs * UCBRS0);

    /* Mark the hal active */
    hal->num_rx = hal->num_tx = 0;

    /* Release the USCI and enable the interrupts.  Interrupts are
     * disabled and cleared when UCSWRST is set. */
    SERIAL_HAL_HPL(hal)->ctl1 &= ~UCSWRST;
    if (0 != hal->rx_callback) {
      *SERIAL_HAL_HPLAUX(hal)->iep |= SERIAL_HAL_HPLAUX(hal)->rx_bit;
    }
  }
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return hal;
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
iBSP430usciTransmitByte_ni (hBSP430halSERIAL hal, int c)
{
  if (hal->tx_callback) {
    return -1;
  }
  RAW_TRANSMIT_HAL_NI(hal, c);
  return c;
}

int
iBSP430usciTransmitData_ni (hBSP430halSERIAL hal,
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
iBSP430usciTransmitASCIIZ_ni (hBSP430halSERIAL hal, const char * str)
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

static struct sBSP430serialDispatch dispatch_ = {
  .openUART = xBSP430usciOpenUART,
  //  .openSPI = xBSP430usciOpenSPI,
  .configureCallbacks = iBSP430usciConfigureCallbacks,
  .close = iBSP430usciClose,
  .wakeupTransmit_ni = vBSP430usciWakeupTransmit_ni,
  .flush_ni = vBSP430usciFlush_ni,
  .transmitByte_ni = iBSP430usciTransmitByte_ni,
  .transmitData_ni = iBSP430usciTransmitData_ni,
  .transmitASCIIZ_ni = iBSP430usciTransmitASCIIZ_ni
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
