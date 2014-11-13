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
#include <bsp430/periph/cs4.h>
#include <stdint.h>
#include <bsp430/utility/led.h>

/* Mask for SELMS bits in CSCTL4 */
#define SELMS_MASK (SELMS0 | SELMS1 | SELMS2)

/* Mask for DIVA bits in CSCTL6.  There are potentially four bits, but
 * if the device does not support a high-frequency XT1 the mask is
 * zero. */
#if defined(DIVA0)
#define DIVA_MASK (DIVA0 | DIVA1 | DIVA2 | DIVA3)
#else /* DIVA0 */
#define DIVA0 1
#define DIVA_MASK (0)
#endif /* DIVA0 */

/* Mask for DIVS bits in CSCTL5 */
#define DIVS_MASK (DIVS0 | DIVS1)

/* Mask for DIVM bits in CSCTL5 */
#define DIVM_MASK (DIVM0 | DIVM1 | DIVM2)

/* Mask for SELREF bits in CSCTL3 */
#define SELREF_MASK (SELREF0 | SELREF1)

/* Mask for FLLREFDIV bits in CSCTL3 */
#define FLLREFDIV_MASK (FLLREFDIV0 | FLLREFDIV1 | FLLREFDIV2)

/* Mask for DCORSEL bits in CSCTL1 */
#define DCORSEL_MASK (DCORSEL0 | DCORSEL1 | DCORSEL2)

/* Basic clock sources.  These values may be used in CSCTL4. */
#define SMCSEL_DCOCLKDIV 0
#define SMCSEL_REFOCLK 1
#define SMCSEL_XT1CLK 2
#define SMCSEL_VLOCLK 3

/* Mask for legal CSEL value range */
#define SMCSEL_MASK 0x07

/* Mask for legal FLLD range */
#define FLLD_MASK (FLLD0 | FLLD1 | FLLD2)

/* Mask for legal FLLN range */
#define FLLN_MASK 0x3FF

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
      CSCTL6 = XT1DRIVE_3 | (CSCTL6 & ~(XTS | XT1BYPASS | XT1AGCOFF | XT1AUTOOFF));
      do {
        BSP430_CLOCK_CLEAR_FAULTS_NI();
        loop_limit -= loop_delta;
        BSP430_CORE_WATCHDOG_CLEAR();
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
      } while ((BSP430_CS4_LFXT1_IS_FAULTED_NI()) && (0 != loop_limit));
      rc = ! BSP430_CS4_LFXT1_IS_FAULTED_NI();
    }
  }

  BSP430_CLOCK_OSC_CLEAR_FAULT_NI();
  CSCTL6 &= ~XT1DRIVE_3;
  if (! rc) {
    (void)iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_LFXT1, 0, 0);
    /* Disable as an indication that XIN is not enabled.  Also remove
     * capacitance setting. */
    CSCTL6 = XT1BYPASS | (CSCTL6 & ~(XT1DRIVE0 | XT1DRIVE1));
  }
  return rc;
}

static const eBSP430clockSource cselMap_[8] = {
  eBSP430clockSRC_DCOCLKDIV,
  eBSP430clockSRC_REFOCLK,
  eBSP430clockSRC_XT1CLK,
  eBSP430clockSRC_VLOCLK,
  eBSP430clockSRC_UNKNOWN_4,
  eBSP430clockSRC_UNKNOWN_5,
  eBSP430clockSRC_UNKNOWN_6,
  eBSP430clockSRC_UNKNOWN_7,
};

eBSP430clockSource xBSP430clockACLKSource ()
{
  return (SELA & CSCTL4) ? eBSP430clockSRC_XT1CLK : eBSP430clockSRC_REFOCLK;
}

eBSP430clockSource xBSP430clockSMCLKSource ()
{
  return cselMap_[(CSCTL4 & SELMS_MASK) / SELMS0];
}

eBSP430clockSource xBSP430clockMCLKSource ()
{
  return cselMap_[(CSCTL4 & SELMS_MASK) / SELMS0];
}

