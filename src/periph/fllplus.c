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

#include <bsp430/periph/fllplus.h>
#include <bsp430/platform.h>

#define DIVA_MASK (FLL_DIV0 | FLL_DIV1)

/* Mask extracting the N bits from SCFQCTL */
#define FLL_N_MASK 127

unsigned long
ulBSP430clockConfigureMCLK_ni (unsigned long mclk_Hz)
{
  unsigned int flld = 0;
  unsigned int fn_x = 0;
  int dcoplus = 0;
  unsigned int dcoclk_xt1 = (mclk_Hz + BSP430_CLOCK_NOMINAL_XT1CLK_HZ / 2) / BSP430_CLOCK_NOMINAL_XT1CLK_HZ;

  /* Convert a value in MHz to the same value in ticks of LXFT1 */
#define MHZ_TO_XT1(_n) (((_n)*1000000UL) / BSP430_CLOCK_NOMINAL_XT1CLK_HZ)

  /* Gross selection of DCO range.  We select the range if the target
   * frequency is above the midpoint of the previous range. */
  if (MHZ_TO_XT1(26/2) < dcoclk_xt1) {
    fn_x = FN_8;
  } else if (MHZ_TO_XT1(17/2) < dcoclk_xt1) {
    fn_x = FN_4;
  } else if (MHZ_TO_XT1(12/2) < dcoclk_xt1) {
    fn_x = FN_3;
  } else if (MHZ_TO_XT1(6/2) < dcoclk_xt1) {
    fn_x = FN_2;
  }

#undef MHZ_TO_XT1

  /* Need a divider if multiplier is too large. */
  while (FLL_N_MASK < (dcoclk_xt1 - 1)) {
    dcoplus = 1;
    ++flld;
    dcoclk_xt1 /= 2;
  }

  (void)iBSP430clockConfigureLFXT1_ni(1, -1);

  if (dcoplus) {
    FLL_CTL0 |= DCOPLUS;
  }
  SCFQCTL = dcoclk_xt1 - 1;
  SCFI0 = (flld * FLLD0) | fn_x;

  /* Clear all the oscillator faults and spin until DCO stabilized. */
  do {
    BSP430_CLOCK_CLEAR_FAULTS_NI();
    BSP430_CORE_WATCHDOG_CLEAR();
    /* Conservatively assume a 32 MHz clock */
    BSP430_CORE_DELAY_CYCLES(32 * BSP430_CLOCK_FAULT_RECHECK_DELAY_US);
  } while (BSP430_FLLPLUS_DCO_IS_FAULTED_NI());

  return ulBSP430clockMCLK_Hz_ni();
}

unsigned long
ulBSP430clockMCLK_Hz_ni (void)
{
  unsigned int mclk_xt1 = 1 + (SCFQCTL & FLL_N_MASK);
  if (FLL_CTL0 & DCOPLUS) {
    mclk_xt1 <<= (SCFI0 & (FLLD0 | FLLD1)) / FLLD0;
  }
  return mclk_xt1 * (unsigned long) BSP430_CLOCK_NOMINAL_XT1CLK_HZ;
}

unsigned long
ulBSP430clockSMCLK_Hz_ni (void)
{
  return ulBSP430clockMCLK_Hz_ni();
}

int
iBSP430clockSMCLKDividingShift_ni (void)
{
  /* Dividing shift is not configurable on this peripheral */
  return 0;
}

int
iBSP430clockConfigureSMCLKDividingShift_ni (int shift_pos)
{
#if defined(SELS)
  /* If supported, source SMCLK from same source as MCLK */
  if (FLL_CTL1 & SELM1) {
    FLL_CTL1 |= SELS;
  } else {
    FLL_CTL1 &= ~SELS;
  }
#endif /* SELS */
  /* Regardless, MCU doesn't support dividing SMCLK */
  return 0;
}

unsigned long
ulBSP430clockACLK_Hz_ni (void)
{
  unsigned long freq_hz;
  freq_hz = BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? BSP430_CLOCK_NOMINAL_VLOCLK_HZ : BSP430_CLOCK_NOMINAL_XT1CLK_HZ;
  return freq_hz >> ((FLL_CTL1 & DIVA_MASK) / FLL_DIV0);
}

int
iBSP430clockConfigureACLK_ni (eBSP430clockSource sel,
                              unsigned int dividing_shift)
{
  switch (sel) {
    default:
    case eBSP430clockSRC_NONE:
      return -1;
    case eBSP430clockSRC_XT1CLK:
      break;
    case eBSP430clockSRC_VLOCLK:
    case eBSP430clockSRC_REFOCLK:
    case eBSP430clockSRC_DCOCLK:
    case eBSP430clockSRC_DCOCLKDIV:
    case eBSP430clockSRC_XT2CLK:
      return -1;
    case eBSP430clockSRC_XT1CLK_FALLBACK:
    case eBSP430clockSRC_XT1CLK_OR_VLOCLK:
    case eBSP430clockSRC_XT1CLK_OR_REFOCLK:
      break;
  }
  FLL_CTL1 = (FLL_CTL1 & ~DIVA_MASK) | (DIVA_MASK & (dividing_shift * FLL_DIV0));
  return 0;
}

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

      FLL_CTL0 = (FLL_CTL0 & ~(OSCCAP0 | OSCCAP1 | XTS_FLL)) | BSP430_CLOCK_LFXT1_XCAP;
      do {
        BSP430_CLOCK_CLEAR_FAULTS_NI();
        loop_limit -= loop_delta;
        BSP430_CORE_WATCHDOG_CLEAR();
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
      } while (BSP430_FLLPLUS_LFXT1_IS_FAULTED_NI() && (0 != loop_limit));
      
      rc = ! BSP430_FLLPLUS_LFXT1_IS_FAULTED_NI();
    }
  }
  BSP430_CLOCK_OSC_CLEAR_FAULT_NI();
  if (! rc) {
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, 0);
    FLL_CTL0 &= ~(OSCCAP0 | OSCCAP1);
  }
  return rc;
}
