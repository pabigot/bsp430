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

/** @file
 *
 * @brief Basic support for 1-Wire(R) communications.
 *
 * This currently supports enough to use DS18X one-wire temperature
 * sensors with a dedicated bus and external power.  Multiple devices
 * on the bus, and parasite powered devices, have not been tested.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_ONEWIRE_H
#define BSP430_UTILITY_ONEWIRE_H

#include <bsp430/periph/port.h>

/** Structure identifying 1-wire bus information. */
typedef struct sBSP430onewireBus {
  /** The peripheral port containing the bus. */
  hBSP430halPORT port;
  /** The pin by which the bus is connected to the MCU */
  unsigned int bit;
} sBSP430onewireBus;

/** Structure holding a 1-wire serial number. */
typedef struct sBSP430onewireSerialNumber {
  /** The serial number in order MSB to LSB */
  uint8_t id[6];
} sBSP430onewireSerialNumber;

enum {
  /** Read 64-bit ROM code without using search procedure */
  BSP430_ONEWIRE_CMD_READ_ROM = 0x33,

  /** Skip ROM sends the following command to all bus devices */
  BSP430_ONEWIRE_CMD_SKIP_ROM = 0xcc,

  /** Store data from EEPROM into RAM */
  BSP430_ONEWIRE_CMD_RECALL_EE = 0xb8,

  /** Read the RAM area. */
  BSP430_ONEWIRE_CMD_READ_SCRATCHPAD = 0xbe,

  /** Initiate a temperature conversion.
   *
   * Be aware that temperature conversion can take up to 750ms at the
   * default 12-bit resolution.
   *
   * For an externally (non-parasite) powered sensor, the caller may
   * use #iBSP430onewireReadBit_ni to determine whether the conversion
   * has completed.  Completion is indicated when the device responds
   * with a 1. */
  BSP430_ONEWIRE_CMD_CONVERT_T = 0x44,
};

/** Reset the bus and check for device presence.
 *
 * This executes the 1-wire bus reset protocol, then detects whether a
 * 1-wire device is present.  The protocol requires that the bus be
 * held low for a given period, then returns it to pullup input.  A
 * device present on the bus will respond by pulling the bus low.
 *
 * @param bus The port and bit identifying the 1-wire bus
 *
 * @return 0 if no device was detected, 1 if a device responded to the
 * reset.
 */
int iBSP430onewireReset_ni (const sBSP430onewireBus * bus);

/** Configure the bus pin for low power mode.
 *
 * This reconfigures the port as output low.  Note that this affects
 * the MCU power, not the device power.  It does remove power from
 * parasitic devices, but externally powered devices will still draw
 * some amount of current.
 *
 * @param bus The port and bit identifying the 1-wire bus
 */
void vBSP430onewireShutdown_ni (const sBSP430onewireBus * bus);

/** Write a byte onto the 1-wire bus.
 *
 * @param bus The port and bit identifying the 1-wire bus
 *
 * @param byte The value to be written.  The low 8 bits are
 * transmitted LSB-first. */
void vBSP430onewireWriteByte_ni (const sBSP430onewireBus * bus,
                                 int byte);

/** Read a bit from the 1-wire bus.
 *
 * @param bus The port and bit identifying the 1-wire bus
 *
 * @return The value of the bit read (0 or 1). */
int iBSP430onewireReadBit_ni (const sBSP430onewireBus * bus);

/** Read a byte from the 1-wire bus.
 *
 * Invokes #iBSP430onewireReadBit_ni eight times to read the data, least
 * significant bit first.
 *
 * @param bus The port and bit identifying the 1-wire bus
 *
 * @return The value of the byte read (0 to 255). */
int iBSP430onewireReadByte_ni (const sBSP430onewireBus * bus);

/** Calculate the CRC over the data.
 *
 * When the last byte of the data is the CRC of the preceding bytes,
 * the return value of this function should be zero.
 *
 * @param data data for which CRC is desired
 *
 * @param len the number of bytes that contribute to CRC calculation
 *
 * @return the calculated CRC value */
int iBSP430onewireComputeCRC (const unsigned char * data, int len);

/** Read the serial number from a 1-wire device.
 *
 * @param bus The port and bit identifying the 1-wire bus
 *
 * @param snp Pointer to where the serial number should be stored
 *
 * @return 0 if the serial number was successfully read; -1 if an
 * error occurred. */
int iBSP430onewireReadSerialNumber (const sBSP430onewireBus * bus,
                                    sBSP430onewireSerialNumber * snp);

/** Send the command sequence to initiate a temperature measurement
 *
 * Note that this simply requests that the device start to measure the
 * temperature.  The measurement process may take up to 750 msec at
 * the default 12-bit resolution.
 *
 * If the device is externally powered,
 * iBSP430onewireTemperatureReady_ni() can be invoked to determine
 * whether the requested measurement has completed.  If using
 * parasitic power, the application should wait for at least the
 * maximum sample time before invoking
 * iBSP430onewireReadTemperature_ni().
 *
 * @param bus The port and bit identifying the 1-wire bus
 *
 * @return 0 if the request was successfully submitted, -1 if an error
 * occured. */
int iBSP430onewireRequestTemperature_ni (const sBSP430onewireBus * bus);

/** Test whether the device has completed measuring the temperature
 *
 * @param bus The port and bit identifying the 1-wire bus
 *
 * @note Do not invoke this for parasitically-powered devices.  See
 * iBSP430onewireRequestTemperature_ni().
 *
 * @return 0 if the device is still busy; 1 if the sample is ready. */
static int
__inline__
iBSP430onewireTemperatureReady_ni (const sBSP430onewireBus * bus)
{
  return iBSP430onewireReadBit_ni(bus);
}

/** Read the most recent temperature measurement
 *
 * The temperature is a signed 16-bit value representing 1/16th
 * degrees Celcius.  I.e., a value of 1 is 0.0625 Cel.
 *
 * @note The accuracy and precision of the measurement are much more
 * coarse than its resolution.
 *
 * @param bus The port and bit identifying the 1-wire bus
 *
 * @param temp_xCel Pointer to a location into which the temperature
 * will be written.  This must not be null.
 *
 * @return 0 if the read was successful; -1 if an error occurred.  If
 * there was an error, the value pointed to by temp_xCel remains
 * unchanged. */
int iBSP430onewireReadTemperature_ni (const sBSP430onewireBus * bus,
                                      int * temp_xCel);

/** Convert temperature from 1/16th Cel to tenths Fahrhenheit (d[degF])
 *
 * For those of us who live in the US.
 *
 * 9 * t / 8 == (9/5) * 10 * (t / 16) without incidental overflow
 */
#define BSP430_ONEWIRE_xCel_TO_ddegF(_xcel) (320 + ((9 * t_c) / 8))

#endif /* BSP430_UTILITY_ONEWIRE_H */
