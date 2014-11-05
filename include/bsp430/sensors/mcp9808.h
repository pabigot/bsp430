/* Copyright 2014, Peter A. Bigot
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

#ifndef BSP430_SENSORS_MCP9808_H
#define BSP430_SENSORS_MCP9808_H

/** @file
 *
 * @brief Interface to the <a
 * href="http://www.microchip.com/MCP9808">MCP9808</a> digital
 * temperature sensor.
 *
 * This device measures temperature in Celcius (K + 273.15) with an
 * accuracy of 0.25 K and a resolution of 0.0625 K.
 *
 * The device permits read and (sometimes) write of 16-bit registers
 * that are stored in MSB order, except for the 8-bit resolution
 * register.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/serial.h>
#include <bsp430/periph/timer.h>

/** The 7-bit I2C slave address for the device.  If you pull up any of
 * the three address lines, add the binary value to this. */
#define BSP430_SENSORS_MCP9808_I2C_ADDRESS 0x18

/** Register index for configuration */
#define BSP430_SENSORS_MCP9808_REGNO_CONFIG 1

/** Register index for alert upper boundary trip */
#define BSP430_SENSORS_MCP9808_REGNO_TUPPER 2

/** Register index for alert lower boundary trip */
#define BSP430_SENSORS_MCP9808_REGNO_TLOWER 3

/** Register index for critical temperature */
#define BSP430_SENSORS_MCP9808_REGNO_TCRIT 4

/** Register index for temperature */
#define BSP430_SENSORS_MCP9808_REGNO_TA 5

/** Register index for manufacturer ID */
#define BSP430_SENSORS_MCP9808_REGNO_MFGID 6

/** Register index for device ID/revision */
#define BSP430_SENSORS_MCP9808_REGNO_DEVID 7

/** Register index for device resolution (8-bit) */
#define BSP430_SENSORS_MCP9808_REGNO_RESOLUTION 8

/** Configuration bitmask for no hysteresis */
#define BSP430_SENSORS_MCP9808_CONFIG_HYST0p0 0x0000

/** Configuration bitmask for 1.5 K  */
#define BSP430_SENSORS_MCP9808_CONFIG_HYST1p5 0x0200

/** Configuration bitmask for 3.0 K  */
#define BSP430_SENSORS_MCP9808_CONFIG_HYST3p0 0x0400

/** Configuration bitmask for 6.0 K  */
#define BSP430_SENSORS_MCP9808_CONFIG_HYST6p0 0x0600

/** Mask for hysterisis bits */
#define BSP430_SENSORS_MCP9808_CONFIG_HYSTM 0x0600

/** Shutdown mode.  Setting this enables low-power; clearing it
 * enables continuous conversion. */
#define BSP430_SENSORS_MCP9808_CONFIG_SHDN 0x0010

/** Critical register lock bit */
#define BSP430_SENSORS_MCP9808_CONFIG_CRITLCK 0x0080

/** Upper/lower window register lock bit */
#define BSP430_SENSORS_MCP9808_CONFIG_WNDWLCK 0x0040

/** Interrupt clear */
#define BSP430_SENSORS_MCP9808_CONFIG_INTCLR 0x0020

/** Alert status */
#define BSP430_SENSORS_MCP9808_CONFIG_ALRSTAT 0x0010

/** Alert control */
#define BSP430_SENSORS_MCP9808_CONFIG_ALRCTL 0x0008

/** Alert select */
#define BSP430_SENSORS_MCP9808_CONFIG_ALRSEL 0x0004

/** Alert polarity */
#define BSP430_SENSORS_MCP9808_CONFIG_ALRPOL 0x0002

/** Alert mode */
#define BSP430_SENSORS_MCP9808_CONFIG_ALRMOD 0x0001

/** Conversion time, in milliseconds, for the given resolution setting */
#define BSP430_SENSORS_MCP9808_CONVERSION_ms(res_) ((0 == (res_)) ? 30  \
                                                    : (1 == (res_)) ? 35 \
                                                    : (2 == (res_)) ? 130 \
                                                    : 250)

/** Bit set in raw temperature to indicate measured temperature at or
 * above configured critical temperature */
#define BSP430_SENSORS_MCP9808_TEMP_CRIT 0x8000

/** Bit set in raw temperature to indicate measured temperature above
 * configured upper bound temperature */
#define BSP430_SENSORS_MCP9808_TEMP_HIGH 0x4000

/** Bit set in raw temperature to indicate measured temperature below
 * configured lower bound temperature */
#define BSP430_SENSORS_MCP9808_TEMP_LOW 0x2000

/** Bit set in raw temperature to indicate negative temperatures */
#define BSP430_SENSORS_MCP9808_TEMP_SIGN 0x1000

/** Mask for temperature absolute value in Cel/16 */
#define BSP430_SENSORS_MCP9808_TEMP_MASK 0x0FFF

/** Offset added to convert measured Cel/160 to K/160.  Value is
 * (273.15*160) which provides an exact integer. */
#define BSP430_SENSORS_MCP9808_OFFSET_Kd160 (43704UL)

/** Convert a temperature value from the raw value Cel/16 to K/100. */
static BSP430_CORE_INLINE int
iBSP430sensorsMCP9808tempFromRaw_cK (uint16_t raw)
{
  long v = 10 * (raw & BSP430_SENSORS_MCP9808_TEMP_MASK);
  v += BSP430_SENSORS_MCP9808_OFFSET_Kd160;
  /* Convert to K/1600 then divide by 16 to get K/100 = cK. */
  v = (v * 10) / 16;
  if (BSP430_SENSORS_MCP9808_TEMP_SIGN & raw) {
    v = -v;
  }
  return (int)v;
}

#endif /* BSP430_SENSORS_MCP9808_H */
