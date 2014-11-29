/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/port.h>
#include <bsp430/serial.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <bsp430/utility/gps.h>
#include <bsp430/sensors/skytraq.h>

/** Infrastructure has updated the 1PPS timestamp */
#define GPS_STATE_1PPS 0x01

/** Infrastructure has received a message over the serial port */
#define GPS_STATE_RX 0x02

/** Infrastructure has completed transmitting the message */
#define GPS_STATE_TX_DONE 0x04

/** Infrastructure saw and dropped an NMEA-format text message.
 * (Since this application expects the sensor to generate binary
 * NAV_DATA packets this suggests the sensor has been externally
 * reset.) */
#define GPS_STATE_SAW_NMEA 0x08

/** State reflecting GPS driver updates and application policy.  This
 * is mostly consulted and updated by the callbacks invoked by the
 * driver, and is processed by the application when it is woken by the
 * callbacks. */
typedef struct sAppGPSState {
  /** Flags like #GPS_STATE_1PPS that reflect the accuracy of the
   * remainder of this structure. */
  unsigned int flags;

  /** The minimum fix_mode value for NAV_DATA packets that should be
   * fed to the application.  Packets with a lesser fix have
   * incomplete data and are dropped at the driver/application
   * interface. */
  uint8_t keep_fix;

  /** The fix_mode from the last received packet.  This is used to
   * pass through packets where the fix_mode indicates a loss or
   * increase of accuracy even if the packet would otherwise be
   * dropped.  Useful for notification of problems and progress toward
   * synchronization. */
  uint8_t last_fix;

  /** The GPS week of the last received packet.  Again used to forward
   * data that might otherwise be dropped, as an indication that
   * progress is being made */
  unsigned int last_week_be;

  /** The number of packets that failed to pass the filter due to
   * fix_mode issues. */
  unsigned int dropped_fix;

  /** The number of packets lost because the application had not
   * finished processing a previous packet. */
  unsigned int app_lost_pkts;

  /** The number of packets lost within the driver due to checksum
   * failures or lack of buffer space. */
  unsigned int driver_lost_pkts;

  /** Total number of octets in packets lost by the driver. */
  unsigned int lost_octets;

  /** A pointer to a message received from the driver.  The driver
   * transfered ownership of this message to the application, and the
   * field should be cleared once the application accepts
   * ownership. */
  const uint8_t * msg;

  /** Number of octets in the msg field. */
  unsigned int msg_len;

  /** Driver-provided timestamp recording when the contents of @p msg
   * started appearing on the serial line. */
  unsigned long rx_utt;

  /** The timer value captured at the last 1PPS tick.  For this
   * application the uptime timer is used. */
  unsigned long pps_tt;

  /** Result code provided from the last transmission. */
  int tx_rc;
} sAppGPSState;

/** State updated by driver and application */
static volatile sAppGPSState gps_state_;

static int
gps_serial_cb (const uint8_t * msg,
               size_t len,
               unsigned long rx_utt)
{
  int rc;
  uSkyTraqMsg * up = (uSkyTraqMsg *)msg;
  unsigned int flags = GPS_STATE_RX;

  /* The going-in assumption is the driver should wake the
   * application, but branches below might override this. */
  rc = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
  if (NULL == msg) {
    /* Record a packet lost within the driver itself. */
    ++gps_state_.driver_lost_pkts;
    gps_state_.lost_octets += len;
  } else if (NULL == gps_state_.msg) {
    switch (up->generic.mid) {
      case eSkyTraqMID_NMEA:
        /* We don't decode NMEA messages; drop it and flag it so the
         * application can switch the GPS to binary mode. */
        flags = GPS_STATE_SAW_NMEA;
        break;
      case eSkyTraqMIDout_NAV_DATA:
        /* Drop data with inadequate GPS fix unless either the fix
         * level or the gross time has changed since the last
         * notification. */
        if ((up->out.nav_data.fix_mode < gps_state_.keep_fix)
            && (up->out.nav_data.fix_mode == gps_state_.last_fix)
            && (up->out.nav_data.gps_week_be == gps_state_.last_week_be)) {
          flags = 0;
          rc = 0;
          gps_state_.dropped_fix += 1;
          break;
        }
        /* Record the filter-related information from this packet and
         * pass it on to the application. */
        gps_state_.last_fix = up->out.nav_data.fix_mode;
        gps_state_.last_week_be = up->out.nav_data.gps_week_be;
        goto accept;
      case eSkyTraqMIDout_ACK:
        /* Ignore the ACKs that don't have a valid MID.  Accept the
         * others. */
        if (0 == up->out.ack.in_mid) {
          rc = 0;
          flags = 0;
          break;
        }
      /*FALLTHRU*/
      default:
        /* Accept everything else */
accept:
        gps_state_.msg = msg;
        gps_state_.msg_len = len;
        gps_state_.rx_utt = rx_utt;
        break;
    }
  } else {
    /* We only buffer a single packet and the application hasn't
     * completed processing the last one we received. */
    ++gps_state_.app_lost_pkts;
    gps_state_.lost_octets += len;
  }

  /* Free this message if it isn't now the responsibility of the
   * application. */
  if ((NULL != msg) && (gps_state_.msg != msg)) {
    vBSP430gpsReleaseMessage_ni(msg);
  }
  gps_state_.flags |= flags;
  return rc;
}

