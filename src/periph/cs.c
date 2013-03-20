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

#include <bsp430/periph/cs.h>
#include <bsp430/platform.h>
#include <stdlib.h>

#if (BSP430_CS_IS_FR58XX - 0)
/* Provide aliases for name change from CS peripheral */
#define XT1DRIVE_3 LFXTDRIVE_3
#define XT1BYPASS LFXTBYPASS
#define XT1OFF LFXTOFF
#define XT1OFFG LFXTOFFG
#define XT1DRIVE0 LFXTDRIVE0
#define XT1DRIVE1 LFXTDRIVE1
#define SELA__XT1CLK SELA__LFXTCLK
#define XTS 0
#endif /* BSP430_CS_IS_FR58XX */

/* Basic clock sources on both CS and CS_A.  These values may be used
 * within CS registers. */
#define CSEL_XT1CLK 0
#define CSEL_VLOCLK 1
#define CSEL_DCOCLK 3
#define CSEL_XT2CLK 5

/* Mask for legal CSEL value range */
#define CSEL_MASK 0x07

#if (BSP430_CS_IS_FR57XX - 0)
#define DCOFSEL_MASK (DCOFSEL0 | DCOFSEL1)

#elif (BSP430_CS_IS_FR58XX - 0)
#define DCOFSEL_MASK (DCOFSEL0 | DCOFSEL1 | DCOFSEL2)

/* Additional clock sources on CS_A.  These values may be used within
 * CS registers. */
#define CSEL_LFXTCLK CSEL_XT1CLK
#define CSEL_LFMODCLK 2
#define CSEL_MODCLK 4
#define CSEL_HFXTCLK CSEL_XT2CLK

#else /* BSP430_CS_IS_FR57XX */
#error Unrecognized peripheral
#endif /* BSP430_CS_IS_FR57XX */

/* Mask for SELA bits in CSCTL2 */
#define SELA_MASK (SELA0 | SELA1 | SELA2)

/* Mask for SELS bits in CSCTL2 */
#define SELS_MASK (SELS0 | SELS1 | SELS2)

/* Mask for SELM bits in CSCTL2 */
#define SELM_MASK (SELM0 | SELM1 | SELM2)

/* Mask for DIVA bits in CSCTL3 */
#define DIVA_MASK (DIVA0 | DIVA1 | DIVA2)

/* Mask for DIVS bits in CSCTL3 */
#define DIVS_MASK (DIVS0 | DIVS1 | DIVS2)

/* Mask for DIVM bits in CSCTL3 */
#define DIVM_MASK (DIVM0 | DIVM1 | DIVM2)

/* Maximum respected value for dividing shifts */
#define DIV_MAX_VALUE 5

static const int32_t supported_freq[] = {
#if (BSP430_CS_IS_FR57XX - 0)
  /*  0       DCORSEL */
  5330000UL, 16000000UL,        /* DCOFSEL_0 */
  6670000UL, 20000000UL,        /* DCOFSEL_1 */
  5330000UL, 16000000UL,        /* DCOFSEL_2 */
  8000000UL, 24000000UL         /* DCOFSEL_3 */
#elif (BSP430_CS_IS_FR58XX - 0)
  /*  0       DCORSEL */
  1000000UL, 1000000UL,        /* DCOFSEL_0 */
  2670000UL, 5330000UL,        /* DCOFSEL_1 */
  3330000UL, 6670000UL,        /* DCOFSEL_2 */
  4000000UL, 8000000UL,        /* DCOFSEL_3 */
  5330000UL, 16000000UL,       /* DCOFSEL_4 */
  6670000UL, 20000000UL,       /* DCOFSEL_5 */
  8000000UL, 24000000UL,       /* DCOFSEL_6 */
  8000000UL, 24000000UL,       /* DCOFSEL_7 */
#else /* BSP430_CS_IS_FR57XX */
#error Unrecognized peripheral
#endif /* BSP430_CS_IS_FR57XX*/
};

static unsigned long
ulBSP430csDCOCLK_Hz_ni (void)
{
  unsigned int csctl1 = CSCTL1;

  return supported_freq[!!(csctl1 & DCORSEL) + 2 * (csctl1 & DCOFSEL_MASK) / DCOFSEL0];
}

