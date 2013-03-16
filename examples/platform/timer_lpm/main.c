/** This file is in the public domain.
 *
 * Demonstrates delays in interrupt handling with different clock
 * peripherals, by setting to wake when a 32 kiHz timer reaches a
 * specific value and capturing the actual timer counter at the start
 * of the interrupt handler.  A delta of N indicates a 30*N usec delay
 * between the event and when the interrupt handler started processing
 * the event.
 *
 * See the data sheet for MCUs that support the Power Management
 * Module to confirm that 150us is the normal wake-up time in default
 * slow wake-up.
 *
 * The user guide documents that wake times with a FET-UIF or other
 * debug hardware attached may not represent the performance observed
 * when debug hardware is disconnected.
 *
 * BC2 results from exp430g2 with MSP430G2553 show no delay in any mode:

LPMx   CCR0  CAP0  Delta0   CCR1  CAP1  Delta1   SR
LPM0: 14487 14487     0    23339 23339     0    0008
LPM1: 32599 32599     0    41451 41451     0    0008
LPM2: 50711 50711     0    59563 59563     0    0008
LPM3:  3287  3287     0    12137 12137     0    0008

 * CS results from exp430fr5739 show delay in LPM3:

LPMx   CCR0  CAP0  Delta0   CCR1  CAP1  Delta1   SR
LPM0: 14624 14624     0    23475 23475     0    0008
LPM1: 32734 32734     0    41585 41585     0    0008
LPM2: 50844 50844     0    59695 59695     0    0008
LPM3:  3418  3420     2    12270 12272     2    0008

 * UCS results from trxeb with MSP430F5438A show delay in LPM2 and
 * LPM3, regardless of #configBSP430_CORE_DISABLE_FLL:

LPM exit from ISRs clears: 00f0
LPMx   CCR0  CAP0  Delta0   CCR1  CAP1  Delta1   SR
LPM0: 15812 15812     0    24663 24663     0    0008
LPM1: 33922 33922     0    42773 42773     0    0008
LPM2: 52032 52036     4    60888 60892     4    0008
LPM3:  4616  4620     4    13470 13474     4    0008

LPM exit from ISRs clears: 00b0
LPMx   CCR0  CAP0  Delta0   CCR1  CAP1  Delta1   SR
LPM0: 15812 15812     0    24663 24663     0    0048
LPM1: 33922 33922     0    42773 42773     0    0048
LPM2: 52032 52036     4    60888 60892     4    0048
LPM3:  4616  4620     4    13470 13474     4    0048

 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/timer.h>

struct sLPMconfig {
  const char * tag;
  unsigned int lpm_bits;
};

const struct sLPMconfig lpm_configs[] = {
  { "LPM0", LPM0_bits },
  { "LPM1", LPM1_bits },
  { "LPM2", LPM2_bits },
  { "LPM3", LPM3_bits },
};

volatile unsigned int ta0r;

#pragma vector=TIMER0_A0_VECTOR
__interrupt void
ta0cc0_isr (void)
{
  ta0r = TA0R;
  __bic_status_register_on_exit(BSP430_CORE_LPM_EXIT_MASK);
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void
ta0_isr (void)
{
  (void)TA0IV;
  ta0r = TA0R;
  __bic_status_register_on_exit(BSP430_CORE_LPM_EXIT_MASK);
}

void main ()
{
  volatile sBSP430hplTIMER * const hpl = xBSP430hplLookupTIMER(BSP430_PERIPH_TA0);
  unsigned long ta0_Hz;
  unsigned int delta_ta0;
  const struct sLPMconfig * lcp = lpm_configs;
  const struct sLPMconfig * const elcp = lpm_configs + sizeof(lpm_configs)/sizeof(*lpm_configs);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  TA0CTL = TASSEL_1 | MC_2 | TACLR;
  ta0_Hz = ulBSP430timerFrequency_Hz_ni(BSP430_PERIPH_TA0);

#if 0
  /* This sequence eliminates the wakeup delay on the MSP430F5438A.
   * The ramifications of doing this are to be found in the Power
   * Management Module and Supply Voltage Supervisor chapter of the
   * 5xx/6xx Family User's Guide, in the section "SVS and SVM
   * Performance Modes and Wakeup Times".
   *
   * Also check MCU errata related to the PMM.  THere are several that
   * appear relevant when changing the module from its power-up
   * state. */
  PMMCTL0_H = PMMPW_H;
#if 1
  /* This variant works */
  SVSMLCTL &= ~(SVSLE | SVMLE);
#else
  /* This appears to have no effect, though it should work. */
  SVSMLCTL |= SVSLFP;
#endif
  PMMCTL0_H = !PMMPW_H;
#endif

  BSP430_CORE_ENABLE_INTERRUPT();

  cputs("\n\nTimer LPM wake delay test\n");
  delta_ta0 = ta0_Hz / 4;
  cprintf("TA0 is at %lu Hz; sleep time %u ticks\n", ta0_Hz, delta_ta0);
  cprintf("Standard mode SR is %04x\n", __read_status_register());
  cprintf("SR bits: SCG0 %04x ; SCG1 %04x\n", SCG0, SCG1);
  cprintf("LPM exit from ISRs clears: %04x\n", BSP430_CORE_LPM_EXIT_MASK);

  cputs("LPMx   CCR0  CAP0  Delta0   CCR1  CAP1  Delta1   SR");
  while (lcp < elcp) {
    unsigned int tar;

    cprintf("%s: ", lcp->tag);
    BSP430_CORE_DISABLE_INTERRUPT();
    ta0r = 0;
    hpl->cctl[0] = CCIE;
    tar = uiBSP430timerAsyncCounterRead_ni(hpl);
    hpl->ccr[0] = tar + delta_ta0;
    BSP430_CORE_LPM_ENTER_NI(lcp->lpm_bits);
    cprintf("%5u %5u %5u    ", hpl->ccr[0], ta0r, ta0r-hpl->ccr[0]);

    BSP430_CORE_DISABLE_INTERRUPT();
    ta0r = 0;
    hpl->cctl[1] = CCIE;
    tar = uiBSP430timerAsyncCounterRead_ni(hpl);
    hpl->ccr[1] = tar + delta_ta0;
    BSP430_CORE_LPM_ENTER_NI(lcp->lpm_bits);
    cprintf("%5u %5u %5u    ", hpl->ccr[1], ta0r, ta0r-hpl->ccr[1]);

    cprintf("%04x\n", __read_status_register());
    ++lcp;
  }
  cprintf("Done\n");

}
