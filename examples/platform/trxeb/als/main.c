/** This file is in the public domain.
 *
 * Basic demonstration of on-board light sensor.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/tlv.h>

void main ()
{
  volatile sBSP430hplPORT * hpl = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_ALS_PORT_PERIPH_HANDLE);
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  /* Power the ALS */
  hpl->dir |= BSP430_PLATFORM_TRXEB_ALS_PWR_PORT_BIT;
  hpl->out |= BSP430_PLATFORM_TRXEB_ALS_PWR_PORT_BIT;

  /* REF to 2.5V */
  while (REFCTL0 & REFGENBUSY) {
    ;
  }
  REFCTL0 = REFVSEL_2 | REFON | REFMSTR;

  ADC12CTL0 = 0;
  ADC12CTL0 = ADC12SHT0_10 | ADC12ON;
  ADC12CTL1 = ADC12SHS_0 | ADC12SHP | ADC12SSEL_0 | ADC12CONSEQ_0;
  ADC12CTL2 = ADC12RES_2 | ADC12SR;
  ADC12MCTL0 = ADC12EOS | ADC12SREF_1 | BSP430_PLATFORM_TRXEB_ALS_OUT_INCH;

  while (1) {
    unsigned int adc;

    ADC12CTL0 |= ADC12ENC + ADC12SC;
    while (ADC12CTL1 & ADC12BUSY) {
      ;
    }
    adc = ADC12MEM0;
    ADC12CTL0 &= ~ADC12ENC;
    cprintf("%s: %u adc %u mV\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), adc, (unsigned int)((2500L * adc) / 4096));
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
