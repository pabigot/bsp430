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
#include <bsp430/periph/bc2.h>
#if BSP430_BC2_TRIM_TO_MCLK - 0
#include <bsp430/periph/timer.h>
#endif /* BSP430_BC2_TRIM_TO_MCLK */

/** The last frequency configured using #ulBSP430clockConfigureMCLK_ni */
static unsigned long configuredMCLK_Hz = BSP430_CLOCK_PUC_MCLK_HZ;

#define SELS_MASK (SELS)
#define SELM_MASK (SELM0 | SELM1)
#define DIVM_MASK (DIVM0 | DIVM1)
#define DIVS_MASK (DIVS0 | DIVS1)
#define SELA_MASK (LFXT1S0 | LFXT1S1)

int
iBSP430clockConfigureLFXT1_ni (int enablep,
                               int loop_limit)
{
  int loop_delta;
  int rc = 0;

  if (enablep && (0 != loop_limit)) {
    rc = iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, 1);
    if (0 != rc) {
      return rc;
    }
    loop_delta = (0 < loop_limit) ? 1 : 0;

    /* See whether the crystal is populated and functional.  Do
     * this with the DCO reset to the power-up configuration,
     * where clock should be nominal 1 MHz. */
    BCSCTL3 = XCAP_1;
    do {
      BSP430_CLOCK_LFXT1_CLEAR_FAULT_NI();
      BSP430_CORE_WATCHDOG_CLEAR();
      loop_limit -= loop_delta;
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
    } while ((BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) && (0 != loop_limit));
    rc = ! BSP430_CLOCK_LFXT1_IS_FAULTED_NI();
  }
  if (! rc) {
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, 0);
    BCSCTL3 = LFXT1S_2;
  }
  BSP430_CLOCK_LFXT1_CLEAR_FAULT_NI();
  return rc;
}

int
iBSP430clockSMCLKDividingShift_ni (void)
{
  int divs;

  /* Assume that the source for both MCLK and SMCLK is the same, but
   * account for a potential DIVM. */
  divs = (BCSCTL2 & DIVS_MASK) / DIVS0;
  divs -= (BCSCTL2 & DIVM_MASK) / DIVM0;
  return divs;
}

int
iBSP430clockConfigureSMCLKDividingShift_ni (int shift_pos)
{
  unsigned char bcsctl2 = BCSCTL2;

  /* Set SMCLK source to the same as MCLK */
  if (SELM1 & bcsctl2) {
    bcsctl2 |= SELS;
  } else {
    bcsctl2 &= ~SELS;
  }
  /* Adjust for division of MCLK */
  shift_pos += (bcsctl2 & DIVM_MASK) / DIVM0;
  BCSCTL2 = (bcsctl2 & ~DIVS_MASK) | (DIVS_MASK & (shift_pos * DIVS0));
  return iBSP430clockSMCLKDividingShift_ni();
}

unsigned int
uiBSP430clockACLK_Hz_ni (void)
{
  unsigned int clk_hz;
  int diva = (BCSCTL1 & DIVA_3) / DIVA0;
  switch (BCSCTL3 & SELA_MASK) {
    default:
    case LFXT1S_0:
      if (BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) {
        clk_hz = BSP430_CLOCK_NOMINAL_VLOCLK_HZ;
      } else {
        clk_hz = BSP430_CLOCK_NOMINAL_XT1CLK_HZ;
      }
      break;
    case LFXT1S_2:
      clk_hz = BSP430_CLOCK_NOMINAL_VLOCLK_HZ;
      break;
  }
  return clk_hz >> diva;
}

int
iBSP430clockConfigureACLK_ni (eBSP430clockSource sel)
{
  unsigned int sela = 0;
  switch (sel) {
    default:
    case eBSP430clockSRC_NONE:
      return -1;
    case eBSP430clockSRC_XT1CLK:
      sela = LFXT1S_0;
      break;
    case eBSP430clockSRC_VLOCLK:
      sela = LFXT1S_2;
      break;
    case eBSP430clockSRC_REFOCLK:
    case eBSP430clockSRC_DCOCLK:
    case eBSP430clockSRC_DCOCLKDIV:
    case eBSP430clockSRC_XT2CLK:
      return -1;
    case eBSP430clockSRC_XT1CLK_FALLBACK:
    case eBSP430clockSRC_XT1CLK_OR_VLOCLK:
      sela = BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? LFXT1S_2 : LFXT1S_0;
      break;
    case eBSP430clockSRC_XT1CLK_OR_REFOCLK:
      return -1;
  }
  BCSCTL3 = (BCSCTL3 & ~SELA_MASK) | sela;
  return 0;
}

unsigned long
ulBSP430clockMCLK_Hz_ni (void)
{
  return configuredMCLK_Hz;
}

#if BSP430_BC2_TRIM_TO_MCLK - 0

