/* Copyright 2012-2013, Peter A. Bigot
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

/* Maximum respected value for dividing shifts */
#define DIV_MAX_VALUE 5

/** Range selection supports 3 bits.  Note that this mask is not for
 * the RSEL field in the UCSCTL1 register, it's for the RSEL index in isolation. */
#define RSEL_INDEX_MASK 0x07

/** DCO selection supports 5 bits.  Note that this mask is not for the
 * DCO field in the UCSCTL0 register, it's for the DCO index in
 * isolation. */
#define DCO_INDEX_MASK 0x1F

/* Basic clock sources.  These values may be used in UCS registers. */
#define CSEL_XT1CLK 0
#define CSEL_VLOCLK 1
#define CSEL_REFOCLK 2
#define CSEL_DCOCLK 3
#define CSEL_DCOCLKDIV 4
#define CSEL_XT2CLK 5

/* Mask for legal CSEL value range */
#define CSEL_MASK 0x07

/** Frequency of ACLK when used for trimming.
 *
 * This assumes that a 32 kiHz crystal will be used for XT1.  If not,
 * the trim frequency would be different when REFOCLK is used and that
 * would make things messy. */
#define TRIM_ACLK_HZ 32768

/** Best estimate of frequency for the clock driving the FLL */
#if SELREF__XT2CLK <= BSP430_UCS_FLL_SELREF
#define FLLREFCLK_HZ BSP430_CLOCK_NOMINAL_XT2CLK_HZ
#else /* BSP430_UCS_FLL_SELREF */
#define FLLREFCLK_HZ TRIM_ACLK_HZ
#endif

/* Frequency measurement occurs over this duration when determining
 * whether trim is required.  The number of SMCLK ticks in an ACLK
 * period is the target frequency divided by ACLK_HZ (32768 KiHz);
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
#define HZ_TO_TSP(clk_hz_) ((clk_hz_) / (TRIM_ACLK_HZ / TRIM_SAMPLE_PERIOD_ACLK))

/** Convert from Trim Sample Periods to Hz */
#define TSP_TO_HZ(clk_tsp_) (((clk_tsp_) * (unsigned long)TRIM_ACLK_HZ) / TRIM_SAMPLE_PERIOD_ACLK)

#if (BSP430_UCS_TRIM_DCOCLKDIV - 0)
#include <bsp430/periph/timer.h>

/* The target frequency expressed as the number of SMCLK ticks
 * expected within TRIM_SAMPLE_PERIOD ACLK ticks, assuming that SMCLK
 * is undivided DCOCLKDIV. */
static uint16_t targetFrequency_tsp_;

