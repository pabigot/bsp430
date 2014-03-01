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

#include <string.h>
#include <stdlib.h>
#include <bsp430/platform.h>    /* BSP430_PLATFORM_TIMER_CCACLK defined by this */
#include <bsp430/periph/timer.h>
#include <bsp430/clock.h>

#if (BSP430_CORE_FAMILY_IS_5XX - 0)
/* In 5xx Timer_A and Timer_B use the same layout with 0x0E denoting
 * overflow */
#define TA_OVERFLOW 0x0E
#else /* 5xx */
/* Pre-5xx Timer_A only supported 5 CCs and used 0x0A to denote
 * overflow. */
#define TA_OVERFLOW 0x0A
#endif /* 5xx */
/* All families use 0x0E for overflow in Timer_B */
#define TB_OVERFLOW 0x0E

/* !BSP430! periph=timer */
/* !BSP430! instance=TA0,TA1,TA2,TA3,TB0,TB1,TB2 */

#if (configBSP430_HAL_TA0 - 0)

static const sBSP430halISRIndexedChainNode * cc_callback_TA0[
#if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_T0A2__)
  2
#elif defined(__MSP430_HAS_TA3__) || defined(__MSP430_HAS_T0A3__)
  3
#else
  5
#endif /* TA0 */
];

sBSP430halTIMER xBSP430hal_TA0_ = {
  .hal_state = {
    .cflags = BSP430_TIMER_HAL_HPL_VARIANT_TIMER
#if (configBSP430_HAL_TA0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_TA0_ISR */
#if (configBSP430_HAL_TA0_CC0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_TA0_CC0_ISR */
  },
  .hpl = BSP430_HPL_TA0,
  .cc_cbchain_ni = cc_callback_TA0
};
#endif /* configBSP430_HAL_TA0 */

#if (configBSP430_HAL_TA1 - 0)

static const sBSP430halISRIndexedChainNode * cc_callback_TA1[
#if defined(__MSP430_HAS_T1A2__)
  2
#elif defined(__MSP430_HAS_T1A3__)
  3
#else
  5
#endif /* TA1 */
];

sBSP430halTIMER xBSP430hal_TA1_ = {
  .hal_state = {
    .cflags = BSP430_TIMER_HAL_HPL_VARIANT_TIMER
#if (configBSP430_HAL_TA1_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_TA1_ISR */
#if (configBSP430_HAL_TA1_CC0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_TA1_CC0_ISR */
  },
  .hpl = BSP430_HPL_TA1,
  .cc_cbchain_ni = cc_callback_TA1
};
#endif /* configBSP430_HAL_TA1 */

#if (configBSP430_HAL_TA2 - 0)

static const sBSP430halISRIndexedChainNode * cc_callback_TA2[
#if defined(__MSP430_HAS_T2A2__)
  2
#elif defined(__MSP430_HAS_T2A3__)
  3
#else
  5
#endif /* TA2 */
];

sBSP430halTIMER xBSP430hal_TA2_ = {
  .hal_state = {
    .cflags = BSP430_TIMER_HAL_HPL_VARIANT_TIMER
#if (configBSP430_HAL_TA2_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_TA2_ISR */
#if (configBSP430_HAL_TA2_CC0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_TA2_CC0_ISR */
  },
  .hpl = BSP430_HPL_TA2,
  .cc_cbchain_ni = cc_callback_TA2
};
#endif /* configBSP430_HAL_TA2 */

#if (configBSP430_HAL_TA3 - 0)

static const sBSP430halISRIndexedChainNode * cc_callback_TA3[
#if defined(__MSP430_HAS_T3A2__)
  2
#elif defined(__MSP430_HAS_T3A3__)
  3
#else
  5
#endif /* TA3 */
];

sBSP430halTIMER xBSP430hal_TA3_ = {
  .hal_state = {
    .cflags = BSP430_TIMER_HAL_HPL_VARIANT_TIMER
#if (configBSP430_HAL_TA3_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_TA3_ISR */
#if (configBSP430_HAL_TA3_CC0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_TA3_CC0_ISR */
  },
  .hpl = BSP430_HPL_TA3,
  .cc_cbchain_ni = cc_callback_TA3
};
#endif /* configBSP430_HAL_TA3 */

#if (configBSP430_HAL_TB0 - 0)

static const sBSP430halISRIndexedChainNode * cc_callback_TB0[
#if defined(__MSP430_HAS_TB3__) || defined(__MSP430_HAS_T0B3__)
  3
#else
  7
#endif /* TB0 */
];

sBSP430halTIMER xBSP430hal_TB0_ = {
  .hal_state = {
    .cflags = BSP430_TIMER_HAL_HPL_VARIANT_TIMER
#if (configBSP430_HAL_TB0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_TB0_ISR */
#if (configBSP430_HAL_TB0_CC0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_TB0_CC0_ISR */
  },
  .hpl = BSP430_HPL_TB0,
  .cc_cbchain_ni = cc_callback_TB0
};
#endif /* configBSP430_HAL_TB0 */

#if (configBSP430_HAL_TB1 - 0)

static const sBSP430halISRIndexedChainNode * cc_callback_TB1[
#if defined(__MSP430_HAS_T1B3__)
  3
#else
  7
#endif /* TB1 */
];

