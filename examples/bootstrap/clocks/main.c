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

/* We're going to use a console. */
#include <bsp430/utility/console.h>

/* LPM bits are relevant when figuring out what clocks might be
 * enabled */
#include <bsp430/lpm.h>

#if configBSP430_PLATFORM_SPIN_FOR_JUMPER - 0
#include <bsp430/utility/led.h>
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

#include <stdlib.h>

void main ()
{
  unsigned long smclk_hz;
  unsigned short aclk_hz;

  xBSP430serialHandle console_handle = NULL;

  /* First thing you do in main is configure the platform. */
  vBSP430platformInitialize_ni();

#if configBSP430_PLATFORM_SPIN_FOR_JUMPER - 0
  vBSP430ledInitialize_ni();
  vBSP430platformSpinForJumper_ni();
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

  /* Configure the console to use the default UART handle, if we know that.
   * If something goes wrong, then nothing will show up on the serial port,
   * but the clocks should still be running. */
#ifdef BSP430_CONSOLE_SERIAL_PERIPH_HANDLE
  console_handle = xBSP430serialOpen(BSP430_CONSOLE_SERIAL_PERIPH_HANDLE, 0, BSP430_CONSOLE_BAUD_RATE);
  iBSP430consoleConfigure(console_handle);

  cputtext_ni("\nSystem running\n");
  cputtext_ni("\nNominal platform MCLK (Hz): ");
  cputul_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ, 10);
  cputtext_ni("\nQueried platform MCLK (Hz): ");
  cputul_ni(ulBSP430clockMCLK_Hz_ni(), 10);
  cputtext_ni("\nNominal SMCLK divides MCLK by shifting: ");
  cputi_ni(BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT, 10);
  cputtext_ni("\nQueried SMCLK shift: ");
  cputi_ni(iBSP430clockSMCLKDividingShift_ni(), 10);
  cputtext_ni("\nQueried platform SMCLK (Hz): ");
  smclk_hz = ulBSP430clockSMCLK_Hz_ni();
  cputul_ni(smclk_hz, 10);
  cputtext_ni("\nCrystal stabilization delay per iteration: ");
  cputul_ni(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES, 10);
  cputtext_ni("\nBoot crystal stabilization delay (seconds): ");
  cputu_ni(BSP430_PLATFORM_LFXT1_BOOT_DELAY_SEC, 10);
  cputtext_ni("\nLFXT1 shows ");
  cputtext_ni(BSP430_CLOCK_LFXT1_IS_FAULTED() ? "FAULT" : "ok");
  cputtext_ni("\nNominal VLOCLK (Hz): ");
  cputu_ni(BSP430_CLOCK_NOMINAL_VLOCLK_HZ, 10);
  cputtext_ni("\nNominal ACLK (Hz): ");
  cputu_ni(BSP430_CLOCK_NOMINAL_ACLK_HZ, 10);
  cputtext_ni("\nQueried platform ACLK (Hz): ");
  aclk_hz = usBSP430clockACLK_Hz_ni();
  cputu_ni(aclk_hz, 10);

