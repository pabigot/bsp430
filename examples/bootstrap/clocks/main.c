/** This file is in the public domain.
 *
 * This program initializes the platform then displays various information
 * about clock signals, including bringing them out to test points.
 *
 * Its role is to verify clock configuration support on a new platform.
 *
 * Where the platform supports it, the relative speed of SMCLK and
 * ACLK will be calculated.  This is useful, for example, to verify
 * that a calibrated value is consistent, or to reveal exactly how far
 * off nominal the VLOCLK frequency really is.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

/* Include the generic platform header.  This assumes that
 * #BSP430_PLATFORM_EXP430FR5739 (or another marker that identifies a
 * platform BSP430 supports) has been defined for the preprocessor.
 * If not, you may include the platform-specific version,
 * e.g. <bsp430/platform/exp430g2.h>. */
#include <bsp430/platform.h>

/* We want to know the nominal clock speed so we can delay. */
#include <bsp430/clock.h>

/* We're going to want to differentially compare ACLK and SMCLK if we
 * can */
#include <bsp430/periph/timer.h>

/* We will use a console if we can. */
#if BSP430_CONSOLE - 0
#include <bsp430/utility/console.h>
#endif /* BSP430_CONSOLE */

/* LPM bits are relevant when figuring out what clocks might be
 * enabled */
#include <bsp430/lpm.h>

#include <stdlib.h>

void main ()
{
#if BSP430_CONSOLE - 0
  unsigned long smclk_Hz;
  unsigned short aclk_Hz;
#endif /* BSP430_CONSOLE */

  /* First thing you do in main is configure the platform. */
  vBSP430platformInitialize_ni();

#if BSP430_PLATFORM_SPIN_FOR_JUMPER - 0
  vBSP430platformSpinForJumper_ni();
#endif /* BSP430_PLATFORM_SPIN_FOR_JUMPER */

  /* If we support a console, dump out a bunch of configuration
   * information. */
#if BSP430_CONSOLE - 0
  (void)xBSP430consoleInitialize();
  cputtext_ni("\nSystem running\n");
  cputtext_ni("\nBSP430_PLATFORM_BOOT_CONFIGURE_LFXT1: ");
  cputu_ni(BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1, 10);
  cputtext_ni("\nBSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES: ");
  cputul_ni(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES, 10);
  cputtext_ni("\nBSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC: ");
  cputu_ni(BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC, 10);
  cputtext_ni("\nBSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS: ");
  cputu_ni(BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS, 10);

  cputtext_ni("\nBSP430_CLOCK_PUC_MCLK_HZ: ");
  cputul_ni(BSP430_CLOCK_PUC_MCLK_HZ, 10);
  cputtext_ni("\nBSP430_CLOCK_NOMINAL_MCLK_HZ: ");
  cputul_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ, 10);
  cputtext_ni("\nulBSP430clockMCLK_Hz_ni(): ");
  cputul_ni(ulBSP430clockMCLK_Hz_ni(), 10);
  cputtext_ni("\nBSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT: ");
  cputi_ni(BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT, 10);
  cputtext_ni("\niBSP430clockSMCLKDividingShift_ni(): ");
  cputi_ni(iBSP430clockSMCLKDividingShift_ni(), 10);
  cputtext_ni("\nulBSP430clockSMCLK_Hz_ni(): ");
  smclk_Hz = ulBSP430clockSMCLK_Hz_ni();
  cputul_ni(smclk_Hz, 10);
  cputtext_ni("\nBSP430_CLOCK_LFXT1_IS_FAULTED(): ");
  cputu_ni(BSP430_CLOCK_LFXT1_IS_FAULTED(), 10);
  cputtext_ni("\nBSP430_CLOCK_NOMINAL_VLOCLK_HZ: ");
  cputu_ni(BSP430_CLOCK_NOMINAL_VLOCLK_HZ, 10);
  cputtext_ni("\nBSP430_CLOCK_NOMINAL_ACLK_HZ: ");
  cputu_ni(BSP430_CLOCK_NOMINAL_ACLK_HZ, 10);
  cputtext_ni("\nusBSP430clockACLK_Hz_ni(): ");
  aclk_Hz = usBSP430clockACLK_Hz_ni();
  cputu_ni(aclk_Hz, 10);

