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

/* Mask for DIVA bits in UCSCTL5 */
#define DIVA_MASK (DIVA0 | DIVA1 | DIVA2)

/* Mask for DIVS bits in UCSCTL5 */
#define DIVS_MASK (DIVS0 | DIVS1 | DIVS2)

/* Mask for DIVM bits in UCSCTL5 */
#define DIVM_MASK (DIVM0 | DIVM1 | DIVM2)

/** Range selection supports 3 bits.  Note that this mask is not for
 * the RSEL field in the UCSCTL1 register, it's for the RSEL index in isolation. */
#define RSEL_INDEX_MASK 0x07

/** DCO selection supports 5 bits.  Note that this mask is not for the
 * DCO field in the UCSCTL0 register, it's for the DCO index in
 * isolation. */
#define DCO_INDEX_MASK 0x1F

/** Constant used in UCSCTL3/USCTL4 to select XT1CLK */
#define CLOCKSEL_XT1CLK 0
/** Constant used in UCSCTL3/USCTL4 to select REFOCLK */
#define CLOCKSEL_REFOCLK 2
/** Constant used in UCSCTL4 to select DCOCLKDIV */
#define CLOCKSEL_DCOCLKDIV 4

/** Frequency measurement will be done with ACLK, which will be
 * configured to either XT1CLK or REFOCLK. */
#if configBSP430_UCS_TRIM_ACLK_IS_XT1CLK - 0
#define TRIM_ACLK_HZ BSP430_CLOCK_NOMINAL_XT1CLK_HZ
#define CLOCKSEL_TRIM_ACLK CLOCKSEL_XT1CLK
#else /* configBSP430_UCS_TRIM_ACLK_IS_XT1CLK */
#define TRIM_ACLK_HZ BSP430_UCS_NOMINAL_REFOCLK_HZ
#define CLOCKSEL_TRIM_ACLK CLOCKSEL_REFOCLK
#endif /* configBSP430_UCS_TRIM_ACLK_IS_XT1CLK */

/** The FLL reference clock may be either XT1 or REFOCLK. */
#if configBSP430_UCS_FLLREFCLK_IS_XT1CLK - 0
#define FLLREFCLK_HZ BSP430_CLOCK_NOMINAL_XT1CLK_HZ
#define CLOCKSEL_FLLREFCLK CLOCKSEL_XT1CLK
#else /* configBSP430_UCS_FLLREFCLK_IS_XT1CLK */
#define FLLREFCLK_HZ BSP430_UCS_NOMINAL_REFOCLK_HZ
#define CLOCKSEL_FLLREFCLK CLOCKSEL_REFOCLK
#endif /* configBSP430_UCS_FLLREFCLK_IS_XT1CLK */

/* Frequency measurement occurs over this duration when determining
 * whether trim is required.  The number of SMCLK ticks in an ACLK
 * period is the target frequency divided by ACLK_HZ (32768 kiHz);
 * accumulating over multiple ACLK periods decreases the measurement
 * error.  At a maximum target frequency of 2^25 (32 MiHz) the tick
 * count for a single period might require 11 bits to represent, so do
 * not exceed 32 (2^5) lest the 16-bit delta value overflow.  Select a
 * value so that the number of ticks within the sample period is some
 * small (~3) multiple of TRIM_TOLERANCE_DIVISOR. */
#define TRIM_SAMPLE_PERIOD_ACLK 8

/** Tolerance for SMCLK ticks within a trim sample period.  The target
 * frequency count is divided by this number; if the measured
 * frequency count is not within that distance of the target, the FLL
 * is enabled for a short duration.  512 is about 0.2% */
#define TRIM_TOLERANCE_DIVISOR 512

/* The last calculated trim frequency of DCOCLKDIV.  Power-up to
 * 21MiHz/2. */
static unsigned long lastTrimDCOCLKDIV_Hz_ = BSP430_CLOCK_PUC_MCLK_HZ;

/** Convert from Hz to Trim Sample Periods */
#define HZ_TO_TSP(_clk_hz) ((_clk_hz) / (TRIM_ACLK_HZ / TRIM_SAMPLE_PERIOD_ACLK))

/** Convert from Trim Sample Periods to Hz */
#define TSP_TO_HZ(_clk_tsp) (((_clk_tsp) * (unsigned long)TRIM_ACLK_HZ) / TRIM_SAMPLE_PERIOD_ACLK)

#if BSP430_CLOCK_TRIM_FLL - 0
#include <bsp430/periph/timer_.h>

/* The target frequency expressed as the number of SMCLK ticks
 * expected within TRIM_SAMPLE_PERIOD ACLK ticks, assuming that SMCLK
 * is undivided DCOCLKDIV. */
static uint16_t targetFrequency_tsp_;

