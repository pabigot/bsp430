/** This file is in the public domain.
 *
 * This grotesque code exists primarily to serve as a "known good"
 * example of using various ADCs.  It also demonstrates how bad the
 * factory temperature calibrations really are.
 *
 * ADC10 is classic 1xx/2xx 10-bit ADC.
 *
 * ADC10_A is ADC10 on a 5xx/6xx MCU.  It has external channels on A8
 * and A9 instead of reference voltage inputs.
 *
 * ADC10_B is ADC10 on a FR57xx MCU.  It provides a facility to
 * measure VeREF+ and VeREF- on channels 8 and 9.
 *
 * ADC12 is classic 1xx/2xx/4xx 12-bit ADC.
 *
 * ADC12_A (identified as ADC12_PLUS) is ADC12 on a 5xx/6xx MCU.  It
 * adds REFOUT not available on ADC12.
 *
 * ADC12_B is ADC12 on a FR58xx MCU.  It supports up to 32 external
 * and 6 internal channels.
 *
 * REF is classic 5xx REF.
 *
 * REF_A is an FR58xx REF which adds status registers to indicate when
 * the reference is ready, and supports 1.2V instead of 1.5V.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/tlv.h>
#include <string.h>

#define HAVE_REF (defined(__MSP430_HAS_REF__)           \
                  || defined(__MSP430_HAS_REF_A__))

#define HAVE_ADC10 (defined(__MSP430_HAS_ADC10__)       \
                    || defined(__MSP430_HAS_ADC10_A__)  \
                    || defined(__MSP430_HAS_ADC10_B__))

#define HAVE_ADC12 (defined(__MSP430_HAS_ADC12__)               \
                    || defined(__MSP430_HAS_ADC12_PLUS__)       \
                    || defined(__MSP430_HAS_ADC12_B__))

#if HAVE_REF
const sBSP430tlvREFCAL * cal_ref;
#else
const void * cal_ref;
#endif /* HAVE_REF */
#if (BSP430_TLV_IS_5XX - 0)
typedef const sBSP430tlvADCCAL * cal_adc_ptr_type;
#else /* BSP430_TLV_IS_5XX */
typedef const sBSP430tlvADC * cal_adc_ptr_type;
#endif /* BSP430_TLV_IS_5XX */
cal_adc_ptr_type cal_adc;

#if HAVE_ADC10
#ifndef ADC10ENC
#define ADC10ENC ENC
#endif /* ADC10ENC */
#ifndef ADC10MEM0_
#define ADC10MEM0 ADC10MEM
#endif /* ADC10MEM0_ */
#ifdef ADC10INCH_10
#define INCH_TEMP ADC10INCH_10
#define INCH_VMID ADC10INCH_11
#else /* ADC10INCH_10 */
#define INCH_TEMP INCH_10
#define INCH_VMID INCH_11
#endif /* ADC10INCH_10 */
#elif HAVE_ADC12
#ifndef ADC12ENC
#define ADC12ENC ENC
#endif /* ADC12ENC */
#if defined(__MSP430_HAS_ADC12_B__)
#define INCH_TEMP ADC12INCH_30
#define INCH_VMID ADC12INCH_31
#else /* ADC12_B */
#ifdef ADC12INCH_10
#define INCH_TEMP ADC12INCH_10
#define INCH_VMID ADC12INCH_11
#else /* ADC12INCH_10 */
#define INCH_TEMP INCH_10
#define INCH_VMID INCH_11
#endif /* ADC12INCH_10 */
#endif /* ADC12_B */
#else /* HAVE_ADC* */
#error No ADC available */
#endif /* HAVE_ADC* */

typedef struct sSample {
  unsigned int raw;
  unsigned int adj;
  union {
    struct {
      int cCel;
      int cF;
    };
    int mV;
  };
} sSample;

#if defined(__MSP430_HAS_REF_A__)
#define REF_1pX 12
#define REF_1pX_SCALE 1200
#define REF_1pX_STR "1.2"
#else /* REF_A */
#define REF_1pX 15
#define REF_1pX_SCALE 1500
#define REF_1pX_STR "1.5"
#endif /* REF_A */
#define REF_2p0 20
#define REF_2p5 25