unsigned long
ulBSP430clockConfigureMCLK_ni (unsigned long mclk_Hz)
{
  unsigned int csctl1;
  int ci;
  int best_ci;
  unsigned long delta;
  unsigned long best_delta;

  if (0 == mclk_Hz) {
    mclk_Hz = BSP430_CLOCK_PUC_MCLK_HZ;
  }
  best_ci = ci = 0;
  best_delta = labs(supported_freq[best_ci] - (int32_t)mclk_Hz);
  for (ci = 1; ci < sizeof(supported_freq)/sizeof(supported_freq[0]); ++ci) {
    delta = labs(supported_freq[ci] - (int32_t)mclk_Hz);
    if (delta < best_delta) {
      best_delta = delta;
      best_ci = ci;
    }
  }
  csctl1 = (best_ci & 1) ? DCORSEL : 0;
  csctl1 |= (best_ci / 2) * DCOFSEL0;
  CSCTL0_H = 0xA5;
  CSCTL1 = csctl1;
  CSCTL2 = (CSCTL2 & (SELA_MASK | SELS_MASK)) | SELM__DCOCLK;
  CSCTL3 &= ~DIVM_MASK;

  /* The CS peripheral does not support a DCO fault signal */

  CSCTL0_H = !0xA5;

  return ulBSP430clockMCLK_Hz_ni();
}

int
iBSP430clockConfigureLFXT1_ni (int enablep,
                               int loop_limit)
{
  int rc = 0;

  CSCTL0_H = 0xA5;
  BSP430_CS_CLEAR_FAULTS_NI();
  if (enablep && (0 != loop_limit)) {
    rc = iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, enablep);
    if (0 == rc) {
      int loop_delta = (0 < loop_limit) ? 1 : 0;

      /* Low frequency XT1 needed.  Spin at high drive to stability, then
       * drop back. */
      CSCTL4 = (CSCTL4 | XT1DRIVE_3) & ~(BSP430_CS_XTS_ | BSP430_CS_XT1BYPASS_ | BSP430_CS_XT1OFF_);
      do {
        BSP430_CS_CLEAR_FAULTS_NI();
        loop_limit -= loop_delta;
        BSP430_CORE_WATCHDOG_CLEAR();
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
      } while ((BSP430_CS_LFXT1_IS_FAULTED_NI()) && (0 != loop_limit));
      rc = ! BSP430_CS_LFXT1_IS_FAULTED_NI();
    }
  }
  BSP430_CLOCK_OSC_CLEAR_FAULT_NI();
  CSCTL4 &= ~XT1DRIVE_3;
  if (! rc) {
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, 0);
    /* Set this as an indication that the pin configuration is off */
    CSCTL4 |= BSP430_CS_XT1OFF_;
  }
  CSCTL0_H = !0xA5;
  return rc;
}

static const eBSP430clockSource cselMap_[8] = {
  eBSP430clockSRC_XT1CLK,
  eBSP430clockSRC_VLOCLK,
  eBSP430clockSRC_LFMODCLK,
  eBSP430clockSRC_DCOCLK,
  eBSP430clockSRC_MODCLK,
  eBSP430clockSRC_HFXTCLK,
  eBSP430clockSRC_UNKNOWN_6,
  eBSP430clockSRC_UNKNOWN_7,
};

eBSP430clockSource xBSP430clockACLKSource ()
{
  return cselMap_[(CSCTL2 & SELA_MASK) / SELA0];
}

eBSP430clockSource xBSP430clockSMCLKSource ()
{
  return cselMap_[(CSCTL2 & SELS_MASK) / SELS0];
}

eBSP430clockSource xBSP430clockMCLKSource ()
{
  return cselMap_[(CSCTL2 & SELM_MASK) / SELM0];
}

