/* Copyright (c) 2012, Peter A. Bigot
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

/**
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/gps.h>
#include <bsp430/sensors/skytraq.h>
#include <fragpool/fragpool.h>
#include <string.h>

static hBSP430halSERIAL uart_hal;
static iBSP430gpsSerialCallback_ni serial_cb;
static iBSP430gpsPPSCallback_ni pps_cb;
static int gpsToUtcOffset_s_ = BSP430_GPS_GPS_UTC_OFFSET_S;

int
iBSP430gpsGPStoUTCOffset_s_ni (void)
{
  return gpsToUtcOffset_s_;
}

void
vBSP430gpsGPStoUTCOffset_s_ni (int gps_to_utc_offset_s)
{
  gpsToUtcOffset_s_ = gps_to_utc_offset_s;
}

time_t
xBSP430gpsConvertGPStoUTC_ni (unsigned int weekno,
                              unsigned long sow)
{
  const uint32_t EPOCH_UNIX_GPS = BSP430_GPS_EPOCH_POSIX;
  const uint32_t SECONDS_PER_WEEK = 7 * 24 * (60 * 60UL);

  return (time_t)EPOCH_UNIX_GPS + SECONDS_PER_WEEK * weekno + sow + gpsToUtcOffset_s_;
}

static uint8_t nmea_rx_data[SKYTRAQ_NMEA_RX_POOL_SIZE];

static union {
  struct {
    FP_POOL_STRUCT_COMMON;
    struct fp_fragment_t fragment[SKYTRAQ_NMEA_RX_POOL_FRAGMENTS];
  } fixed;
  struct fp_pool_t generic;
} nmea_rx_pool_union = {
  .generic = {
    .pool_start = nmea_rx_data,
    .pool_end = nmea_rx_data + sizeof(nmea_rx_data),
    .pool_alignment = 1,
    .fragment_count = SKYTRAQ_NMEA_RX_POOL_FRAGMENTS
  }
};

typedef enum eRxState {
  SRX_unsync,
  SRX_start_Binary,
  SRX_store_NMEA,
  SRX_store_Binary,
  SRX_read_csum_NMEA,
  SRX_read_csum_NMEA_2,
  SRX_read_len_Binary,
  SRX_read_len_Binary_2,
  SRX_read_csum_Binary,
} eRxState;

typedef struct sNMEAmessage {
  /** The time at which the message started being received.  This is
   * the uptime clock at the point the first character of the
   * start-of-sentence sequence was processed by the RX interrupt
   * handler. */
  unsigned long timestamp_utt;

  /** The fragment pool entry for the message. */
  uint8_t * message;

  /** A pointer to one octet past the end of the message. */
  uint8_t * message_endp;
} sNMEAmessage;

typedef struct sRxState {
  /** Callback for interrupt connection */
  sBSP430halISRVoidChainNode cb_node;

  /** The pool used to allocate received messages */
  fp_pool_t const pool;

  /** RX machine state */
  eRxState state;

  /** Checksum of data being accumulated.  Reset on receipt of
   * start-of-sentence. */
  uint8_t csum_calc;

  /** Checksum from received nmea message.  Set on receipt from
   * UART. */
  uint8_t csum_rx;

  /** Length of message.  Only applies to binary messages, set on
   * receipt. */
  fp_size_t length_rx;

  /** The message currently being read in.  If the machine is in
   * SRX_unsync, the message field will be null. */
  sNMEAmessage curmsg;

  /** Index into current message.  Reset on start-of-sentence,
   * incremented as data arrives. */
  fp_size_t message_idx;
} sRxState;

#define IS_HEXDIGIT(c_) ((('0' <= (c_)) && ((c_) <= '9')) || (('A' <= (c_)) && ((c_) <= 'F')))
#define HEXDIGIT_VALUE(c_) (((c_) <= '9') ? ((c_) - '0') : (10 + (c_) - 'A'))

