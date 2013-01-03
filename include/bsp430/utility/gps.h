/* Copyright 2013, Peter A. Bigot
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

/** @file
 *
 * @brief Standardized interface for GPS sensors
 * 
 * This header declares functions that should be implemented for
 * specific GPS sensors, such as the @link bsp430/sensors/skytraq.h
 * SkyTraq Venus 638FLPx@endlink, to provide access to NMEA messages
 * and a one-pulse-per-second (1PPS) capability.  See @ref
 * ex_sensors_venus6pps for an example application.
 *
 * The API also includes some functions implemented in a generic
 * module to assist with decoding NMEA sentences and converting
 * between GPS and UTC time bases.  BSP430 re-uses the time
 * representation of POSIX @c time_t and <tt>struct tm</tt>, and may
 * assume availability of functions in the POSIX @c <time.h> header.
 *
 * The intent is that generic GPS capability is implemented in
 * accordance with this header, but specific devices may provide
 * enhanced capabilities in device-specific headers.
 *
 * @note Much of this module was created to leverage the 1PPS and
 * time-keeping capabilities of the GPS system to provide clock data
 * for a wireless sensor network.  There are references to leap
 * seconds, POSIX time encodings, UTC and GPS time systems, and POSIX
 * and GPS epochs.  The documentation assumes the reader is familiar
 * with these concepts.  If you are not, see
 * http://www.usno.navy.mil/USNO/time/gps and
 * http://www.madore.org/~david/computers/unix-leap-seconds.html among
 * <a href="http://lmgtfy.com/?q=gps+time">other references</a>.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_GPS_H
#define BSP430_UTILITY_GPS_H

#include <bsp430/core.h>
#include <time.h>

/** The GPS epoch in the POSIX epoch.
 *
 * The epoch for GPS time is 1980-01-06T00:00:00Z.  POSIX measures
 * time in seconds since 1970-01-01T00:00:00Z.
 *
 * POSIX time expresses time in UTC which is affected by leap seconds.
 * GPS time does not incorporate leap seconds.  At the GPS epoch, UTC
 * was 19 seconds ahead of TAI.  As of 2012-07-01T00:00:00Z GPS time
 * was 16 seconds ahead of UTC. */
#define BSP430_GPS_EPOCH_POSIX 315964800UL

/** Default offset from GPS to UTC.
 *
 * This is the number of seconds that must be added to a time in GPS
 * to convert it to be in UTC.  The correct value will change over
 * time based on IERS Bulletin C.  The application is responsible for
 * managing any updates that might occur during fielded life; see
 * vBSP430gpsGPStoUTCOffset_s_ni(). */
#define BSP430_GPS_GPS_UTC_OFFSET_S -16

/** Prototype for applications that receive GPS messages from the infrastructure.
 *
 * The GPS sensor will generally receive messages over a serial
 * connection.  When a message is successfully received, it will
 * invoke a function conforming to this prototype that was provided to
 * it by iBSP430gpsInitialize() so that the application can process
 * the message.
 *
 * This function is invoked within a hard-interrupt context, so the
 * provided data should be recorded and the function should return as
 * quickly as possible.  Ownership of the memory associated with the
 * message is transferred to the callee, which must in turn invoke
 * vBSP430gpsReleaseMessage_ni() to release it when the message is no
 * longer needed.  It is permitted to invoke
 * vBSP430gpsReleaseMessage_ni() within this callback if the
 * application does not need the message.
 *
 * The @p msg pointer will be null if the driver started to receive a
 * message but either discovered it to be corrupt or did not have
 * sufficient memory to store it.
 *
 * There is no notification related to messages that were dropped
 * because they were corrupted.
 *
 * The return value is incorporated into the caller's ISR return
 * value, so the application can request a wake-up from LPM if the
 * message requires processing.
 *
 * @param msg A pointer to the start of the received message (null if
 * the message could not be stored).  The message body will be a
 * NUL-terminated NMEA sentence (excluding start-of-sentence and
 * checksum) or a binary packet depending on the underlying sensor's
 * configuration.  There is no alignment promise on binary messages.
 *
 * @param len The number of octets within the message.  For NMEA text
 * messages the length includes the terminating NUL added by the
 * driver.
 *
 * @param rx_utt The ulBSP430uptime() value when the start-of-sentence
 * character for @p msg was received.
 *
 * @return A ISR return bitmask as described by @ref
 * callback_retval. */
