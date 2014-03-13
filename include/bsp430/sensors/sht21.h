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

#ifndef BSP430_SENSORS_SHT21_H
#define BSP430_SENSORS_SHT21_H

/** @file
 *
 * @brief Interface to the <a
 * href="http://www.sensirion.com/en/products/humidity-temperature/humidity-sensor-sht21/">Sensirion
 * SHT21 Digital Humidity Sensor</a> or the <a
 * href="http://www.meas-spec.com/product/humidity/HTU21D.aspx">Measurement
 * Specialties HTU21D</a> clone.
 *
 * This device measures relative humidity and temperature.
 *
 * To use this with BSP430 you will need an I2C device.  See @c
 * examples/sensors/bmp085 for an example of how to use it.
 *
 * @note For all operations that involve I2C, the caller must hold the
 * @p i2c resource.  For most functions the following conditions apply
 * at the time the function is invoked:
 *
 * @note @li @p i2c may be in @link iBSP430serialSetReset_rh reset
 * mode @endlink on entry, in which case it will be put back into
 * reset mode before returning.  @p i2c must @b not be in @link
 * iBSP430serialSetHold_rh hold mode @endlink.
 *
 * @note @li The slave address of the @p i2c resource will be
 * reconfigured as a side effect of calling the function.
 *
 * @note When @p hold_master is used in
 * iBSP430sensorsSHT21initiateMeasurement() the I2C bus is not
 * restored to reset mode on return.  The caller must do so, after
 * successful completion of iBSP430sensorsSHT21getSample(), if
 * desired.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/serial.h>
#include <bsp430/periph/timer.h>

/** The 7-bit I2C slave address for the device.  This is not
 * configurable. */
#define BSP430_SENSORS_SHT21_I2C_ADDRESS 0x40

/** The maximum time required by the sensor to stabilize after
 * iBSP430sensorsSHT21reset(), in milliseconds. */
#define BSP430_SENSORS_SHT21_RESET_DELAY_MS 15

/** @p resolution parameter to iBSP430sensorsSHT21configuration() to
 * configure for 12-bit humidity and 14-bit temperature
 * measurements. */
#define BSP430_SENSORS_SHT21_CONFIG_H12T14 0x00

/** @p resolution parameter to iBSP430sensorsSHT21configuration() to
 * configure for 8-bit humidity and 12-bit temperature
 * measurements. */
#define BSP430_SENSORS_SHT21_CONFIG_H8T12 0x01

/** @p resolution parameter to iBSP430sensorsSHT21configuration() to
 * configure for 10-bit humidity and 13-bit temperature
 * measurements. */
#define BSP430_SENSORS_SHT21_CONFIG_H10T13 0x80

/** @p resolution parameter to iBSP430sensorsSHT21configuration() to
 * configure for 11-bit humidity and 11-bit temperature
 * measurements. */
#define BSP430_SENSORS_SHT21_CONFIG_H11T11 0x81

/** Bits that represent measurement resolution in the value returned
 * by iBSP430sensorsSHT21configuration(). */
#define BSP430_SENSORS_SHT21_CONFIG_RESOLUTION_MASK 0x81

/** Bits that represent end-of-battery state in the value returned
 * by iBSP430sensorsSHT21configuration(). */
#define BSP430_SENSORS_SHT21_CONFIG_EOB_MASK 0x40

/** Bits that represent on-chip heater enabled state in the value
 * returned by iBSP430sensorsSHT21configuration(). */
#define BSP430_SENSORS_SHT21_CONFIG_HEATER_MASK 0x02

/** Number of octets in the SHT21 Electronic Identification Code */
#define BSP430_SENSORS_SHT21_EIC_LENGTH 8

/** Convert a raw humidity value to parts-per-thousand as an unsigned
 * int */
/* RH_pph = -6 + 125 * S / 2^16 */
#define BSP430_SENSORS_SHT21_HUMIDITY_RAW_TO_ppth(raw_) (unsigned int)(((1250UL * (raw_)) >> 16) - 60)

/** Convert a raw temperature value to centi-degrees Kelvin as an
 * unsigned int */
/* T_dC = -46.85 + 175.72 * S / 2^16
 * T_cK = 27315 - 4685 + 17572 * S / 2^16
 *      = 22630 + 17572 * S / 2^16
 */
#define BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_cK(raw_) (22630U + (unsigned int)((17572UL * (raw_)) >> 16))

/** Convert a raw temperature value to deci-degrees Kelvin as an
 * unsigned int. */
#define BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_dK(raw_) ((unsigned int)((5 + BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_cK(raw_)) / 10))

/** Calculate the Cyclic Redundancy Checksum over a range of data.
 *
 * This uses the Sensirion polynomial as described in the <a
 * href="http://www.sensirion.com/en/products/humidity-temperature/download-center/">CRC
 * Calculation Humidity Sensor SHT2x</a> application note to calculate
 * an 8-bit checksum over the provided data.
 *
 * @param data the sequence of octets to be checked
 *
 * @param len the number of octets in the sequence
 *
 * @return the 8-bit checksum, which is zero if the data terminates
 * with the expected checksum of the preceding octets. */
int iBSP430sensorsSHT21crc (const uint8_t * data,
                            int len);