#if BSP430_TIMER_CCACLK - 0
  do {
    const int SAMPLE_PERIOD_ACLK = 10;
    volatile xBSP430periphTIMER * tp = xBSP430periphLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
    unsigned int cc_delta;
    unsigned long aclk_rel_smclk_Hz;
    unsigned long smclk_rel_aclk_Hz;


    if (! tp) {
      cputtext_ni("\nUnable to access configured CCACLK timer");
      break;
    }
    /* Capture the SMCLK ticks between adjacent ACLK ticks */
    tp->ctl = TASSEL_2 | MC_2 | TACLR;
    cc_delta = uiBSP430timerCCACLKMeasureDelta_ni(CM_2, SAMPLE_PERIOD_ACLK);
    tp->ctl = 0;
    if (-1 == cc_delta) {
      cputtext_ni("\nCCACLK measurement failed");
      break;
    }
    cputchar_ni('\n');
    cputu_ni(SAMPLE_PERIOD_ACLK, 10);
    cputtext_ni(" ticks of ACLK produced ");
    cputu_ni(cc_delta, 10);
    cputtext_ni(" ticks of SMCLK");
    cputtext_ni("\nComparison with NOMINAL values:");
    cputtext_ni("\n\tSMCLK (Hz) (if ACLK correct): ");
    smclk_rel_aclk_Hz = (cc_delta * (unsigned long)aclk_Hz) / SAMPLE_PERIOD_ACLK;
    cputul_ni(smclk_rel_aclk_Hz, 10);
    cputtext_ni(" (error ");
    cputl_ni(smclk_rel_aclk_Hz - smclk_Hz, 10);
    cputtext_ni(" = ");
    cputl_ni(1000 * labs(smclk_rel_aclk_Hz - smclk_Hz) / smclk_Hz, 10);
    cputtext_ni(" kHz/MHz)");
    cputtext_ni("\n\tACLK (Hz) (if SMCLK correct): ");
    aclk_rel_smclk_Hz = SAMPLE_PERIOD_ACLK * smclk_Hz / cc_delta;
    cputul_ni(aclk_rel_smclk_Hz, 10);
    cputtext_ni(" (error ");
    cputl_ni(aclk_rel_smclk_Hz - aclk_Hz, 10);
    cputtext_ni(" = ");
    cputl_ni(1000 * labs(aclk_rel_smclk_Hz - aclk_Hz) / aclk_Hz, 10);
    cputtext_ni(" Hz/kHz)");
  } while (0);
#else /* BSP430_TIMER_CCACLK */
  cputtext_ni("\nNo CCACLK timer available for ACLK/SMCLK comparison");
#endif /* BSP430_TIMER_CCACLK */

  cputchar_ni('\n');
#endif /* BSP430_CONSOLE */

#if defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__)
  cprintf("\nUCS RSEL %d DCO %d MOD %d:"
          "\n\tCTL0 %04x CTL1 %04x CTL2 %04x CTL3 %04x"
          "\n\tCTL4 %04x CTL5 %04x CTL6 %04x CTL7 %04x",
          0x1F & (UCSCTL1 / DCORSEL0), 0x1F & (UCSCTL0 / DCO0), 0x1F & (UCSCTL0 / MOD0),
          UCSCTL0, UCSCTL1, UCSCTL2, UCSCTL3,
          UCSCTL4, UCSCTL5, UCSCTL6, UCSCTL7);
#endif
  
  if (0 == iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_EXPOSED_CLOCKS, 1)) {
#if BSP430_CONSOLE - 0
    cputtext_ni("\nClock signals exposed:\n\t");
#ifdef BSP430_PERIPH_EXPOSED_CLOCKS_HELP
    cputtext_ni(BSP430_PERIPH_EXPOSED_CLOCKS_HELP);
#else
    cputtext_ni("Go look, cuz I don't know where they are");
#endif /* BSP430_PERIPH_EXPOSED_CLOCKS_HELP */
    cprintf("\nStatus register LPM bits: ");
    cputu_ni(__read_status_register() & BSP430_LPM_SR_MASK, 16);
    cputchar_ni('\n');
#endif /* BSP430_CONSOLE */
    /* Spin here with CPU active.  In LPM0, MCLK is disabled.  Other
     * clocks get disabled at deeper sleep modes; if you fall off the
     * bottom, you might end up in LPM4 with all clocks disabled. */
    while (1) {
      BSP430_CORE_WATCHDOG_CLEAR();
    }
  } else {
#if BSP430_CONSOLE - 0
    cputtext_ni("\nFailed to expose clock signals\n");
#endif /* BSP430_CONSOLE */
  }
}
