/** This file is in the public domain.
 *
 * This program initializes the platform then displays various information
 * about clock signals, including bringing them out to test points.
 *
 * Its role is to verify clock configuration support on a new platform.
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

/* We're going to use a console. */
#include <bsp430/utility/console.h>

/* LPM bits are relevant when figuring out what clocks might be
 * enabled */
#include <bsp430/lpm.h>

#if configBSP430_PLATFORM_SPIN_FOR_JUMPER - 0
#include <bsp430/utility/led.h>
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

void main ()
{
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
  console_handle = xBSP430serialOpen(BSP430_CONSOLE_SERIAL_PERIPH_HANDLE, 0, 9600);
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
  cputul_ni(ulBSP430clockSMCLK_Hz_ni(), 10);
  cputtext_ni("\nCrystal stabilization delay per iteration: ");
  cputul_ni(BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES, 10);
  cputtext_ni("\nLFXT1 shows ");
  cputtext_ni(BSP430_CLOCK_LFXT1_IS_FAULTED() ? "FAULT" : "ok");
  cputtext_ni("\nNominal VLOCLK (Hz): ");
  cputu_ni(BSP430_CLOCK_NOMINAL_VLOCLK_HZ, 10);
  cputtext_ni("\nNominal ACLK (Hz): ");
  cputu_ni(BSP430_CLOCK_NOMINAL_ACLK_HZ, 10);
  cputtext_ni("\nQueried platform ACLK (Hz): ");
  cputu_ni(usBSP430clockACLK_Hz_ni(), 10);
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