#if defined(BSP430_TIMER_CCACLK_PERIPH_HANDLE)
  do {
    int i;
    const int SAMPLE_PERIOD_ACLK = 10;
    volatile xBSP430periphTIMER * tp = xBSP430periphLookupTIMER(BSP430_TIMER_CCACLK_PERIPH_HANDLE);
    const int ccidx = BSP430_TIMER_CCACLK_CC_INDEX;
    unsigned int c0 = 0;
    unsigned int c1;
    unsigned int cc_delta;
    unsigned long aclk_rel_smclk_hz;
    unsigned long smclk_rel_aclk_hz;


    if (! tp) {
      cputtext_ni("\nUnable to access configured CCACLK timer");
      break;
    }
    BSP430_CORE_WATCHDOG_CLEAR();
    /* Capture the SMCLK ticks between adjacent ACLK ticks */
    tp->ctl = TASSEL_2 | MC_2 | TACLR;
    tp->cctl[ccidx] = CM_2 | BSP430_TIMER_CCACLK_CCIS | CAP | SCS;
    /* NOTE: CCIFG seems to be set immediately on the second and
     * subsequent iterations.  Flush the first capture. */
    while (! (tp->cctl[ccidx] & CCIFG)) {
      ; /* nop */
    }
    for (i = 0; i <= SAMPLE_PERIOD_ACLK; ++i) {
      tp->cctl[ccidx] &= ~CCIFG;
      while (! (tp->cctl[ccidx] & CCIFG)) {
        ; /* nop */
      }
      if (0 == i) {
        c0 = tp->ccr[ccidx];
      }
    }
    c1 = tp->ccr[ccidx];
    tp->ctl = 0;
    tp->cctl[ccidx] = 0;
    cc_delta = (c0 > c1) ? (c0 - c1) : (c1 - c0);
    cputchar_ni('\n');
    cputu_ni(SAMPLE_PERIOD_ACLK, 10);
    cputtext_ni(" ticks of ACLK produced ");
    cputu_ni(cc_delta, 10);
    cputtext_ni(" ticks of SMCLK");
    cputtext_ni("\n\tSMCLK (Hz) (if ACLK correct): ");
    smclk_rel_aclk_hz = (cc_delta * (unsigned long)aclk_hz) / SAMPLE_PERIOD_ACLK;
    cputul_ni(smclk_rel_aclk_hz, 10);
    cputtext_ni(" (error ");
    cputl_ni(smclk_rel_aclk_hz - smclk_hz, 10);
    cputtext_ni(" = ");
    cputl_ni(1000 * labs(smclk_rel_aclk_hz - smclk_hz) / smclk_hz, 10);
    cputtext_ni(" kHz/MHz)");
    cputtext_ni("\n\tACLK (Hz) (if SMCLK correct): ");
    aclk_rel_smclk_hz = SAMPLE_PERIOD_ACLK * smclk_hz / cc_delta;
    cputul_ni(aclk_rel_smclk_hz, 10);
    cputtext_ni(" (error ");
    cputl_ni(aclk_rel_smclk_hz - aclk_hz, 10);
    cputtext_ni(" = ");
    cputl_ni(1000 * labs(aclk_rel_smclk_hz - aclk_hz) / aclk_hz, 10);
    cputtext_ni(" Hz/kHz)");
  } while (0);
#else
  cputtext_ni("\nNo CCACLK timer available for ACLK/SMCLK comparison");
#endif

  cputchar_ni('\n');
#endif /* BSP430_CONSOLE_SERIAL_PERIPH_HANDLE */
  if (0 == iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_EXPOSED_CLOCKS, 1)) {
#ifdef BSP430_CONSOLE_SERIAL_PERIPH_HANDLE
    cputtext_ni("\nClock signals exposed:\n\t");
#ifdef BSP430_PERIPH_EXPOSED_CLOCKS_HELP
    cputtext_ni(BSP430_PERIPH_EXPOSED_CLOCKS_HELP);
#else
    cputtext_ni("Go look, cuz I don't know where they are");
#endif /* BSP430_PERIPH_EXPOSED_CLOCKS_HELP */
    cprintf("\nStatus register LPM bits: ");
    cputu_ni(__read_status_register() & BSP430_LPM_SR_MASK, 16);
    cputchar_ni('\n');
#endif /* BSP430_CONSOLE_SERIAL_PERIPH_HANDLE */
    /* Spin here with CPU active.  In LPM0, MCLK is disabled.  Other
     * clocks get disabled at deeper sleep modes; if you fall off the
     * bottom, you might end up in LPM4 with all clocks disabled. */
    while (1) {
      ;
    }
  } else {
#ifdef BSP430_CONSOLE_SERIAL_PERIPH_HANDLE
    cputtext_ni("\nFailed to expose clock signals\n");
#endif /* BSP430_PERIPH_EXPOSED_CLOCKS_HELP */
  }
}
