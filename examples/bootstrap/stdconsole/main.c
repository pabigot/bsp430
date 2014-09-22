/** This file is in the public domain.
 *
 * This program initializes the platform then spins toggling the first
 * LED and printing a monotonically increasing counter to stdout.
 *
 * Its role is primarily as a sanity check when bootstrapping a new
 * platform: the third step is to support printf.
 *
 * Note if BSP430_CONSOLE_RX_BUFFER_SIZE is zero (which is the default
 * value), character reception is polled.  The underlying UART RX
 * buffer is one character deep; if more characters are received, the
 * UCOE bit will be set in the status byte and only the last character
 * will be returned.

 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>

/* This application expects to be using newlib. */
#if ! (BSP430_CORE_TOOLCHAIN_LIBC_NEWLIB - 0)
#error Application requires use of newlib
#endif /* BSP430_CORE_TOOLCHAIN_LIBC_NEWLIB */

/* We're going to use LEDs so we need the interface. */
#include <bsp430/utility/led.h>

/* We want to know the nominal clock speed so we can delay the
 * appropriate interval. */
#include <bsp430/clock.h>

/* For the standard API the standard header is all we need. */
#include <stdio.h>

void main ()
{
  unsigned int counter = 0;

  /* Initialize platform. */
  vBSP430platformInitialize_ni();

  /* Turn on the first LED */
  vBSP430ledSet(BSP430_LED_RED, 1);

#if (WITH_INPUT - 0)
  /* Indicate we're expecting to acept input. */
  vBSP430ledSet(BSP430_LED_GREEN, 1);
#endif /* WITH_INPUT */

  /* Now blink and display a counter value every nominal half
   * second.  The blink proves that the loop is running even if
   * nothing's coming out the serial port.  On each iteration,
   * attempt to read a character and display it if one is
   * present. */
  while (1) {
    int rc;
    BSP430_CORE_WATCHDOG_CLEAR();
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
    while (0 <= ((rc = getchar()))) {
      vBSP430ledSet(BSP430_LED_GREEN, -1);
      printf(" rx char %u '%c'", rc, rc);
    }
    printf("\nctr %u", counter);
#if (APP_VERBOSE - 0)
    printf("; tx %lu; rx %lu");
#endif /* APP_VERBOSE */
    ++counter;
    vBSP430ledSet(BSP430_LED_RED, -1);
  }
}