/** Read the 64-bit electronic identification code from the chip.
 *
 * This is a unique identifier comprising 16-bit, 32-bit, and 16-bit
 * components in MSB sequence.  No information is provided about the
 * internal structure of the components.  See the <a
 * href="http://www.sensirion.com/en/products/humidity-temperature/download-center/">SHT2x
 * Electronic Identification Code</a> application node.
 *
 * @param i2c the I2C bus on which the SHT21 device can be contacted
 *
 * @param eic pointer to an #BSP430_SENSORS_SHT21_EIC_LENGTH-octet
 * buffer into which the retrieved identifier may be written.
 *
 * @return 0 on success, or a negative error code.
 */
int iBSP430sensorsSHT21eic (hBSP430halSERIAL i2c,
                            uint8_t * eic);

/** Read and update the user register.
 *
 * The user register controls the measurement resolution and whether
 * the on-chip heater is enabled.  This function reads the register,
 * calculates a new setting as requested by @p resolution and @p
 * heater, and if the setting has changed writes the new setting.  The
 * value that was read originally is returned.  Bits unrelated to
 * measurement resolution and on-chip heater are left unchanged.
 *
 * The resolution of the device is configured by passing @p resolution
 * with one of the values in the following table:
 *
 * @p resolution                       | RH Res (bits) | Temp Res (bits)
 * :---------------------------------- | :-----------: | :-------------:
 * #BSP430_SENSORS_SHT21_CONFIG_H8T12  |        8      |       12
 * #BSP430_SENSORS_SHT21_CONFIG_H10T13 |       10      |       13
 * #BSP430_SENSORS_SHT21_CONFIG_H11T11 |       11      |       11
 * #BSP430_SENSORS_SHT21_CONFIG_H12T14 |       12      |       14
 *
 * @param i2c the I2C bus on which the SHT21 device can be contacted
 *
 * @param resolution Control the resolution of temperature and
 * humidity measurements.  Values in the table above (which are all
 * non-negative) are accepted.  Negative values will result in no
 * change to the configuration; unrecognized non-negative values
 * produce an error.
 *
 * @param heater A positive value to turn the heater on, zero to turn
 * the heater off, a negative value to leave the heater at its current
 * setting.
 *
 * @return The non-negative eight-bit register value, or a negative
 * error code. */
int iBSP430sensorsSHT21configuration (hBSP430halSERIAL i2c,
                                      int resolution,
                                      int heater);

/** Execute a soft reset of the SHT21.
 *
 * This restores all @link iBSP430sensorsSHT21configuration()
 * configuration settings @endlink except the on-chip heater to their
 * power-up state.  The chip will be ready for use
 * #BSP430_SENSORS_SHT21_RESET_DELAY_MS milliseconds after this
 * function is invoked; the user is responsible for this delay.
 *
 * @param i2c the I2C bus on which the SHT21 device can be contacted
 *
 * @return 0 on success, a negative error code on failure. */
int iBSP430sensorsSHT21reset (hBSP430halSERIAL i2c);

/** Silly two-valued enumeration to avoid using literal integers
    (e.g. 0 and 1) to select between the available measurement
    types. */
typedef enum eBSP430sensorsSHT21measurement {
  eBSP430sensorsSHT21measurement_HUMIDITY, /**< Measurement of relative humidity */
  eBSP430sensorsSHT21measurement_TEMPERATURE /**< Measurement of temperature */
} eBSP430sensorsSHT21measurement;

/** Initiate a measurement.
 *
 * @param i2c the I2C bus on which the SHT21 device can be contacted
 *
 * @param hold_master nonzero if the SHT21 should hold the I2C bus
 * until the measurement is complete; zero if it should release it for
 * other use.  If nonzero the i2c bus is not restored to its original
 * state on return; it must remain untouched until the corresponding
 * iBSP430sensorsSHT21getSample() succeeds, and must be manually
 * placed back in reset mode if so desired.
 *
 * @param type the type of measurement requested.
 *
 * @return 0 if successful, otherwise a negative error code. */
int iBSP430sensorsSHT21initiateMeasurement (hBSP430halSERIAL i2c,
                                            int hold_master,
                                            eBSP430sensorsSHT21measurement type);

/** Read a measurement from the SHT21.
 *
 * The type of the measurement is not provided by this API: the caller
 * must be aware of whether the last initiated sample was for
 * temperature or for humidity.
 *
 * @param i2c the I2C bus on which the SHT21 device can be contacted
 *
 * @param hold_master nonzero if the measurement initiation used
 * hold-master mode; zero if it did not.  If nonzero the i2c bus must
 * not have been used since the measurement was initiated.  If zero,
 * this function will return -1 if the measurement is not yet
 * available.
 *
 * @param rawp pointer to where the 16-bit raw measurement should be
 * stored.
 *
 * @return A negative error code on I2C failure, otherwise the result
 * of running iBSP430sensorsSHT21crc() on the data read from the
 * sensor. */
int iBSP430sensorsSHT21getSample (hBSP430halSERIAL i2c,
                                  int hold_master,
                                  uint16_t * rawp);

#endif /* BSP430_SENSORS_SHT21_H */