sBSP430halTIMER xBSP430hal_TB1_ = {
  .hal_state = {
    .cflags = BSP430_TIMER_HAL_HPL_VARIANT_TIMER
#if (configBSP430_HAL_TB1_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_TB1_ISR */
#if (configBSP430_HAL_TB1_CC0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_TB1_CC0_ISR */
  },
  .hpl = BSP430_HPL_TB1,
  .cc_cbchain_ni = cc_callback_TB1
};
#endif /* configBSP430_HAL_TB1 */

#if (configBSP430_HAL_TB2 - 0)

static const sBSP430halISRIndexedChainNode * cc_callback_TB2[
#if defined(__MSP430_HAS_T2B3__)
  3
#else
  7
#endif /* TB2 */
];

sBSP430halTIMER xBSP430hal_TB2_ = {
  .hal_state = {
    .cflags = BSP430_TIMER_HAL_HPL_VARIANT_TIMER
#if (configBSP430_HAL_TB2_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_TB2_ISR */
#if (configBSP430_HAL_TB2_CC0_ISR - 0)
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR2
#endif /* configBSP430_HAL_TB2_CC0_ISR */
  },
  .hpl = BSP430_HPL_TB2,
  .cc_cbchain_ni = cc_callback_TB2
};
#endif /* configBSP430_HAL_TB2 */

eBSP430clockSource
xBSP430timerClockSource (volatile sBSP430hplTIMER * hpl)
{
  switch (hpl->ctl & TASSEL_3) {
    case TASSEL_0:
      return eBSP430clockSRC_TCLK;
    case TASSEL_1:
      return xBSP430clockACLKSource();
    case TASSEL_2:
      return xBSP430clockSMCLKSource();
    case TASSEL_3:
      return eBSP430clockSRC_ITCLK;
  }
  return eBSP430clockSRC_NONE;
}

void
vBSP430timerInferHints_ni (hBSP430halTIMER timer)
{
  unsigned char flags = timer->hal_state.flags & ~(BSP430_TIMER_FLAG_MCLKSYNC | BSP430_TIMER_FLAG_SLOW);
  unsigned long timer_Hz;
  unsigned long mclk_Hz;

  if (xBSP430timerClockSource(timer->hpl) == xBSP430clockMCLKSource()) {
    flags |= BSP430_TIMER_FLAG_MCLKSYNC;
  }
  timer_Hz = ulBSP430timerFrequency_Hz_ni(xBSP430periphFromHPL(timer->hpl));
  mclk_Hz = ulBSP430clockMCLK_Hz_ni();
  if ((0 != timer_Hz) && (timer_Hz <= (mclk_Hz / 16))) {
    flags |= BSP430_TIMER_FLAG_SLOW;
  }
  timer->hal_state.flags = flags;
}

unsigned long
ulBSP430timerFrequency_Hz_ni (tBSP430periphHandle periph)
{
  volatile sBSP430hplTIMER * tp = xBSP430hplLookupTIMER(periph);
  unsigned long freq_Hz;

  if (0 == tp) {
    return (unsigned long)-1;
  }
  switch (tp->ctl & TASSEL_3) {
    case TASSEL_0:
      return 0;
    case TASSEL_1:
      freq_Hz = ulBSP430clockACLK_Hz_ni();
      break;
    case TASSEL_2:
      freq_Hz = ulBSP430clockSMCLK_Hz_ni();
      break;
    default:
    case TASSEL_3:
      return (unsigned long)-1;
  }
  return freq_Hz >> ((tp->ctl & ID_3) / ID0);
}

unsigned int
uiBSP430timerCaptureDelta_ni (tBSP430periphHandle periph,
                              int ccidx,
                              unsigned int capture_mode,
                              unsigned int ccis,
                              unsigned int count)
{
  volatile sBSP430hplTIMER * tp = xBSP430hplLookupTIMER(periph);
  int i;
  unsigned int c0 = 0;
  unsigned int c1;

  /* Eliminate stray bits from capture mode and ccis. */
  capture_mode &= CM0 | CM1;
  ccis &= CCIS0 | CCIS1;

  /* Fail if no capture mode specified or timer is unrecognized or stopped */
  if ((0 == capture_mode)
      || (NULL == tp)
      || (0 == (tp->ctl & (MC0 | MC1)))) {
    return (unsigned int)-1;
  }

  /* Configure synchronous capture.  SCS is not defined on a small
   * handful of 3xx chips, so the code won't build on those, but
   * you're not using one of them anyway, are you? */
  tp->cctl[ccidx] = capture_mode | ccis | CAP | SCS;

  /* Synchronize by discarding the first capture. */
  while (! (tp->cctl[ccidx] & CCIFG)) {
    BSP430_CORE_WATCHDOG_CLEAR();
  }
  for (i = 0; i <= count; ++i) {
    tp->cctl[ccidx] &= ~CCIFG;
    while (! (tp->cctl[ccidx] & CCIFG)) {
      BSP430_CORE_WATCHDOG_CLEAR();
    }
    if (0 == i) {
      c0 = tp->ccr[ccidx];
    }
    BSP430_CORE_WATCHDOG_CLEAR();
  }
  c1 = tp->ccr[ccidx];

  /* Reset the CCTL */
  tp->cctl[ccidx] = 0;

#if (configBSP430_TIMER_VALID_COUNTER_READ - 0)
  if (ccidx == BSP430_TIMER_VALID_COUNTER_READ_CCIDX) {
    /* Reset the safe counter read configuration for this timer */
    vBSP430timerSafeCounterInitialize_ni(tp);
  }
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */

  return c1 - c0;
}