static int
nmea_rx_isr_ni (const struct sBSP430halISRVoidChainNode * cb,
                void * context)
{
  sRxState * sp = (sRxState *)cb;
  sBSP430halSERIAL * hal = (sBSP430halSERIAL *) context;
  sNMEAmessage * cp = &sp->curmsg;
  int rv = 0;

  switch (sp->state) {
    case SRX_unsync:
      if (NULL != cp->message) {
        if (NULL != serial_cb) {
          rv |= serial_cb(NULL, sp->message_idx, cp->timestamp_utt);
        }
        (void)fp_release(sp->pool, cp->message);
        cp->message = NULL;
      }
      if ('$' == hal->rx_byte) {
        sp->state = SRX_store_NMEA;
      } else if (0xA0 == hal->rx_byte) {
        sp->state = SRX_start_Binary;
      } else {
        break;
      }
      cp->timestamp_utt = ulBSP430uptime_ni();
      sp->message_idx = 0;
      sp->csum_calc = 0;
      cp->message = fp_request(sp->pool, 4, FP_MAX_FRAGMENT_SIZE, &cp->message_endp);
      if (NULL == cp->message) {
        sp->state = SRX_unsync;
        if (NULL != serial_cb) {
          rv |= serial_cb(NULL, 0, cp->timestamp_utt);
        }
      }
      break;
    case SRX_store_NMEA:
      if ('*' == hal->rx_byte) {
        if ((cp->message + sp->message_idx) == cp->message_endp) {
          sp->state = SRX_unsync;
          break;
        }
        cp->message[sp->message_idx] = 0;
        sp->message_idx += 1;
        sp->state = SRX_read_csum_NMEA;
        break;
      }
      /*FALLTHRU*/
    case SRX_store_Binary:
      /* If attempt to store would go beyond end, abort the message. */
      if ((cp->message + sp->message_idx) == cp->message_endp) {
        sp->state = SRX_unsync;
        break;
      }
      /* Add character to checksum and accumulated buffer */
      sp->csum_calc ^= hal->rx_byte;
      cp->message[sp->message_idx] = hal->rx_byte;
      sp->message_idx += 1;
      if ((SRX_store_Binary == sp->state) && (sp->message_idx == sp->length_rx)) {
        sp->state = SRX_read_csum_Binary;
      }
      break;
    case SRX_start_Binary:
      sp->length_rx = 0;
      sp->state = (0xA1 == hal->rx_byte) ? SRX_read_len_Binary : SRX_unsync;
      break;
    case SRX_read_csum_NMEA:
    case SRX_read_csum_NMEA_2:
      if (! IS_HEXDIGIT(hal->rx_byte)) {
        sp->state = SRX_unsync;
        break;
      }
      sp->csum_rx = (sp->csum_rx << 4) | HEXDIGIT_VALUE(hal->rx_byte);
      if (SRX_read_csum_NMEA == sp->state) {
        sp->state = SRX_read_csum_NMEA_2;
        break;
      }
      /*FALLTHRU*/
validate:
      if (sp->csum_rx == sp->csum_calc) {
        if (NULL != serial_cb) {
          uint8_t * msg = fp_resize(sp->pool, cp->message, sp->message_idx, &cp->message_endp);
          cp->message = NULL;
          rv |= serial_cb(msg, sp->message_idx, cp->timestamp_utt);
        }
      }
      sp->state = SRX_unsync;
      break;
    case SRX_read_csum_Binary:
      sp->csum_rx = hal->rx_byte;
      goto validate;
    case SRX_read_len_Binary:
      sp->length_rx = hal->rx_byte;
      sp->state = SRX_read_len_Binary_2;
      break;
    case SRX_read_len_Binary_2:
      sp->length_rx = (sp->length_rx << 8) | hal->rx_byte;
      sp->state = SRX_store_Binary;
      break;
  }
  return rv;
}

static sRxState rx_state_ = {
  .cb_node = { .callback = nmea_rx_isr_ni },
  .pool = &nmea_rx_pool_union.generic,
};

typedef enum eTxState {
  STX_idle,
  STX_sos_1,
  STX_sos_2,
  STX_len_1,
  STX_len_2,
  STX_message,
  STX_csum,
  STX_cr,
  STX_lf,
} eTxState;

typedef struct sTxState {
  /** Callback for interrupt connection */
  sBSP430halISRVoidChainNode cb_node;

  /** TX machine state */
  eTxState state;

  /** Message being transmitted.  Valid except when #state is #STX_idle. */
  const uint8_t * msg;

  /** Index of next octet of msg to be transmitted */
  size_t idx;

  /** Total number of octets to write */
  size_t len;

  /** Callback to notify application when transmission completes */
  iBSP430gpsTransmitComplete_ni complete_cb;

  /** Checksum of message transmitted so far */
  uint8_t csum;
} sTxState;

static int
nmea_tx_isr_ni (const struct sBSP430halISRVoidChainNode * cb,
                void * context)
{
  sTxState * sp = (sTxState *)cb;
  sBSP430halSERIAL * hal = (sBSP430halSERIAL *) context;
  int rv = BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN;

  switch (sp->state) {
    case STX_sos_1:
      hal->tx_byte = 0xA0;
      sp->state = STX_sos_2;
      break;
    case STX_sos_2:
      hal->tx_byte = 0xA1;
      sp->state = STX_len_1;
      break;
    case STX_len_1:
      hal->tx_byte = (sp->len >> 8);
      sp->state = STX_len_2;
      break;
    case STX_len_2:
      hal->tx_byte = sp->len;
      sp->state = STX_message;
      sp->idx = 0;
      sp->csum = 0;
      break;
    case STX_message:
      hal->tx_byte = sp->msg[sp->idx];
      sp->csum ^= hal->tx_byte;
      sp->idx += 1;
      if (sp->idx == sp->len) {
        sp->state = STX_csum;
      }
      break;
    case STX_csum:
      hal->tx_byte = sp->csum;
      sp->state = STX_cr;
      break;
    case STX_cr:
      hal->tx_byte = 0x0d;
      sp->state = STX_lf;
      break;
    case STX_lf:
      hal->tx_byte = 0x0a;
      if (NULL != sp->complete_cb) {
        /* Keep default flags and add flags requested by the
         * application */
        rv |= sp->complete_cb(sp->msg, 0);
      }
      /*FALLTHRU*/
    default:
    case STX_idle:
      rv |= BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
      sp->msg = NULL;
      sp->state = STX_idle;
      break;
  }
  return rv;
}

