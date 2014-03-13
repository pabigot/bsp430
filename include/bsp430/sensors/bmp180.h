/* Copyright 2012-2014, Peter A. Bigot
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

#ifndef BSP430_SENSORS_BMP180_H
#define BSP430_SENSORS_BMP180_H

/** @file
 *
 * @brief Interface to the <a
 * href="https://www.bosch-sensortec.com/en/homepage/products_3/environmental_sensors_1/bmp180_1/bmp180">Bosch
 * Sensotec BMP180 Barometric Pressure Sensor</a> or its predecessor
 * <a
 * href="https://www.bosch-sensortec.com/en/homepage/products_3/environmental_sensors_1/bmp085_1/bmp085">BMP085</a>.
 *
 * This device measures absolute atmospheric pressure and temperature.
 *
 * To use this with BSP430 you will need an I2C device.  See @c
 * examples/sensors/bmp085 for an example of how to use it.
 *
 * @note For all operations that involve I2C, the following conditions
 * apply at the time the function is invoked:
 *
 * @note @li The caller must hold the @p i2c resource
 *
 * @note @li @p i2c may be in @link iBSP430serialSetReset_rh reset
 * mode @endlink on entry, in which case it will be put back into
 * reset mode before returning.  @p i2c must @b not be in @link
 * iBSP430serialSetHold_rh hold mode @endlink.
 *
 * @note @li The slave address of the @p i2c resource will be
 * reconfigured as a side effect of calling the function.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/serial.h>
#include <bsp430/periph/timer.h>

/** The 7-bit I2C slave address for the device.  This is not
 * configurable. */
#define BSP430_SENSORS_BMP180_I2C_ADDRESS 0x77

/** Calibration constants retrieved from the device */
typedef struct sBSP430sensorsBMP180calibration {
  int16_t ac1;
  int16_t ac2;
  int16_t ac3;
  uint16_t ac4;
  uint16_t ac5;
  uint16_t ac6;
  int16_t b1;
  int16_t b2;
  int16_t mb;
  int16_t mc;
  int16_t md;
} sBSP430sensorsBMP180calibration;

/** A handle for BMP180 calibration constants */
typedef sBSP430sensorsBMP180calibration * hBSP430sensorsBMP180calibration;

/** Data defining a BMP180 sample */
typedef struct sBSP430sensorsBMP180sample {
  /** The raw uncompensated pressure provided by the device. */
  uint32_t pressure_uncomp;

  /** The absolute pressure, in Pascals, calculated from
   * #pressure_uncomp and the calibration constants. */
  int32_t pressure_Pa;

  /** The raw uncompensated temperatures provided by the device. */
  uint16_t temperature_uncomp;

  /** The corrected temperature, in tenths of a degree Kelvin. */
  int16_t temperature_dK;

  /** The oversampling setting: an integer from 0 to 3 inclusive.  For
   * value n, 2^n internal samples are combined to produce the final
   * sample. */
  uint8_t oversampling;
} sBSP430sensorsBMP180sample;

/** A handle for a BMP180 sample */
typedef sBSP430sensorsBMP180sample * hBSP430sensorsBMP180sample;

/** Read the calibration data from an BMP180 device.
 *
 * @param i2c the I2C bus on which the BMP180 device can be contacted.
 *
 * @param calh reference to the calibration structure for the device.
 *
 * return 0 if the calibration constants could be read; otherwise a
 * negative error code.
 */
int iBSP430sensorsBMP180getCalibration (hBSP430halSERIAL i2c,
                                        hBSP430sensorsBMP180calibration calh);

/** Read a single sample from the device.
 *
 * @note This function will not return until the sample has been
 * obtained or an error detected.  Time required to request and
 * retrieve a sample ranges from about 10ms to 30ms depending on
 * sBSP430sensorsBMP180sample::oversampling.  The function will sleep
 * in LPM0 with interrupts enabled while awaiting the result.
 *
 * @param i2c the I2C bus on which the BMP180 device can be contacted
 *
 * @param sample a pointer to where the sample data should be stored.
 * sBSP430sensorsBMP180sample::oversampling must have been set prior
 * to the call.  On a successful call,
 * sBSP430sensorsBMP180sample::temperature_uncomp and
 * sBSP430sensorsBMP180sample::pressure_uncomp will have been updated.
 *
 * @return 0 if the sample was successfully retrieved, otherwise a
 * negative error code.
 */
int iBSP430sensorsBMP180getSample (hBSP430halSERIAL i2c,
                                   hBSP430sensorsBMP180sample sample);

/** Apply calibration correction to set the final temperature and
 * pressure values for a sample.
 *
 * @param calh reference to calibration constants retrieved by
 * iBSP430sensorsBMP180getCalibration()
 *
 * @param cample reference to a sample retrieved by
 * iBSP430sensorsBMP180getSample.  On return,
 * sBSP430sensorsBMP180sample::temperature_dK and
 * sBSP430sensorsBMP180sample::pressure_Pa will have been updated.
 */
void vBSP430sensorsBMP180convertSample (hBSP430sensorsBMP180calibration calh,
                                        hBSP430sensorsBMP180sample sample);

#endif /* BSP430_SENSORS_BMP180_H */