static BSP430_CORE_INLINE_FORCED
unsigned long
timerOverflowAdjusted_ni (hBSP430halTIMER timer,
                          unsigned int ctr)
{
  unsigned long overflow_count = timer->overflow_count;

  /* Get the overflow counter for this result.  If an overflow has
   * occurred but has not been processed and the counter value is
   * below the midway point, register the overflow effect locally but
   * keep the flag set for handling by the interrupt handler so that
   * overflow callbacks are invoked properly.
   *
   * Note that TAIFG, TBIFG, and TDIFG all have value 0x0001 so it
   * doesn't matter which type of timer this is. */
  if ((0 <= (int)ctr) && (timer->hpl->ctl & TAIFG)) {
    ++overflow_count;
  }
  return overflow_count;
}

unsigned long
ulBSP430timerOverflowAdjusted_ni (hBSP430halTIMER timer,
                                  unsigned int ctr)
{
  return timerOverflowAdjusted_ni(timer, ctr);
}

unsigned long
ulBSP430timerCounter_ni (hBSP430halTIMER timer,
                         unsigned int * overflowp)
{
  unsigned int r;
  unsigned long overflow_count;

  r = uiBSP430timerBestCounterRead_ni(timer->hpl, timer->hal_state.flags);
  overflow_count = timerOverflowAdjusted_ni(timer, r);
  if (overflowp) {
    *overflowp = overflow_count >> 16;
  }
  return (overflow_count << 16) + r;
}

unsigned long
ulBSP430timerCaptureCounter_ni (hBSP430halTIMER timer,
                                unsigned int ccidx)
{
  unsigned int lo = timer->hpl->ccr[ccidx];
  return (timerOverflowAdjusted_ni(timer, lo) << 16) | lo;
}

void
vBSP430timerResetCounter_ni (hBSP430halTIMER timer)
{
  unsigned int mc = timer->hpl->ctl & (MC0 | MC1);
  timer->hpl->ctl &= ~(MC0 | MC1);
  timer->overflow_count = 0;
  timer->hpl->r = 0;
  timer->hpl->ctl |= mc;
}

void
vBSP430timerSetCounter_ni (hBSP430halTIMER timer,
                           unsigned int overflow,
                           unsigned long counter)
{
  unsigned int mc = timer->hpl->ctl & (MC0 | MC1);
  timer->hpl->ctl &= ~(MC0 | MC1);
  timer->overflow_count = ((unsigned long)overflow << 16) | (counter >> 16);
  timer->hpl->r = (counter & 0xFFFF);
  timer->hpl->ctl |= mc;
}

/** Set the interrupt flags based on the alarm needs.
 *
 * This gets invoked both when the alarm is originally set and, if
 * necessary, within the overflow handler.  At the point where the
 * overflow counter is consistent with the upper word of the scheduled
 * alarm time, check to see whether the timer counter is at or below
 * the low word of the set time.  If not, we'll let the counter change
 * cue the interrupt; otherwise we'll force one immediately. */
static inline
void alarmConfigureInterrupts_ni (sBSP430timerAlarm * map)
{
  unsigned int chi = map->timer->overflow_count;
  unsigned int shi = map->setting_tck >> 16;
  unsigned int slo = map->setting_tck;

  /* NB: When invoked from the overflow handler we can expect there
   * are no unhandled overflow events; that's not true when invoked
   * timerAlarmSet_ni(), and chi may be one too low.  If so it'll get
   * corrected when the overflow event is handled. */
  if (chi == shi) {
    volatile sBSP430hplTIMER * hpl = map->timer->hpl;

    /* Clear any flag set in previous cycles, then set to generate an
     * interrupt on the next event. */
    hpl->cctl[map->ccidx] &= ~CCIFG;
    hpl->cctl[map->ccidx] |= CCIE;

    /* If it looks like we missed the count-to-lo event, set one.  NB:
     * The low part intentionally uses unsigned compare, since we
     * really do want to know whether the counter value is higher than
     * the value we're trying to count to. */
    if (uiBSP430timerBestCounterRead_ni(hpl, map->timer->hal_state.flags) >= slo) {
      hpl->cctl[map->ccidx] |= CCIFG;
    }
  }
}

/* The overflow callback registered for enabled alarms.  It is
 * responsible for enabling an interrupt on compare match within the
 * cycle at which the alarm event is due. */
static int
alarmOFcb_ni (const struct sBSP430halISRVoidChainNode * cb,
              void * context)
{
  sBSP430timerAlarm * malarmp = (sBSP430timerAlarm *)(-offsetof(sBSP430timerAlarm, overflow_cb) + (unsigned char *)cb);

  if (malarmp->flags & BSP430_TIMER_ALARM_FLAG_SET) {
    alarmConfigureInterrupts_ni(malarmp);
  }
  return 0;
}

/* The capture/compare callback registered for enabled alarms.  It is
 * responsible for clearing the alarm and invoking the user-provided
 * callback. */
static int
alarmCCcb_ni (const struct sBSP430halISRIndexedChainNode * cb,
              void * context,
              int idx)
{
  sBSP430timerAlarm * malarmp = (sBSP430timerAlarm *)(-offsetof(sBSP430timerAlarm, cc_cb) + (unsigned char *)cb);
  int rv = 0;

  if (! (malarmp->flags & BSP430_TIMER_ALARM_FLAG_SET)) {
    return rv;
  }
  malarmp->flags &= ~BSP430_TIMER_ALARM_FLAG_SET;
  malarmp->timer->hpl->cctl[malarmp->ccidx] &= ~CCIE;
  if (NULL != malarmp->callback_ni) {
    rv = malarmp->callback_ni(malarmp);
  } else {
    rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
  }
  return rv;
}

