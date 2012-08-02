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

#include <bsp430/periph/timer_.h>
#include <bsp430/platform.h>    /* BSP430_PLATFORM_TIMER_CCACLK defined by this */
#include <bsp430/clock.h>

#if defined(__MSP430_HAS_MSP430XV2_CPU__)
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

/* Declares additional callback pointers that immediately follow the
 * one in the base type.  This allows us to use only the amount of
 * space necessary to support the CC blocks on the target MCU. */
#define DECLARE_AUX_CCS(_n)                                     \
  struct xBSP430periphISRCallbackIndexed * _aux_cc_callback[_n]

#if configBSP430_HAL_TA0 - 0
static struct {
  struct xBSP430timerState state;
  /* State includes one CC record.  Add more as required. */
#if defined(__MSP430_HAS_TA2__) || defined(__MSP430_HAS_T0A2__)
  DECLARE_AUX_CCS(1);			/* 2 total */
#elif defined(__MSP430_HAS_TA3__) || defined(__MSP430_HAS_T0A3__)
  DECLARE_AUX_CCS(2);			/* 3 total */
#else
  DECLARE_AUX_CCS(4);			/* 5 total */
#endif /* TA0 */
} state_TA0_ = {
  .state = { .hpl = BSP430_HPL_TA0 }
};

xBSP430timerHandle const hBSP430timer_TA0 = &state_TA0_.state;
#endif /* configBSP430_HAL_TA0 */

#if configBSP430_HAL_TA1 - 0
static struct {
  struct xBSP430timerState state;
  /* State includes one CC record.  Add more as required. */
#if defined(__MSP430_HAS_T1A2__)
  DECLARE_AUX_CCS(1);			/* 2 total */
#elif defined(__MSP430_HAS_T1A3__)
  DECLARE_AUX_CCS(2);			/* 3 total */
#else
  DECLARE_AUX_CCS(4);			/* 5 total */
#endif /* TA1 */
} state_TA1_ = {
  .state = { .hpl = BSP430_HPL_TA1 }
};

xBSP430timerHandle const hBSP430timer_TA1 = &state_TA1_.state;
#endif /* configBSP430_HAL_TA1 */

#if configBSP430_HAL_TA2 - 0
static struct {
  struct xBSP430timerState state;
  /* State includes one CC record.  Add more as required. */
#if defined(__MSP430_HAS_T2A2__)
  DECLARE_AUX_CCS(1);			/* 2 total */
#elif defined(__MSP430_HAS_T2A3__)
  DECLARE_AUX_CCS(2);			/* 3 total */
#else
  DECLARE_AUX_CCS(4);			/* 5 total */
#endif /* TA2 */
} state_TA2_ = {
  .state = { .hpl = BSP430_HPL_TA2 }
};

xBSP430timerHandle const hBSP430timer_TA2 = &state_TA2_.state;
#endif /* configBSP430_HAL_TA2 */

#if configBSP430_HAL_TA3 - 0
static struct {
  struct xBSP430timerState state;
  /* State includes one CC record.  Add more as required. */
#if defined(__MSP430_HAS_T3A2__)
  DECLARE_AUX_CCS(1);			/* 2 total */
#elif defined(__MSP430_HAS_T3A3__)
  DECLARE_AUX_CCS(2);			/* 3 total */
#else
  DECLARE_AUX_CCS(4);			/* 5 total */
#endif /* TA3 */
} state_TA3_ = {
  .state = { .hpl = BSP430_HPL_TA3 }
};

xBSP430timerHandle const hBSP430timer_TA3 = &state_TA3_.state;
#endif /* configBSP430_HAL_TA3 */

#if configBSP430_HAL_TB0 - 0
static struct {
  struct xBSP430timerState state;
  /* State includes one CC record.  Add more as required. */
#if defined(__MSP430_HAS_TB3__) || defined(__MSP430_HAS_T0B3__)
  DECLARE_AUX_CCS(2);			/* 3 total */
#else
  DECLARE_AUX_CCS(6);			/* 7 total */
#endif /* TB0 */
} state_TB0_ = {
  .state = { .hpl = BSP430_HPL_TB0 }
};

xBSP430timerHandle const hBSP430timer_TB0 = &state_TB0_.state;
#endif /* configBSP430_HAL_TB0 */

#if configBSP430_HAL_TB1 - 0
static struct {
  struct xBSP430timerState state;
  /* State includes one CC record.  Add more as required. */
#if defined(__MSP430_HAS_T1B3__)
  DECLARE_AUX_CCS(2);			/* 3 total */
#else
  DECLARE_AUX_CCS(6);			/* 7 total */
#endif /* TB1 */
} state_TB1_ = {
  .state = { .hpl = BSP430_HPL_TB1 }
};

xBSP430timerHandle const hBSP430timer_TB1 = &state_TB1_.state;
#endif /* configBSP430_HAL_TB1 */