static int
iBSP430ucsTrimFLL_ni (void)
{
  unsigned int ucsctl4;
  unsigned int ucsctl5;
  unsigned int scg0;
  short taps_left = 32;
  unsigned short last_ctl0;
  uint16_t tolerance_tsp;
  uint16_t current_frequency_tsp = 0;
  volatile xBSP430periphTIMER * tp = xBSP430periphLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
  
  if (! tp) {
    return -1;
  }

  /* Save clock source and divisor configuration */
  ucsctl4 = UCSCTL4;
  ucsctl5 = UCSCTL5;

#if (configBSP430_UCS_TRIM_ACLK_IS_XT1CLK - 0) || (configBSP430_UCS_FLLREFCLK_IS_XT1CLK - 0)
  /* Require XT1 valid and use it as ACLK source.  If it can't be
   * stabilized, this function call won't return. */
  if (UCSCTL7 & XT1LFOFFG) {
    return -1;
  }
#endif /* Require LFXT1 */

  /* Preserve the incoming value of the SCG0 flag, and disable FLL
   * while assessing out-of-trim */
  scg0 = SCG0 & __read_status_register();
  __bis_status_register(SCG0);

  /* Configure clock sources for trim analysis */
  UCSCTL4 = (CLOCKSEL_TRIM_ACLK * SELA0) | SELS__DCOCLKDIV | SELM__DCOCLKDIV;
  UCSCTL5 = 0;

  /* Almost-certainly-invalid UCSCTL0 value.  This includes reserved
   * bits that read back as zero, so we "know" we won't terminate
   * without trimming at least once. */
  last_ctl0 = ~0;

  tolerance_tsp = targetFrequency_tsp_ / TRIM_TOLERANCE_DIVISOR;
  while (0 < taps_left--) {
    uint16_t abs_freq_err_tsp;

    BSP430_CORE_WATCHDOG_CLEAR();
    /* Capture the SMCLK ticks between adjacent ACLK ticks */
    tp->ctl = TASSEL__SMCLK | MC__CONTINOUS | TACLR;
    current_frequency_tsp = uiBSP430timerCaptureDelta_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE,
                                                         BSP430_TIMER_CCACLK_CC_INDEX,
                                                         CM_2,
                                                         BSP430_TIMER_CCACLK_CCIS,
                                                         TRIM_SAMPLE_PERIOD_ACLK);
    tp->ctl = 0;
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
     * ticks (potentially trying each modulation within one tap).
     * REFCLK here is probably XT1CLK so a tick is at most 100usec
     * using VLOCLK. */
    last_ctl0 = UCSCTL0;
    tp->ctl = TASSEL__ACLK | MC__CONTINOUS | TBCLR;
    __bic_status_register(SCG0);
    tp->cctl0 = 0;
    tp->ccr0 = tp->r + 32;
    while (! (tp->cctl0 & CCIFG)) {
      BSP430_CORE_WATCHDOG_CLEAR();
    }
    __bis_status_register(SCG0);
    /* Delay another 1..2 ACLK cycles for the integrator to fully
     * update. */
    tp->cctl0 &= ~CCIFG;
    tp->ccr0 = tp->r + 2;
    while (! (tp->cctl0 & CCIFG)) {
      BSP430_CORE_WATCHDOG_CLEAR();
    }
    tp->ctl = 0;
    tp->cctl0 = 0;
  }

  /* Record the measured DCOCLKDIV */
  lastTrimDCOCLKDIV_Hz_ = TSP_TO_HZ(current_frequency_tsp);

  /* Restore clock source and divisor configuration */
  UCSCTL4 = ucsctl4;
  UCSCTL5 = ucsctl5;

  /* Restore SCG0 (if was clear on entry, clear it now) */
  if (! scg0) {
    __bic_status_register(scg0);
  }
  return 0;
}

/* Configure UCS to use DCOCLKDIV without further division for MCLK
 * and SMCLK.  Configure FLL so DCOCLKDIV is the requested MCLK
 * frequency. */
