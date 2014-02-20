/** This file is in the public domain.
 *
 * This demonstrates use of the ADXL335 three-axis analog
 * accelerometer on the EXP430FR5739.  The output is a continuous
 * stream showing the acceleration in the X, Y, and Z axes in
 * milli-gravities.
 *
 * The device is powered through P2.7.  The X, Y, and Z axes are
 * inputs to A12, A13, and A14, respectively, on the ADC10_B
 * peripheral.
 *
 * The ADXL335 is specified with a nominal voltage of 3.0V, which
 * produces an output voltage of 1.5V at 0g.  Since power is supplied
 * by P2.7, the actual voltage may be higher, which results in
 * correspondingly higher measurement voltages.  At 3.6V the output is
 * 1.8V at 0g.  The data sheet confirms that Vs/2 is nominally 0 g,
 * and suggests an approximation of Vs/10 /g sensitivity (slightly
 * less at 2V).  As such it's necessary to also measure what the MCU
 * is providing to the device, so estimates of acceleration are
 * accurate.
 *
 * ADC10 resolution at REF 2.5V is 2.4 mV, which corresponds to about
 * 7 milli-gravities at 3.6V Vcc.  Note that variation in the measured
 * Vcc also contributes to sampling error, so if you're aiming to
 * detect changes it's best to keep the baseline Vcc the same for the
 * samples.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <string.h>
#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/tlv.h>

#if !defined(__MSP430_HAS_ADC10_B__)
#error Requires ADC10_B
#endif
#if !defined(__MSP430_HAS_REF__)
#error Requires REF
#endif
#if ! (BSP430_TLV_IS_5XX - 0)
#error Requires 5xx TLV
#endif

const sBSP430tlvREFCAL * cal_ref;
typedef const sBSP430tlvADCCAL * cal_adc_ptr_type;
cal_adc_ptr_type cal_adc;

/** Select 2.5V reference voltage: 0g is 1.8V on input with 3.6V
 * Vcc. */
#define APP_REFVSEL REFVSEL_2
/** Reference voltage corresponding to #APP_REFVSEL, in millivolts */
#define APP_REFV_mV 2500

typedef struct sSample {
  unsigned int raw;             /** Raw ADC value */
  unsigned int adj;             /** ADC adjusted for calibration */
  unsigned int mV;              /** ADC in millivolts */
  int mg;                       /** Acceleration in milli-gravities */
} sSample;

static void
initializeADC (void)
{
  cal_ref = NULL;
  cal_adc = NULL;
  if (BSP430_TLV_TABLE_IS_VALID()) {
    const sBSP430tlvEntry * ep = (const sBSP430tlvEntry *)TLV_START;
    const sBSP430tlvEntry * const epe = (const sBSP430tlvEntry *)(TLV_END+1);
    while (ep < epe) {
      if (BSP430_TLV_ENTRY_IS_ADC(ep)) {
        cal_adc = (cal_adc_ptr_type)ep;
      }
      if (TLV_REFCAL == ep->tag) {
        cal_ref = (const sBSP430tlvREFCAL *)ep;
      }
      ep = BSP430_TLV_NEXT_ENTRY(ep);
    }
  }

  /* Just in case... */
  while (REFCTL0 & REFGENBUSY) {
    ;
  }

  /* Application-defined reference, stay on for ADC10 stability,
   * disable temperature sensor.  Then delay for the max 30uS settling
   * time. */
  REFCTL0 = APP_REFVSEL | REFON | REFTCOFF;
  BSP430_CORE_DELAY_CYCLES((30 * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000UL);

  ADC10CTL0 = 0;
  ADC10CTL0 = ADC10SHT_3 | ADC10ON;
  ADC10CTL1 = ADC10SHS_0 | ADC10SHP | ADC10DIV_3 | ADC10SSEL_0;
  /* ADC10SR: sample rate below 50 kS/s */
  ADC10CTL2 = ADC10PDIV_0 | ADC10RES | ADC10SR;
}

int getSample (sSample * sp,
               unsigned int inch)
{
  const unsigned int divisor = 1024;
  const unsigned long int vref = APP_REFV_mV;
  unsigned long r32;

  memset(sp, 0, sizeof(*sp));

  ADC10MCTL0 = ADC10SREF_1 | ((0x0F & inch) * ADC10INCH0);
  ADC10CTL0 |= ADC10ENC | ADC10SC;
  while (ADC10CTL1 & ADC10BUSY) {
    ;
  }
  sp->raw = ADC10MEM0;
  ADC10CTL0 &= ~ADC10ENC;

  if (! (cal_ref && cal_adc)) {
    return -1;
  }
  r32 = sp->raw;
  r32 = ((r32 << 1) * cal_ref->cal_adc_15vref_factor) >> 16;
  sp->adj = ((r32 << 1) * cal_adc->cal_adc_gain_factor) >> 16;
  sp->adj += cal_adc->cal_adc_offset;
  sp->mV = (unsigned int)((sp->adj * vref) / divisor);

  return 0;
}

void main ()
{
  sSample vcc;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  BSP430_CORE_ENABLE_INTERRUPT();

  cprintf("\n\n# EXP430FR5739 ADXL335 demo\n# " __DATE__ " " __TIME__ "\n\n");

  initializeADC();
  if (! (cal_adc && cal_ref)) {
    cprintf("ERROR: Unable to get REF and ADC calibration values\n");
    return;
  }

  /* Provide power through P2.7.  PUC P2OUT has BIT7 but make it
   * explicit. */
  BSP430_HPL_PORT2->dir |= BIT7;
  BSP430_HPL_PORT2->out |= BIT7;

  /* ADXL335 requires 1ms to turn on */
  BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000UL);

  if (0 != getSample(&vcc, 11)) {
    cprintf("ERROR: Unable to measure Vcc\n");
    return;
  }
  cprintf("# X mV\tY mV\tZ mV\t\tX (mg)\tY (mg)\tZ (mg)\n");
  while (1) {
    int rc;
    int a;
    sSample axis[3];

    rc += getSample(axis+0, 12); /* Xout */
    rc += getSample(axis+1, 13); /* Yout */
    rc += getSample(axis+2, 14); /* Zout */
    for (a = 0; a < sizeof(axis)/sizeof(*axis); ++a) {
      sSample * sp = axis + a;
      /* m0g = vcc mV, with offset (2*vcc)/10 mV/g.  1000 mg/g.
       * Thus: acc = 1000 * ((ma - m0g) / (vcc / 5))
       * Rearrange to eliminate overflow error. */
      sp->mg = (5000L * ((int)sp->mV - (int)vcc.mV)) / vcc.mV;
    }
    cprintf("% 5d\t% 5d\t% 5d\t\t", axis[0].mV, axis[1].mV, axis[2].mV);
    cprintf("% 5d\t% 5d\t% 5d\n", axis[0].mg, axis[1].mg, axis[2].mg);
  }
}
