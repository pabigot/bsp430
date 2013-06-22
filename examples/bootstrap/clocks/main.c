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
 * @homepage http://github.com/pabigot/bsp430
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
#if (BSP430_CONSOLE - 0)
#include <bsp430/utility/console.h>
#endif /* BSP430_CONSOLE */

/* LPM bits are relevant when figuring out what clocks might be
 * enabled */
#include <bsp430/lpm.h>

#include <stdlib.h>

void main ()
{
#if (BSP430_CONSOLE - 0)
  const char * help;
  unsigned long smclk_Hz;
  unsigned long aclk_Hz;
#endif /* BSP430_CONSOLE */

  /* First thing you do in main is configure the platform. */
  vBSP430platformInitialize_ni();

  /* If we support a console, dump out a bunch of configuration
   * information. */
#if (BSP430_CONSOLE - 0)
  (void)iBSP430consoleInitialize();
  cputtext_ni("\nclocks " __DATE__ " " __TIME__ "\n");
  cputtext_ni("\nBSP430_PLATFORM_BOOT_CONFIGURE_LFXT1: ");
  cputu_ni(BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1, 10);
  cputtext_ni("\nBSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES: ");
  cputul_ni(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES, 10);
  cputtext_ni("\nBSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC: ");
  cputu_ni(BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC, 10);
  cputtext_ni("\nBSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS: ");
  cputu_ni(BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS, 10);
#if defined(__MSP430_HAS_BC2__)
#if (configBSP430_BC2_TRIM_TO_MCLK - 0)
  cputtext_ni("\nconfigBSP430_BC2_TRIM_TO_MCLK: 1");
#else /* configBSP430_BC2_TRIM_TO_MCLK */
  cputtext_ni("\nconfigBSP430_BC2_TRIM_TO_MCLK: 0");
#endif /* configBSP430_BC2_TRIM_TO_MCLK */
#if (BSP430_BC2_TRIM_TO_MCLK - 0)
  cputtext_ni("\nBSP430_BC2_TRIM_TO_MCLK: 1");
#else /* BSP430_BC2_TRIM_TO_MCLK */
  cputtext_ni("\nBSP430_BC2_TRIM_TO_MCLK: 0");
#endif /* BSP430_BC2_TRIM_TO_MCLK */
#endif /* BC2 */
#if defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__)
#if (configBSP430_UCS_TRIM_DCOCLKDIV - 0)
  cputtext_ni("\nconfigBSP430_UCS_TRIM_DCOCLKDIV: 1");
#else /* configBSP430_UCS_TRIM_DCOCLKDIV */
  cputtext_ni("\nconfigBSP430_UCS_TRIM_DCOCLKDIV: 0");
#endif /* configBSP430_UCS_TRIM_DCOCLKDIV */
#if (BSP430_UCS_TRIM_DCOCLKDIV - 0)
  cputtext_ni("\nBSP430_UCS_TRIM_DCOCLKDIV: 1");
#else /* BSP430_UCS_TRIM_DCOCLKDIV */
  cputtext_ni("\nBSP430_UCS_TRIM_DCOCLKDIV: 0");
#endif /* BSP430_UCS_TRIM_DCOCLKDIV */
#endif /* UCS */

  cputtext_ni("\nBSP430_CLOCK_PUC_MCLK_HZ: ");
  cputul_ni(BSP430_CLOCK_PUC_MCLK_HZ, 10);
  cputtext_ni("\nBSP430_CLOCK_NOMINAL_MCLK_HZ: ");
  cputul_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ, 10);
  cputtext_ni("\nBSP430_CLOCK_LFXT1_IS_FAULTED_NI(): ");
  cputu_ni(BSP430_CLOCK_LFXT1_IS_FAULTED_NI(), 10);
  cputtext_ni("\nBSP430_CLOCK_NOMINAL_VLOCLK_HZ: ");
  cputu_ni(BSP430_CLOCK_NOMINAL_VLOCLK_HZ, 10);
  cputtext_ni("\nBSP430_CLOCK_NOMINAL_XT1CLK_HZ: ");
  cputul_ni(BSP430_CLOCK_NOMINAL_XT1CLK_HZ, 10);