int
iBSP430ucsTrimDCOCLKDIV_ni (void)
{
  unsigned int ucsctl4;
  unsigned int ucsctl5;
  unsigned int scg0;
  int taps_left = 32;
  unsigned int last_ctl0;
  uint16_t tolerance_tsp;
  uint16_t current_frequency_tsp = 0;
  volatile sBSP430hplTIMER * tp = xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);

  if (! tp) {
    return -1;
  }

  /* Save clock source and divisor configuration */
  ucsctl4 = UCSCTL4;
  ucsctl5 = UCSCTL5;

  /* Preserve the incoming value of the SCG0 flag, and disable FLL
   * while assessing out-of-trim */
  scg0 = SCG0 & __read_status_register();
  __bis_status_register(SCG0);

  /* Configure clock sources for trim analysis. */
  UCSCTL4 = ((UCSCTL7 & XT1LFOFFG) ? SELA__REFOCLK : SELA__XT1CLK) | SELS__DCOCLKDIV | SELM__DCOCLKDIV;
  UCSCTL5 = 0;

  /* Ensure we can read the CCACLK counter safely */
  vBSP430timerSafeCounterInitialize_ni(tp);

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
                                                         BSP430_TIMER_CCACLK_ACLK_CCIDX,
                                                         CM_2,
                                                         BSP430_TIMER_CCACLK_ACLK_CCIS,
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
    tp->ctl = TASSEL__ACLK | MC__CONTINOUS | TACLR;
    __bic_status_register(SCG0);
    tp->cctl0 = 0;
    tp->ccr0 = uiBSP430timerSafeCounterRead_ni(tp) + 32;
    while (! (tp->cctl0 & CCIFG)) {
      BSP430_CORE_WATCHDOG_CLEAR();
    }
    __bis_status_register(SCG0);
    /* Delay another 1..2 ACLK cycles for the integrator to fully
     * update. */
    tp->cctl0 &= ~CCIFG;
    tp->ccr0 = uiBSP430timerSafeCounterRead_ni(tp) + 2;
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
    __bic_status_register(SCG0);
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

  /* Record where we're trying to get */
  targetFrequency_tsp_ = HZ_TO_TSP(dcoclkdiv_Hz);

  /* Set the FLL reference clock, and the FLL parameters required to
   * obtain the desired DCOCLK. */
  UCSCTL3 = (BSP430_UCS_FLL_SELREF) | FLLREFDIV_0;
  /* All supported frequencies can be efficiently achieved using FFLD
   * set to /2 (>> 1) and FLLREFDIV set to /1 (>> 0).  FLLN is
   * calculated from dcoclkdiv_Hz. */
  dcoclk_Hz = dcoclkdiv_Hz << 1;
  dcoclk_refclk = dcoclk_Hz / (FLLREFCLK_HZ >> 0);
  UCSCTL2 = FLLD_1 | ((dcoclk_refclk >> 1) - 1);

  /* Set MCLK to be undivided DCOCLKDIV */
  UCSCTL4 = (UCSCTL4 & (SELA_MASK | SELS_MASK)) | SELM__DCOCLKDIV;
  UCSCTL5 &= ~DIVM_MASK;

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
    if (0 > iBSP430ucsTrimDCOCLKDIV_ni()) {
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

  /* Clear all the oscillator faults and spin until DCO stabilized. */
  do {
    BSP430_CLOCK_CLEAR_FAULTS_NI();
    BSP430_CORE_WATCHDOG_CLEAR();
    /* Conservatively assume a 32 MHz clock */
    BSP430_CORE_DELAY_CYCLES(32 * BSP430_CLOCK_FAULT_RECHECK_DELAY_US);
  } while (BSP430_UCS_DCO_IS_FAULTED_NI());
}

#endif /* BSP430_UCS_TRIM_DCOCLKDIV */

int
iBSP430clockConfigureLFXT1_ni (int enablep,
                               int loop_limit)
{
  int loop_delta;
  int rc = 0;

  BSP430_CLOCK_CLEAR_FAULTS_NI();
  if (enablep && (0 != loop_limit)) {
    rc = iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, 1);
    if (0 == rc) {
      loop_delta = (0 < loop_limit) ? 1 : 0;

      /* Low frequency XT1 needed.  Spin at high drive to stability, then
       * drop back. */
      UCSCTL6 = XT1DRIVE_3 | BSP430_CLOCK_LFXT1_XCAP | (UCSCTL6 & ~(XTS | XT1BYPASS | XT1OFF | XCAP0 | XCAP1));
      do {
        BSP430_CLOCK_CLEAR_FAULTS_NI();
        loop_limit -= loop_delta;
        BSP430_CORE_WATCHDOG_CLEAR();
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
      } while ((BSP430_UCS_LFXT1_IS_FAULTED_NI()) && (0 != loop_limit));
      rc = ! BSP430_UCS_LFXT1_IS_FAULTED_NI();
    }
  }
  BSP430_CLOCK_OSC_CLEAR_FAULT_NI();
  UCSCTL6 &= ~XT1DRIVE_3;
  if (! rc) {
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, 0);
    /* Disable as an indication that XIN is not enabled.  Also remove
     * capacitance setting. */
    UCSCTL6 = XT1OFF | (UCSCTL6 & ~(XCAP0 | XCAP1));
  }
  return rc;
}