static int
gps_pps_cb (unsigned long pps_tck)
{
  gps_state_.flags |= GPS_STATE_1PPS;
  gps_state_.pps_tt = pps_tck;
  /* Don't wake up on 1PPS; we'll wake on the subsequent message that
   * says what time the 1PPS occurred. */
  return 0;
}

static int
gps_tx_complete (const uint8_t * msg,
                 int rc)
{
  gps_state_.flags |= GPS_STATE_TX_DONE;
  gps_state_.tx_rc = rc;
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

/** Buffer for outgoing messages.  The messages must persist while
 * owned by the driver. */
static uSkyTraqMsgIn tx_message;

void main ()
{
  int rc;
  size_t tx_length;
  const uint8_t * msg_to_release;
  unsigned int num_sync = 0;
  time_t sync_utc = 0;
  unsigned long sync_utt = 0;
  unsigned long min_lag = 0;
  unsigned long max_lag = 0;
  unsigned long utt_per_sec;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  utt_per_sec = ulBSP430uptimeConversionFrequency_Hz();

  cprintf("\n\nrtc " __DATE__ " " __TIME__ "\n");
  cprintf("Uptime clock uses %lu Hz\n", utt_per_sec);

  {
    sBSP430gpsConfiguration config;
    int rv;

    memset(&config, 0, sizeof(config));
    config.nmea_serial = APP_NMEA_UART_PERIPH_HANDLE;
    config.nmea_baud = APP_NMEA_BAUD_RATE;
    config.pps_port = APP_PPS_PORT_PERIPH_HANDLE;
    config.pps_port_bit = APP_PPS_PORT_BIT;
    config.pps_timer = BSP430_UPTIME_TIMER_PERIPH_HANDLE;
    config.pps_ccidx = APP_PPS_CCIDX;
    config.pps_ccis = APP_PPS_CCIS;
    config.serial_cb = gps_serial_cb;
    config.pps_cb = gps_pps_cb;

    cprintf("Connect NMEA serial data at %lu baud to %s at:\n\t%s\n",
            (unsigned long)config.nmea_baud,
            xBSP430serialName(config.nmea_serial),
            xBSP430platformPeripheralHelp(config.nmea_serial, 0));

    cprintf("Connect 1PPS to %s.%u on %s.%u\n",
            xBSP430timerName(config.pps_timer),
            config.pps_ccidx,
            xBSP430portName(config.pps_port),
            iBSP430portBitPosition(config.pps_port_bit));

    rv = iBSP430gpsInitialize(&config, NULL);
    cprintf("GPS init got %d\n", rv);
  }

  /* Queue up a version message so we have evidence the sensor is
   * connected and responsive. */
  {
    struct sSkyTraqMsgIn_SW_VERSION * mp = &tx_message.sw_version;
    memset(mp, 0, sizeof(*mp));
    mp->mid = eSkyTraqMIDin_QRY_SW_VERSION;
    mp->type = 1;
    tx_length = sizeof(*mp);
    cprintf("Queued SW version query\n");
  }

  /* Initialize the application state.  To reduce noise during startup
   * discard NAV_DATA messages with fixes below 2 (=3D) that don't
   * change from the last fix displayed. */
  memset((void*)&gps_state_, 0, sizeof(gps_state_));
  gps_state_.keep_fix = 2;
  gps_state_.last_fix = -1;

  sync_utc = 0;
  msg_to_release = NULL;
  while (1) {
    char timestamp[BSP430_UPTIME_AS_TEXT_LENGTH];
    sAppGPSState state;
    unsigned long now_utt;

    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      /* Free any driver-provided buffer that we're done with. */
      if (NULL != msg_to_release) {
        vBSP430gpsReleaseMessage_ni(msg_to_release);
        msg_to_release = NULL;
      }
      /* Initiate any pending transmission */
      if (0 < tx_length) {
        rc = iBSP430gpsTransmit_ni((uint8_t*)&tx_message, tx_length, gps_tx_complete);
        cprintf("TX MID %u request got %d\n", tx_message.generic.mid, rc);
        /* Clear the request only if the request was accepted.
         * There's error recovery that could be done here. */
        if (0 == rc) {
          tx_length = 0;
        }
      }
      /* Wait 30 seconds or until something happens. */
      BSP430_UPTIME_DELAY_MS_NI(30000, LPM0_bits, gps_state_.flags);

      /* Capture the driver state then clear the pieces that we accept
       * responsibility for. */
      state = gps_state_;
      gps_state_.flags = 0;
      gps_state_.msg = NULL;

      /* Grab the current time while we have interrupts disabled. */
      now_utt = ulBSP430uptime_ni();
    } while (0);
    BSP430_CORE_ENABLE_INTERRUPT();

    cprintf("%s: Wokeup with %x, lost %u octets in %u/d %u/a\n",
            xBSP430uptimeAsText(now_utt, timestamp),
            state.flags, state.lost_octets, state.driver_lost_pkts, state.app_lost_pkts);
    if (NULL != state.msg) {
      uSkyTraqMsg * up = (uSkyTraqMsg *)state.msg;

      xBSP430uptimeAsText(state.rx_utt, timestamp);
      switch (up->generic.mid) {
        case eSkyTraqMID_NMEA:
          /* Emit any NMEA messages that passed the callback filter */
          cprintf("\t%s [%u]: %s\n", timestamp, state.msg_len, state.msg);
          break;
        case eSkyTraqMIDout_ACK:
        case eSkyTraqMIDout_NACK:
          /* Display command acknowledgements */
          cprintf("%s: %s %u\n", timestamp, (eSkyTraqMIDout_NACK == up->generic.mid) ? "NACK" : "ACK", up->out.ack.in_mid);
          break;
        case eSkyTraqMIDout_SW_VERSION:
          /* Display the software version */
          cprintf("\t%s: SW version kernel %08lx odm %08lx rev %08lx\n",
                  timestamp,
                  up->out.sw_version.kernel,
                  up->out.sw_version.odm,
                  up->out.sw_version.revision);
          break;
        case eSkyTraqMIDout_NAV_DATA: {
          struct tm when_tm;
          char cbuf[26];
          struct sSkyTraqMsgOut_NAV_DATA * np = &up->out.nav_data;
          uint16_t gps_week = BSP430_CORE_SWAP_16(np->gps_week_be);
          uint32_t gps_csow = BSP430_CORE_SWAP_32(np->gps_tow_cs_be);
          uint16_t msec = (gps_csow % 100) * 10;
          uint32_t gps_sow = gps_csow / 100;
          time_t when_utc;

          when_utc = xBSP430gpsConvertGPStoUTC(gps_week, gps_sow);
          gmtime_r(&when_utc, &when_tm);
          cprintf("\tfix %u GPS week %d sec %lu: %lu.%03u: %s",
                  np->fix_mode, gps_week, gps_sow, when_utc, msec, ctime_r(&when_utc, cbuf));

          /* Reset if we've lost the GPS fix */
          if (0 == np->fix_mode) {
            sync_utc = 0;
          }

          if (state.flags & GPS_STATE_1PPS) {
            unsigned long lag = state.rx_utt - state.pps_tt;
            int show_lag = (0 == when_tm.tm_sec);

            /* Maintain statistics on the delta between the 1PPS and
             * the start of the following message, which is expected
             * to reflect the time at the most recent 1PPS.  This is
             * partly why we use NAV_DATA: so we get one message per
             * second, rather than a set of NMEA messages.  Display
             * those statistics when they change, or once per
             * minute. */
            if ((0 == min_lag) || (lag < min_lag)) {
              min_lag = lag;
              show_lag = 1;
            }
            if (lag > max_lag) {
              max_lag = lag;
              show_lag = 1;
            }
            if (show_lag) {
              cprintf("\tmin lag %lu = %s;", min_lag, xBSP430uptimeAsText(min_lag, timestamp));
              cprintf(" max lag %lu = %s\n", max_lag, xBSP430uptimeAsText(max_lag, timestamp));
            }

            /* The lag between when a message arrives and the
             * timestamp encoded in it is normally about 150ms +/-
             * 50ms.  If the fractional second in the timestamp is
             * more than 50ms away from the whole second assume the
             * sensor is gratuitously delaying the notification so
             * that the update is spaced consistently with previous
             * updates.  This means the delay gets mixed up with the
             * lag so we can't be sure that the timestamp doesn't
             * refer to a different 1PPS event.  (The offset does
             * drift, so give it 50ms leeway.)
             *
             * If it looks like the sensor is introducing gratuitous
             * delays, use a hot restart that preserves mode to change
             * the update basis so the timestamp is re-aligned with
             * the 1PPS.  It'll still be about 150ms late, but there
             * won't be such a risk that the notification is for a
             * previous 1PPS event.  (Don't do this if the 1PPS isn't
             * valid since the reset might prevent the unit from
             * resynchronizing quickly.  Also don't try it if the
             * transmit buffer isn't available.) */
            if (((50 <= msec) && (msec <= 950))
                && (2 <= np->fix_mode)) {
              cprintf("\tExcessive timestamp offset detected: %u ms\n", msec);
              if (0 == tx_message.generic.mid) {
                struct sSkyTraqMsgIn_RESTART * mp = &tx_message.restart;

                memset(mp, 0, sizeof(*mp));
                mp->mid = eSkyTraqMIDin_RESTART;
                mp->mode = 0;
                mp->year_be = BSP430_CORE_SWAP_16(when_tm.tm_year);
                mp->mon = when_tm.tm_mon - 1;
                mp->mday = when_tm.tm_mday;
                mp->hour = when_tm.tm_hour;
                mp->min = when_tm.tm_min;
                mp->sec = when_tm.tm_sec;
                tx_length = sizeof(*mp);
                cprintf("** Queued restart command\n");
              }
            } else if (0 == sync_utc) {
              ++num_sync;
              sync_utc = when_utc;
              sync_utt = state.pps_tt;
              cprintf("** First sync: %lu POSIX == %lu uptime\n", sync_utc, sync_utt);
            } else {
              unsigned int sync_duration_s = (when_utc - sync_utc);
              unsigned long sync_duration_utt = sync_duration_s * utt_per_sec;
              unsigned long expected_utt = sync_utt + sync_duration_utt;
              long drift_utt = (long)state.pps_tt - (long)expected_utt;

              /* Calculate the drift of uptime clock away from UTC
               * since the last synchronization. */
              cprintf("\tUTT-to-UTC offset %ld after %u s; UTT ", drift_utt, sync_duration_s);
              if (0 > drift_utt) {
                cprintf(" gained");
                drift_utt = -drift_utt;
              } else {
                cprintf(" lost");
              }
              cprintf(" %s ", xBSP430uptimeAsText(drift_utt, timestamp));
              cprintf(" in %s\n", xBSP430uptimeAsText(sync_duration_utt, timestamp));
              /* Reset synchronization epoch 90 seconds after first
               * sync to eliminate accumulated error when uptime clock
               * needed to warm up to stability and the GPS provided a
               * synchronization before that completed. */
              if ((1 == num_sync) && (90 == sync_duration_s)) {
                cprintf("** Reset sync\n");
                ++num_sync;
                sync_utc = when_utc;
                sync_utt = state.pps_tt;
              }
            }
          }
          break;
        }
        default:
          /* Some unrecognized binary message. */
          cprintf("\t%s [%u]: Unhandled mid %u\n", xBSP430uptimeAsText(state.rx_utt, timestamp), state.msg_len, up->generic.mid);
          break;
      }
      msg_to_release = state.msg;
    }
    if (state.flags & GPS_STATE_1PPS) {
#if 0
      cprintf("\t1PPS at %lu\n", state.pps_tt);
#endif
    }
    if (state.flags & GPS_STATE_TX_DONE) {
      /* Mark the transmission buffer as available */
      tx_message.generic.mid = 0;
      cprintf("\tTX completed %d\n", state.tx_rc);
    }
    /* If we got an NMEA text message and the transmission buffer is
     * available, send a command to put the sensor into binary-output
     * mode. */
    if ((state.flags & GPS_STATE_SAW_NMEA) && (0 == tx_message.generic.mid)) {
      struct sSkyTraqMsgIn_CFG_FORMAT * mp = &tx_message.cfg_format;

      memset(mp, 0, sizeof(*mp));
      mp->mid = eSkyTraqMIDin_CFG_FORMAT;
      mp->type = 2;
      tx_length = sizeof(*mp);
      cprintf("Queued format=binary command\n");
    }
  }
}