#if configBSP430_HAL_TB2 - 0
static struct {
  struct xBSP430timerState state;
  /* State includes one CC record.  Add more as required. */
#if defined(__MSP430_HAS_T2B3__)
  DECLARE_AUX_CCS(2);			/* 3 total */
#else
  DECLARE_AUX_CCS(6);			/* 7 total */
#endif /* TB2 */
} state_TB2_ = {
  .state = { .hpl = BSP430_HPL_TB2 }
};

xBSP430timerHandle const hBSP430timer_TB2 = &state_TB2_.state;
#endif /* configBSP430_HAL_TB2 */

unsigned long
ulBSP430timerFrequency_Hz_ni (xBSP430timerHandle timer)
{
  unsigned long freq_Hz;
  switch (timer->hpl->ctl & TASSEL_3) {
    case TASSEL_0:
      return 0;
    case TASSEL_1:
      freq_Hz = usBSP430clockACLK_Hz_ni();
      break;
    case TASSEL_2:
      freq_Hz = ulBSP430clockSMCLK_Hz_ni();
      break;
    default:
    case TASSEL_3:
      return -1;
  }
  return freq_Hz >> ((timer->hpl->ctl & ID_3) / ID0);
}

unsigned int
uiBSP430timerCaptureDelta_ni (tBSP430periphHandle periph,
                              int ccidx,
                              unsigned int capture_mode,
                              unsigned int ccis,
                              unsigned int count)
{
  volatile xBSP430periphTIMER * tp = xBSP430periphLookupTIMER(periph);
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
    return -1;
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
  return (c0 > c1) ? (c0 - c1) : (c1 - c0);
}

unsigned long
ulBSP430timerCounter_ni (xBSP430timerHandle timer,
                         unsigned int * overflowp)
{
  unsigned int r;
  unsigned int ctla;
  unsigned int ctlb;

  /* What we're doing is ensuring that we have a counter value that
   * was captured at a point where the control word did not change.
   * The expectation is that any pending overflow flag reflected in
   * that CTL word applies to the counter value.  (Since the counter
   * value may be tied to SMCLK, it's not possible to expect to read
   * the same counter value twice.) */
  do {
    ctla = timer->hpl->ctl;
    r = timer->hpl->r;
    ctlb = timer->hpl->ctl;
  } while (ctla != ctlb);

  /* If an overflow has occurred, register its effect.  Note that
   * TAIFG and TBIFG have the same value, so it doesn't matter which
   * type of timer this is. */
  if (ctla & TAIFG) {
    ++timer->overflow_count;
    timer->hpl->ctl &= ~TAIFG;
  }
  if (overflowp) {
    *overflowp = timer->overflow_count >> 16;
  }
  return (timer->overflow_count << 16) + r;
}

void
vBSP430timerResetCounter_ni (xBSP430timerHandle timer)
{
  timer->overflow_count = 0;
  timer->hpl->r = 0;
  timer->hpl->ctl &= ~TAIFG;
}

/* !BSP430! TYPE=A subst=TYPE instance=0,1,2,3 insert=hal_timer_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_timer_isr_defn] */
#if configBSP430_HAL_TA0_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER0_A0_VECTOR)))
isr_cc0_TA0 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TA0;
  int rv = iBSP430callbackInvokeISRVoid_ni(&timer->cc0_callback, timer, 0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA0_CC0_ISR */

#if configBSP430_HAL_TA0_ISR - 0
static void
__attribute__((__interrupt__(TIMER0_A1_VECTOR)))
isr_TA0 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TA0;
  int iv = TA0IV;
  int rv = 0;
  if (0 != iv) {
    if (TA_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_callback, timer, rv);
    } else {
      int cc = (iv - 4) / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_callback, timer, 1+cc, rv);
    }
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA0_ISR */

#if configBSP430_HAL_TA1_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER1_A0_VECTOR)))
isr_cc0_TA1 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TA1;
  int rv = iBSP430callbackInvokeISRVoid_ni(&timer->cc0_callback, timer, 0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA1_CC0_ISR */

#if configBSP430_HAL_TA1_ISR - 0
static void
__attribute__((__interrupt__(TIMER1_A1_VECTOR)))
isr_TA1 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TA1;
  int iv = TA1IV;
  int rv = 0;
  if (0 != iv) {
    if (TA_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_callback, timer, rv);
    } else {
      int cc = (iv - 4) / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_callback, timer, 1+cc, rv);
    }
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA1_ISR */

#if configBSP430_HAL_TA2_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER2_A0_VECTOR)))
isr_cc0_TA2 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TA2;
  int rv = iBSP430callbackInvokeISRVoid_ni(&timer->cc0_callback, timer, 0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA2_CC0_ISR */

#if configBSP430_HAL_TA2_ISR - 0
static void
__attribute__((__interrupt__(TIMER2_A1_VECTOR)))
isr_TA2 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TA2;
  int iv = TA2IV;
  int rv = 0;
  if (0 != iv) {
    if (TA_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_callback, timer, rv);
    } else {
      int cc = (iv - 4) / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_callback, timer, 1+cc, rv);
    }
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA2_ISR */

