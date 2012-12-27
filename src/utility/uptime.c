/* Copyright (c) 2012, Peter A. Bigot
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

#include <bsp430/platform.h>
#include <bsp430/utility/uptime.h>
#include <stdio.h>

#if BSP430_UPTIME - 0
/* Inhibit definition if required components were not provided. */

hBSP430halTIMER xBSP430uptimeTIMER_;
unsigned long ulBSP430uptimeConversionFrequency_Hz_ni_;

void
vBSP430uptimeStart_ni (void)
{
  xBSP430uptimeTIMER_ = hBSP430timerLookup(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
  xBSP430uptimeTIMER_->hpl->ctl = 0;
  vBSP430timerResetCounter_ni(xBSP430uptimeTIMER_);
  xBSP430uptimeTIMER_->hpl->ctl =
    ((TASSEL0 | TASSEL1) & (BSP430_UPTIME_TASSEL))
    | ((ID0 | ID1) & (BSP430_UPTIME_DIVIDING_SHIFT))
    | TACLR | TAIE;
  vBSP430uptimeResume_ni();
}

void
vBSP430uptimeSuspend_ni (void)
{
  xBSP430uptimeTIMER_->hpl->ctl &= ~(MC0 | MC1);
}

void
vBSP430uptimeResume_ni (void)
{
  ulBSP430uptimeConversionFrequency_Hz_ni_ = ulBSP430timerFrequency_Hz_ni(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
  xBSP430uptimeTIMER_->hpl->ctl |= MC_2;
}

unsigned long
ulBSP430uptimeConversionFrequency_Hz_ni (void)
{
  return ulBSP430uptimeConversionFrequency_Hz_ni_;
}

unsigned long
ulBSP430uptimeSetConversionFrequency_ni (unsigned long frequency_Hz)
{
  unsigned long rv = ulBSP430uptimeConversionFrequency_Hz_ni_;
  ulBSP430uptimeConversionFrequency_Hz_ni_ = frequency_Hz;
  return rv;
}

const char *
xBSP430uptimeAsText_ni (unsigned long duration_utt)
{
  static char buf[sizeof("HHH:MM:SS.mmm")];
  unsigned long conversionFrequency_Hz;
  unsigned int msec;
  unsigned int sec;
  unsigned int min;
  unsigned long q_sec;
  unsigned long r_utt;
  unsigned long q_min;
  unsigned long q_hr;

  conversionFrequency_Hz = ulBSP430uptimeConversionFrequency_Hz_ni_;
  q_sec = duration_utt / conversionFrequency_Hz;
  r_utt = duration_utt - (q_sec * conversionFrequency_Hz);
  msec = (1000L * r_utt) / conversionFrequency_Hz;
  q_min = q_sec / 60;
  sec = q_sec - (q_min * 60);
  q_hr = q_min / 60;
  min = (q_min - (q_hr * 60));
  if (0 < q_hr) {
    snprintf(buf, sizeof(buf), "%u:%02u:%02u.%03u", (unsigned int)q_hr, min, sec, msec);
  } else {
    snprintf(buf, sizeof(buf), "%2u:%02u.%03u", min, sec, msec);
  }
  return buf;
}

#endif /* BSP430_UPTIME */