#if defined(BSP430_CLOCK_NOMINAL_XT2CLK_HZ)
  cputtext_ni("\nBSP430_PLATFORM_BOOT_CONFIGURE_XT2: ");
  cputu_ni(BSP430_PLATFORM_BOOT_CONFIGURE_XT2, 10);
  cputtext_ni("\nBSP430_CLOCK_XT2_IS_FAULTED_NI(): ");
  cputu_ni(BSP430_CLOCK_XT2_IS_FAULTED_NI(), 10);
  cputtext_ni("\nBSP430_CLOCK_NOMINAL_XT2CLK_HZ: ");
  cputul_ni(BSP430_CLOCK_NOMINAL_XT2CLK_HZ, 10);
#endif /* BSP430_CLOCK_NOMINAL_XT2CLK_HZ */
  cputtext_ni("\nulBSP430clockMCLK_Hz_ni(): ");
  cputul_ni(ulBSP430clockMCLK_Hz_ni(), 10);
  cputtext_ni("\nBSP430_PLATFORM_BOOT_SMCLK_DIVIDING_SHIFT: ");
  cputi_ni(BSP430_PLATFORM_BOOT_SMCLK_DIVIDING_SHIFT, 10);
  cputtext_ni("\nulBSP430clockSMCLK_Hz_ni(): ");
  smclk_Hz = ulBSP430clockSMCLK_Hz_ni();
  cputul_ni(smclk_Hz, 10);
  cputtext_ni("\nBSP430_PLATFORM_BOOT_ACLK_DIVIDING_SHIFT: ");
  cputi_ni(BSP430_PLATFORM_BOOT_ACLK_DIVIDING_SHIFT, 10);
  cputtext_ni("\nulBSP430clockACLK_Hz_ni(): ");
  aclk_Hz = ulBSP430clockACLK_Hz_ni();
  cputul_ni(aclk_Hz, 10);

#if (BSP430_TIMER_CCACLK - 0)
  if (1000000UL <= aclk_Hz) {
    cputtext_ni("\nUnable to use high-speed ACLK for differential timing of SMCLK");
  } else {
    do {
      const unsigned int SAMPLE_PERIOD_ACLK = 10;
      volatile sBSP430hplTIMER * tp = xBSP430hplLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
      unsigned int cc_delta;
      unsigned long aclk_rel_smclk_Hz;
      unsigned long smclk_rel_aclk_Hz;

      if (! tp) {
        cputtext_ni("\nUnable to access configured CCACLK timer");
        break;
      }
      /* Capture the SMCLK ticks between adjacent ACLK ticks */
      tp->ctl = TASSEL_2 | MC_2 | TACLR;
      cc_delta = uiBSP430timerCaptureDelta_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE,
                                              BSP430_TIMER_CCACLK_ACLK_CCIDX,
                                              CM_2,
                                              BSP430_TIMER_CCACLK_ACLK_CCIS,
                                              SAMPLE_PERIOD_ACLK);
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
      cputtext_ni("\nComparison with measured values:");
      cputtext_ni("\n SMCLK (Hz) (if measured ACLK correct): ");
      smclk_rel_aclk_Hz = (cc_delta * aclk_Hz) / SAMPLE_PERIOD_ACLK;
      cputul_ni(smclk_rel_aclk_Hz, 10);
      cputtext_ni(" (error ");
      cputl_ni(smclk_rel_aclk_Hz - smclk_Hz, 10);
      cputtext_ni(" = ");
      cputl_ni(1000 * labs(smclk_rel_aclk_Hz - smclk_Hz) / smclk_Hz, 10);
      cputtext_ni(" kHz/MHz)");
      cputtext_ni("\n ACLK (Hz) (if measured SMCLK correct): ");
      aclk_rel_smclk_Hz = SAMPLE_PERIOD_ACLK * smclk_Hz / cc_delta;
      cputul_ni(aclk_rel_smclk_Hz, 10);
      cputtext_ni(" (error ");
      cputl_ni(aclk_rel_smclk_Hz - aclk_Hz, 10);
      cputtext_ni(" = ");
      cputl_ni(1000 * labs(aclk_rel_smclk_Hz - aclk_Hz) / aclk_Hz, 10);
      cputtext_ni(" Hz/kHz)");
    } while (0);
  }
#else /* BSP430_TIMER_CCACLK */
  cputtext_ni("\nNo CCACLK timer available for ACLK/SMCLK comparison");
#endif /* BSP430_TIMER_CCACLK */

  cputchar_ni('\n');