#if configBSP430_HAL_TA3_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER3_A0_VECTOR)))
isr_cc0_TA3 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TA3;
  int rv = iBSP430callbackInvokeISRVoid_ni(&timer->cc0_callback, timer, 0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA3_CC0_ISR */

#if configBSP430_HAL_TA3_ISR - 0
static void
__attribute__((__interrupt__(TIMER3_A1_VECTOR)))
isr_TA3 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TA3;
  int iv = TA3IV;
  int rv = 0;
  if (0 != iv) {
    if (TA_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_callback, timer, rv);
    } else {
      int cc = (iv - 4) / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_callback, timer, 1+cc, rv);
    }
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TA3_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_timer_isr_defn] */
/* !BSP430! end=hal_timer_isr_defn */
/* !BSP430! TYPE=B subst=TYPE instance=0,1,2 insert=hal_timer_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_timer_isr_defn] */
#if configBSP430_HAL_TB0_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER0_B0_VECTOR)))
isr_cc0_TB0 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TB0;
  int rv = iBSP430callbackInvokeISRVoid_ni(&timer->cc0_callback, timer, 0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB0_CC0_ISR */

#if configBSP430_HAL_TB0_ISR - 0
static void
__attribute__((__interrupt__(TIMER0_B1_VECTOR)))
isr_TB0 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TB0;
  int iv = TB0IV;
  int rv = 0;
  if (0 != iv) {
    if (TB_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_callback, timer, rv);
    } else {
      int cc = (iv - 4) / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_callback, timer, 1+cc, rv);
    }
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB0_ISR */

#if configBSP430_HAL_TB1_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER1_B0_VECTOR)))
isr_cc0_TB1 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TB1;
  int rv = iBSP430callbackInvokeISRVoid_ni(&timer->cc0_callback, timer, 0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB1_CC0_ISR */

#if configBSP430_HAL_TB1_ISR - 0
static void
__attribute__((__interrupt__(TIMER1_B1_VECTOR)))
isr_TB1 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TB1;
  int iv = TB1IV;
  int rv = 0;
  if (0 != iv) {
    if (TB_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_callback, timer, rv);
    } else {
      int cc = (iv - 4) / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_callback, timer, 1+cc, rv);
    }
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB1_ISR */

#if configBSP430_HAL_TB2_CC0_ISR - 0
static void
__attribute__((__interrupt__(TIMER2_B0_VECTOR)))
isr_cc0_TB2 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TB2;
  int rv = iBSP430callbackInvokeISRVoid_ni(&timer->cc0_callback, timer, 0);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB2_CC0_ISR */

#if configBSP430_HAL_TB2_ISR - 0
static void
__attribute__((__interrupt__(TIMER2_B1_VECTOR)))
isr_TB2 (void)
{
  xBSP430timerHandle timer = hBSP430timer_TB2;
  int iv = TB2IV;
  int rv = 0;
  if (0 != iv) {
    if (TB_OVERFLOW == iv) {
      ++timer->overflow_count;
      rv = iBSP430callbackInvokeISRVoid_ni(&timer->overflow_callback, timer, rv);
    } else {
      int cc = (iv - 4) / 2;
      rv = iBSP430callbackInvokeISRIndexed_ni(cc + timer->cc_callback, timer, 1+cc, rv);
    }
  }
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_TB2_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_timer_isr_defn] */
/* !BSP430! end=hal_timer_isr_defn */
/* !BSP430! instance=TA0,TA1,TA2,TA3,TB0,TB1,TB2 */

volatile xBSP430periphTIMER *
xBSP430periphLookupTIMER (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_ba_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_ba_hpl_demux] */
#if configBSP430_HPL_TA0 - 0
  if (BSP430_PERIPH_TA0 == periph) {
    return hBSP430periph_TA0;
  }
#endif /* configBSP430_HPL_TA0 */

#if configBSP430_HPL_TA1 - 0
  if (BSP430_PERIPH_TA1 == periph) {
    return hBSP430periph_TA1;
  }
#endif /* configBSP430_HPL_TA1 */

#if configBSP430_HPL_TA2 - 0
  if (BSP430_PERIPH_TA2 == periph) {
    return hBSP430periph_TA2;
  }
#endif /* configBSP430_HPL_TA2 */

#if configBSP430_HPL_TA3 - 0
  if (BSP430_PERIPH_TA3 == periph) {
    return hBSP430periph_TA3;
  }
#endif /* configBSP430_HPL_TA3 */

#if configBSP430_HPL_TB0 - 0
  if (BSP430_PERIPH_TB0 == periph) {
    return hBSP430periph_TB0;
  }
#endif /* configBSP430_HPL_TB0 */

#if configBSP430_HPL_TB1 - 0
  if (BSP430_PERIPH_TB1 == periph) {
    return hBSP430periph_TB1;
  }
#endif /* configBSP430_HPL_TB1 */

#if configBSP430_HPL_TB2 - 0
  if (BSP430_PERIPH_TB2 == periph) {
    return hBSP430periph_TB2;
  }
#endif /* configBSP430_HPL_TB2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_ba_hpl_demux] */
  /* !BSP430! end=periph_ba_hpl_demux */
  return NULL;
}