static int
sourceToSMCSEL_ (eBSP430clockSource sel)
{
  switch (sel) {
    default:
    case eBSP430clockSRC_NONE:
      return -1;
    case eBSP430clockSRC_XT1CLK:
      return SMCSEL_XT1CLK;
    case eBSP430clockSRC_VLOCLK:
      return SMCSEL_VLOCLK;
    case eBSP430clockSRC_REFOCLK:
      return SMCSEL_REFOCLK;
    case eBSP430clockSRC_SMCLK_PU_DEFAULT:
    case eBSP430clockSRC_DCOCLKDIV:
      return SMCSEL_DCOCLKDIV;
    case eBSP430clockSRC_XT1CLK_OR_VLOCLK:
      return BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? SMCSEL_VLOCLK : SMCSEL_XT1CLK;
    case eBSP430clockSRC_XT1CLK_FALLBACK:
    case eBSP430clockSRC_XT1CLK_OR_REFOCLK:
      return BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? SMCSEL_REFOCLK : SMCSEL_XT1CLK;
  }
  return -1;
}

static unsigned long
fllRefFrequency_Hz ()
{
  switch (CSCTL3 & SELREF_MASK) {
    default:
    case SELREF__REFOCLK:
      return BSP430_CS4_NOMINAL_REFOCLK_HZ;
      break;
    case SELREF__XT1CLK:
      return BSP430_CS4_NOMINAL_XT1CLK_HZ;
      break;
  }
}

static unsigned long
fllFrequency_Hz ()
{
  unsigned long ref_Hz = fllRefFrequency_Hz();
  unsigned int refdiv = (CSCTL3 & FLLREFDIV_MASK) / FLLREFDIV0;
  if (0 != refdiv) {
    /* 1 -> /32, 2 -> /64, etc. */
    ref_Hz >>= (4 + refdiv);
  }
  return ref_Hz * (1 + (CSCTL2 & FLLN_MASK));
}

unsigned long
ulBSP430clockMCLK_Hz_ni (void)
{
  return fllFrequency_Hz() >> ((CSCTL5 & DIVM_MASK) / DIVM0);
}

unsigned long
ulBSP430clockSMCLK_Hz_ni (void)
{
  return fllFrequency_Hz() >> ((CSCTL5 & DIVS_MASK) / DIVS0);
}

unsigned long
ulBSP430clockACLK_Hz_ni (void)
{
  unsigned long src_Hz = (SELA & CSCTL4) ? BSP430_CS4_NOMINAL_REFOCLK_HZ : BSP430_CS4_NOMINAL_XT1CLK_HZ;
  return src_Hz >> (CSCTL6 & DIVA_MASK) / DIVA0;
}

int
iBSP430clockConfigureACLK_ni (eBSP430clockSource sel,
                              unsigned int dividing_shift)
{
  int csel_bits;
  unsigned int dividing_shift_bits = dividing_shift * DIVA0;

  if (eBSP430clockSRC_XT1CLK == sel) {
    csel_bits = 0;
  } else if (eBSP430clockSRC_REFOCLK == sel) {
    csel_bits = SELA;
  } else {
    return -1;
  }
  if (DIVA_MASK < dividing_shift_bits) {
    dividing_shift_bits = DIVA_MASK;
  }
  CSCTL5 = (CSCTL5 & ~DIVA_MASK) | dividing_shift_bits;
  CSCTL4 = (CSCTL4 & ~SELA) | csel_bits;
  return 0;
}

int
iBSP430clockConfigureSMCLK_ni (eBSP430clockSource sel,
                               unsigned int dividing_shift)
{
  int csel = sourceToSMCSEL_(sel);
  unsigned int dividing_shift_bits = dividing_shift * DIVS0;
  if (0 > csel) {
    return -1;
  }
  if (DIVS_MASK < dividing_shift_bits) {
    dividing_shift_bits = DIVS_MASK;
  }
  CSCTL5 = (CSCTL5 & ~DIVS_MASK) | dividing_shift_bits;
  CSCTL4 = (CSCTL4 & ~SELMS_MASK) | (csel * SELMS0);
  return 0;
}