static unsigned long
cselToFreq_Hz_ (int csel)
{
  switch (csel & CSEL_MASK) {
#if (BSP430_CS_IS_FR57XX - 0)
    case 0: /* XT1CLK */
      if (! BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) {
        return BSP430_CLOCK_NOMINAL_XT1CLK_HZ;
      }
      /*FALLTHRU*/
    case 1: /* VLOCLK */
    case 2: /* Reserved, VLOCLK */
      return BSP430_CLOCK_NOMINAL_VLOCLK_HZ;
    case 5: /* XT2CLK, or DCOCLK */
    case 6: /* Reserved, XT2CLK or DCOCLK*/
    case 7: /* Reserved, XT2CLK or DCOCLK*/
#if defined(SELA__XT2CLK) && (configBSP430_PERIPH_XT2 - 0)
      return BSP430_CLOCK_NOMINAL_XT2CLK_HZ;
#endif /* XT2CLK supported */
    case 3: /* DCOCLK */
    case 4: /* Reserved, DCOCLK */
      return ulBSP430csDCOCLK_Hz_ni();
#elif (BSP430_CS_IS_FR58XX - 0)
    case 0: /* XT1CLK */
      if (! BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) {
        return BSP430_CLOCK_NOMINAL_XT1CLK_HZ;
      }
      /*FALLTHRU*/
    case 1: /* VLOCLK */
      return BSP430_CLOCK_NOMINAL_VLOCLK_HZ;
    case 2: /* LFMODCLK */
      return BSP430_CS_NOMINAL_LFMODCLK_HZ;
    case 4: /* MODCLK */
      return BSP430_CS_NOMINAL_MODCLK_HZ;
    case 5: /* HFXTCLK, or DCOCLK */
    case 6: /* Reserved, HFXTCLK or DCOCLK*/
    case 7: /* Reserved, HFXTCLK or DCOCLK*/
#if defined(SELS__HFXTCLK) && (configBSP430_PERIPH_XT2 - 0)
      return BSP430_CLOCK_NOMINAL_XT2CLK_HZ;
#endif /* HFXTCLK supported */
    case 3: /* DCOCLK */
      return ulBSP430csDCOCLK_Hz_ni();
#endif /* CS variant */
  }
  return 0;
}

static int
sourceToCSEL_ (eBSP430clockSource sel)
{
  switch (sel) {
    default:
    case eBSP430clockSRC_NONE:
    case eBSP430clockSRC_REFOCLK:
    case eBSP430clockSRC_DCOCLKDIV:
      return -1;
    case eBSP430clockSRC_XT1CLK:
      return CSEL_XT1CLK;
    case eBSP430clockSRC_VLOCLK:
      return CSEL_VLOCLK;
    case eBSP430clockSRC_SMCLK_PU_DEFAULT:
    case eBSP430clockSRC_DCOCLK:
      return CSEL_DCOCLK;
#if (defined(SELS__XT2CLK) || defined(SELS__HFXTCLK)) && (configBSP430_PERIPH_XT2 - 0)
    case eBSP430clockSRC_XT2CLK:
      return CSEL_XT2CLK;
#endif /* XT2CLK supported */
#if (BSP430_CS_IS_FR58XX - 0)
    case eBSP430clockSRC_MODCLK:
      return CSEL_MODCLK;
    case eBSP430clockSRC_LFMODCLK:
      return CSEL_LFMODCLK;
#endif /* BSP430_CS_IS_FR58XX */
    case eBSP430clockSRC_XT1CLK_FALLBACK:
    case eBSP430clockSRC_XT1CLK_OR_VLOCLK:
    case eBSP430clockSRC_XT1CLK_OR_REFOCLK:
      if (! BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) {
        return CSEL_XT1CLK;
      }
      if (eBSP430clockSRC_XT1CLK_OR_REFOCLK == sel) {
        return -1;
      }
      return CSEL_VLOCLK;
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
  return divideFrequency_(cselToFreq_Hz_((CSCTL2 & SELM_MASK) / SELM0),
                          (CSCTL3 & DIVM_MASK) / DIVM0);
}

unsigned long
ulBSP430clockSMCLK_Hz_ni (void)
{
  return divideFrequency_(cselToFreq_Hz_((CSCTL2 & SELS_MASK) / SELS0),
                          (CSCTL3 & DIVS_MASK) / DIVS0);
}

unsigned long
ulBSP430clockACLK_Hz_ni (void)
{
  return divideFrequency_(cselToFreq_Hz_((CSCTL2 & SELA_MASK) / SELA0),
                          (CSCTL3 & DIVA_MASK) / DIVA0);
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
#if (BSP430_CS_IS_FR58XX - 0)
  if (csel >= CSEL_DCOCLK) {
    /* CS_A does not permit use of DCOCLK as source for ACLK.
     * All values above LFMODCLK default to LFMODCLK*/
    csel = CSEL_LFMODCLK;
  }
#endif /* BSP430_CS_IS_FR58XX */
  CSCTL0_H = 0xA5;
  CSCTL3 = (CSCTL3 & ~DIVA_MASK) | (DIVA_MASK & (dividing_shift * DIVA0));
  CSCTL2 = (CSCTL2 & ~SELA_MASK) | (csel * SELA0);
  CSCTL0_H = !0xA5;
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
  CSCTL0_H = 0xA5;
  CSCTL3 = (CSCTL3 & ~DIVS_MASK) | (DIVS_MASK & (dividing_shift * DIVS0));
  CSCTL2 = (CSCTL2 & ~SELS_MASK) | (csel * SELS0);
  CSCTL0_H = !0xA5;
  return 0;
}

