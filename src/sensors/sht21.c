/* Copyright 2013-2014, Peter A. Bigot
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

#include <string.h>
#include <bsp430/platform.h>
#include <bsp430/serial.h>
#include <bsp430/sensors/sht21.h>

/* These constants define the bits for componentized basic 8-bit
 * commands described in the data sheet. */
#define SHT21_CMDBIT_BASE 0xE0
#define SHT21_CMDBIT_READ 0x01
#define SHT21_CMDBIT_TEMP 0x02
#define SHT21_CMDBIT_RH 0x04
#define SHT21_CMDBIT_UR 0x06
#define SHT21_CMDBIT_NOHOLD 0x10

#define SHT21_TRIGGER_T_HM (SHT21_CMDBIT_BASE | SHT21_CMDBIT_TEMP | SHT21_CMDBIT_READ)
#define SHT21_TRIGGER_RH_HM (SHT21_CMDBIT_BASE | SHT21_CMDBIT_RH | SHT21_CMDBIT_READ)
#define SHT21_TRIGGER_T (SHT21_CMDBIT_NOHOLD | SHT21_TRIGGER_T_HM)
#define SHT21_USERREG_W (SHT21_CMDBIT_BASE | SHT21_CMDBIT_UR)
#define SHT21_USERREG_R (SHT21_CMDBIT_BASE | SHT21_CMDBIT_UR | SHT21_CMDBIT_READ)

/* This is a basic 8-bit command that does not conform to the
 * componentized structure used for most commands. */
#define SHT21_SOFT_RESET 0xFE

static int
configure_i2c (hBSP430halSERIAL i2c)
{
  int reset_mode = iBSP430serialSetReset_rh(i2c, -1);
  int rc = reset_mode;
  if (0 <= reset_mode) {
    rc = iBSP430i2cSetAddresses_rh(i2c, -1, BSP430_SENSORS_SHT21_I2C_ADDRESS);
    if (0 == rc) {
      rc = iBSP430serialSetReset_rh(i2c, 0);
    }
  }
  return (0 > rc) ? rc : reset_mode;
}