unsigned long
ulBSP430clockConfigureMCLK_ni (unsigned long mclk_Hz)
{
  unsigned int scg0;
  unsigned int dcorsel = 0;
  const unsigned long ref_Hz =
#if ((BSP430_CLOCK_NOMINAL_XT1CLK_HZ != BSP430_CLOCK_NOMINAL_REFOCLK_HZ) || (0 != DIVA_MASK))
    fllRefFrequency_Hz()
#else /* XT1CLK ~ REFOCLK */
    /* Avoid a bunch of runtime math by assuming that XT1 and REFO both
     * run at the same low frequency, nominally 32 kiHz. */
    BSP430_CLOCK_NOMINAL_XT1CLK_HZ
#endif /* XT1CLK ~ REFOCLK */
    ;
  unsigned long dcoclk_ref;
  unsigned long flln;
  unsigned int flld;

  if (ref_Hz >= mclk_Hz) {
    mclk_Hz = BSP430_CLOCK_PUC_MCLK_HZ;
  }

  /* Calculate the desired multiplier of the reference clock, then
   * calculate the corresponding DCOCLK assuming the default
   * divider. */
  flln = (mclk_Hz - ref_Hz) / ref_Hz;
  flld = 1;
  dcoclk_ref = flln << flld;

  /* Gross selection of DCO range.  Note that FUNLOCK will not clear
   * if the wrong range is selected; setting to 1 MiHz DCODIV provides
   * >2 MHz DCO but DCORSEL_2 (4 MHz) doesn't stabilize.  This appears
   * to be mostly a problem with selecting a higher-rate DCO, so shift
   * the upper bound 6.25% above the theoretical limit. */
#define MHZ_TO_REF(_n) (((17*(_n)*1000000UL) / (16*ref_Hz)) - flld)
  if (MHZ_TO_REF(1) >= dcoclk_ref) {
    dcorsel = DCORSEL_0; /* 1 MHz */
  } else if (MHZ_TO_REF(2) >= dcoclk_ref) {
    dcorsel = DCORSEL_1; /* 2 MHz */
  } else if (MHZ_TO_REF(4) >= dcoclk_ref) {
    dcorsel = DCORSEL_2; /* 4 MHz */
  } else if (MHZ_TO_REF(8) >= dcoclk_ref) {
    dcorsel = DCORSEL_3; /* 8 MHz */
  } else if (MHZ_TO_REF(12) >= dcoclk_ref) {
    dcorsel = DCORSEL_4; /* 12 MHz */
  } else {
    dcorsel = DCORSEL_5; /* 16 MHz */
  }
#undef MHZ_TO_REF

  iBSP430clockConfigureLFXT1_ni(1, -1);

  /* Preserve the incoming value of the SCG0 flag, and disable FLL */
  scg0 = SCG0 & __read_status_register();
  __bis_status_register(SCG0);

  CSCTL0 = 0;
  CSCTL1 = dcorsel | (CSCTL1 & ~(DISMOD | DCORSEL_MASK));
  CSCTL2 = (flld * FLLD0) | (FLLN_MASK & flln);
  CSCTL0 = 0;
  __nop();
  __nop();
  /* Enable FLL */
  __bic_status_register(SCG0);

  /* Clear all the oscillator faults and spin until DCO stabilized. */
  do {
    BSP430_CLOCK_CLEAR_FAULTS_NI();
    BSP430_CORE_WATCHDOG_CLEAR();
    /* Conservatively assume a 32 MHz clock */
    BSP430_CORE_DELAY_CYCLES(32 * BSP430_CLOCK_FAULT_RECHECK_DELAY_US);
  } while (BSP430_CS4_DCO_IS_FAULTED_NI());

  /* Restore SCG0 (if was set on entry, set it now) */
  __bis_status_register(scg0);

  return ulBSP430clockMCLK_Hz_ni();
}
