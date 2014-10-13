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

#include <bsp430/platform.h>
#include <bsp430/utility/uptime.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#if (BSP430_UPTIME - 0)
/* Inhibit definition if required components were not provided. */

hBSP430halTIMER xBSP430uptimeTIMER_;
unsigned long ulBSP430uptimeConversionFrequency_Hz_ni_;

#if (configBSP430_UPTIME_EPOCH - 0)
/* Flag indicating epoch is valid.  Invalidated by resuming the timer,
 * so needs to be visible to those routines. */
static char epoch_is_valid_ni;

/* Number of bits of precision available for sub-second times.  Set by
 * iBSP430uptimeResume_ni() to a non-negative value based on
 * conversion frequency. */
static int8_t epoch_precision_bits = -1;

#endif /* configBSP430_UPTIME_EPOCH */

#if (configBSP430_UPTIME_DELAY - 0)

/** Bit set when alarm has gone off */
#define DELAY_ALARM_FIRED 0x01

/** Bit set when initialization validated that the alarm could be
 * used.  If this bit is clear, the alarm functionality will never be
 * used, simply because the handle isn't valid. */
#define DELAY_ALARM_VALID 0x02

/** Bit set if delay alarm is enabled.  This does not mean it's
 * registered with the timer infrastructure, only that it should be
 * registered when the uptime timer is running. */
#define DELAY_ALARM_ENABLED 0x04

/** Bit set if the timer alarm underlying the delay alarm is enabled,
 * i.e. the uptime timer is running and delay alarm callback hooks are
 * in place. */
#define DELAY_ALARM_REGISTERED 0x08

/** Bit set if the uptime timer is active.  This is used as a filter
 * on whether to register the delay infrastructure. */
#define DELAY_ALARM_TIMER_ACTIVE 0x10

struct sDelayAlarm {
  /** The underlying alarm infrastructure.  First in the structure so
   * we can access the delay-specific metadata from the handler. */
  sBSP430timerAlarm alarm;

  /** Other information related to delay management */
  volatile unsigned int flags;
};

/* Private delay alarm structure */
static struct sDelayAlarm delayAlarm_;
#define H_delayAlarm (&delayAlarm_.alarm)

