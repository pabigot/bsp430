/** This file is in the public domain.
 *
 * This program initializes the platform then spins toggling the first
 * LED and printing a monotonically increasing counter to the serial
 * port designated as a console.
 *
 * Its role is primarily as a sanity check when bootstrapping a new
 * platform: the third step is to support printf.  (Though printf is
 * heavy-weight, so we're using the console print routines.)
 *
 * It also serves as an example of the sort of changes you need to
 * make if your application uses the watchdog timer.  Test this with:
 *
 * @code

make AUX_CPPFLAGS='-DconfigBSP430_CORE_SUPPORT_WATCHDOG=1 -DBSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES=10000'

 * @endcode
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

/* We're going to use LEDs.  Enable them. */
#include <bsp430/utility/led.h>

/* We want to know the nominal clock speed so we can delay. */
#include <bsp430/clock.h>

/* We require console support as verified by <bsp430/platform.h> */
#ifndef BSP430_CONSOLE
#error No console UART PERIPH handle has been defined
#endif /* BSP430_CONSOLE */
#include <bsp430/utility/console.h>

void main ()
{
  xBSP430serialHandle console_handle;

  /* First thing you do in main is configure the platform. */
  vBSP430platformInitialize_ni();

  /* Now initialize the LEDs */
  vBSP430ledInitialize_ni();

#if BSP430_PLATFORM_SPIN_FOR_JUMPER - 0
  vBSP430platformSpinForJumper_ni();
#endif /* BSP430_PLATFORM_SPIN_FOR_JUMPER */

  /* Turn the first one on */
  vBSP430ledSet(0, 1);

  /* Configure the console to use the default UART handle */
  console_handle = xBSP430consoleInitialize();

  /* Indicate we made it this far. */
  vBSP430ledSet(1, 1);

  if (NULL == console_handle) {
    /* Failed to configure the UART HAL */
    vBSP430ledSet(0, 0);
  } else {
    unsigned int counter = 0;

    /* Now blink and display a counter value every nominal half
     * second.  The blink proves that the loop is running even if
     * nothing's coming out the serial port. */
    while (1) {
#if configBSP430_CORE_SUPPORT_WATCHDOG - 0
      /* BSP430 should support a watchdog timer that will hold off
       * at least 30 msec.  Any loop that exceeds that time needs to
       * be broken up so the watchdog can be cleared.  To be
       * conservative, we'll reset every millisecond, so a
       * half-second loop has to be broken into 500 sections.
       *
       * Note that we need a compile-time constant for
       * __delay_cycles(), so have to rely on
       * BSP430_CLOCK_NOMINAL_MCLK_HZ instead of the actual clock
       * speed. */
      {
        int iters = 500;
#define MSEC_MCLK (BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000UL)
        while (0 < iters--) {
          BSP430_CORE_WATCHDOG_CLEAR();
          __delay_cycles(MSEC_MCLK);
        }
      }
#else /* configBSP430_CORE_SUPPORT_WATCHDOG */
      __delay_cycles(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */
      cputtext_ni("\nCounter ");
      cputu_ni(counter, 10);
      ++counter;
      vBSP430ledSet(0, -1);
    }
  }
}