hBSP430timerAlarm
hBSP430timerAlarmInitialize (sBSP430timerAlarm * alarm,
                             tBSP430periphHandle periph,
                             int ccidx,
                             iBSP430timerAlarmCallback_ni callback)
{
  if (NULL == alarm) {
    return NULL;
  }
  memset(alarm, 0, sizeof(*alarm));
  alarm->timer = hBSP430timerLookup(periph);
  if (NULL == alarm->timer) {
    return NULL;
  }
#if (configBSP430_TIMER_VALID_COUNTER_READ - 0)
  if (BSP430_TIMER_VALID_COUNTER_READ_CCIDX == ccidx) {
    return NULL;
  }
  {
    BSP430_CORE_SAVED_INTERRUPT_STATE(istate);

    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      vBSP430timerSafeCounterInitialize_ni(xBSP430hplLookupTIMER(periph));
    } while (0);
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  }
#endif /* configBSP430_TIMER_VALID_COUNTER_READ */
  alarm->ccidx = ccidx;
  alarm->callback_ni = callback;
  alarm->overflow_cb.callback_ni = alarmOFcb_ni;
  alarm->cc_cb.callback_ni = alarmCCcb_ni;
  return alarm;
}

int
iBSP430timerAlarmSetEnabled_ni (hBSP430timerAlarm alarm,
                                int enablep)
{
  sBSP430timerAlarm * malarm = (sBSP430timerAlarm *)alarm;

  if (NULL == alarm) {
    return -1;
  }
  if (enablep) {
    if (! (alarm->flags & BSP430_TIMER_ALARM_FLAG_ENABLED)) {
      BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRVoidChainNode,
                                      alarm->timer->overflow_cbchain_ni,
                                      malarm->overflow_cb,
                                      next_ni);
      BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRIndexedChainNode,
                                      alarm->timer->cc_cbchain_ni[alarm->ccidx],
                                      malarm->cc_cb,
                                      next_ni);
      malarm->flags |= BSP430_TIMER_ALARM_FLAG_ENABLED;
    }
  } else {
    if (alarm->flags & BSP430_TIMER_ALARM_FLAG_ENABLED) {
      if (BSP430_TIMER_ALARM_FLAG_SET & alarm->flags) {
        (void)iBSP430timerAlarmCancel_ni(alarm);
      }
      BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRVoidChainNode,
                                        alarm->timer->overflow_cbchain_ni,
                                        malarm->overflow_cb,
                                        next_ni);
      BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRIndexedChainNode,
                                        alarm->timer->cc_cbchain_ni[alarm->ccidx],
                                        malarm->cc_cb,
                                        next_ni);
      malarm->flags &= ~BSP430_TIMER_ALARM_FLAG_ENABLED;
    }
  }

  return 0;
}

static int
timerAlarmSet_ni (hBSP430timerAlarm alarm,
                  unsigned long setting_tck,
                  int force)
{
  sBSP430timerAlarm * malarmp = (sBSP430timerAlarm *)alarm;
  int rv = 0;
  long now_tck;
  long delay_tck;

  if (NULL == alarm) {
    return -1;
  }
  now_tck = ulBSP430timerCounter_ni(alarm->timer, NULL);
  delay_tck = (long)setting_tck - now_tck;
  if (! (BSP430_TIMER_ALARM_FLAG_ENABLED & alarm->flags)) {
    /* Can't set an alarm that's not enabled */
    rv = -1;
  } else if (BSP430_TIMER_ALARM_FLAG_SET & alarm->flags) {
    /* Can't set an alarm that's already set.  NB: rv value is
     * negative. */
    rv = BSP430_TIMER_ALARM_SET_ALREADY;
  } else if (0 > delay_tck) {
    /* Maybe can't set an alarm that's in the past.  NB: rv value is
     * positive. */
    rv = BSP430_TIMER_ALARM_SET_PAST;
  } else if (BSP430_TIMER_ALARM_FUTURE_LIMIT > delay_tck) {
    /* Maybe can't set an alarm that's coming up too fast.  NB: rv
     * value is positive. */
    rv = BSP430_TIMER_ALARM_SET_NOW;
  } else {
    /* Can set the alarm */
    rv = 0;
  }

  if ((0 == rv) || (0 < rv && force)) {
    volatile sBSP430hplTIMER * hpl = malarmp->timer->hpl;

    /* Record the time at which the event occurs, and that the alarm
     * is scheduled.  If we've determined that the event is to be
     * forced, just generate an interrupt; otherwise set the CCR to
     * match the point in the cycle at which the event should be
     * raised and hand off control to the routine that checks that
     * we're in the right overflow cycle. */
    malarmp->setting_tck = setting_tck;
    malarmp->flags |= BSP430_TIMER_ALARM_FLAG_SET;
    hpl->ccr[alarm->ccidx] = (unsigned int)setting_tck;
    if (0 < rv) {
      hpl->cctl[malarmp->ccidx] |= CCIE | CCIFG;
    } else {
      alarmConfigureInterrupts_ni(malarmp);
    }
  }
  return rv;
}