static int
delayCallback_ (hBSP430timerAlarm alarm)
{
  struct sDelayAlarm * ap = (struct sDelayAlarm *)alarm;
  ap->flags |= DELAY_ALARM_FIRED;
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

static int
delayAlarmSetRegistered_ni_ (int enablep)
{
  int rv = 0;
  if (enablep) {
    /* Enable the delay alarm if desired and not already registered. */
    if ((DELAY_ALARM_ENABLED & delayAlarm_.flags)
        && (! (DELAY_ALARM_REGISTERED & delayAlarm_.flags))) {
      rv = iBSP430timerAlarmSetEnabled_ni(H_delayAlarm, 1);
      if (0 == rv) {
        delayAlarm_.flags |= DELAY_ALARM_REGISTERED;
      }
    }
  } else {
    if (DELAY_ALARM_REGISTERED & delayAlarm_.flags) {
      (void)iBSP430timerAlarmCancel_ni(H_delayAlarm);
      (void)iBSP430timerAlarmSetEnabled_ni(H_delayAlarm, 0);
      delayAlarm_.flags &= ~DELAY_ALARM_REGISTERED;
    }
  }
  return rv;
}

#endif /* configBSP430_UPTIME_DELAY */

void
vBSP430uptimeStart_ni (void)
{
  xBSP430uptimeTIMER_ = hBSP430timerLookup(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
  vBSP430timerSafeCounterInitialize_ni(xBSP430uptimeTIMER_->hpl);
  xBSP430uptimeTIMER_->hpl->ctl = 0;
  vBSP430timerResetCounter_ni(xBSP430uptimeTIMER_);
  xBSP430uptimeTIMER_->hpl->ctl =
    ((TASSEL0 | TASSEL1) & (BSP430_UPTIME_TASSEL))
    | ((ID0 | ID1) & (BSP430_UPTIME_DIVIDING_SHIFT * ID0))
    | TACLR | TAIE;
  vBSP430timerInferHints_ni(xBSP430uptimeTIMER_);
#if (configBSP430_UPTIME_DELAY - 0)
  {
    hBSP430timerAlarm delay_alarm;

    delayAlarm_.flags = 0;
    delay_alarm = hBSP430timerAlarmInitialize(&delayAlarm_.alarm, BSP430_UPTIME_TIMER_PERIPH_HANDLE, BSP430_UPTIME_DELAY_CCIDX, delayCallback_);
    while (H_delayAlarm != delay_alarm) {
      /* Hang if the alarm cannot be used. */
    }
    delayAlarm_.flags = DELAY_ALARM_VALID | DELAY_ALARM_ENABLED | DELAY_ALARM_TIMER_ACTIVE;
  }
#endif /* configBSP430_UPTIME_DELAY */
  vBSP430uptimeResume_ni();
}

void
vBSP430uptimeSuspend_ni (void)
{
  xBSP430uptimeTIMER_->hpl->ctl &= ~(MC0 | MC1);
#if (configBSP430_UPTIME_DELAY - 0)
  (void)delayAlarmSetRegistered_ni_(0);
  delayAlarm_.flags &= ~DELAY_ALARM_TIMER_ACTIVE;
#endif /* configBSP430_UPTIME_DELAY */
}

void
vBSP430uptimeResume_ni (void)
{
  ulBSP430uptimeConversionFrequency_Hz_ni_ = ulBSP430timerFrequency_Hz_ni(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
#if (configBSP430_UPTIME_DELAY - 0)
  delayAlarm_.flags |= DELAY_ALARM_TIMER_ACTIVE;
  (void)delayAlarmSetRegistered_ni_(1);
#endif /* configBSP430_UPTIME_DELAY */
#if (configBSP430_UPTIME_EPOCH - 0)
  epoch_is_valid_ni = 0;
  {
    unsigned long divisor = ulBSP430uptimeConversionFrequency_Hz_ni_;
    epoch_precision_bits = 0;
    while (0 != divisor) {
      ++epoch_precision_bits;
      divisor >>= 1;
    }
  }
#endif /* configBSP430_UPTIME_EPOCH */
  xBSP430uptimeTIMER_->hpl->ctl |= MC_2;
}

unsigned long
ulBSP430uptimeConversionFrequency_Hz (void)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  unsigned long rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  rv = ulBSP430uptimeConversionFrequency_Hz_ni_;
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

unsigned long
ulBSP430uptimeSetConversionFrequency_ni (unsigned long frequency_Hz)
{
  unsigned long rv = ulBSP430uptimeConversionFrequency_Hz_ni_;
  ulBSP430uptimeConversionFrequency_Hz_ni_ = frequency_Hz;
  return rv;
}

const char *
xBSP430uptimeAsText (unsigned long duration_utt,
                     char * buffer)
{
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
    snprintf(buffer, BSP430_UPTIME_AS_TEXT_LENGTH, "%u:%02u:%02u.%03u", (unsigned int)q_hr, min, sec, msec);
  } else {
    snprintf(buffer, BSP430_UPTIME_AS_TEXT_LENGTH, "%2u:%02u.%03u", min, sec, msec);
  }
  return buffer;
}

const char *
xBSP430uptimeAsText_ni (unsigned long duration_utt)
{
  static char buf[BSP430_UPTIME_AS_TEXT_LENGTH];
  return xBSP430uptimeAsText(duration_utt, buf);
}

unsigned int
uiBSP430uptimeScaleForDisplay (unsigned long long duration_utt,
                               const char ** unitp)
{
  const unsigned long long s_utt = ulBSP430uptimeConversionFrequency_Hz_ni_;
  const unsigned long long min_utt = 60U * s_utt;
  const unsigned long long h_utt = 60U * min_utt;
  const unsigned long long us_cutoff_utt = (10 * s_utt) / 1000U;
  const unsigned long long ms_cutoff_utt = 10 * s_utt;
  const unsigned long long s_cutoff_utt = 3U * min_utt;
  const unsigned long long min_cutoff_utt = 3U * h_utt;

  if (duration_utt < us_cutoff_utt) {
    *unitp = "us";
    return (1000000U * duration_utt) / s_utt;
  }
  if (duration_utt < ms_cutoff_utt) {
    *unitp = "ms";
    return (1000U * duration_utt) / s_utt;
  }
  if (duration_utt < s_cutoff_utt) {
    *unitp = "s";
    return duration_utt / s_utt;
  }
  if (duration_utt < min_cutoff_utt) {
    *unitp = "min";
    return duration_utt / min_utt;
  }
  *unitp = "h";
  return duration_utt / h_utt;
}

void
vBSP430uptimeActivityReset (sBSP430uptimeActivityTotals * sp,
                            bool from_now)
{
  memset(sp, 0, sizeof(*sp));
  if (from_now) {
    sp->last_wake_utt = sp->last_sleep_utt = ulBSP430uptime();
  }
}

#if (configBSP430_UPTIME_DELAY - 0)

int
iBSP430uptimeDelaySetEnabled_ni (int enablep)
{
  int rv = 0;

  /* If the alarm isn't valid, error. */
  if (! (DELAY_ALARM_VALID & delayAlarm_.flags)) {
    return -1;
  }
  if (enablep) {
    delayAlarm_.flags |= DELAY_ALARM_ENABLED;
    if (delayAlarm_.flags & DELAY_ALARM_TIMER_ACTIVE) {
      rv = delayAlarmSetRegistered_ni_(1);
    }
  } else {
    if (delayAlarm_.flags & DELAY_ALARM_TIMER_ACTIVE) {
      rv = delayAlarmSetRegistered_ni_(0);
    }
    delayAlarm_.flags &= ~DELAY_ALARM_ENABLED;
  }
  return rv;
}

long
lBSP430uptimeSleepUntil (unsigned long setting_utt,
                         unsigned int lpm_bits)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  long rv = 0;
  int rc;

  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    /* Exit immediately unless the alarm callback is registered. */
    if (! (DELAY_ALARM_REGISTERED & delayAlarm_.flags)) {
      break;
    }
    delayAlarm_.flags &= ~DELAY_ALARM_FIRED;
    rc = iBSP430timerAlarmSet_ni(H_delayAlarm, setting_utt);
    if (0 != rc) {
      break;
    }
    /* Sleep until the alarm goes off, or something else wakes us up.
     * Immediately disable the interrupts in the unlikely event the
     * interrupt return enabled them. */
    BSP430_CORE_LPM_ENTER_NI(lpm_bits);
    BSP430_CORE_DISABLE_INTERRUPT();

    /* Cancel the alarm if it hasn't fired yet. */
    if (! (delayAlarm_.flags & DELAY_ALARM_FIRED)) {
      (void)iBSP430timerAlarmCancel_ni(H_delayAlarm);
    }
    rv = setting_utt - ulBSP430uptime_ni();
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

#endif /* configBSP430_UPTIME_DELAY */

#if (configBSP430_UPTIME_EPOCH - 0)

#define ntohl(x_) BSP430_CORE_SWAP_32(x_)
#define htonl(x_) BSP430_CORE_SWAP_32(x_)
#define ntohs(x_) BSP430_CORE_SWAP_16(x_)
#define htons(x_) BSP430_CORE_SWAP_16(x_)

/** Number of microseconds per second, for convenience */
#define US_PER_S 1000000UL

/** The duration of an era in uptime ticks. */
#define ERA_UTT (1 + (uint64_t)~(unsigned long)0)

/* NTP epoch. */
static uint64_t epoch_ntp_ni;
static unsigned long epoch_updated_utt_ni;
static struct timeval epoch_tv_ni;

int
iBSP430uptimeCheckEpochValidity ()
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  long rv;
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = epoch_is_valid_ni ? 0 : -1;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

unsigned long
ulBSP430uptimeLastEpochUpdate ()
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  unsigned long rv;
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = epoch_updated_utt_ni;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

long
lBSP430uptimeEpochAge ()
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  long rv;
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    if (! epoch_is_valid_ni) {
      rv = -1L;
    } else {
      rv = ulBSP430uptime_ni() - epoch_updated_utt_ni;
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

static int
epoch_era_ni (unsigned long utt)
{
  const unsigned long HALF_ERA = 0x80000000L;
  unsigned long delta_utt;
  int is_before;

  if (! epoch_is_valid_ni) {
    return -1;
  }
  delta_utt = epoch_updated_utt_ni - utt;
  is_before = (delta_utt < HALF_ERA);
  if (is_before) {
    /* utt appears to precede the time the epoch was set.  If it's too
     * far in the past, the epoch is not valid for this time. */
    if (delta_utt >= BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT) {
      return -1;
    }
    /* If utt is before the update time, but its value is greater,
     * then return 0 to indicate it belongs to the previous era. */
    if (utt > epoch_updated_utt_ni) {
      return 0;
    }
  } else {
    /* utt appears to follow the time the epoch was set.  If it's too
     * far in the future, the epoch is not valid for this time. */
    if (-delta_utt >= BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT) {
      return -1;
    }
    /* If utt is after the update time, but its value is lesser, then
     * return 2 to indicate it belongs to the next era. */
    if (utt < epoch_updated_utt_ni) {
      return 2;
    }
  }
  /* Return 1 to indicate that the epoch is valid for and set within
   * the same era as utt. */
  return 1;
}

int
iBSP430uptimeEpochEra (unsigned long time_utt)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = epoch_era_ni(time_utt);
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

static uint64_t
timestamp_to_ntp (const sBSP430uptimeNTPTimestamp * fp)
{
  return ((uint64_t)ntohl(fp->integral) << 32) | ntohl(fp->fractional);
}

/* Note that this converts 64-bit uptime counters which may have
 * values longer than an era can represent. */
static void
get_relative_timeval (uint64_t utt,
                      struct timeval * tv)
{
  tv->tv_sec = (utt / ulBSP430uptimeConversionFrequency_Hz_ni_);
  tv->tv_usec  = (US_PER_S * (utt % ulBSP430uptimeConversionFrequency_Hz_ni_)) / ulBSP430uptimeConversionFrequency_Hz_ni_;
  return;
}

/* Note that this converts 64-bit uptime counters which may have
 * values longer than an era can represent. */
static uint64_t
get_relative_ntp (uint64_t utt)
{
  return (utt << 32) / ulBSP430uptimeConversionFrequency_Hz_ni_;
}

int
iBSP430uptimeSetNTPXmtField (sBSP430uptimeNTPPacketHeader * ntpp,
                             unsigned long * putt)
{
  unsigned long utt;
  int rv;
  uint64_t ntp;

  if (putt) {
    utt = *putt;
  } else {
    utt = ulBSP430uptime();
  }
  rv = iBSP430uptimeAsNTP(utt, &ntp, 1);
  if (0 == rv) {
    ntpp->xmt.integral = htonl((uint32_t)(ntp >> 32));
    ntpp->xmt.fractional = htonl((uint32_t)(ntp & ~(uint32_t)0));
  }
  return rv;
}

int
iBSP430uptimeInitializeNTPRequest (sBSP430uptimeNTPPacketHeader * ntpp)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);

  if (! ntpp) {
    return -1;
  }
  memset(ntpp, 0, sizeof(*ntpp));
  ntpp->li_vn_mode = 0x23; /* No leap info, NTP version 4, client mode */
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    ntpp->precision = -epoch_precision_bits;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return 0;
}

int
iBSP430uptimeSetEpochFromNTP (uint64_t epoch_ntp)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    epoch_ntp_ni = epoch_ntp;
    epoch_tv_ni.tv_sec = ((uint32_t)(epoch_ntp >> 32) - BSP430_UPTIME_POSIX_EPOCH_NTPIS);
    epoch_tv_ni.tv_usec = (US_PER_S * (epoch_ntp & ~(uint32_t)0)) >> 32;
    epoch_updated_utt_ni = ulBSP430uptime_ni();
    epoch_is_valid_ni = 1;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return 0;
}