typedef int (* iBSP430gpsSerialCallback_ni) (const uint8_t * msg,
                                             size_t len,
                                             unsigned long rx_utt);

/** Function called by application to release a message received through iBSP430gpsSerialCallback_ni()
 *
 * This invocation returns ownership of @p msg to the GPS driver.  It
 * must be invoked lest the GPS driver run out of memory in which to
 * store received messages.  It is safe to invoke this from within the
 * callback itself, or for the application to do so when it has
 * completed processing the message.
 *
 * Messages need not be released in the order in which the driver
 * provided them; e.g. an ACK/NACK could be processed within the
 * callback before the application completes processing a message
 * received earlier.
 *
 * @param msg the message pointer as passed to iBSP430gpsSerialCallback_ni(). */
void vBSP430gpsReleaseMessage_ni (const uint8_t * msg);

/** Prototype for applications that need 1PPS notifications the infrastructure.
 *
 * When configured properly a 1PPS signal from a GPS unit is connected
 * to a capture/compare register associated with an MSP430 timer.  On
 * receipt of a 1PPS signal, the infrastructure will invoke a function
 * conforming to this prototype that was provided to it by
 * iBSP430gpsInitialize() to tell the application when the pulse was
 * received.
 *
 * This function is invoked within a hard-interrupt context, so the
 * provided data should be recorded and the function should return as
 * quickly as possible.
 *
 * The return value is incorporated into the caller's ISR return
 * value, so the application can request a wake-up from LPM if the
 * message requires processing.
 *
 * @param pps_tck The captured time at which the rising edge of the
 * 1PPS signal was detected.  The resolution is dependent on the
 * underlying timer provided via sBSP430gpsConfiguration.pps_timer. 
 *
 * @return A ISR return bitmask as described by @ref
 * callback_retval. */
typedef int (* iBSP430gpsPPSCallback_ni) (unsigned long pps_tck);

/** Driver notification of transmission completion.
 *
 * This function is invoked by the driver, possibly from a
 * hard-interrupt context, when transmission of a message submitted by
 * iBSP430gpsTransmit_ni() has been completed.
 *
 * This invocation returns ownership of @p msg to the application.
 *
 * The return value is incorporated into the caller's ISR return
 * value, so the application can request a wake-up from LPM when the
 * transmission has completed.
 *
 * @param msg the message as originally passed to iBSP430gpsTransmit_ni()
 *
 * @param rc A non-negative value indicates success and represents the
 * number of octets transmitted (excluding any driver-specific wrapper
 * data such as checksums).  A negative value indicates a failure to
 * transmit.
 *
 * @return An ISR return bitmask as described by @ref
 * callback_retval. */
typedef int (* iBSP430gpsTransmitComplete_ni) (const uint8_t * msg,
                                               int rc);

/** Information required to initialize a GPS driver. */
typedef struct sBSP430gpsConfiguration {
  /** Handle for the serial peripheral used for NMEA messages.  The
   * @HAL interface must have been requested. */
  tBSP430periphHandle nmea_serial;

  /** Baud rate for the NMEA serial connection */
  unsigned long nmea_baud;

  /** MSP430 port handle on which the 1PPS signal will be coming in.
   * This should correspond to the @p ccis input of the @p pps_ccidx
   * capture/compare register of @p pps_timer.  The @HAL interface
   * should have been requested.  This may be #BSP430_PERIPH_NONE if
   * 1PPS signals are not needed. */
  tBSP430periphHandle pps_port;

  /** Bit on the @p pps_port for the 1PPS signal input.  This is bit
   * mask (e.g., #BIT3), not the bit position. */
  unsigned int pps_port_bit;

  /** The timer peripheral used to capture the 1PPS signal.  This must
   * be externally configured to count in continuous mode from some
   * clock source.  The @HAL interface must have been requested.  Pass
   * #BSP430_PERIPH_NONE if 1PPS signals are not needed. */
  tBSP430periphHandle pps_timer;

  /** The capture/compare index on @p pps_timer that's used to capture
   * the 1PPS signal.  This should be associated with @p pps_port and
   * @p pps_pin. */
  int pps_ccidx;

  /** The capture/compare input corresponding to @p pps_port and @p
   * pps_pin. */
  unsigned int pps_ccis;

  /** A callback invoked by the driver when a message is
   * successfully received over the NMEA serial port.  This may be a
   * null pointer if messages are not needed. */
  iBSP430gpsSerialCallback_ni serial_cb;

  /** A callback invoked by the driver when a 1PPS signal is received.
   * This may be a null pointer if 1PPS captures are not needed. */
  iBSP430gpsPPSCallback_ni pps_cb;
} sBSP430gpsConfiguration;

