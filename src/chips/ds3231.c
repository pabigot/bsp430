/* Copyright (c) 2013, Peter A. Bigot
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

/**
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/chips/ds3231.h>
#include <string.h>

#define ENCODE_BCD(_v) ((((_v) / 10) << 4) | ((_v) % 10))
#define DECODE_BCD(_v) (((_v) & 0x0F) + 10 * (((_v) >> 4) & 0x0F))

uint8_t ucDS3231encodeHour (unsigned int hours,
                            int is_12h)
{
  uint8_t rv;

  rv = (is_12h ? BIT6 : 0);
  if (BIT6 & rv) {
    /* 12-hour mode */
    if (12 <= hours) {
      rv |= BIT5;
      if (12 < hours) {
        hours -= 12;
      }
    }
    rv |= ENCODE_BCD(hours);
  } else {
    rv |= (hours % 10);
    if (20 <= hours) {
      rv |= BIT5;
    } else if (10 <= hours) {
      rv |= BIT4;
    }
  }
  return rv;
}

unsigned int uiDS3231decodeHour (uint8_t hour_ds3231)
{
  unsigned int hour;
  hour = hour_ds3231 & 0x0F;
  if (BIT4 & hour_ds3231) {
    hour += 10;
  }
  if (BIT6 & hour_ds3231) {
    /* 12-hour mode */
    if ((BIT5 & hour_ds3231) && (12 > hour)) {
      hour += 12;
    }
  } else {
    /* 24-hour mode */
    if (BIT5 & hour_ds3231) {
      hour += 20;
    }
  }
  return hour;
}

const struct tm *
xDS3231registersToTm (const sDS3231registers * regp,
                      struct tm * timep) {
  memset(timep, 0, sizeof(*timep));
  timep->tm_sec = DECODE_BCD(regp->seconds);
  timep->tm_min = DECODE_BCD(regp->minutes);
  timep->tm_hour = uiDS3231decodeHour(regp->hours);
  timep->tm_mday = DECODE_BCD(regp->dom);
  timep->tm_year = DECODE_BCD(regp->year);
  timep->tm_mon = DECODE_BCD(regp->mon_cen & 0x1F) - 1;
  if (BIT7 & regp->mon_cen) {
    timep->tm_year += 100;
  }
  timep->tm_wday = regp->dow - 1;
  return timep;
}

const sDS3231registers *
xDS3231tmToRegisters (const struct tm * timep,
                      sDS3231registers * regp)
{
  regp->seconds = ENCODE_BCD(timep->tm_sec);
  regp->minutes = ENCODE_BCD(timep->tm_min);
  regp->hours = ucDS3231encodeHour(timep->tm_hour, regp->hours & BIT6);
  regp->dow = timep->tm_wday + 1;
  regp->dom = ENCODE_BCD(timep->tm_mday);
  regp->mon_cen = 1 + ENCODE_BCD(timep->tm_mon);
  if (100 <= timep->tm_year) {
    regp->mon_cen |= BIT7;
  }
  regp->year = ENCODE_BCD(timep->tm_year % 100);
  return regp;
}
