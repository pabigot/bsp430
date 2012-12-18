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

#include <bsp430/periph/cs.h>
#include <bsp430/platform.h>
#include <stdlib.h>

#ifdef __MSP430_HAS_CS_A__
/* Provide aliases for name change from CS peripheral */
#define XT1DRIVE_3 LFXTDRIVE_3
#define XT1BYPASS LFXTBYPASS
#define XT1OFF LFXTOFF
#define XT1OFFG LFXTOFFG
#define XT1DRIVE0 LFXTDRIVE0
#define XT1DRIVE1 LFXTDRIVE1
#define SELA__XT1CLK SELA__LFXTCLK
#define XTS 0
#endif /* __MSP430_HAS_CS_A__ */

#if defined(__MSP430_HAS_CS__)
#define DCOFSEL_MASK (DCOFSEL0 | DCOFSEL1)
#elif defined(__MSP430_HAS_CS_A__)
#define DCOFSEL_MASK (DCOFSEL0 | DCOFSEL1 | DCOFSEL2)
#else /* __MSP430_HAS_CS__ */
#error Unrecognized peripheral
#endif /* __MSP430_HAS_CS__*/

/* Mask for SELA bits in CSCTL2 */
#define SELA_MASK (SELA0 | SELA1 | SELA2)

/* Mask for SELS bits in CSCTL2 */
#define SELS_MASK (SELS0 | SELS1 | SELS2)

/* Mask for SELM bits in CSCTL2 */
#define SELM_MASK (SELM0 | SELM1 | SELM2)

/* Mask for DIVS bits in CSCTL3 */
#define DIVS_MASK (DIVS0 | DIVS1 | DIVS2)

/* Mask for DIVM bits in CSCTL3 */
#define DIVM_MASK (DIVM0 | DIVM1 | DIVM2)

static const int32_t supported_freq[] = {
#if defined(__MSP430_HAS_CS__)
  /*  0       DCORSEL */
  5330000UL, 16000000UL,        /* DCOFSEL_0 */
  6670000UL, 20000000UL,        /* DCOFSEL_1 */
  5330000UL, 16000000UL,        /* DCOFSEL_2 */
  8000000UL, 24000000UL         /* DCOFSEL_3 */
#elif defined(__MSP430_HAS_CS_A__)
  /*  0       DCORSEL */
  1000000UL, 1000000UL,        /* DCOFSEL_0 */
  2670000UL, 5330000UL,        /* DCOFSEL_1 */
  3330000UL, 6670000UL,        /* DCOFSEL_2 */
  4000000UL, 8000000UL,        /* DCOFSEL_3 */
  5330000UL, 16000000UL,       /* DCOFSEL_4 */
  6670000UL, 20000000UL,       /* DCOFSEL_5 */
  8000000UL, 24000000UL,       /* DCOFSEL_6 */
  8000000UL, 24000000UL,       /* DCOFSEL_7 */
#else /* __MSP430_HAS_CS__ */
#error Unrecognized peripheral
#endif /* __MSP430_HAS_CS__*/
};

static unsigned long
ulBSP430csDCOCLK_Hz_ni (void)
{
  unsigned int csctl1 = CSCTL1;

  return supported_freq[!!(csctl1 & DCORSEL) + 2 * (csctl1 & DCOFSEL_MASK) / DCOFSEL0];
}

unsigned long
ulBSP430clockMCLK_Hz_ni (void)
{
  unsigned int divm = (CSCTL3 & DIVM_MASK) / DIVM0;
  return ulBSP430csDCOCLK_Hz_ni() >> divm;
}

int
iBSP430clockSMCLKDividingShift_ni (void)
{
  int divs;
  /* Assume that the source for both MCLK and SMCLK is the same, but
   * account for a potential DIVM. */
  divs = (CSCTL3 & DIVS_MASK) / DIVS0;
  divs -= (CSCTL3 & DIVM_MASK) / DIVM0;
  return divs;
}