int
iBSP430bc2TrimToMCLK_ni (unsigned long mclk_Hz)
{
  volatile sBSP430hplTIMER * tp = xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
  const int MAX_ITERATIONS = 16 * 256;
  int rv = -1;
  unsigned int aclk_Hz;
  int iter = 0;
  const int SAMPLE_PERIOD_ACLK = 10;
  unsigned int target_tsp;

  if (! tp) {
    return -1;
  }

  aclk_Hz = uiBSP430clockACLK_Hz_ni();
  target_tsp = (SAMPLE_PERIOD_ACLK * mclk_Hz) / aclk_Hz;
  tp->ctl = TASSEL_2 | MC_2 | TACLR;
  /* SELM = DCOCLK; DIVM = /1 */
  BCSCTL2 &= ~(SELM_MASK | DIVM_MASK);
  while (iter++ < MAX_ITERATIONS) {
    unsigned int freq_tsp;
    freq_tsp = uiBSP430timerCaptureDelta_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE,
                                            BSP430_TIMER_CCACLK_ACLK_CC,
                                            CM_2,
                                            BSP430_TIMER_CCACLK_ACLK_CCIS,
                                            SAMPLE_PERIOD_ACLK);
    if (freq_tsp == target_tsp) {
      configuredMCLK_Hz = mclk_Hz;
      rv = 0;
      break;
    }
    if (target_tsp < freq_tsp) {
      /* DCO too fast.  Decrement modulator; if underflowed,
       * decrement RSEL */
      if (0xFF == --DCOCTL) {
        --BCSCTL1;
      }
    } else {
      /* DCO too slow.  Increment modulator; if overflowed,
       * increment RSEL */
      if (0 == ++DCOCTL) {
        ++BCSCTL1;
      }
    }
  }
  tp->ctl = 0;
  return rv;
}
#endif /* BSP430_BC2_TRIM_TO_MCLK */

unsigned long
ulBSP430clockConfigureMCLK_ni (unsigned long mclk_Hz)
{
  unsigned char dcoctl;
  unsigned char bcsctl1;
  unsigned long error_Hz;
  int use_trim_to_mclk = 1;
  long freq_Hz;

  if (0 == mclk_Hz) {
    mclk_Hz = BSP430_CLOCK_PUC_MCLK_HZ;
    use_trim_to_mclk = 0;
  }
  /* Power-up defaults */
  dcoctl = 0x60;
  bcsctl1 = 0x87;
  freq_Hz = BSP430_CLOCK_PUC_MCLK_HZ;

  /* Calculate absolute error from _freq_Hz to target */
#define ERROR_HZ(_freq_Hz) ((mclk_Hz < _freq_Hz) ? (_freq_Hz - mclk_Hz) : (mclk_Hz - _freq_Hz))
  error_Hz = ERROR_HZ(freq_Hz);

  /* Test a candidate to see if it's better than what we've got now */
#define TRY_FREQ(_tag, _cand_Hz) do {                   \
    unsigned long cand_error_Hz = ERROR_HZ(_cand_Hz);   \
    if (cand_error_Hz < error_Hz) {                     \
      dcoctl = CALDCO_##_tag;                           \
      bcsctl1 = CALBC1_##_tag;                          \
      freq_Hz = _cand_Hz;                               \
      error_Hz = cand_error_Hz;                         \
    }                                                   \
  } while (0)

  /* Candidate availability is MCU-specific and can be determined by
   * checking for a corresponding preprocessor definition */
#if defined(CALDCO_1MHZ_)
  TRY_FREQ(1MHZ, 1000000UL);
#endif /* CALDCO_1MHZ */
#if defined(CALDCO_8MHZ_)
  TRY_FREQ(8MHZ, 8000000UL);
#endif /* CALDCO_8MHZ */
#if defined(CALDCO_12MHZ_)
  TRY_FREQ(12MHZ, 12000000UL);
#endif /* CALDCO_12MHZ */
#if defined(CALDCO_16MHZ_)
  TRY_FREQ(16MHZ, 16000000UL);
#endif /* CALDCO_16MHZ */

#undef TRY_FREQ
#undef ERROR_HZ

  /* Clearing DCO bits first supports workaround for erratum BCL12 */
  DCOCTL = 0;
  BCSCTL1 = bcsctl1;
  DCOCTL = dcoctl;
  /* SELM = DCOCLK; DIVM = /1 */
  BCSCTL2 &= ~(SELM_MASK | DIVM_MASK);
  configuredMCLK_Hz = freq_Hz;

  if (use_trim_to_mclk) {
#if BSP430_BC2_TRIM_TO_MCLK - 0
    if (! BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) {
      (void)iBSP430bc2TrimToMCLK_ni(mclk_Hz);
    }
#endif /* BSP430_BC2_TRIM_TO_MCLK */
  }

  /* Spin until DCO faults cleared */
  do {
    BSP430_CLOCK_LFXT1_CLEAR_FAULT_NI();
    IFG1 &= ~OFIFG;
    BSP430_CORE_WATCHDOG_CLEAR();
    /* Wait at least 50 usec, assuming speed does not exceed 16 MHz.
     * Delay suggested by SLAU144I "2xx Family Users Guide" section
     * 5.2.7.1 "Sourcing MCLK from a Crystal".  This applies to using
     * XT2, and we're guessing it's a sufficient delay to detect
     * faults in other configurations. */
    __delay_cycles(16 * 50);
  } while (IFG1 & OFIFG);

  return configuredMCLK_Hz;
}
