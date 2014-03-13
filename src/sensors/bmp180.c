/* The sBMP085calibration structure definition and the algorithm to
 * calculate pressure and temperature from uncompensated sensor values
 * are directly derived from Figure 3 of the BST-BMP085-DS000-06.pdf
 * which specifies:
 *
 * Copyright [2011] Bosch Sensortec GmbH reserves all rights even in
 * the event of industrial property rights. We reserve all rights of
 * disposal such as copying and passing on to third parties. BOSCH and
 * the symbol are registered trademarks of Robert Bosch GmbH, Germany.
 *
 * All remaining material:
 *
 * Copyright 2012-2014, Peter A. Bigot
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
#include <bsp430/utility/uptime.h>
#include <bsp430/sensors/bmp180.h>

#define BMP180_REG_CALIBRATION 0xAA
#define BMP180_REG_CMD 0xF4
#define BMP180_REG_DATA 0xF6
#define BMP180_VAL_TEMP 0x2E
#define BMP180_VAL_PRESSURE(oss_) (0x34 + ((0x03 & (oss_)) << 6))

static int
configure_i2c (hBSP430halSERIAL i2c)
{
  int reset_mode = iBSP430serialSetReset_rh(i2c, -1);
  int rc = reset_mode;
  if (0 <= reset_mode) {
    rc = iBSP430i2cSetAddresses_rh(i2c, -1, BSP430_SENSORS_BMP180_I2C_ADDRESS);
    if (0 == rc) {
      rc = iBSP430serialSetReset_rh(i2c, 0);
    }
  }
  return (0 > rc) ? rc : reset_mode;
}

int
iBSP430sensorsBMP180getCalibration (hBSP430halSERIAL i2c,
                                    hBSP430sensorsBMP180calibration calh)
{
  int rv = -1;
  int reset_mode;

  if (! (calh && i2c)) {
    return rv;
  }
  reset_mode = configure_i2c(i2c);
  if (0 > reset_mode) {
    return rv;
  }
  do {
    int rc;
    int i;
    uint8_t data[sizeof(*calh)];
    uint16_t * wp;

    data[0] = BMP180_REG_CALIBRATION;
    rc = iBSP430i2cTxData_rh(i2c, data, 1);
    if (0 > rc) {
      break;
    }
    memset(data, 0, sizeof(data));
    rc = iBSP430i2cRxData_rh(i2c, data, sizeof(data));
    if (0 > rc) {
      break;
    }
    if (0 <= rc) {
      wp = (uint16_t *)calh;
      i = 0;
      while (i < rc) {
        *wp++ = (data[i] << 8) | data[i+1];
        i += 2;
      }
    }
    rv = 0;
  } while (0);
  if (0 < reset_mode) {
    iBSP430serialSetReset_rh(i2c, reset_mode);
  }

  return rv;
}

int
iBSP430sensorsBMP180getSample (hBSP430halSERIAL i2c,
                               hBSP430sensorsBMP180sample sample)
{
  int rv = -1;
  int reset_mode;

  if (! (i2c && sample)) {
    return rv;
  }
  reset_mode = configure_i2c(i2c);
  if (0 > reset_mode) {
    return rv;
  }
  do {
    uint16_t u16;
    uint32_t u32;
    int rc;
    uint8_t data[4];

    if (0 > reset_mode) {
      break;
    }
    data[0] = BMP180_REG_CMD;
    data[1] = BMP180_VAL_TEMP;
    rc = iBSP430i2cTxData_rh(i2c, data, 2);
    if (0 > rc) {
      break;
    }

    /* 4.5 ms but make it 5 */
    BSP430_UPTIME_DELAY_MS(5, LPM0_bits, 0);

    data[0] = BMP180_REG_DATA;
    rc = iBSP430i2cTxData_rh(i2c, data, 1);
    if (0 > rc) {
      break;
    }
    rc = iBSP430i2cRxData_rh(i2c, data, 2);
    if (0 > rc) {
      break;
    }

    u16 = data[0];
    u16 = (u16 << 8) | data[1];
    sample->temperature_uncomp = u16;

    sample->oversampling &= 0x03;

    data[0] = BMP180_REG_CMD;
    data[1] = BMP180_VAL_PRESSURE(sample->oversampling);
    rc = iBSP430i2cTxData_rh(i2c, data, 2);
    if (0 > rc) {
      break;
    }

    /* 1.5 ms plus 3 ms for each sample. */
    BSP430_UPTIME_DELAY_MS(2 + (3 << sample->oversampling), LPM0_bits, 0);
    data[0] = BMP180_REG_DATA;
    rc = iBSP430i2cTxData_rh(i2c, data, 1);
    if (0 > rc) {
      break;
    }
    rc = iBSP430i2cRxData_rh(i2c, data, 3);
    if (0 > rc) {
      break;
    }

    u32 = data[0];
    u32 = (u32 << 8) | data[1];
    u32 = (u32 << 8) | data[2];
    u32 >>= 8 - sample->oversampling;
    sample->pressure_uncomp = u32;

    rv = 0;

  } while (0);
  if (0 < reset_mode) {
    iBSP430serialSetReset_rh(i2c, reset_mode);
  }

  return rv;
}

void
vBSP430sensorsBMP180convertSample (hBSP430sensorsBMP180calibration calh,
                                   hBSP430sensorsBMP180sample sample)
{
  int32_t x1;
  int32_t x2;
  int32_t x3;
  int32_t b3;
  uint32_t b4;
  int32_t b5;
  int32_t b6;
  uint32_t b7;
  int32_t p;

  x1 = (((int32_t)sample->temperature_uncomp - calh->ac6) * calh->ac5) >> 15;
  x2 = ((int32_t)calh->mc << 11) / (x1 + calh->md);
  b5 = x1 + x2;
  /* temp_dC = (b5 + 8) >> 4;
   * dK = 2731.5 + (b5 + 8) / 16
   *    = (27315 + 10 * (b5 + 8) / 16) / 10
   *    = (27315 + (10 * b5) / 16 + 5) / 10
   *    = (27320 + (10 * b5) / 16) / 10
   *    = 2732 + b5 / 16
   */
  sample->temperature_dK = 2732 + (uint16_t)(b5 / 16);
  b6 = b5 - 4000;
  x1 = (calh->b2 * ((b6 * b6) >> 12)) >> 11;
  x2 = (calh->ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = ((((calh->ac1 * 4) + x3) << sample->oversampling) + 2) / 4;
  x1 = (calh->ac3 * b6) >> 13;
  x2 = (calh->b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (calh->ac4 * (uint32_t)(x3 + 32768UL)) >> 15;
  b7 = ((uint32_t)sample->pressure_uncomp - b3) * (50000 >> sample->oversampling);
  if (0x80000000UL > b7) {
    p = (b7 * 2) / b4;
  } else {
    p = (b7 / b4) * 2;
  }
  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  sample->pressure_Pa = p + ((x1 + x2 + 3791) >> 4);
}