#if defined(BSP430_CLOCK_NOMINAL_XT2CLK_HZ)
int
iBSP430clockConfigureXT2_ni (int enablep,
                             int loop_limit)
{
  int loop_delta;
  int rc = 0;

  if (enablep && (0 != loop_limit)) {
    rc = iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_XT2, 0, 1);
    if (0 == rc) {
      loop_delta = (0 < loop_limit) ? 1 : 0;

#if defined(__MSP430_HAS_UCS_RF__)
      /* RF version has no control of anything but XT2OFF. */
      UCSCTL6 &= ~XT2OFF;
#else /* UCS_RF */
      /* XT2 start at high drive to stability, then drop back. */
      UCSCTL6 = XT2DRIVE_3 | (UCSCTL6 & ~(XT2BYPASS | XT2OFF));
#endif /* UCS_RF */
      do {
        BSP430_CLOCK_CLEAR_FAULTS_NI();
        loop_limit -= loop_delta;
        BSP430_CORE_WATCHDOG_CLEAR();
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_XT2_STABILIZATION_DELAY_CYCLES);
      } while ((BSP430_CLOCK_XT2_IS_FAULTED_NI()) && (0 != loop_limit));
#if ! defined(__MSP430_HAS_UCS_RF__)
      UCSCTL6 &= ~XT2DRIVE_3;
#endif /* UCS_RF */
      rc = ! BSP430_CLOCK_XT2_IS_FAULTED_NI();
    }
  }
  BSP430_CLOCK_OSC_CLEAR_FAULT_NI();
#if ! defined(__MSP430_HAS_UCS_RF__)
  UCSCTL6 &= ~XT2DRIVE_3;
#endif /* UCS_RF */
  if (! rc) {
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_XT2, 0, 0);
    UCSCTL6 |= XT2OFF;
  }
  return rc;
}
#endif /* BSP430_CLOCK_NOMINAL_XT2CLK_HZ */

static const eBSP430clockSource cselMap_[8] = {
  eBSP430clockSRC_XT1CLK,
  eBSP430clockSRC_VLOCLK,
  eBSP430clockSRC_REFOCLK,
  eBSP430clockSRC_DCOCLK,
  eBSP430clockSRC_DCOCLKDIV,
  eBSP430clockSRC_XT2CLK,
  eBSP430clockSRC_UNKNOWN_6,
  eBSP430clockSRC_UNKNOWN_7,
};

eBSP430clockSource xBSP430clockACLKSource ()
{
  return cselMap_[(UCSCTL4 & SELA_MASK) / SELA0];
}

eBSP430clockSource xBSP430clockSMCLKSource ()
{
  return cselMap_[(UCSCTL4 & SELS_MASK) / SELS0];
}

eBSP430clockSource xBSP430clockMCLKSource ()
{
  return cselMap_[(UCSCTL4 & SELM_MASK) / SELM0];
}

static unsigned long
cselToFreq_Hz_ (int csel)
{
  switch (csel & CSEL_MASK) {
    case CSEL_XT1CLK: /* XT1CLK */
      if (! BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) {
        return BSP430_CLOCK_NOMINAL_XT1CLK_HZ;
      }
    /*FALLTHRU*/
    case CSEL_REFOCLK: /* REFOCLK */
      return BSP430_UCS_NOMINAL_REFOCLK_HZ;
    case CSEL_VLOCLK: /* VLOCLK */
      return BSP430_CLOCK_NOMINAL_VLOCLK_HZ;
    case CSEL_DCOCLK: /* DCOCLK */
      return 2 * lastTrimDCOCLKDIV_Hz_;
    case CSEL_XT2CLK: /* XT2CLK */
#if defined(SELA__XT2CLK) && (configBSP430_PERIPH_XT2 - 0)
      return BSP430_CLOCK_NOMINAL_XT2CLK_HZ;
#endif /* XT2CLK supported */
    default:
    case CSEL_DCOCLKDIV: /* DCOCLKDIV */
      return lastTrimDCOCLKDIV_Hz_;
  }
}