static void
vBSP430ucsConfigureMCLK_ni (unsigned long mclk_Hz,
                            int rsel)
{
  unsigned long dcoclkdiv_Hz = mclk_Hz;
  unsigned long dcoclk_Hz;
  unsigned int dcoclk_refclk;

  /* Sanity check the RSEL.  If asked to provide a best-guess, pick
   * the middle one. */
  if (0 > rsel) {
    rsel = RSEL_INDEX_MASK / 2;
  }
  rsel &= RSEL_INDEX_MASK;

#if (configBSP430_UCS_TRIM_ACLK_IS_XT1CLK - 0) || (configBSP430_UCS_FLLREFCLK_IS_XT1CLK - 0)
  /* Require XT1 valid and use it as ACLK source.  If it can't be
   * stabilized, this function call won't return. */
  if (UCSCTL7 & XT1LFOFFG) {
    (void)iBSP430clockConfigureLFXT1_ni (1, -1);
  }
#endif /* Require LFXT1 */

  /* Record where we're trying to get */
  targetFrequency_tsp_ = HZ_TO_TSP(dcoclkdiv_Hz);

  /* Set the FLL reference clock, and the FLL parameters required to
   * obtain the desired DCOCLK. */
  UCSCTL3 = (CLOCKSEL_FLLREFCLK * SELREF0) | FLLREFDIV_0;
  /* All supported frequencies can be efficiently achieved using FFLD
   * set to /2 (>> 1) and FLLREFDIV set to /1 (>> 0).  FLLN is
   * calculated from dcoclkdiv_Hz. */
  dcoclk_Hz = dcoclkdiv_Hz << 1;
  dcoclk_refclk = dcoclk_Hz / (FLLREFCLK_HZ >> 0);
  UCSCTL2 = FLLD_1 | ((dcoclk_refclk >> 1) - 1);

  /* Set SMCLK and MCLK to be undivided DCOCLKDIV */
  UCSCTL4 = (UCSCTL4 & SELA_MASK) | SELS__DCOCLKDIV | SELM__DCOCLKDIV;
  UCSCTL5 &= (UCSCTL4 & DIVA_MASK);

  /* Stabilize on an RSEL where the trimmed frequency does not leave
   * the DCO in a faulted state */
  while (1) {
    unsigned int dco;

    BSP430_CORE_WATCHDOG_CLEAR();

    /* Select the desired range, enable modulation, and start at the
     * middle tap and first modulation pattern */
    UCSCTL0 = (DCO_INDEX_MASK / 2) * DCO0;
    UCSCTL1 = (DCORSEL0 | DCORSEL1 | DCORSEL2) & (rsel * DCORSEL0);

    /* Execute the trim function.  If something went wrong, just give
     * up. */
    if (0 > iBSP430ucsTrimFLL_ni()) {
      break;
    }

    /* The first and last taps record a DCO fault in UCSCTL7.DCOFFG,
     * so if we ended up there move to the next range and try
     * again.  Otherwise we can stop. */
    dco = (UCSCTL0 / DCO0) & DCO_INDEX_MASK;
    if (0 == dco) {
      --rsel;
    } else if (DCO_INDEX_MASK == dco) {
      ++rsel;
    } else {
      break;
    }
  }

  /* Clear all the oscillator faults and spin until DCO stabilized.
   * Not all fault sources are supported on all MCUs, so only include
   * the ones that the header supports.  Chances are this executes
   * once; if there's an oscillator fault in the DCO here there's
   * nothing that can be done to stabilize it unless the FLL is
   * enabled and it happens by chance. */
  do {
    UCSCTL7 &= ~(XT1LFOFFG
#if defined(XT2OFFG)
                 + XT2OFFG
#endif /* XT2OFFG */
#if defined(XT1HFOFFG)
                 + XT1HFOFFG
#endif /* XT1HFOFFG */
                 + DCOFFG);
    SFRIFG1 &= ~OFIFG;
  } while (UCSCTL7 & DCOFFG);
}

#endif /* BSP430_CLOCK_TRIM_FLL */

unsigned long
ulBSP430clockMCLK_Hz_ni (void)
{
  unsigned int divm = (UCSCTL5 & DIVM_MASK) / DIVM0;
  return lastTrimDCOCLKDIV_Hz_ >> divm;
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

int
iBSP430clockConfigureLFXT1_ni (int enablep,
                               int loop_limit)
{
  int loop_delta;
  int rc = 0;

  if (enablep && (0 != loop_limit)) {
    rc = iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 1);
    if (0 != rc) {
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
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
    } while ((BSP430_CLOCK_LFXT1_IS_FAULTED()) && (0 != loop_limit));
    UCSCTL6 &= ~XT1DRIVE_3;
    rc = ! BSP430_CLOCK_LFXT1_IS_FAULTED();
  }
  if (! rc) {
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0);
    UCSCTL6 |= XT1OFF;
    UCSCTL6 &= XT1DRIVE0 | XT1DRIVE1;
  }
  return rc;
}

unsigned short
usBSP430clockACLK_Hz_ni (void)
{
  switch (UCSCTL4 & SELA_MASK) {
    case SELA_0: /* XT1CLK */
      if (! BSP430_CLOCK_LFXT1_IS_FAULTED()) {
        return BSP430_CLOCK_NOMINAL_XT1CLK_HZ;
      }
      /*FALLTHRU*/
    case SELA_2: /* REFOCLK */
      return BSP430_UCS_NOMINAL_REFOCLK_HZ;
    case SELA_1: /* VLOCLK */
      return BSP430_CLOCK_NOMINAL_VLOCLK_HZ;
    case SELA_3: /* DCOCLK */
      return 2 * lastTrimDCOCLKDIV_Hz_;
    default:
    case SELA_4: /* DCOCLKDIV */
      return lastTrimDCOCLKDIV_Hz_;
  }
}

int iBSP430clockConfigureACLK_ni (unsigned int sela)
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
    mclk_Hz = BSP430_CLOCK_PUC_MCLK_HZ;
  }
#if BSP430_CLOCK_TRIM_FLL - 0
  vBSP430ucsConfigureMCLK_ni(mclk_Hz, -1);
#endif /* BSP430_CLOCK_TRIM_FLL */
  return ulBSP430clockMCLK_Hz_ni();
}

unsigned long
ulBSP430clockTrimFLL_ni (void)
{
#if BSP430_CLOCK_TRIM_FLL - 0
  if (0 > iBSP430ucsTrimFLL_ni()) {
    return 0;
  }
#endif /* BSP430_CLOCK_TRIM_FLL */
  return ulBSP430clockMCLK_Hz_ni();
}