int
iBSP430timerAlarmSet_ni (hBSP430timerAlarm alarm,
                         unsigned long setting_tck)
{
  return timerAlarmSet_ni(alarm, setting_tck, 0);
}

int
iBSP430timerAlarmSetForced_ni (hBSP430timerAlarm alarm,
                               unsigned long setting_tck)
{
  return timerAlarmSet_ni(alarm, setting_tck, 1);
}

int
iBSP430timerAlarmCancel_ni (hBSP430timerAlarm alarm)
{
  sBSP430timerAlarm * malarm = (sBSP430timerAlarm *)alarm;
  if (NULL == alarm) {
    return -1;
  }
  if (! (BSP430_TIMER_ALARM_FLAG_ENABLED & alarm->flags)) {
    return -1;
  }
  if (! (BSP430_TIMER_ALARM_FLAG_SET & alarm->flags)) {
    return 0;
  }
  /* Clear the set and clear the CCIE and CCIFG bits in the timer
   * (along with everything else). */
  malarm->flags &= ~BSP430_TIMER_ALARM_FLAG_SET;
  alarm->timer->hpl->cctl[alarm->ccidx] = 0;
  return 1;
}

/* The capture/compare callback registered for enabled alarms.  It is
 * responsible for clearing the alarm and invoking the user-provided
 * callback. */
static int
muxAlarm_cb_ni (hBSP430timerAlarm alarm)
{
  hBSP430timerMuxSharedAlarm sap = (sBSP430timerMuxSharedAlarm *)(-offsetof(sBSP430timerMuxSharedAlarm, dedicated) + (char *)alarm);
  unsigned long now_tck = ulBSP430timerCounter_ni(sap->dedicated.timer, NULL);
  hBSP430timerMuxAlarm fired = sap->alarms;
  hBSP430timerMuxAlarm * ap = &sap->alarms;
  int rv = 0;

  while (NULL != *ap) {
    if (0 < ((long)((*ap)->setting_tck) - (long)now_tck)) {
      break;
    }
    ap = &(*ap)->next;
  }
  sap->alarms = *ap;
  if (sap->alarms) {
    (void)timerAlarmSet_ni(&sap->dedicated, sap->alarms->setting_tck, 1);
  }
  if (fired != sap->alarms) {
    *ap = NULL;
    while (NULL != fired) {
      hBSP430timerMuxAlarm notify = fired;
      fired = notify->next;
      notify->next = NULL;
      rv |= notify->callback_ni(sap, notify);
    }
  }
  return rv;
}

hBSP430timerMuxSharedAlarm
hBSP430timerMuxAlarmStartup (sBSP430timerMuxSharedAlarm * shared,
                             tBSP430periphHandle periph,
                             int ccidx)
{
  hBSP430timerMuxSharedAlarm rv = NULL;
  hBSP430timerAlarm ap;
  int rc;

  if (NULL == shared) {
    return rv;
  }
  memset(shared, 0, sizeof(*shared));
  ap = hBSP430timerAlarmInitialize(&shared->dedicated, periph, ccidx, muxAlarm_cb_ni);
  if (NULL == ap) {
    return rv;
  }
  rc = iBSP430timerAlarmEnable(ap);
  if (0 != rc) {
    return rv;
  }
  rv = shared;
  return rv;
}

