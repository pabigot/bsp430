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

#ifndef BSP430_SENSORS_SKYTRAQ_H
#define BSP430_SENSORS_SKYTRAQ_H

/** @file
 * @brief Sensor-specific interface for SkyTraq Venus 6 GPS sensors
 *
 * This module implements the BSP430 @link bsp430/utility/gps.h
 * GPS@endlink interface for SkyTraq Venus 6 GPS sensors.  See @ref
 * ex_sensors_venus6pps for an example application.
 *
 * The implementation is the BSP430 @c src/sensors/skytraq.c file.
 *
 * @note The implementation for this sensor makes use of <a
 * href="http://pabigot.github.com/fragpool">Fragpool</a>, a library
 * for managing packetized data in embedded communications stacks.
 *
 * @note All multi-byte data in binary packets is stored in big-endian
 * form, and this is highlighted through the @c _be suffix on such
 * fields.  The MSP430 is a little-endian architecture; use
 * #BSP430_CORE_SWAP_16 and #BSP430_CORE_SWAP_32 as necessary to
 * convert between packet and internal representations.
 *
 * @note Only a subset of available commands have structures defined.
 *
 * @section h_sensors_skytraq_resources Resources
 * @li http://www.sparkfun.com/products/11058
 * @li http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/GPS/Venus/638/doc/AN0003_v1.4.19.pdf
 * @li http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/GPS/Venus/638/doc/Venus638FLPx_DS_v07.pdf
 * @li http://www.sparkfun.com/datasheets/GPS/Modules/AN0003_v1.4.14_FlashOnly.pdf
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 * @copyright Data structures and command descriptions are derived
 * from <a
 * href="http://www.sparkfun.com/datasheets/GPS/Modules/AN0003_v1.4.14_FlashOnly.pdf">Application
 * Note AN0003: Binary Messages of SkyTraq Venus 6 GPS Receiver</a>,
 * copyright 2006 SkyTraq Technology Inc., provided as-is for use by
 * SkyTraq customers.
 *
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/core.h>

/** Message identifiers */
typedef enum eSkyTraqMessageID {
  eSkyTraqMID_NMEA = 71,                  /**< 'G' for GP* NMEA messages */

  /* Input System Messages */
  eSkyTraqMIDin_RESTART = 1,              /**< System Restart */
  eSkyTraqMIDin_QRY_SW_VERSION = 2,       /**< Query Software Version */
  eSkyTraqMIDin_QRY_SW_CRC = 3,           /**< Query Software CRC */
  eSkyTraqMIDin_SET_FACTORY_DEFAULTS = 4, /**< Set Factory Defaults */
  eSkyTraqMIDin_CFG_SERIAL = 7,           /**< Configure Serial Port */
  eSkyTraqMIDin_CFG_NMEA = 8,             /**< Configure NMEA */
  eSkyTraqMIDin_CFG_FORMAT = 9,           /**< Configure Output Message Format */
  eSkyTraqMIDin_CFG_POWER = 12,           /**< Configure Power Mode */
  eSkyTraqMIDin_CFG_PUR = 14,             /**< Configure Position Update Rate */
  eSkyTraqMIDin_QRY_PUR = 16,             /**< Query Position Update Rate */

  /* Input GPS messages */
  eSkyTraqMIDin_CFG_DATUM = 41,           /**< Configure Datum */
  eSkyTraqMIDin_QRY_DATUM = 45,           /**< Query Datum */
  eSkyTraqMIDin_GET_EPHEMERIS = 48,       /**< Get Ephemeris */
  eSkyTraqMIDin_SET_EPHEMERIS = 49,       /**< Set Ephemeris */
  eSkyTraqMIDin_CFG_WAAS = 55,            /**< Configure WAAS */
  eSkyTraqMIDin_QRY_WAAS = 56,            /**< Query WAAS Status */
  eSkyTraqMIDin_CFG_PPIN = 57,            /**< Configure Position Pinning */
  eSkyTraqMIDin_QRY_PPIN = 58,            /**< Query Position Pinning */
  eSkyTraqMIDin_CFG_PPIN_PARAM = 59,      /**< Configure Position Pinning Parameters */
  eSkyTraqMIDin_CFG_NAV_MODE = 60,        /**< Configure Navigation Mode */
  eSkyTraqMIDin_QRY_NAV_MODE = 61,        /**< Query Navigation Mode */
  eSkyTraqMIDin_CFG_1PPS = 62,            /**< Configure 1PPS Mode */
  eSkyTraqMIDin_QRY_1PPS = 63,            /**< Query 1PPS Mode */

  /* Output System Messages */
  eSkyTraqMIDout_SW_VERSION = 128,        /**< Software Version */
  eSkyTraqMIDout_SW_CRC = 129,            /**< Software CRC */
  eSkyTraqMIDout_ACK = 131,               /**< ACK */
  eSkyTraqMIDout_NACK = 132,              /**< NACK */
  eSkyTraqMIDout_PUR = 134,               /**< Position Update Rate */

  /* Output GPS Messages */
  eSkyTraqMIDout_NAV_DATA = 168,         /**< Navigation Data */
  eSkyTraqMIDout_DATUM = 174,            /**< GPS Datum */
  eSkyTraqMIDout_WAAS = 179,             /**< GPS WAAS Status */
  eSkyTraqMIDout_PPIN = 180,             /**< GPS Position Pinning Status */
  eSkyTraqMIDout_NAV_MODE = 181,         /**< GPS Navigation Mode */
  eSkyTraqMIDout_1PPS = 182,             /**< GPS 1PPS Mode */

} eSkyTraqMessageID;

