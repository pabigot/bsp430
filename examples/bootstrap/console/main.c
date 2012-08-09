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
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

#include <bsp430/platform.h>

/* We're going to use LEDs so we need the interface. */
#include <bsp430/utility/led.h>

/* We want to know the nominal clock speed so we can delay the
 * appropriate interval. */
#include <bsp430/clock.h>

/* We require console support as verified by <bsp430/platform.h> */
#ifndef BSP430_CONSOLE
#error No console UART PERIPH handle has been defined
#endif /* BSP430_CONSOLE */
#include <bsp430/utility/console.h>

void main ()
{
  int rv;

  /* Initialize platform. */
  vBSP430platformInitialize_ni();

  /* Turn on the first LED */
  vBSP430ledSet(0, 1);

  /* Configure the console to use the default UART handle */
  rv = iBSP430consoleInitialize();

#if BSP430_CONSOLE_RX_BUFFER_SIZE - 0
  /* If we're using interrupt-driven reception, we'd better enable
   * interrupts. */
  BSP430_CORE_ENABLE_INTERRUPT();
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */

  /* Indicate we made it this far. */
  vBSP430ledSet(1, 1);

  if (0 > rv) {
    /* Failed to configure the UART HAL */
    vBSP430ledSet(0, 0);
  } else {
    unsigned int counter = 0;

    /* Now blink and display a counter value every nominal half
     * second.  The blink proves that the loop is running even if
     * nothing's coming out the serial port.  On each iteration,
     * attempt to read a character and display it if one is
     * present. */
    while (1) {
      int rc;
      BSP430_CORE_WATCHDOG_CLEAR();
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
      while (0 <= ((rc = cgetchar_ni()))) {
        cputtext_ni(" rx char ");
        cputu_ni(rc, 10);
        cputtext_ni(" '");
        cputchar_ni(rc);
        cputtext_ni("'");
      }
      cputtext_ni("\nCounter ");
      cputu_ni(counter, 10);
      ++counter;
      vBSP430ledSet(0, -1);
    }
  }
}