int initializeADC (void)
{
  cal_adc = NULL;
  if (BSP430_TLV_TABLE_IS_VALID()) {
    const sBSP430tlvEntry * ep = (const sBSP430tlvEntry *)TLV_START;
    const sBSP430tlvEntry * const epe = (const sBSP430tlvEntry *)(TLV_END+1);
    while (ep < epe) {
      if (BSP430_TLV_ENTRY_IS_ADC(ep)) {
        cal_adc = (cal_adc_ptr_type)ep;
      }
#if HAVE_REF
      if (TLV_REFCAL == ep->tag) {
        cal_ref = (const sBSP430tlvREFCAL *)ep;
      }
#endif /* HAVE_REF */
      ep = BSP430_TLV_NEXT_ENTRY(ep);
    }
  }

#if HAVE_REF
  while (REFCTL0 & REFGENBUSY) {
    ;
  }
  REFCTL0 = REFVSEL_1 | REFON
#ifdef REFMSTR
            /* REFMSTR is implicit (and not defined) in FR57xx/FR58xx
             * devices, required on ADC10_A devices, and optional for
             * ADC12_A (but if not selected you have to control the
             * reference voltage from the legacy ADC12 settings).  Use
             * it if you've got it. */
            | REFMSTR
#endif
            ;
#endif

#if defined(__MSP430_HAS_ADC10__)
  ADC10CTL0 = 0;
  ADC10CTL0 = SREF_1 | ADC10SHT_3 | REFON | ADC10ON;
  ADC10CTL1 = ADC10DIV_3;
#elif defined(__MSP430_HAS_ADC10_A__)
#error Not implemented
#elif defined(__MSP430_HAS_ADC10_B__)
  ADC10CTL0 = 0;
  ADC10CTL0 = ADC10SHT_3 | ADC10ON;
  ADC10CTL1 = ADC10SHS_0 | ADC10SHP | ADC10DIV_3 | ADC10SSEL_0;
  ADC10CTL2 = ADC10PDIV_0 | ADC10RES | ADC10SR;
#elif defined(__MSP430_HAS_ADC12__)
  /* Original ADC12 does not use namespaced prefixes */
  /* ~ADC12ENC: Place module into hold before modifying configuration */
  ADC12CTL0 &= ~ADC12ENC;
  /* ADC12SHT_10:  512 ADC12CLK cycles per sample
   * REFON: Enable internal reference
   * ADC12ON: Turn module on
   * Do not enable yet.
   */
  ADC12CTL0 = SHT0_10 | REFON | ADC12ON;
  /* Start collection with conversion register zero
   * ADC12SHS_0: Trigger on ADC12SC bit
   * ADC12SHP: Pulsed sampling
   * No sample-input signal inversion
   * Divide clock by 1
   * ADC12SSEL_0: Clock source is MODCLK (nominal 5MHz)
   * ADC12CONSEQ_0: Single-channel, single-conversion
   */
  ADC12CTL1 = SHS_0 | SHP | ADC12SSEL_0 | CONSEQ_0;

  /* Delay 17ms to allow REF to stabilize */
  {
    unsigned long wake_utt = ulBSP430uptime_ni() + BSP430_UPTIME_MS_TO_UTT(17);
    while (0 < lBSP430uptimeSleepUntil_ni(wake_utt, LPM0_bits)) {
      /* nop */
    }
  }
#elif defined(__MSP430_HAS_ADC12_PLUS__)
  /* ~ADC12ENC: Place module into hold before modifying configuration */
  ADC12CTL0 &= ~ADC12ENC;
  /* ADC12SHT_10:  512 ADC12CLK cycles per sample
   * ADC12ON: Turn module on
   * Do not enable yet.
   */
  ADC12CTL0 = ADC12SHT0_10 | ADC12ON;
  /* Start collection with conversion register zero
   * ADC12SHS_0: Trigger on ADC12SC bit
   * ADC12SHP: Pulsed sampling
   * No sample-input signal inversion
   * Divide clock by 1
   * ADC12SSEL_0: Clock source is MODCLK (nominal 5MHz)
   * ADC12CONSEQ_0: Single-channel, single-conversion
   */
  ADC12CTL1 = ADC12SHS_0 | ADC12SHP | ADC12SSEL_0 | ADC12CONSEQ_0;
  /* No predivider
   * ADC12RES: 12-bit resolution
   * Binary unsigned read-back
   * ADC12SR: Sampling rate limited to 50 ksps (reduce current) */
  ADC12CTL2 = ADC12RES_2 | ADC12SR;
#elif defined(__MSP430_HAS_ADC12_B__)
  /* ~ADC12ENC: Place module into hold before modifying configuration */
  ADC12CTL0 &= ~ADC12ENC;
  /* ADC12SHT_10:  512 ADC12CLK cycles per sample
   * ADC12ON: Turn module on
   * Do not enable yet.
   */
  ADC12CTL0 = ADC12SHT0_10 | ADC12ON;
  /* Start collection with conversion register zero
   * ADC12SHS_0: Trigger on ADC12SC bit
   * ADC12SHP: Pulsed sampling
   * No sample-input signal inversion
   * Divide clock by 1
   * ADC12SSEL_0: Clock source is MODCLK (nominal 5MHz)
   * ADC12CONSEQ_0: Single-channel, single-conversion
   */
  ADC12CTL1 = ADC12SHS_0 | ADC12SHP | ADC12SSEL_0 | ADC12CONSEQ_0;
  /* No predivider
   * ADC12RES: 12-bit resolution
   * Binary unsigned read-back */
  ADC12CTL2 = ADC12RES_2;
  /* Disable channel mapping.
   * Use internal temperature and voltage channels */
  ADC12CTL3 = ADC12TCMAP | ADC12BATMAP;

  /* Delay 75us to allow REF to stabilize */
  BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_US_TO_NOMINAL_MCLK(75));
