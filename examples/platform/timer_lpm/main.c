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
#include <bsp430/periph/pmm.h>
#include <bsp430/periph/sys.h>

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
#if (BSP430_MODULE_SYS - 0)
  unsigned long reset_causes = 0;
  unsigned int reset_flags = 0;
#endif /* BSP430_MODULE_SYS */

#if (BSP430_MODULE_SYS - 0)
  {
    unsigned int sysrstiv;

    /* Record all the reset causes */
    while (0 != ((sysrstiv = uiBSP430sysSYSRSTGenerator_ni(&reset_flags)))) {
      reset_causes |= 1UL << (sysrstiv / 2);
    }

#ifdef BSP430_PMM_ENTER_LPMXp5_NI
    /* If we woke from LPMx.5, we need to clear the lock in PM5CTL0.
     * We'll do it early, since we're not really interested in
     * retaining the current IFG settings. */
    if (reset_flags & BSP430_SYS_FLAG_SYSRST_LPM5WU) {
      PMMCTL0_H = PMMPW_H;
      PM5CTL0 = 0;
      PMMCTL0_H = 0;
    }
#endif /* BSP430_PMM_ENTER_LPMXp5_NI */
  }
#endif /* BSP430_MODULE_SYS */

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\n\ntest_lpm " __DATE__ " " __TIME__ "\n");

#if (BSP430_MODULE_SYS - 0)
  cprintf("System reset bitmask %lx; causes:\n", reset_causes);
  {
    int bit = 0;
    while (bit < (8 * sizeof(reset_causes))) {
      if (reset_causes & (1UL << bit)) {
        cprintf("\t%s\n", xBSP430sysSYSRSTIVDescription(2*bit));
      }
      ++bit;
    }
  }

  cputtext("System reset included:");
  if (reset_flags & BSP430_SYS_FLAG_SYSRST_BOR) {
    cputtext(" BOR");
  }
  if (reset_flags & BSP430_SYS_FLAG_SYSRST_LPM5WU) {
    cputtext(" LPM5WU");
  }
  if (reset_flags & BSP430_SYS_FLAG_SYSRST_POR) {
    cputtext(" POR");
  }
  if (reset_flags & BSP430_SYS_FLAG_SYSRST_PUC) {
    cputtext(" PUC");
  }
  cputchar('\n');

#endif

  TA0CTL = TASSEL_1 | MC_2 | TACLR;
  ta0_Hz = ulBSP430timerFrequency_Hz_ni(BSP430_PERIPH_TA0);

  /* FRAM PMM does not support SVS */
#if (BSP430_MODULE_PMM - 0) && ! (BSP430_MODULE_PMM_FRAM - 0)
  cprintf("PMM is supported; boot SVSMLCTL %04x\n", SVSMLCTL);

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
  /* This is the reset value */
  SVSMLCTL = SVMLE | SVSLE;
#if 1
  /* Turning off the low-side supervision and monitoring sets the
   * device in full power mode.  This variant works, and consumes no
   * current.  It's probably the simplest approach, but make sure any
   * changes to Vcore temporarily re-enable the module. */
  SVSMLCTL = 0;
#endif
#if 0
  /* Placing both SVS_L and SVM_L into automatic mode with full power
   * mode enabled also works.  Current consumption is 1.5uA for each
   * of SVS_L and SVS_M for MSP430F5438A, though I have been unable to
   * demonstrate this by differentiating configurations.
   *
   * It's unlikely that both supervision and monitoring provide value,
   * so consider turning off monitoring.  Doing so naively (viz., just
   * setting SVSLFP | SVSLE) will not work, though: an SVSL interrupt
   * causes a POR in LPM2.
   *
   * Adding SVMLOVPE would enable overvoltage protection.  Modifying
   * SVSLRVL and SVSMLRRL would change the voltage thresholds, which
   * might be appropriate if you're playing with Vcore.  For now we're
   * just interested in decreasing wakeup time. */
  SVSMLCTL = SVMLFP | SVMLE | SVSLFP | SVSLE | SVSMLACE | SVSLMD;
#endif
  PMMCTL0_H = !PMMPW_H;

  cprintf("Test SVSMLCTL is %04x\n", SVSMLCTL);
  do {
    cprintf("... waiting for SVSMLDLYST to clear: %04x\n", SVSMLCTL);
  } while (SVSMLCTL & SVSMLDLYST);

#endif

  BSP430_CORE_ENABLE_INTERRUPT();

  delta_ta0 = ta0_Hz / 4;
  cprintf("\nTA0 is at %lu Hz; sleep time %u ticks\n", ta0_Hz, delta_ta0);
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