int
iBSP430sensorsSHT21crc (const uint8_t * data,
                        int len)
{
  static const uint16_t SHT_CRC_POLY = 0x131;
  uint8_t crc = 0;

  while (0 < len--) {
    int bi;

    crc ^= *data++;
    for (bi = 8; bi > 0; --bi) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ SHT_CRC_POLY;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

int
iBSP430sensorsSHT21eic (hBSP430halSERIAL i2c,
                        uint8_t * eic)
{
  int rv = -1;
  int reset_mode;

  if (! (i2c && eic)) {
    return rv;
  }
  memset(eic, 0, BSP430_SENSORS_SHT21_EIC_LENGTH);
  reset_mode = configure_i2c(i2c);
  if (0 > reset_mode) {
    return rv;
  }
  do {
    int rc;
    uint8_t data[8];
    const uint8_t * dpe;
    uint8_t * dp;

    dp = data;
    *dp++ = 0xFA;
    *dp++ = 0x0F;
    rc = iBSP430i2cTxData_rh(i2c, data, dp-data);
    if ((dp-data) != rc) {
      break;
    }
    dpe = data + 8;
    rc = iBSP430i2cRxData_rh(i2c, data, dpe-data);
    if ((dpe-data) != rc) {
      break;
    }
    dp = data;
    rc = 0;
    while ((0 == rc) && (dp < dpe)) {
      if (0 != iBSP430sensorsSHT21crc(dp, 2)) {
        rc = 1;
      }
      dp += 2;
    }
    if (0 != rc) {
      break;
    }
    eic[2] = data[0];
    eic[3] = data[2];
    eic[4] = data[4];
    eic[5] = data[6];

    dp = data;
    *dp++ = 0xFC;
    *dp++ = 0xC9;
    rc = iBSP430i2cTxData_rh(i2c, data, dp-data);
    if ((dp-data) != rc) {
      break;
    }
    dpe = data + 6;
    rc = iBSP430i2cRxData_rh(i2c, data, dpe-data);
    if ((dpe-data) != rc) {
      break;
    }
    dp = data;
    rc = 0;
    while ((0 == rc) && (dp < dpe)) {
      if (0 != iBSP430sensorsSHT21crc(dp, 3)) {
        rc = 1;
      }
      dp += 3;
    }
    if (0 != rc) {
      break;
    }
    eic[0] = data[3];
    eic[1] = data[4];
    eic[6] = data[0];
    eic[7] = data[1];
    rv = 0;
  } while (0);
  if (0 < reset_mode) {
    iBSP430serialSetReset_rh(i2c, reset_mode);
  }
  return rv;
}

int
iBSP430sensorsSHT21configuration (hBSP430halSERIAL i2c,
                                  int resolution,
                                  int heater)
{
  uint8_t ur_orig;
  uint8_t ur_new;
  int rv = -1;
  int reset_mode;

  if ((0 <= resolution)
      && ((resolution & BSP430_SENSORS_SHT21_CONFIG_RESOLUTION_MASK) != resolution)) {
    return rv;
  }
  if (! i2c) {
    return rv;
  }
  reset_mode = configure_i2c(i2c);
  if (0 > reset_mode) {
    return rv;
  }
  do {
    int rc;
    uint8_t cmd = SHT21_USERREG_R;

    rc = iBSP430i2cTxData_rh(i2c, &cmd, sizeof(cmd));
    if (sizeof(cmd) != rc) {
      break;
    }
    rc = iBSP430i2cRxData_rh(i2c, &ur_orig, sizeof(ur_orig));
    if (sizeof(ur_orig) != rc) {
      break;
    }
    ur_new = ur_orig;
    if (0 <= resolution) {
      ur_new = (ur_new & ~BSP430_SENSORS_SHT21_CONFIG_RESOLUTION_MASK) | resolution;
    }
    if (0 <= heater) {
      ur_new = (ur_new & ~BSP430_SENSORS_SHT21_CONFIG_HEATER_MASK);
      if (heater) {
        ur_new |= BSP430_SENSORS_SHT21_CONFIG_HEATER_MASK;
      }
    }
    if (ur_orig != ur_new) {
      uint8_t cmd2[2];
      cmd2[0] = SHT21_USERREG_W;
      cmd2[1] = ur_new;
      rc = iBSP430i2cTxData_rh(i2c, cmd2, sizeof(cmd2));
      if (sizeof(cmd2) != rc) {
        break;
      }
    }
    rv = ur_orig;
  } while (0);
  if (0 < reset_mode) {
    iBSP430serialSetReset_rh(i2c, reset_mode);
  }
  return rv;
}

int
iBSP430sensorsSHT21reset(hBSP430halSERIAL i2c)
{
  int rv = -1;
  int reset_mode;

  if (! i2c) {
    return rv;
  }
  reset_mode = configure_i2c(i2c);
  if (0 > reset_mode) {
    return rv;
  }
  do {
    uint8_t cmd = SHT21_SOFT_RESET;
    int rc;

    rc = iBSP430i2cTxData_rh(i2c, &cmd, sizeof(cmd));
    if (sizeof(cmd) == rc) {
      rv = 0;
    }
  } while (0);
  if (0 < reset_mode) {
    iBSP430serialSetReset_rh(i2c, reset_mode);
  }
  return rv;
}

int
iBSP430sensorsSHT21initiateMeasurement (hBSP430halSERIAL i2c,
                                        int hold_master,
                                        eBSP430sensorsSHT21measurement type)
{
  int rv = -1;
  int reset_mode = 0;
  uint8_t cmd = SHT21_CMDBIT_BASE | SHT21_CMDBIT_READ;

  switch (type) {
    case eBSP430sensorsSHT21measurement_TEMPERATURE:
      cmd |= SHT21_CMDBIT_TEMP;
      break;
    case eBSP430sensorsSHT21measurement_HUMIDITY:
      cmd |= SHT21_CMDBIT_RH;
      break;
    default:
      return rv;
  }
  if (! hold_master) {
    cmd |= SHT21_CMDBIT_NOHOLD;
  }
  if (! i2c) {
    return rv;
  }
  reset_mode = configure_i2c(i2c);
  if (0 > reset_mode) {
    return rv;
  }
  do {
    int rc;

    rc = iBSP430i2cTxData_rh(i2c, &cmd, sizeof(cmd));
    if (sizeof(cmd) == rc) {
      rv = 0;
    }
  } while (0);

  /* If SHT21 holds I2C and the request was successful, do nothing;
   * otherwise if the I2C bus was in reset mode on entry put it back
   * in reset mode on exit. */
  if ((! (hold_master && (0 == rv))) && (0 < reset_mode)) {
    iBSP430serialSetReset_rh(i2c, reset_mode);
  }
  return rv;
}

int
iBSP430sensorsSHT21getSample (hBSP430halSERIAL i2c,
                              int hold_master,
                              uint16_t * rawp)
{
  int rv = -1;
  int reset_mode = -1;

  if (! i2c) {
    return rv;
  }
  if (! hold_master) {
    reset_mode = configure_i2c(i2c);
    if (0 > reset_mode) {
      return rv;
    }
  }
  do {
    uint8_t data[3];
    int rc;

    rc = iBSP430i2cRxData_rh(i2c, data, sizeof(data));
    if (sizeof(data) != rc) {
      break;
    }
    /* Store the MSB value, masking off the status bits. */
    *rawp = ((data[0] << 8) | data[1]) & ~0x03;
    rv = iBSP430sensorsSHT21crc(data, sizeof(data));
  } while (0);
  /* If we took the I2C out of reset mode on entry, put it back in
   * reset mode on exit. */
  if (0 < reset_mode) {
    iBSP430serialSetReset_rh(i2c, reset_mode);
  }
  return rv;
}