#else
#error No ADC available
#endif /* ADC */
  return 0;
}

int setReferenceVoltage (int ref)
{
#if HAVE_REF
  REFCTL0 &= ~REFVSEL_3;
  if (REF_1pX == ref) {
    REFCTL0 |= REFVSEL_0;
  } else if (REF_2p0 == ref) {
    REFCTL0 |= REFVSEL_1;
  } else if (REF_2p5 == ref) {
    REFCTL0 |= REFVSEL_2;
  } else {
    return -1;
  }
  return 0;
#else /* HAVE_REF */
  if (REF_2p0 == ref) {
    return -1;
  }
#if HAVE_ADC10
  if (REF_1pX == ref) {
    ADC10CTL0 &= ~REF2_5V;
  } else if (REF_2p5 == ref) {
    ADC10CTL0 |= REF2_5V;
  } else {
    return -1;
  }
#elif HAVE_ADC12
  if (REF_1pX == ref) {
    ADC12CTL0 &= ~REF2_5V;
  } else if (REF_2p5 == ref) {
    ADC12CTL0 |= REF2_5V;
  } else {
    return -1;
  }
#else
#error No ADC available
#endif /* ADC */
#endif /* HAVE_REF */
  return 0;
}

int setSource (unsigned int inch)
{
#if defined(__MSP430_HAS_ADC10__)
  ADC10CTL1 = (ADC10CTL1 & 0x0FFF) | inch;
#elif defined(__MSP430_HAS_ADC10_A__)
#error Not implemented
#elif defined(__MSP430_HAS_ADC10_B__)
  ADC10MCTL0 = ADC10SREF_1 | (inch * ADC10INCH0);
#elif defined(__MSP430_HAS_ADC12__)
  ADC12MCTL0 = EOS | SREF_1 | (inch * INCH0);
#elif defined(__MSP430_HAS_ADC12_PLUS__)
  ADC12MCTL0 = ADC12EOS | ADC12SREF_1 | (inch * ADC12INCH0);
#elif defined(__MSP430_HAS_ADC12_B__)
  ADC12MCTL0 = ADC12EOS | ADC12VRSEL_1 | (inch * ADC12INCH0);
#else
#error No ADC available
#endif /* ADC */
  return 0;
}