int
iBSP430timerMuxAlarmShutdown (hBSP430timerMuxSharedAlarm shared)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rc = -1;

  if (NULL == shared) {
    return rc;
  }
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rc = iBSP430timerAlarmCancel_ni(&shared->dedicated);
    if (0 <= rc) {
      rc = iBSP430timerAlarmSetEnabled_ni(&shared->dedicated, 0);
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

int
iBSP430timerMuxAlarmAdd_ni (hBSP430timerMuxSharedAlarm shared,
                            hBSP430timerMuxAlarm alarm)
{
  int rc;

  rc = iBSP430timerAlarmCancel_ni(&shared->dedicated);
  if (0 <= rc) {
    hBSP430timerMuxAlarm * np = &shared->alarms;
    hBSP430halTIMER timer = shared->dedicated.timer;
    unsigned long now_tck = ulBSP430timerCounter_ni(timer, NULL);
    long delay_tck = alarm->setting_tck - now_tck;

    /* Insert the alarm into the sequence after any alarm that should
     * fire at or before the time of the new alarm. */
    while (NULL != *np) {
      long next_delay_tck = (*np)->setting_tck - now_tck;
      if (next_delay_tck > delay_tck) {
        break;
      }
      np = &(*np)->next;
    }
    alarm->next = *np;
    *np = alarm;
    rc = timerAlarmSet_ni(&shared->dedicated, shared->alarms->setting_tck, 1);
  }
  return rc;
}

int
iBSP430timerMuxAlarmRemove_ni (hBSP430timerMuxSharedAlarm shared,
                               hBSP430timerMuxAlarm alarm)
{
  int rc;

  rc = iBSP430timerAlarmCancel_ni(&shared->dedicated);
  if (0 <= rc) {
    hBSP430timerMuxAlarm * np = &shared->alarms;
    while (NULL != *np) {
      if (alarm == *np) {
        *np = alarm->next;
        break;
      }
      np = &(*np)->next;
    }
    rc = 0;
    if (NULL != shared->alarms) {
      rc = timerAlarmSet_ni(&shared->dedicated, shared->alarms->setting_tck, 1);
    }
  }
  return rc;
}

static int
pulsecap_isr (const struct sBSP430halISRIndexedChainNode * cb,
              void * context,
              int idx)
{
  hBSP430halTIMER timer = (hBSP430halTIMER)context;
  hBSP430timerPulseCapture pulsecap = (hBSP430timerPulseCapture)(-offsetof(sBSP430timerPulseCapture, cb) + (unsigned char *)cb);
  unsigned int flags = pulsecap->flags_ni;
  unsigned int ccr;
  unsigned int cctl;
  int do_callback = 0;
  int rv = 0;

  /* Record capture counter value then the state describing that
   * value. */
  ccr  = timer->hpl->ccr[idx];
  cctl = timer->hpl->cctl[idx];

  /* COV means a second capture occured before the interrupt handler
   * was entered.  CCIFG means a second capture occured after the
   * interrupt handler was entered but before this callback was
   * invoked.  In both cases data was lost. */
  if (cctl & (COV | CCIFG)) {
    timer->hpl->cctl[idx] &= ~(COV | CCIFG);
    flags |= BSP430_TIMER_PULSECAP_OVERFLOW;
  }
  if (! (BSP430_TIMER_PULSECAP_OVERFLOW & flags)) {
    unsigned long cap_tt = (timerOverflowAdjusted_ni(timer, ccr) << 16) | ccr;
    if (! (BSP430_TIMER_PULSECAP_START_VALID & flags)) {
      pulsecap->start_tt_ni = cap_tt;
      flags |= BSP430_TIMER_PULSECAP_START_VALID;
      if (cctl & CCI) {
        flags |= BSP430_TIMER_PULSECAP_ACTIVE_HIGH;
      } else {
        flags &= ~BSP430_TIMER_PULSECAP_ACTIVE_HIGH;
      }
      do_callback = !!(BSP430_TIMER_PULSECAP_START_CALLBACK & flags);
    } else if (! (BSP430_TIMER_PULSECAP_END_VALID & flags)) {
      pulsecap->end_tt_ni = cap_tt;
      flags |= BSP430_TIMER_PULSECAP_END_VALID;
      do_callback = !!(BSP430_TIMER_PULSECAP_END_CALLBACK & flags);
    } else {
      flags |= BSP430_TIMER_PULSECAP_OVERFLOW;
    }
  }
  if (BSP430_TIMER_PULSECAP_OVERFLOW & flags) {
    do_callback = 1;
  }
  while (! (flags & (BSP430_TIMER_PULSECAP_START_CALLBACK | BSP430_TIMER_PULSECAP_END_CALLBACK))) {
    ;
  }
  pulsecap->flags_ni = flags;
  if ((NULL != pulsecap->callback_ni) && do_callback) {
    pulsecap->flags_ni |= BSP430_TIMER_PULSECAP_CALLBACK_ACTIVE;
    rv = pulsecap->callback_ni(pulsecap);
    pulsecap->flags_ni &= ~BSP430_TIMER_PULSECAP_CALLBACK_ACTIVE;
  }
  return rv;
}

int
iBSP430timerPulseCaptureSetActive_ni (hBSP430timerPulseCapture pulsecap,
                                      int activep)
{
  if ((NULL == pulsecap)
      || (NULL == pulsecap->hal)
      || (! (BSP430_TIMER_PULSECAP_ENABLED & pulsecap->flags_ni))) {
    return -1;
  }
  if (activep) {
    pulsecap->flags_ni |= BSP430_TIMER_PULSECAP_ACTIVE;
    pulsecap->hal->hpl->cctl[pulsecap->ccidx] &= ~(COV | CCIFG);
    pulsecap->hal->hpl->cctl[pulsecap->ccidx] |= CCIE;
  } else {
    pulsecap->flags_ni &= ~BSP430_TIMER_PULSECAP_ACTIVE;
    pulsecap->hal->hpl->cctl[pulsecap->ccidx] &= ~CCIE;
  }
  return 0;
}

int
iBSP430timerPulseCaptureSetEnabled_ni (hBSP430timerPulseCapture pulsecap,
                                       int enablep)
{
  if ((NULL == pulsecap)
      || (NULL == pulsecap->hal)) {
    return -1;
  }
  /* Spin for diagnostic, or return error to avoid corruption, if this
   * is invoked from the callback. */
  while (BSP430_TIMER_PULSECAP_CALLBACK_ACTIVE & pulsecap->flags_ni) {
#if (BSP430_CORE_NDEBUG - 0)
    return -1;
#endif /* BSP430_CORE_NDEBUG */
  }
  if (enablep) {
    if (! (pulsecap->flags_ni & BSP430_TIMER_PULSECAP_ENABLED)) {
      BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRIndexedChainNode,
                                      pulsecap->hal->cc_cbchain_ni[pulsecap->ccidx],
                                      pulsecap->cb,
                                      next_ni);
      pulsecap->flags_ni |= BSP430_TIMER_PULSECAP_ENABLED;
    }
  } else {
    /* Deactivate before disabling */
    iBSP430timerPulseCaptureSetActive_ni(pulsecap, 0);
    if (pulsecap->flags_ni & BSP430_TIMER_PULSECAP_ENABLED) {
      pulsecap->flags_ni &= ~BSP430_TIMER_PULSECAP_ENABLED;
      BSP430_HAL_ISR_CALLBACK_UNLINK_NI(sBSP430halISRIndexedChainNode,
                                        pulsecap->hal->cc_cbchain_ni[pulsecap->ccidx],
                                        pulsecap->cb,
                                        next_ni);
    }
  }
  return 0;
}