int
iBSP430uptimeAdjustEpochFromNTP (int64_t adjustment_ntp)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv;

  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = iBSP430uptimeSetEpochFromNTP(epoch_ntp_ni + adjustment_ntp);
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

int
iBSP430uptimeSetEpochFromTimeval (const struct timeval * tv,
                                  unsigned long when_utt)
{
  uint64_t epoch_ntp;

  if (! tv) {
    return -1;
  }
  epoch_ntp = (BSP430_UPTIME_POSIX_EPOCH_NTPIS + (uint64_t)tv->tv_sec) << 32;
  epoch_ntp += ((uint64_t)tv->tv_usec << 32) / US_PER_S;
  epoch_ntp -= get_relative_ntp(when_utt);
  return iBSP430uptimeSetEpochFromNTP(epoch_ntp);
}

int
iBSP430uptimeAsNTP (unsigned long utt,
                    uint64_t * ntpp,
                    int bypass_validation)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rv = -1;
  uint64_t ntp;

  ntp = get_relative_ntp(utt);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    int era;
    era = epoch_era_ni(utt);
    if (0 > era) {
      if (! bypass_validation) {
        epoch_is_valid_ni = 0;
        break;
      }
      era = 0;
      ntp += BSP430_UPTIME_BYPASS_EPOCH_NTP;
    } else {
      era -= 1;
      ntp += epoch_ntp_ni;
    }
    if (0 != era) {
      uint64_t era_ntp = get_relative_ntp(ERA_UTT);
      if (0 > era) {
        ntp -= era_ntp;
      } else {
        ntp += era_ntp;
      }
    }
    rv = 0;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  if (0 == rv) {
    if (ntpp) {
      *ntpp = ntp;
    }
  }
  return rv;
}