#if defined(__MSP430_HAS_BC2__)
  cputtext_ni("\nBC2: DCO ");
  cputu_ni(DCOCTL, 16);
  cputtext_ni(" CTL1  ");
  cputu_ni(BCSCTL1, 16);
  cputtext_ni(" CTL2  ");
  cputu_ni(BCSCTL2, 16);
  cputtext_ni(" CTL3  ");
  cputu_ni(BCSCTL3, 16);

#endif

#if defined(__MSP430_HAS_FLL__) || defined(__MSP430_HAS_FLLPLUS__)
  cprintf("\nFLL: SCF QCTL %02x I0 %02x I1 %02x ; CTL0 %02x CTL1 %02x CTL2 %02x\n",
          SCFQCTL, SCFI0, SCFI1, FLL_CTL0, FLL_CTL1,
#if defined(FLL_CTL2_)
          FLL_CTL2
#else /* FLL_CTL2 */
          ~0
#endif /* FLL_CTL2 */
         );
#endif /* FLL/PLUS */

#if defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__)
  cputtext_ni("\nBSP430_UCS_FLL_SELREF: "
#if SELREF__XT2CLK <= BSP430_UCS_FLL_SELREF
              "XT2CLK"
#elif SELREF__REFOCLK <= BSP430_UCS_FLL_SELREF
              "REFOCLK"
#else /* BSP430_UCS_FLL_SELREF */
              "XT1CLK"
#endif /* BSP430_UCS_FLL_SELREF */
             );
  cprintf("\nUCS RSEL %d DCO %d MOD %d:"
          "\n CTL0 %04x CTL1 %04x CTL2 %04x CTL3 %04x"
          "\n CTL4 %04x CTL5 %04x CTL6 %04x CTL7 %04x",
          0x1F & (UCSCTL1 / DCORSEL0), 0x1F & (UCSCTL0 / DCO0), 0x1F & (UCSCTL0 / MOD0),
          UCSCTL0, UCSCTL1, UCSCTL2, UCSCTL3,
          UCSCTL4, UCSCTL5, UCSCTL6, UCSCTL7);
#endif /* UCS */

#if defined(__MSP430_HAS_CS__) || defined(__MSP430_HAS_CS_A__)
  cprintf("\nCS RSEL %d DCOFSEL %d:"
          "\n CTL0 %04x CTL1 %04x CTL2 %04x CTL3 %04x"
          "\n CTL4 %04x CTL5 %04x CTL6 %04x",
          !!(DCORSEL & CSCTL1), 0x07 & (CSCTL1 / DCOFSEL0),
          CSCTL0, CSCTL1, CSCTL2, CSCTL3,
          CSCTL4, CSCTL5, CSCTL6);
#endif /* CS */

#endif /* BSP430_CONSOLE */

  if (0 == iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_EXPOSED_CLOCKS, 0, 1)) {
#if (BSP430_CONSOLE - 0)
    cputtext_ni("\n\nClock signals exposed:\n ");
    help = NULL;
#ifdef BSP430_PLATFORM_PERIPHERAL_HELP
    help = xBSP430platformPeripheralHelp(BSP430_PERIPH_EXPOSED_CLOCKS, 0);
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
    if (NULL == help) {
      help = "Go look at the data sheet and source, because nobody told me where they are";
    }
    cputtext_ni(help);
    cputtext_ni("\nStatus register LPM bits: ");
    cputu_ni(__read_status_register() & BSP430_CORE_LPM_SR_MASK, 16);
    cputtext_ni("\nIFG1 bits: ");
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
    cputu_ni(SFRIFG1, 16);
#else /* CPUX */
    cputu_ni(IFG1, 16);
#endif /* CPUX */
    cputtext_ni(" with OFIFG ");
    cputu_ni(OFIFG, 16);
    cputchar_ni('\n');
#endif /* BSP430_CONSOLE */

    /* Spin here with CPU active.  In LPM0, MCLK is disabled.  Other
     * clocks get disabled at deeper sleep modes; if you fall off the
     * bottom, you might end up in LPM4 with all clocks disabled. */
    while (1) {
      BSP430_CORE_WATCHDOG_CLEAR();
    }
  } else {
#if (BSP430_CONSOLE - 0)
    cputtext_ni("\nFailed to expose clock signals\n");
#endif /* BSP430_CONSOLE */
  }
}