static int
sourceToCSEL_ (eBSP430clockSource sel)
{
  switch (sel) {
    default:
    case eBSP430clockSRC_NONE:
      return -1;
    case eBSP430clockSRC_XT1CLK:
      return CSEL_XT1CLK;
    case eBSP430clockSRC_VLOCLK:
      return CSEL_VLOCLK;
    case eBSP430clockSRC_REFOCLK:
      return CSEL_REFOCLK;
    case eBSP430clockSRC_DCOCLK:
      return CSEL_DCOCLK;
    case eBSP430clockSRC_SMCLK_PU_DEFAULT:
    case eBSP430clockSRC_DCOCLKDIV:
      return CSEL_DCOCLKDIV;
#if defined(SELA__XT2CLK) && (configBSP430_PERIPH_XT2 - 0)
    case eBSP430clockSRC_XT2CLK:
      return CSEL_XT2CLK;
#endif /* XT2CLK supported */
    case eBSP430clockSRC_XT1CLK_OR_VLOCLK:
      return BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? CSEL_VLOCLK : CSEL_XT1CLK;
    case eBSP430clockSRC_XT1CLK_FALLBACK:
    case eBSP430clockSRC_XT1CLK_OR_REFOCLK:
      return BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? CSEL_REFOCLK : CSEL_XT1CLK;
  }
  return -1;
}

static unsigned long
divideFrequency_ (unsigned long freq_Hz,
                  unsigned int div)
{
  if (DIV_MAX_VALUE < div) {
    div = DIV_MAX_VALUE;
  }
  return freq_Hz >> div;
}

unsigned long
ulBSP430clockMCLK_Hz_ni (void)
{
  return divideFrequency_(cselToFreq_Hz_((UCSCTL4 & SELM_MASK) / SELM0),
                          (UCSCTL5 & DIVM_MASK) / DIVM0);
}

unsigned long
ulBSP430clockSMCLK_Hz_ni (void)
{
  return divideFrequency_(cselToFreq_Hz_((UCSCTL4 & SELS_MASK) / SELS0),
                          (UCSCTL5 & DIVS_MASK) / DIVS0);
}

unsigned long
ulBSP430clockACLK_Hz_ni (void)
{
  return divideFrequency_(cselToFreq_Hz_((UCSCTL4 & SELA_MASK) / SELA0),
                          (UCSCTL5 & DIVA_MASK) / DIVA0);
}

int
iBSP430clockConfigureACLK_ni (eBSP430clockSource sel,
                              unsigned int dividing_shift)
{
  int csel = sourceToCSEL_(sel);
  if (0 > csel) {
    return -1;
  }
  if (DIV_MAX_VALUE < dividing_shift) {
    dividing_shift = DIV_MAX_VALUE;
  }
  UCSCTL5 = (UCSCTL5 & ~DIVA_MASK) | (DIVA_MASK & (dividing_shift * DIVA0));
  UCSCTL4 = (UCSCTL4 & ~SELA_MASK) | (csel * SELA0);
  return 0;
}

int
iBSP430clockConfigureSMCLK_ni (eBSP430clockSource sel,
                               unsigned int dividing_shift)
{
  int csel = sourceToCSEL_(sel);
  if (0 > csel) {
    return -1;
  }
  if (DIV_MAX_VALUE < dividing_shift) {
    dividing_shift = DIV_MAX_VALUE;
  }
  UCSCTL5 = (UCSCTL5 & ~DIVS_MASK) | (DIVS_MASK & (dividing_shift * DIVS0));
  UCSCTL4 = (UCSCTL4 & ~SELS_MASK) | (csel * SELS0);
  return 0;
}


unsigned long
ulBSP430clockConfigureMCLK_ni (unsigned long mclk_Hz)
{
  if (0 == mclk_Hz) {
    mclk_Hz = BSP430_CLOCK_PUC_MCLK_HZ;
  }
#if (BSP430_UCS_TRIM_DCOCLKDIV - 0)
  vBSP430ucsConfigureMCLK_ni(mclk_Hz, -1);
#endif /* BSP430_UCS_TRIM_DCOCLKDIV */
  return ulBSP430clockMCLK_Hz_ni();
}