int
iBSP430uptimeAsTimeval (unsigned long utt,
                        struct timeval * tvp)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  struct timeval tv;
  int rv = -1;

  get_relative_timeval(utt, &tv);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    int era = epoch_era_ni(utt);
    if (0 > era) {
      epoch_is_valid_ni = 0;
      break;
    }
    era -= 1;
    tv.tv_sec += epoch_tv_ni.tv_sec;
    tv.tv_usec += epoch_tv_ni.tv_usec;
    if (0 != era) {
      struct timeval etv;
      get_relative_timeval(ERA_UTT, &etv);
      if (0 > era) {
        /* Subtract the duration of an era.  Note that modern timeval
         * has signed tv_usec so this is safe. */
        tv.tv_sec -= etv.tv_sec;
        tv.tv_usec -= etv.tv_usec;
      } else {
        /* Add the duration of an era */
        tv.tv_sec += etv.tv_sec;
        tv.tv_usec += etv.tv_usec;
      }
    }
    rv = 0;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  if (0 == rv) {
    /* Normalize the result */
    while (0 > tv.tv_usec) {
      tv.tv_sec -= 1;
      tv.tv_usec += US_PER_S;
    }
    while (US_PER_S <= tv.tv_usec) {
      tv.tv_usec -= US_PER_S;
      tv.tv_sec += 1;
    }
    if (tvp) {
      *tvp = tv;
    }
  }
  return rv;
}

