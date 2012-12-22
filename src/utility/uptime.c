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
#include <stdlib.h>
#include <stdio.h>

#if BSP430_UPTIME - 0
/* Inhibit definition if required components were not provided. */

hBSP430halTIMER xBSP430uptimeTIMER_;

void
vBSP430uptimeStart_ni (void)
{
  xBSP430uptimeTIMER_ = hBSP430timerLookup(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
  xBSP430uptimeTIMER_->hpl->ctl = 0;
  vBSP430timerResetCounter_ni(xBSP430uptimeTIMER_);
  xBSP430uptimeTIMER_->hpl->ctl =
    ((TASSEL0 | TASSEL1) & (BSP430_UPTIME_TASSEL))
    | ((ID0 | ID1) & (BSP430_UPTIME_DIVIDING_SHIFT))
    | MC_2 | TACLR | TAIE;
}

void
vBSP430uptimeSuspend_ni (void)
{
  xBSP430uptimeTIMER_->hpl->ctl &= ~(MC0 | MC1);
}

void
vBSP430uptimeResume_ni (void)
{
  xBSP430uptimeTIMER_->hpl->ctl |= MC_2;
}

static unsigned long uptimeConversionFrequency_Hz_;

static BSP430_CORE_INLINE
unsigned long
uptimeConversionFrequency_Hz_ni (void)
{
  if (0 == uptimeConversionFrequency_Hz_) {
    /* Get the frequency of the underlying timer.  If there's a
     * divisor, the timer routine will take that into account. */
    uptimeConversionFrequency_Hz_ = ulBSP430timerFrequency_Hz_ni(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
  }
  return uptimeConversionFrequency_Hz_;
}

unsigned long
ulBSP430uptimeConversionFrequency_Hz_ni (void)
{
  return uptimeConversionFrequency_Hz_ni();
}

unsigned long
ulBSP430uptimeSetConversionFrequency_ni (unsigned long frequency_Hz)
{
  unsigned long rv = uptimeConversionFrequency_Hz_;
  uptimeConversionFrequency_Hz_ = frequency_Hz;
  return rv;
}

const char *
xBSP430uptimeAsText_ni (unsigned long duration_utt)
{
  static char buf[sizeof("HHH:MM:SS.mmm")];
  ldiv_t ld;
  unsigned long conversionFrequency_Hz;
  unsigned int msec;
  unsigned int sec;
  unsigned int min;

  conversionFrequency_Hz = uptimeConversionFrequency_Hz_ni();
  ld = ldiv(duration_utt, conversionFrequency_Hz);
  msec = (1000L * ld.rem) / conversionFrequency_Hz;
  ld = ldiv(ld.quot, 60);
  sec = ld.rem;
  ld = ldiv(ld.quot, 60);
  min = ld.rem;
  if (0 < ld.quot) {
    snprintf(buf, sizeof(buf), "%u:%02u:%02u.%03u", (unsigned int)ld.quot, min, sec, msec);
  } else {
    snprintf(buf, sizeof(buf), "%2u:%02u.%03u", min, sec, msec);
  }
  return buf;
}

#endif /* BSP430_UPTIME */