/** The layout common to all SkyTraq Venus 6 binary messages. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsg_GENERIC)
{
  uint8_t mid;                  /**< Message ID */
  uint8_t payload[1];           /**< Message-specific content*/
};

/** This is a request message which will reset and restart the GPS
 * receiver. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgIn_RESTART)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDin_RESTART */
  uint8_t mode;                 /**< Start mode (0: mode-no-change; 1: hot) */
  uint16_t year_be;             /**< UTC year (>= 1980) */
  uint8_t mon;                  /**< UTC month (1..12) */
  uint8_t mday;                 /**< UTC day (1..31) */
  uint8_t hour;                 /**< UTC hour (0..23) */
  uint8_t min;                  /**< UTC minute (0..59) */
  uint8_t sec;                  /**< UTC second (0..59) */
  int16_t lat_cdeg_be;          /**< Pos N latitude, deg * 100 */
  int16_t lon_cdeg_be;          /**< Pos E longitude, deg * 100 */
  int16_t alt_m_be;             /**< Altitude, meters */
};

/** This is a request message which is issued from the host to GPS
 * receiver to retrieve loaded software version. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgIn_SW_VERSION)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDin_QRY_SW_VERSION */
  uint8_t type;                 /**< 0: Reserved, 1: System code */
};

/** This is a request message which is issued from the host to GPS
 * receiver to retrieve loaded software CRC. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgIn_SW_CRC)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDin_QRY_SW_CRC */
  uint8_t type;                 /**< 0: Reserved, 1: System code */
};

/** This is a request message which is issued from the host to GPS
 * receiver to reset the GPS receiver’s internal parameters to factory
 * default values. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgIn_FACTORY_DEFAULTS)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDin_SET_FACTORY_DEFAULTS */
  uint8_t type;                 /**< 0: Reserved, 1: reboot after setting to defaults */
};

/** This is a request message which will configure the serial COM
 * port, baud rate. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgIn_CFG_SERIAL)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDin_CFG_SERIAL */
  uint8_t port;                 /**< COM port = 0 */
  uint8_t baud_idx;             /**< Baud rate (1:9600, 3:38400, 5:115200) */
  uint8_t attributes;           /**< Attributes: 0: SRAM, 1: SRAM&FLASH */
};

/** This is a request message which will set NMEA message
 * configuration. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgIn_CFG_NMEA)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDin_CFG_SERIAL */
  uint8_t gga_s;                /**< GGA interval in seconds */
  uint8_t gsa_s;                /**< GSA interval in seconds */
  uint8_t gsv_s;                /**< GSV interval in seconds */
  uint8_t gll_s;                /**< GLL interval in seconds */
  uint8_t rmc_s;                /**< RMC interval in seconds */
  uint8_t vtg_s;                /**< VTG interval in seconds */
  uint8_t zda_s;                /**< ZDA interval in seconds */
  uint8_t attributes;           /**< Attributes: 0: SRAM, 1: SRAM&FLASH */
};

/** This is a request message which will change the GPS receiver
 * output message type. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgIn_CFG_FORMAT)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDin_CFG_FORMAT */
  uint8_t type;                 /**< 0: no output, 1: NMEA; 2: Binary */
};

/** This is a response message which provides the software version of
 * the GPS receiver. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgOut_SW_VERSION)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDout_SW_VERSION */
  uint8_t type;                 /**< 0: Reserved, 1: System code*/
  uint32_t kernel;              /**< Kernel version (byte-encoded) */
  uint32_t odm;                 /**< ODM version (byte-encoded) */
  uint32_t revision;            /**< SkyTraq revision (??) */
};

/** This is a response message which is an acknowledgement to a
 * request message (indicating command succeeded) or negative
 * acknowledgement (indicating command was rejected). */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgOut_ACK)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDout_ACK or eSkyTraqMIDout_NACK */
  uint8_t in_mid;               /**< ACK/NACK ID (mid from relevant request) */
};

/** This is a response message which provides data of user navigation
 * solution. */