time_t
xBSP430uptimeAsPOSIXTime (unsigned long utt)
{
  struct timeval tv;
  tv.tv_sec = (time_t)-1;
  (void)iBSP430uptimeAsTimeval(utt, &tv);
  return tv.tv_sec;
}

int
iBSP430uptimeProcessNTPResponse (const sBSP430uptimeNTPPacketHeader * req,
                                 const sBSP430uptimeNTPPacketHeader * resp,
                                 uint64_t rec_ntp,
                                 int64_t * adjustment_ntp,
                                 long * adjustment_ms,
                                 unsigned long * rtt_us)
{
  if ((! resp)
      || (0 == resp->stratum)
      || (0 == resp->xmt.integral)
      || (0 == resp->xmt.fractional)) {
    return -1;
  }
  if (req && ((req->xmt.integral != resp->org.integral)
              || (req->xmt.fractional != resp->org.fractional))) {
    return -1;
  }
  uint64_t t1 = timestamp_to_ntp(&resp->org);
  uint64_t t2 = timestamp_to_ntp(&resp->rec);
  uint64_t t3 = timestamp_to_ntp(&resp->xmt);
  uint64_t t4 = rec_ntp;
  int64_t theta = (((int64_t)t2 - (int64_t)t1) + ((int64_t)t3 - (int64_t)t4)) / 2;
  uint64_t delta = (t4 - t1) - (t3 - t2);
  if (adjustment_ntp) {
    *adjustment_ntp = theta;
  }
  if (adjustment_ms) {
    int64_t adjustment64_ms = theta >> 22;
    /* The adjustment is usually either really big, or fairly small.
     * If it's big enough that the approximation of (1024*theta)/2^32
     * doesn't fit in 32 bits, just use the saturated maxima.  If it
     * does fit, calculate the real value (which will be smaller). */
    if ((int32_t)adjustment64_ms == adjustment64_ms) {
      *adjustment_ms = (1000 * theta) >> 32;
    } else {
      *adjustment_ms = (0 > theta) ? LONG_MIN : LONG_MAX;
    }
  }
  if (rtt_us) {
    uint64_t rtt64_us = (US_PER_S * delta) >> 32;
    uint32_t rtt32_us = rtt64_us;
    if (rtt64_us == rtt32_us) {
      *rtt_us = rtt32_us;
    } else {
      *rtt_us = ULONG_MAX;
    }
  }
  return 0;
}

#endif /* configBSP430_UPTIME_DELAY */

#endif /* BSP430_UPTIME */
