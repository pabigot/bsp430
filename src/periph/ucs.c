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

#include <bsp430/platform.h>
#include <bsp430/periph/ucs.h>
#include <stdint.h>

/* Mask for SELA bits in UCSCTL4 */
#define SELA_MASK (SELA0 | SELA1 | SELA2)

/* Mask for SELS bits in UCSCTL4 */
#define SELS_MASK (SELS0 | SELS1 | SELS2)

/* Mask for SELM bits in UCSCTL4 */
#define SELM_MASK (SELM0 | SELM1 | SELM2)

/* Mask for DIVS bits in UCSCTL5 */
#define DIVS_MASK (DIVS0 | DIVS1 | DIVS2)

/* Mask for DIVM bits in UCSCTL5 */
#define DIVM_MASK (DIVM0 | DIVM1 | DIVM2)

/* Frequency measurement occurs over this duration when determining
 * whether trim is required.  The number of SMCLK ticks in an ACLK
 * period is the target frequency divided by 32768; accumulating over
 * multiple ACLK periods decreases the measurement error.  At a target
 * frequency of 2^25 (32 MiHz) the tick count for a single period
 * might require 11 bits to represent, so do not exceed 32 lest the
 * 16-bit delta value overflow.  Select a value so that the number of
 * ticks within the sample period is some small (~3) multiple of
 * TRIM_TOLERANCE_DIVISOR. */
#define TRIM_SAMPLE_PERIOD_ACLK 8

/* Tolerance for SMCLK ticks within a trim sample period.  The target
 * frequency count is divided by this number; if the measured
 * frequency count is not within that distance of the target, the FLL
 * is enabled for a short duration.  512 is about 0.2% */
#define TRIM_TOLERANCE_DIVISOR 512

/* The last calculated trim frequency of DCOCLKDIV.  Power-up to
 * 21MiHz/2. */
static unsigned long lastTrimFrequency_Hz_ = (1UL << 20);

#include <bsp430/utility/console.h>
#include <bsp430/periph/timer_.h>

#ifdef BSP430_TIMER_CCACLK_PERIPH_HANDLE

/* The target frequency expressed as the number of SMCLK ticks
 * expected within a trim sample period. */
static uint16_t targetFrequency_tsp_;

unsigned long
ulBSP430ucsTrimFLL_ni (void)
{
  short taps_left = 32;
  unsigned short last_ctl0;
  uint16_t tolerance_tsp;
  uint16_t current_frequency_tsp = 0;
  volatile xBSP430periphTIMER * tp = xBSP430periphLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
  const int ccidx = BSP430_TIMER_CCACLK_CC_INDEX;

  if (! tp) {
    return 0;
  }
  last_ctl0 = ~0;
  tolerance_tsp = targetFrequency_tsp_ / TRIM_TOLERANCE_DIVISOR;
  while (0 < taps_left--) {
    int i;
    unsigned int c0 = 0;
    unsigned int c1;
    uint16_t abs_freq_err_tsp;

    BSP430_CORE_WATCHDOG_CLEAR();
    /* Capture the SMCLK ticks between adjacent ACLK ticks */
    tp->ctl = TASSEL__SMCLK | MC__CONTINOUS | TBCLR;
    tp->cctl[ccidx] = CM_2 | BSP430_TIMER_CCACLK_CCIS | CAP | SCS;
    /* NOTE: CCIFG seems to be set immediately on the second and
     * subsequent iterations.  Flush the first capture. */
    while (! (tp->cctl[ccidx] & CCIFG)) {
      ; /* nop */
    }
    for (i = 0; i <= TRIM_SAMPLE_PERIOD_ACLK; ++i) {
      tp->cctl[ccidx] &= ~CCIFG;
      while (! (tp->cctl[ccidx] & CCIFG)) {
        ; /* nop */
      }
      if (0 == i) {
        c0 = tp->ccr[ccidx];
      }
    }
    c1 = tp->ccr[ccidx];
    tp->ctl = 0;
    tp->cctl[ccidx] = 0;
    current_frequency_tsp = (c0 > c1) ? (c0 - c1) : (c1 - c0);
    if (current_frequency_tsp > targetFrequency_tsp_) {
      abs_freq_err_tsp = current_frequency_tsp - targetFrequency_tsp_;
    } else {
      abs_freq_err_tsp = targetFrequency_tsp_ - current_frequency_tsp;
    }
#if 0
    cprintf("RSEL %u DCO %u MOD %u current %u target %u tol %u err %u ; ctl0 %04x last %04x\n",
            (UCSCTL1 >> 4) & 0x07, (UCSCTL0 >> 8) & 0x1F, (UCSCTL0 >> 3) & 0x1F,
            current_frequency_tsp, targetFrequency_tsp_, tolerance_tsp, abs_freq_err_tsp,
            UCSCTL0, last_ctl0);
#endif
    if ((abs_freq_err_tsp <= tolerance_tsp)
        || (UCSCTL0 == last_ctl0)) {
#if 0
      cprintf("terminate tap %u error %u ctl0 %04x was %04x\n",
              taps_left, abs_freq_err_tsp, UCSCTL0, last_ctl0);
#endif
      break;
    }
#if 0
    cprintf("running fll, error %u\n", abs_freq_err_tsp);
#endif
    /* Save current DCO/MOD values, then let FLL run for 32 REFCLK
     * ticks (potentially trying each modulation within one
     * tap) */
    last_ctl0 = UCSCTL0;
    tp->ctl = TASSEL__ACLK | MC__CONTINOUS | TBCLR;
    __bic_status_register(SCG0);
    tp->cctl0 = 0;
    tp->ccr0 = tp->r + 32;
    while (! (tp->cctl0 & CCIFG)) {
      /* nop */
    }
    __bis_status_register(SCG0);
    /* Delay another 1..2 ACLK cycles for the integrator to fully
     * update. */
    tp->cctl0 &= ~CCIFG;
    tp->ccr0 = tp->r + 2;
    while (! (tp->cctl0 & CCIFG)) {
      /* nop */
    }
    tp->ctl = 0;
    tp->cctl0 = 0;
  }
  lastTrimFrequency_Hz_ = current_frequency_tsp * (32768UL / TRIM_SAMPLE_PERIOD_ACLK);
  return lastTrimFrequency_Hz_;
}