int getSample (sSample * sp,
               int refv,
               unsigned int inch)
{
  unsigned int divisor;
  unsigned int vref_scale = 0;

  memset(sp, 0, sizeof(*sp));
  if (0 != setSource(inch)) {
    return -1;
  }
  __delay_cycles(1000);
#if HAVE_ADC10
  divisor = 1024;
  ADC10CTL0 |= ADC10ENC | ADC10SC;
  while (ADC10CTL1 & ADC10BUSY) {
    ;
  }
  sp->adj = sp->raw = ADC10MEM0;
  ADC10CTL0 &= ~ADC10ENC;
#elif HAVE_ADC12
  divisor = 4096;
  ADC12CTL0 |= ADC12ENC + ADC12SC;
  while (ADC12CTL1 & ADC12BUSY) {
    ;
  }
  sp->adj = sp->raw = ADC12MEM0;
  ADC12CTL0 &= ~ADC12ENC;
#endif

  if (REF_1pX == refv) {
    vref_scale = REF_1pX_SCALE;
#if HAVE_REF
  } else if (REF_2p0 == refv) {
    vref_scale = 2000;
#endif /* HAVE_REF */
  } else if (REF_2p5 == refv) {
    vref_scale = 2500;
  }

  if (cal_adc) {
    unsigned long r32 = sp->raw;
    unsigned int vref_factor;
    int t30;
    int t85;

    if (REF_1pX == refv) {
#if HAVE_REF
      vref_factor = cal_ref->cal_adc_15vref_factor;
#else /* HAVE_REF */
      vref_factor = cal_adc->cal_adc_15vref_factor;
#endif /* HAVE_REF */
      t30 = cal_adc->cal_adc_15t30;
      t85 = cal_adc->cal_adc_15t85;
#if HAVE_REF
    } else if (REF_2p0 == refv) {
      vref_factor = cal_ref->cal_adc_20vref_factor;
      t30 = cal_adc->cal_adc_20t30;
      t85 = cal_adc->cal_adc_20t85;
#endif /* HAVE_REF */
    } else if (REF_2p5 == refv) {
#if HAVE_REF
      vref_factor = cal_ref->cal_adc_25vref_factor;
#else /* HAVE_REF */
      vref_factor = cal_adc->cal_adc_25vref_factor;
#endif /* HAVE_REF */
      t30 = cal_adc->cal_adc_25t30;
      t85 = cal_adc->cal_adc_25t85;
    } else {
      return -1;
    }

    r32 = ((r32 << 1) * vref_factor) >> 16;
    sp->adj = ((r32 << 1) * cal_adc->cal_adc_gain_factor) >> 16;
    sp->adj += cal_adc->cal_adc_offset;
    if (INCH_TEMP == inch) {
#define ADC_TO_dC(_v) (3000 + (int)((85 - 30) * ((100L * ((int)(_v) - t30)) / (long)(t85 - t30))))
      sp->cCel = ADC_TO_dC(sp->raw);
#define dC_TO_dF(_v) (3200 + (9 * (_v)) / 5)
      sp->cF = dC_TO_dF(sp->cCel);
    }
  }
  if (INCH_TEMP == inch) {
  } else {
#define ADC_TO_mV(_v) ((unsigned int)((2 * (_v) * (long)vref_scale) / divisor))
    sp->mV = ADC_TO_mV(sp->adj);
  }

  return 0;
}

#if defined(__MSP430_HAS_ADC10__)
#elif defined(__MSP430_HAS_ADC10_A__)
#elif defined(__MSP430_HAS_ADC10_B__)
#elif defined(__MSP430_HAS_ADC12__)
#elif defined(__MSP430_HAS_ADC12_B__)
#elif defined(__MSP430_HAS_ADC12_PLUS__)
#else
#error No ADC available
#endif /* ADC */

#define VALID_T_1pX 0x01
#define VALID_T_2p0 0x02
#define VALID_T_2p5 0x04
#define VALID_V_1pX 0x10
#define VALID_V_2p0 0x20
#define VALID_V_2p5 0x40

void displayTemperature (const sSample * sp)
{
  cprintf(" T %u %u : %d cCel %d cF ;", sp->raw, sp->adj, sp->cCel, sp->cF);
}

void displayVoltage (const sSample * sp)
{
  cprintf(" V %u %u : %u mV ;", sp->raw, sp->adj, sp->mV);
}