static sTxState tx_state_ = {
  .cb_node = { .callback = nmea_tx_isr_ni },
};

int iBSP430gpsTransmit_ni (const uint8_t * msg,
                           size_t len,
                           iBSP430gpsTransmitComplete_ni complete_cb)
{
  if (STX_idle != tx_state_.state) {
    return -1;
  }
  tx_state_.msg = msg;
  tx_state_.len = len;
  tx_state_.idx = 0;
  tx_state_.state = STX_sos_1;
  tx_state_.complete_cb = complete_cb;
  vBSP430serialWakeupTransmit_ni(uart_hal);
  return 0;
}

void
vBSP430gpsReleaseMessage_ni (const uint8_t * msg)
{
  (void)fp_release(rx_state_.pool, msg);
}
struct sPpsState {
  sBSP430halISRIndexedChainNode cb;
};

static int
capture_isr_ni (const struct sBSP430halISRIndexedChainNode * cb,
                void * context,
                int idx)
{
  hBSP430halTIMER timer = (hBSP430halTIMER)context;
  int rv = 0;

  if (NULL != pps_cb) {
    volatile unsigned long now_tt;
    volatile unsigned long pps_tt;

    /* Adjust time from 16-bit to 32-bit value including overflow,
     * under assumption now_tt is not more than 65535 ticks past the
     * captured value.  Sign extension will adjust value if overflow
     * occurred between capture and now. */
    now_tt = ulBSP430timerCounter_ni(timer, NULL);
    pps_tt = now_tt + (int)(timer->hpl->ccr[idx] - (unsigned int)now_tt);
    rv = pps_cb(pps_tt);
  }
  return rv;
}

static struct sPpsState pps_state_ = {
  .cb = { .callback = capture_isr_ni },
};

int
iBSP430gpsInitialize_ni (const sBSP430gpsConfiguration * configp,
                         void * devconfigp)
{
  /* Request and configure the serial port. */
  uart_hal = hBSP430serialOpenUART(hBSP430serialLookup(configp->nmea_serial),
                                   0, 0, configp->nmea_baud);
  if (NULL == uart_hal) {
    return -1;
  }

  /* Put serial port into hold while callbacks configured */
  iBSP430serialSetHold_ni(uart_hal, 1);

  serial_cb = configp->serial_cb;
  pps_cb = configp->pps_cb;

  BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode,
                                  uart_hal->rx_cbchain_ni,
                                  rx_state_.cb_node,
                                  next_ni);

  BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode,
                                  uart_hal->tx_cbchain_ni,
                                  tx_state_.cb_node,
                                  next_ni);

  /* Configure 1PPS-related peripherals */
  if (BSP430_PERIPH_NONE != configp->pps_timer) {
    hBSP430halPORT pps_port_hal;
    hBSP430halTIMER pps_timer_hal;
    unsigned int bit;

    pps_port_hal = hBSP430portLookup(configp->pps_port);
    if (NULL == pps_port_hal) {
      return -1;
    }
    pps_timer_hal = hBSP430timerLookup(configp->pps_timer);
    if (NULL == pps_timer_hal) {
      return -1;
    }
    if (configp->pps_ccidx >= iBSP430timerSupportedCCs(configp->pps_timer)) {
      return -1;
    }
    bit = configp->pps_port_bit;
    if ((0 == bit) || (0 != (bit & (bit - 1)))) {
      return -1;
    }
    /* Capture synchronously on rising edge and interrupt */
    pps_timer_hal->hpl->cctl[configp->pps_ccidx] = (configp->pps_ccis & (CCIS0 | CCIS1)) | CM_1 | SCS | CAP | CCIE;

    /* Set PPS port for input as CC trigger, with weak pull-down */
    BSP430_PORT_HAL_HPL_DIR(pps_port_hal) &= ~bit;
#if BSP430_PORT_SUPPORTS_REN
    BSP430_PORT_HAL_HPL_OUT(pps_port_hal) &= ~bit;
    BSP430_PORT_HAL_HPL_REN(pps_port_hal) |= bit;
#endif /* BSP430_PORT_SUPPORTS_REN */
    BSP430_PORT_HAL_HPL_SEL(pps_port_hal) |= bit;

    /* Chain in PPS interrupt handler */
    BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRIndexedChainNode,
                                    xBSP430uptimeTimer()->cc_cbchain_ni[APP_PPS_CCIDX],
                                    pps_state_.cb,
                                    next_ni);
  }

  /* Reset the NMEA state and release its uart */
  fp_reset(rx_state_.pool);
  rx_state_.curmsg.message = NULL;
  rx_state_.state = SRX_unsync;
  tx_state_.msg = NULL;
  tx_state_.state = STX_idle;

  iBSP430serialSetHold_ni(uart_hal, 0);
  return 0;
}