hBSP430timerPulseCapture
hBSP430timerPulseCaptureInitialize (hBSP430timerPulseCapture pulsecap,
                                    tBSP430periphHandle periph,
                                    int ccidx,
                                    unsigned int ccis,
                                    unsigned int flags,
                                    iBSP430timerPulseCaptureCallback_ni callback)
{
  memset(pulsecap, 0, sizeof(*pulsecap));
  pulsecap->cb.callback_ni = pulsecap_isr;
  pulsecap->hal = hBSP430timerLookup(periph);
  if (NULL == pulsecap->hal) {
    return NULL;
  }
  if (iBSP430timerSupportedCCs(periph) <= ccidx) {
    pulsecap->hal = NULL;
    return NULL;
  }
  pulsecap->ccidx = ccidx;
  pulsecap->ccis = ccis;
  pulsecap->callback_ni = callback;
  pulsecap->flags_ni = flags & (BSP430_TIMER_PULSECAP_START_CALLBACK
                                | BSP430_TIMER_PULSECAP_END_CALLBACK);
  pulsecap->hal->hpl->cctl[ccidx] = CM_3 | (ccis & (CCIS0 | CCIS1)) | SCS | CAP;
  return pulsecap;
}

/* !BSP430! TYPE=A subst=TYPE instance=0,1,2,3 insert=hal_timer_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_timer_isr_defn] */
#if (configBSP430_HAL_TA0_CC0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER0_A0_VECTOR)
isr_cc0_TA0 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TA0;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    timer->hpl->cctl[0] &= ~CCIE;
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA0_CC0_ISR */

#if (configBSP430_HAL_TA0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER0_A1_VECTOR)
isr_TA0 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TA0;
  unsigned int iv = TA0IV;
  int rv = 0;
  if (0 != iv) {
    if (TA_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_cbchain_ni, timer, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->ctl &= ~TAIE;
      }
    } else {
      int cc = iv / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_cbchain_ni, timer, cc, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->cctl[cc] &= ~CCIE;
      }
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA0_ISR */

#if (configBSP430_HAL_TA1_CC0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER1_A0_VECTOR)
isr_cc0_TA1 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TA1;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    timer->hpl->cctl[0] &= ~CCIE;
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA1_CC0_ISR */

#if (configBSP430_HAL_TA1_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER1_A1_VECTOR)
isr_TA1 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TA1;
  unsigned int iv = TA1IV;
  int rv = 0;
  if (0 != iv) {
    if (TA_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_cbchain_ni, timer, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->ctl &= ~TAIE;
      }
    } else {
      int cc = iv / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_cbchain_ni, timer, cc, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->cctl[cc] &= ~CCIE;
      }
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA1_ISR */

#if (configBSP430_HAL_TA2_CC0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER2_A0_VECTOR)
isr_cc0_TA2 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TA2;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    timer->hpl->cctl[0] &= ~CCIE;
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA2_CC0_ISR */

#if (configBSP430_HAL_TA2_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER2_A1_VECTOR)
isr_TA2 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TA2;
  unsigned int iv = TA2IV;
  int rv = 0;
  if (0 != iv) {
    if (TA_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_cbchain_ni, timer, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->ctl &= ~TAIE;
      }
    } else {
      int cc = iv / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_cbchain_ni, timer, cc, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->cctl[cc] &= ~CCIE;
      }
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA2_ISR */

#if (configBSP430_HAL_TA3_CC0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER3_A0_VECTOR)
isr_cc0_TA3 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TA3;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    timer->hpl->cctl[0] &= ~CCIE;
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA3_CC0_ISR */

#if (configBSP430_HAL_TA3_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER3_A1_VECTOR)
isr_TA3 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TA3;
  unsigned int iv = TA3IV;
  int rv = 0;
  if (0 != iv) {
    if (TA_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_cbchain_ni, timer, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->ctl &= ~TAIE;
      }
    } else {
      int cc = iv / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_cbchain_ni, timer, cc, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->cctl[cc] &= ~CCIE;
      }
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA3_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_timer_isr_defn] */
/* !BSP430! end=hal_timer_isr_defn */
/* !BSP430! TYPE=B subst=TYPE instance=0,1,2 insert=hal_timer_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_timer_isr_defn] */
#if (configBSP430_HAL_TB0_CC0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER0_B0_VECTOR)
isr_cc0_TB0 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TB0;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    timer->hpl->cctl[0] &= ~CCIE;
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB0_CC0_ISR */

#if (configBSP430_HAL_TB0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER0_B1_VECTOR)
isr_TB0 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TB0;
  unsigned int iv = TB0IV;
  int rv = 0;
  if (0 != iv) {
    if (TB_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_cbchain_ni, timer, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->ctl &= ~TAIE;
      }
    } else {
      int cc = iv / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_cbchain_ni, timer, cc, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->cctl[cc] &= ~CCIE;
      }
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB0_ISR */

#if (configBSP430_HAL_TB1_CC0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER1_B0_VECTOR)
isr_cc0_TB1 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TB1;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    timer->hpl->cctl[0] &= ~CCIE;
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB1_CC0_ISR */

#if (configBSP430_HAL_TB1_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER1_B1_VECTOR)
isr_TB1 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TB1;
  unsigned int iv = TB1IV;
  int rv = 0;
  if (0 != iv) {
    if (TB_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_cbchain_ni, timer, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->ctl &= ~TAIE;
      }
    } else {
      int cc = iv / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_cbchain_ni, timer, cc, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->cctl[cc] &= ~CCIE;
      }
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB1_ISR */