/** Initialize a GPS driver.
 *
 * On successful completion the NMEA serial and 1PPS capture
 * interfaces are running with interrupt handlers registered.
 *
 * @param configp The configuration information required by the driver
 * for NMEA serial and 1PPS interfaces.  @note The driver does not
 * dereference the configuration pointer after initialization
 * completes.
 *
 * @param devconfigp optional device-specific information.  Pass a
 * null pointer if no device-specific information is required.  @note
 * The driver may retain the pointer for ongoing use.
 *
 * @return 0 if the GPS was successfully initialized; -1 to indicate a
 * misconfiguration (e.g. inconsistent information). */
int iBSP430gpsInitialize_ni (const sBSP430gpsConfiguration * configp,
                             void * devconfigp);

/** Deconfigure a GPS driver.
 *
 * This shuts down the NMEA serial interface and unhooks the 1PPS
 * capture callbacks.
 *
 * @return 0 */
int iBSP430gpsShutdown_ni ();

/** @cond DOXYGEN_EXCLUDE
 *
 * Not implemented. */

/** Utility to extract time and date information from a NMEA GPS message.
 *
 * @param msg the NMEA message.  Recognized messages include GPRMC and
 * GPZDA.  GPGGA will only set time-related fields.
 *
 * @param len the number of octets in the NMEA message.
 *
 * @param tmp a pointer to a structure into which time and date
 * related fields are stored.  If only time can be extracted from @p
 * msg, the date-related fields are left unchanged.
 *
 * @return A non-negative result indicates success and expresses any
 * fractional time in milliseconds.  A negative result indicates a
 * failure to extract data. */
int iBSP430gpsExtractTime (const uint8_t * msg,
                           size_t len,
                           struct tm * tmp);

/** @endcond */

/** Query the GPS to UTC offset.
 *
 * xBSP430gpsConvertGPStoUTC needs to know how GPS time differs from
 * UTC.  This information is present in the GPS signal, but may not be
 * made available by any particular GPS sensor.
 *
 * @warning The correct value for this adjustment will change when a
 * leap second occurs.  BSP430 provides no provision to synchronize
 * updates with any use of this call, though the driver for a
 * particular GPS system may do this internally or through some other
 * notification mechanism (e.g. a synthesized NMEA message). */
int iBSP430gpsGPStoUTCOffset_s_ni (void);

/** Set the GPS to UTC offset.
 * 
 * @param gps_to_utc_offset_s The number of seconds that need to be
 * added to a GPS time to produce a UTC time.  Unless the rotation of
 * the earth speeds up unexpectedly, this should be a negative value
 * such as -16.
 *
 * @see iBSP430gpsGPStoUTCOffset_s_ni() */
void vBSP430gpsGPStoUTCOffset_s_ni (int gps_to_utc_offset_s);

/** Convert a GPS time into UTC.
 *
 * This function converts a GPS time expressed as a week number and
 * second within the week to a UTC time expressed as seconds since the
 * POSIX epoch.  This references iBSP430gpsGPStoUTCOffset_s_ni().
 *
 * @param weekno GPS week number.  The value passed should have been
 * adjusted for any week number rollovers due to the specification
 * requiring it to be taken modulo 1024; i.e. it should be the total
 * number of weeks since the GPS epoch.
 *
 * @param sow Number of seconds within the GPS week.
 *
 * @return A POSIX time in UTC. */
time_t xBSP430gpsConvertGPStoUTC_ni (unsigned int weekno,
                                     unsigned long sow);

/** Send a driver-specific message to the GPS sensor.
 *
 * This should be implemented by all drivers, but may simply return -1
 * if the driver does not support commands to the sensor.
 *
 * @param msg pointer to the start of the message.  If this call
 * succeeds, the data is owned by the driver and the application must
 * not change its contents until it has been notified through @p
 * complete_cb that the transmission is complete.
 *
 * @param len number of octets in the message
 *
 * @param complete_cb the callback to be invoked when transmission has
 * completed
 * 
 * @return 0 if the driver has accepted ownership of @p msg and will
 * transmit it; a negative error code if transmission could not be
 * accepted. */
int iBSP430gpsTransmit_ni (const uint8_t * msg,
                           size_t len,
                           iBSP430gpsTransmitComplete_ni complete_cb);

#endif /* BSP430_UTILITY_GPS_H */