BSP430_CORE_PACKED_STRUCT(sSkyTraqMsgOut_NAV_DATA)
{
  uint8_t mid;                  /**< Message ID = #eSkyTraqMIDout_NAV_DATA */
  uint8_t fix_mode;             /**< Fix mode: 0: no fix, 1: 2D, 2: 3D, 3: 3D+DGPS */
  uint8_t nsv;                  /**< Number of SV in fix */
  uint16_t gps_week_be;         /**< GPS week number */
  uint32_t gps_tow_cs_be;       /**< GPS time of week, sec * 100 */
  int32_t lat_xdeg_be;          /**< Pos N latitude, deg * 10^7 */
  int32_t lon_xdeg_be;          /**< Pos E longitude, deg * 10^7 */
  uint32_t ell_alt_cm_be;       /**< Ellipsoid altitude, cm */
  uint32_t msl_alt_cm_be;       /**< Mean Sea Level altitude, cm */
  uint16_t gdop_pc_be;          /**< Geometric dilution of precision, percent */
  uint16_t pdop_pc_be;          /**< Position dilution of precision, percent */
  uint16_t hdop_pc_be;          /**< Horizontal dilution of precision, percent */
  uint16_t vdop_pc_be;          /**< Vertical dilution of precision, percent */
  uint16_t tdop_pc_be;          /**< Time dilution of precision, percent */
  int32_t ecef_x_cm_be;         /**< ECEF X coordinate, cm */
  int32_t ecef_y_cm_be;         /**< ECEF Y coordinate, cm */
  int32_t ecef_z_cm_be;         /**< ECEF Z coordinate, cm */
  int32_t ecef_vx_cm_be;        /**< ECEF X velocity, cm/s */
  int32_t ecef_vy_cm_be;        /**< ECEF Y velocity, cm/s */
  int32_t ecef_vz_cm_be;        /**< ECEF Z velocity, cm/s */
};

/** A union for access to SkyTraq Venus 6 binary messages sent from
 * application to sensor (the "in" direction). */
typedef union uSkyTraqMsgIn {
  struct sSkyTraqMsg_GENERIC generic; /**< Access to message id regardless of type */
  struct sSkyTraqMsgIn_RESTART restart;
  struct sSkyTraqMsgIn_SW_VERSION sw_version;
  struct sSkyTraqMsgIn_FACTORY_DEFAULTS factory_defaults;
  struct sSkyTraqMsgIn_CFG_SERIAL cfg_serial;
  struct sSkyTraqMsgIn_CFG_NMEA cfg_nmea;
  struct sSkyTraqMsgIn_CFG_FORMAT cfg_format;
} uSkyTraqMsgIn;

/** A union for access to SkyTraq Venus 6 binary messages sent from
 * sensor to the application (the "out" direction). */
typedef union uSkyTraqMsgOut {
  struct sSkyTraqMsg_GENERIC generic; /**< Access to message id regardless of type */
  struct sSkyTraqMsgOut_SW_VERSION sw_version;
  struct sSkyTraqMsgOut_ACK ack;
  struct sSkyTraqMsgOut_ACK nack;
  struct sSkyTraqMsgOut_NAV_DATA nav_data;
} uSkyTraqMsgOut;

/** A union for access to specific SkyTraq Venus 6 binary messages
 * content from a generic buffer.  Acccess @c generic.mid to identify
 * the contents, then access the specific union field. */
typedef union uSkyTraqMsg {
  struct sSkyTraqMsg_GENERIC generic; /**< Access to message id regardless of type */
  char nmea[1];                       /**< Access to NUL-terminated NMEA message (mid == #eSkyTraqMID_NMEA) */
  uSkyTraqMsgIn in;                   /**< Access to messages from application to sensor */
  uSkyTraqMsgOut out;                 /**< Access to messages from sensor to application */
} uSkyTraqMsg;

/** Recommended number of octets in the pool for incoming messages.
 *
 * The driver will receive messages into blocks allocated from this
 * pool.  This should be large enough to hold the message being
 * processed by the application along with a new incoming message and
 * any ACK/NACK messages from application commands.  Take into account
 * the effect of fragmentation due to held messages.
 *
 * @defaulted */
#ifndef SKYTRAQ_NMEA_RX_POOL_SIZE
#define SKYTRAQ_NMEA_RX_POOL_SIZE 100
#endif /* SKYTRAQ_NMEA_RX_POOL_SIZE */

/** The number of distinct messages that can be allocated from the incoming message pool.
 *
 * Four supports one message being processed by the application, a
 * null ACK plus a valid ACK from an application command, plus one
 * message being actively received.
 *
 * @defaulted */
#ifndef SKYTRAQ_NMEA_RX_POOL_FRAGMENTS
#define SKYTRAQ_NMEA_RX_POOL_FRAGMENTS 4
#endif /* SKYTRAQ_NMEA_RX_POOL_FRAGMENTS */

#endif /* BSP430_SENSORS_SKYTRAQ_H */