int
iBSP430clockConfigureSMCLKDividingShift_ni (int shift_pos)
{
  unsigned int selm;

  /* Set SELS to the same value as SELM. */
  selm = (CSCTL2 & SELM_MASK) / SELM0;

  /* Adjust the shift for any division happening at MCLK. */
  shift_pos += (CSCTL3 & DIVM_MASK) / DIVM0;

  CSCTL0_H = 0xA5;
  CSCTL2 = (CSCTL2 & ~SELS_MASK) | (selm * SELS0);
  CSCTL3 = (CSCTL3 & ~DIVS_MASK) | (DIVS_MASK & (shift_pos * DIVS0));
  CSCTL0_H = !0xA5;

  return iBSP430clockSMCLKDividingShift_ni();
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
  if (enablep && (0 != loop_limit)) {
    rc = iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, enablep);
    if (0 == rc) {
      int loop_delta = (0 < loop_limit) ? 1 : 0;

      /* Low frequency XT1 needed.  Spin at high drive to stability, then
       * drop back.  Preserve XT1 configuration. */
      CSCTL4 = (CSCTL4 | XT1DRIVE_3) & ~(XTS | XT1BYPASS | XT1OFF);
      do {
        CSCTL5 &= ~XT1OFFG;
        BSP430_CORE_WATCHDOG_CLEAR();
        loop_limit -= loop_delta;
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
      } while ((BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) && (0 != loop_limit));
      CSCTL4 = CSCTL4 & ~XT1DRIVE_3;
      rc = ! BSP430_CLOCK_LFXT1_IS_FAULTED_NI();
    }
  }
  if (! rc) {
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, 0);
    CSCTL4 |= XT1OFF;
    CSCTL4 &= ~(XT1DRIVE0 | XT1DRIVE1);
  }
  CSCTL0_H = !0xA5;
  BSP430_CLOCK_LFXT1_CLEAR_FAULT_NI();
  return rc;
}

unsigned int
uiBSP430clockACLK_Hz_ni (void)
{
  switch (CSCTL2 & SELA_MASK) {
    case SELA__XT1CLK: /* XT1CLK */
      if (! BSP430_CLOCK_LFXT1_IS_FAULTED_NI()) {
        return BSP430_CLOCK_NOMINAL_XT1CLK_HZ;
      }
      /*FALLTHRU*/
    case SELA__VLOCLK: /* VLOCLK */
    case SELA_2: /* Reserved */
      return BSP430_CLOCK_NOMINAL_VLOCLK_HZ;
#if defined(SELA__XT2CLK) && defined(BSP430_CLOCK_NOMINAL_XT2CLK_HZ)
    case SELA__XT2CLK: /* XT2CLK */
      return BSP430_CLOCK_NOMINAL_XT2CLK_HZ;
#endif /* XT2CLK supported */
    default:
    case SELA_3: /* DCOCLK */
      return ulBSP430csDCOCLK_Hz_ni();
  }
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
      sela = SELA__XT1CLK;
      break;
    case eBSP430clockSRC_VLOCLK:
      sela = SELA__VLOCLK;
      break;
    case eBSP430clockSRC_REFOCLK:
      return -1;
    case eBSP430clockSRC_DCOCLK:
#if defined(__MSP430_HAS_CS_A__)
      /* CS_A does not permit use of DCOCLK as source for ACLK */
      return -1;
#else /* __MSP430_HAS_CS_A__ */
      sela = SELA__DCOCLK;
      break;
#endif /* __MSP430_HAS_CS_A__ */
    case eBSP430clockSRC_DCOCLKDIV:
      return -1;
#if defined(SELA__XT2CLK) && defined(BSP430_CLOCK_NOMINAL_XT2CLK_HZ)
    case eBSP430clockSRC_XT2CLK:
      sela = SELA__XT2CLK;
      break;
#endif /* XT2CLK supported */
    case eBSP430clockSRC_XT1CLK_FALLBACK:
    case eBSP430clockSRC_XT1CLK_OR_VLOCLK:
      sela = BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? SELA__VLOCLK : SELA__XT1CLK;
      break;
    case eBSP430clockSRC_XT1CLK_OR_REFOCLK:
      return -1;
  }
  CSCTL0_H = 0xA5;
  CSCTL2 = (CSCTL2 & ~SELA_MASK) | sela;
  CSCTL0_H = !0xA5;
  return 0;
}