#if (configBSP430_HAL_TB2_CC0_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER2_B0_VECTOR)
isr_cc0_TB2 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TB2;
  int rv = iBSP430callbackInvokeISRIndexed_ni(0 + timer->cc_cbchain_ni, timer, 0, 0);
  if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
    timer->hpl->cctl[0] &= ~CCIE;
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB2_CC0_ISR */

#if (configBSP430_HAL_TB2_ISR - 0)
BSP430_CORE_DECLARE_INTERRUPT(TIMER2_B1_VECTOR)
isr_TB2 (void)
{
  hBSP430halTIMER timer = BSP430_HAL_TB2;
  unsigned int iv = TB2IV;
  int rv = 0;
  if (0 != iv) {
    if (TB_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_cbchain_ni, timer, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->ctl &= ~TAIE;
      }
    } else {
      int cc = iv / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_cbchain_ni, timer, cc, rv);
      if (rv & BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT) {
        timer->hpl->cctl[cc] &= ~CCIE;
      }
    }
  }
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB2_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_timer_isr_defn] */
/* !BSP430! end=hal_timer_isr_defn */
/* !BSP430! instance=TA0,TA1,TA2,TA3,TB0,TB1,TB2 */

const char *
xBSP430timerName (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_name_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_name_demux] */
#if (configBSP430_HPL_TA0 - 0)
  if (BSP430_PERIPH_TA0 == periph) {
    return "TA0";
  }
#endif /* configBSP430_HPL_TA0 */

#if (configBSP430_HPL_TA1 - 0)
  if (BSP430_PERIPH_TA1 == periph) {
    return "TA1";
  }
#endif /* configBSP430_HPL_TA1 */

#if (configBSP430_HPL_TA2 - 0)
  if (BSP430_PERIPH_TA2 == periph) {
    return "TA2";
  }
#endif /* configBSP430_HPL_TA2 */

#if (configBSP430_HPL_TA3 - 0)
  if (BSP430_PERIPH_TA3 == periph) {
    return "TA3";
  }
#endif /* configBSP430_HPL_TA3 */

#if (configBSP430_HPL_TB0 - 0)
  if (BSP430_PERIPH_TB0 == periph) {
    return "TB0";
  }
#endif /* configBSP430_HPL_TB0 */

#if (configBSP430_HPL_TB1 - 0)
  if (BSP430_PERIPH_TB1 == periph) {
    return "TB1";
  }
#endif /* configBSP430_HPL_TB1 */

#if (configBSP430_HPL_TB2 - 0)
  if (BSP430_PERIPH_TB2 == periph) {
    return "TB2";
  }
#endif /* configBSP430_HPL_TB2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_name_demux] */
  /* !BSP430! end=periph_name_demux */
  return NULL;
}

int
iBSP430timerSupportedCCs (tBSP430periphHandle periph)
{
#if (configBSP430_HPL_TA0 - 0)
  if (BSP430_PERIPH_TA0 == periph) {
#if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_T0A2__)
    return 2;
#elif defined(__MSP430_HAS_TA3__) || defined(__MSP430_HAS_T0A3__)
    return 3;
#else
    return 5;
#endif /* TA0 */
  }
#endif /* configBSP430_HPL_TA0 */

#if (configBSP430_HPL_TA1 - 0)
  if (BSP430_PERIPH_TA1 == periph) {
#if defined(__MSP430_HAS_T1A2__)
    return 2;
#elif defined(__MSP430_HAS_T1A3__)
    return 3;
#else
    return 5;
#endif /* TA1 */
  }
#endif /* configBSP430_HPL_TA1 */

#if (configBSP430_HPL_TA2 - 0)
  if (BSP430_PERIPH_TA2 == periph) {
#if defined(__MSP430_HAS_T2A2__)
    return 2;
#elif defined(__MSP430_HAS_T2A3__)
    return 3;
#else
    return 5;
#endif /* TA2 */
  }
#endif /* configBSP430_HPL_TA2 */

#if (configBSP430_HPL_TA3 - 0)
  if (BSP430_PERIPH_TA3 == periph) {
#if defined(__MSP430_HAS_T3A2__)
    return 2;
#elif defined(__MSP430_HAS_T3A3__)
    return 3;
#else
    return 5;
#endif /* TA3 */
  }
#endif /* configBSP430_HPL_TA3 */

#if (configBSP430_HPL_TB0 - 0)
  if (BSP430_PERIPH_TB0 == periph) {
#if defined(__MSP430_HAS_TB3__) || defined(__MSP430_HAS_T0B3__)
    return 3;
#else
    return 7;
#endif /* TB0 */
  }
#endif /* configBSP430_HPL_TB0 */

#if (configBSP430_HPL_TB1 - 0)
  if (BSP430_PERIPH_TB1 == periph) {
#if defined(__MSP430_HAS_T1B3__)
    return 3;
#else
    return 7;
#endif /* TB1 */
  }
#endif /* configBSP430_HPL_TB1 */

#if (configBSP430_HPL_TB2 - 0)
  if (BSP430_PERIPH_TB2 == periph) {
#if defined(__MSP430_HAS_T2B3__)
    return 3;
#else
    return 7;
#endif /* TB2 */
  }
#endif /* configBSP430_HPL_TB2 */
  return -1;
}