void main ()
{
  int rc;
  unsigned long next_wake_utt;
  unsigned long delta_wake_utt;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\n\nadc demo, " __DATE__ " " __TIME__ "\n");

  delta_wake_utt = 10 * ulBSP430uptimeConversionFrequency_Hz_ni();

  rc = initializeADC();
  cprintf("%s initialized, returned %d, ADC cal at %p, REF cal at %p\n",
#if defined(__MSP430_HAS_ADC10__)
          "ADC10"
#elif defined(__MSP430_HAS_ADC10_A__)
          "ADC10_A"
#elif defined(__MSP430_HAS_ADC10_B__)
          "ADC10_B"
#elif defined(__MSP430_HAS_ADC12__)
          "ADC12"
#elif defined(__MSP430_HAS_ADC12_B__)
          "ADC12_B"
#elif defined(__MSP430_HAS_ADC12_PLUS__)
          "ADC12_PLUS"
#endif /* ADC */
          , rc, cal_adc, cal_ref);

#if HAVE_REF
  if (cal_ref) {
    cprintf("Reference factors:\n"
            "\t" REF_1pX_STR "V %u (0x%04x)\n"
            "\t2.0V %u (0x%04x)\n"
            "\t2.5V %u (0x%04x)\n",
            cal_ref->cal_adc_15vref_factor, cal_ref->cal_adc_15vref_factor,
            cal_ref->cal_adc_20vref_factor, cal_ref->cal_adc_20vref_factor,
            cal_ref->cal_adc_25vref_factor, cal_ref->cal_adc_25vref_factor);
  }
#endif /* HAVE_REF */
  if (cal_adc) {
    cprintf("ADC gain factor %d (0x%04x), offset %d\n",
            cal_adc->cal_adc_gain_factor, cal_adc->cal_adc_gain_factor,
            cal_adc->cal_adc_offset);
    cprintf("Temperature ranges:\n");
    cprintf("\t" REF_1pX_STR "V T30 %u T85 %u\n", cal_adc->cal_adc_15t30, cal_adc->cal_adc_15t85);
#if BSP430_TLV_IS_5XX
    cprintf("\t2.0V T30 %u T85 %u\n", cal_adc->cal_adc_20t30, cal_adc->cal_adc_20t85);
#endif /* BSP430_TLV_IS_5XX */
    cprintf("\t2.5V T30 %u T85 %u\n", cal_adc->cal_adc_25t30, cal_adc->cal_adc_25t85);
  }

  cprintf("Vmid channel %u, Temp channel %u\n",
          INCH_VMID, INCH_TEMP);

  next_wake_utt = ulBSP430uptime_ni();
  while (1) {
    char timestamp[BSP430_UPTIME_AS_TEXT_LENGTH];
    int valid = 0;
    sSample t1X;
    sSample t20;
    sSample t25;
    sSample v1X;
    sSample v20;
    sSample v25;

    if (0 == setReferenceVoltage(REF_1pX)) {
      if (0 == getSample(&t1X, REF_1pX, INCH_TEMP)) {
        valid |= VALID_T_1pX;
      }
      if (0 == getSample(&v1X, REF_1pX, INCH_VMID)) {
        valid |= VALID_V_1pX;
      }
    }
    if (0 == setReferenceVoltage(REF_2p0)) {
      if (0 == getSample(&t20, REF_2p0, INCH_TEMP)) {
        valid |= VALID_T_2p0;
      }
      if (0 == getSample(&v20, REF_2p0, INCH_VMID)) {
        valid |= VALID_V_2p0;
      }
    }
    if (0 == setReferenceVoltage(REF_2p5)) {
      if (0 == getSample(&t25, REF_2p5, INCH_TEMP)) {
        valid |= VALID_T_2p5;
      }
      if (0 == getSample(&v25, REF_2p5, INCH_VMID)) {
        valid |= VALID_V_2p5;
      }
    }
    cprintf("%s: valid %x", xBSP430uptimeAsText(ulBSP430uptime_ni(), timestamp), valid);
    if (valid & (VALID_T_1pX | VALID_V_1pX)) {
      cprintf("\n\t" REF_1pX_STR "V: ");
      if (valid & VALID_T_1pX) {
        displayTemperature(&t1X);
      }
      if (valid & VALID_V_1pX) {
        displayVoltage(&v1X);
      }
    }
    if (valid & (VALID_T_2p0 | VALID_V_2p0)) {
      cprintf("\n\t2.0V: ");
      if (valid & VALID_T_2p0) {
        displayTemperature(&t20);
      }
      if (valid & VALID_V_2p0) {
        displayVoltage(&v20);
      }
    }
    if (valid & (VALID_T_2p5 | VALID_V_2p5)) {
      cprintf("\n\t2.5V: ");
      if (valid & VALID_T_2p5) {
        displayTemperature(&t25);
      }
      if (valid & VALID_V_2p5) {
        displayVoltage(&v25);
      }
    }
    cputchar('\n');
    next_wake_utt += delta_wake_utt;
    while (0 < lBSP430uptimeSleepUntil_ni(next_wake_utt, LPM3_bits)) {
      /* nop */
    }
  }
}