unsigned long
ulBSP430ucsConfigure_ni (unsigned long mclk_Hz,
                         int rsel)
{
  unsigned int divs_bits;
  unsigned int sels_bits;
  /* The values in this table should be roughly half the minimum
   * frequency for the specified RSEL with DCOx=31 and MODx=0,
   * as taken from the device-specific data sheet. */
  static const unsigned long pulRSELCutoffs [] = {
#if defined(__MSP430F5438__) || defined(__MSP430F5438A__)
    700000UL / 2,			/* RSEL0 */
    1470000UL / 2,			/* RSEL1 */
    3170000UL / 2,			/* RSEL2 */
    6070000UL / 2,			/* RSEL3 */
    12300000UL / 2,			/* RSEL4 */
    23700000UL / 2,			/* RSEL5 */
    39000000UL / 2,			/* RSEL6 */
#else
#endif
    UINT32_MAX
  };
  unsigned int ctl1;
  unsigned int ctl2;
  unsigned long ulReturn;

  /* If not told what RSEL to use, pick the one appropriate for the
   * target frequency. */
  if ((0 > rsel) || (7 < rsel)) {
    rsel = 0;
    while (pulRSELCutoffs[rsel] < mclk_Hz) {
      ++rsel;
    }
  }

  /* Require XT1 valid and use it as ACLK source */
  if (UCSCTL7 & XT1LFOFFG) {
    (void)iBSP430clockConfigureLFXT1_ni (1, -1);
  }
  iBSP430ucsConfigureACLK_ni(SELA__XT1CLK);

  /* All supported frequencies can be efficiently achieved using
   * FFLD set to /2 (>> 1) and FLLREFDIV set to /1 (>> 0).
   * FLLREFCLK will always be XT1CLK.  FLLN is calculated from
   * mclk_Hz. */
  ctl1 = (DCORSEL0 | DCORSEL1 | DCORSEL2) & (rsel * DCORSEL0);
  ctl2 = FLLD_1 | ((((mclk_Hz << 1) / (32768 >> 0)) >> 1) - 1);

  /* Disable FLL while manually trimming */
  __bis_status_register(SCG0);
  UCSCTL0 = 0;
  UCSCTL1 = ctl1;
  UCSCTL2 = ctl2;
  UCSCTL3 = SELREF__XT1CLK | FLLREFDIV_0;
  sels_bits = UCSCTL4 & SELS_MASK;
  UCSCTL4 = (UCSCTL4 & SELA_MASK) | SELS__DCOCLKDIV | SELM__DCOCLKDIV;
  divs_bits = UCSCTL5 & DIVS_MASK;
  UCSCTL5 &= ~(DIVS_MASK | DIVM_MASK);

  targetFrequency_tsp_ = mclk_Hz / (32768 / TRIM_SAMPLE_PERIOD_ACLK);
  ulReturn = ulBSP430ucsTrimFLL_ni();

  /* Restore SMCLK source and divisor */
  UCSCTL5 |= divs_bits;
  UCSCTL4 = (UCSCTL4 & ~SELS_MASK) | sels_bits;

  /* Spin until DCO stabilized */
  do {
    UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
    SFRIFG1 &= ~OFIFG;
  } while (UCSCTL7 & DCOFFG);


#if ! (BSP430_CLOCK_DISABLE_FLL - 0)
  /* Turn FLL back on */
  __bic_status_register(SCG0);
#endif

  return ulReturn;
}

