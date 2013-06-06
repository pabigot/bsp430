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

#ifndef BSP430_CHIPS_DS3231_H
#define BSP430_CHIPS_DS3231_H

/** @file
 * @brief Chip data for Maxim DS3231 I2C RTC/TCXO/Crystal
 *
 * This file contains the register map for the DS3231 real-time clock
 * chip.  There is no abstraction for the functional capability of the
 * chip, except that there are routines to convert between the
 * register map and POSIX standard time structures.
 *
 * To incorporate the conversion routines add the following to your @c Makefile:
@verbatim
VPATH += $(BSP430_ROOT)/chips
MODULES += chips/ds3231
@endverbatim
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/core.h>
#include <time.h>

/** I2C address for DS3231 I2C RTX/TCXO/Crystal */
#define DS3231_I2C_ADDRESS 0x68

/** DS3231 register map */
BSP430_CORE_PACKED_STRUCT(sDS3231registers) {
  uint8_t seconds;              /**< 0x00 Seconds (0-59) (BCD) */
  uint8_t minutes;              /**< 0x01 Minutes (0-59) (BCD) */
  uint8_t hours;                /**< 0x02 Hours (0-23) (BCD plus) */
  uint8_t dow;                  /**< 0x03 Day of week (1-7) */
  uint8_t dom;                  /**< 0x04 Date (day of month) (1-31) (BCD) */
  uint8_t mon_cen;              /**< 0x05 Month (BCD), plus century past 1900 */
  uint8_t year;                 /**< 0x06 Year within century (BCD) */
  uint8_t alrm1_sec;            /**< 0x07 Alarm1 seconds (BCD), A1M1 */
  uint8_t alrm1_min;            /**< 0x08 Alarm1 minutes (BCD), A1M2 */
  uint8_t alrm1_hr;             /**< 0x09 Alarm1 hours (BCD plus), A1M3 */
  uint8_t alrm1_doa;            /**< 0x0A Alarm1 day/date (BCD plus), A1M4 */
  uint8_t alrm2_min;            /**< 0x0B Alarm2 minutes (BCD), A2M2 */
  uint8_t alrm2_hr;             /**< 0x0C Alarm2 hours (BCD plus), A2M3 */
  uint8_t alrm2_doa;            /**< 0x0D Alarm2 date/date (BCD plus), A2M4 */
  uint8_t ctrl;                 /**< 0x0E Control bits */
  uint8_t stat;                 /**< 0x0F Control/status bits */
  uint8_t aging;                /**< 0x10 Aging offset */
  uint8_t temp_msb;             /**< 0x11 MSB of temperature (C/256) */
  uint8_t temp_lsb;             /**< 0x12 LSB of temperature (C/256) */
};
typedef struct sDS3231registers sDS3231registers;

/** Convert from integer hour value to DS3231 encoded hour
 *
 * The DS3231 represents hours as a register encoding either a 12-hr
 * or 24-hr time, not a basic BCD representation.
 *
 * @param hours the integer hour value in the range 0..23
 * @param is_12h non-zero to represent the time as 12h with AM/PM; zero to represent time as a 24-hour value.
 * @return hour value encoded in DS3231 format */
uint8_t ucDS3231encodeHour (unsigned int hours,
                            int is_12h);

/** Convert from encoded DS3231 hour value to integer hour
 *
 * @param hour_ds3231 Hour value encoded in DS3231 including 12h/24h flag in #BIT6
 * @return integer hour value in range 0..23 */
unsigned int uiDS3231decodeHour (uint8_t hour_ds3231);

/** Convert DS3231 date/time register settings to POSIX standard structure.
 *
 * The previous contents of @a *timep are completely erased.  The time
 * conversion supports both 12-hour and 24-hour formats.
 *
 * @param regp pointer to a DS3231 register map complete through byte 0x06.
 *
 * @param timep pointer to a POSIX time structure that will be cleared
 * and initialized to the time recorded in @a regp
 *
 * @return @a timep
 */
const struct tm *
xDS3231registersToTm (const sDS3231registers * regp,
                      struct tm * timep);

/** Convert POSIX time structure to DSD3231 date/time register settings.
 *
 * The first seven bytes of @a *regp are erased, except that #BIT6 of
 * sDS3231registers::hours is preserved to record whether 12-hour or
 * 24-hour time is desired.  The remaining bytes of @a *regp are
 * unmodified.
 *
 * @param timep pointer to the POSIX time
 * @param regp pointer to a DS3231 register map
 * @return @a regp */
const sDS3231registers *
xDS3231tmToRegisters (const struct tm * timep,
                      sDS3231registers * regp);

#endif /* BSP430_CHIPS_DS3231_H */