#endif /* BSP430_TIMER_CCACLK_PERIPH_HANDLE */

unsigned long
ulBSP430clockMCLK_Hz_ni (void)
{
  unsigned int divm = (UCSCTL5 & DIVM_MASK) / DIVM0;
  return lastTrimFrequency_Hz_ >> divm;
}

int
iBSP430clockSMCLKDividingShift_ni (void)
{
  int divs;
  /* Assume that the source for both MCLK and SMCLK is the same, but
   * account for a potential DIVM. */
  divs = (UCSCTL5 & DIVS_MASK) / DIVS0;
  divs -= (UCSCTL5 & DIVM_MASK) / DIVM0;
  return divs;
}

int
iBSP430clockConfigureSMCLKDividingShift_ni (int shift_pos)
{
  unsigned int selm;

  /* Set SELS to the same value as SELM. */
  selm = (UCSCTL4 & SELM_MASK) / SELM0;
  UCSCTL4 = (UCSCTL4 & ~SELS_MASK) | (selm * SELS0);

  /* Adjust the shift for any division happening at MCLK. */
  shift_pos += (UCSCTL5 & DIVM_MASK) / DIVM0;
  UCSCTL5 = (UCSCTL5 & ~DIVS_MASK) | (DIVS_MASK & (shift_pos * DIVS0));
  return iBSP430clockSMCLKDividingShift_ni();
}

unsigned short
usBSP430clockACLK_Hz_ni (void)
{
  if ((SELA__XT1CLK == (UCSCTL4 & (SELA0 | SELA1 | SELA2)))
      && !(BSP430_CLOCK_LFXT1_IS_FAULTED())) {
    return 32768U;
  }
  return BSP430_CLOCK_NOMINAL_VLOCLK_HZ;
}

int
iBSP430clockConfigureLFXT1_ni (int enablep,
                               int loop_limit)
{
  int loop_delta;
  int rc = 0;

  if (0 != loop_limit) {
    rc = iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, enablep);
    if ((0 != rc) || (! enablep)) {
      return rc;
    }
    loop_delta = (0 < loop_limit) ? 1 : 0;

    /* Low frequency XT1 needed.  Spin at high drive to stability, then
     * drop back.  Preserve XT1 configuration. */
    UCSCTL6 = (UCSCTL6 | XT1DRIVE_3) & ~(XTS | XT1BYPASS | XT1OFF);
    do {
      BSP430_CLOCK_LFXT1_CLEAR_FAULT();
      loop_limit -= loop_delta;
      BSP430_CORE_WATCHDOG_CLEAR();
      __delay_cycles(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
    } while ((BSP430_CLOCK_LFXT1_IS_FAULTED()) && (0 != loop_limit));
    UCSCTL6 &= ~XT1DRIVE_3;
    rc = ! BSP430_CLOCK_LFXT1_IS_FAULTED();
  }
  if (! rc) {
    UCSCTL6 |= XT1OFF;
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0);
  }
  return rc;
}


int iBSP430ucsConfigureACLK_ni (unsigned int sela)
{
  if (sela & ~SELA_MASK) {
    return -1;
  }
  UCSCTL4 = (UCSCTL4 & ~SELA_MASK) | sela;
  return 0;
}

unsigned long
ulBSP430clockConfigureMCLK_ni (unsigned long mclk_Hz)
{
  if (0 == mclk_Hz) {
    /* DCOCLKDIV = 2 * PUC MCLK */
    mclk_Hz = BSP430_CLOCK_PUC_MCLK_HZ << 1;
  }
#ifdef BSP430_TIMER_CCACLK_PERIPH_HANDLE
  ulBSP430ucsConfigure_ni(mclk_Hz, -1);
#endif
  return ulBSP430clockMCLK_Hz_ni();
}
