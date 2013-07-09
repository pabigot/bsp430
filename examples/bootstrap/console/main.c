/** This file is in the public domain.
 *
 * This program initializes the platform then spins toggling the first
 * LED and printing a monotonically increasing counter to the serial
 * port designated as a console.
 *
 * Its role is primarily as a sanity check when bootstrapping a new
 * platform: the third step is to support printf.  (printf is
 * heavy-weight, so we're using the console print routines.)
 *
 * Note if BSP430_CONSOLE_RX_BUFFER_SIZE is zero (which is the default
 * value), character reception is polled.  The underlying UART RX
 * buffer is one character deep; if more characters are received, the
 * UCOE bit will be set in the status byte and only the last character
 * will be returned.
 *
 * @homepage http://github.com/pabigot/bsp430
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
#if (APP_VERBOSE - 0)
  hBSP430halSERIAL console;
#endif /* APP_VERBOSE */

  /* Initialize platform. */
  vBSP430platformInitialize_ni();

  /* Turn on the first LED */
  vBSP430ledSet(0, 1);

  /* Configure the console to use the default UART handle */
  rv = iBSP430consoleInitialize();
  /* assert(0 == rv); */
#if (APP_VERBOSE - 0)
  console = hBSP430console();
#endif /* APP_VERBOSE */

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
#if (BSP430_CONSOLE_RX_BUFFER_SIZE - 0) || (BSP430_CONSOLE_TX_BUFFER_SIZE - 0)
      /* If we're using interrupt-driven transmission or reception, we'd
       * better enable interrupts. */
      BSP430_CORE_ENABLE_INTERRUPT();
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
#if (BSP430_CONSOLE_RX_BUFFER_SIZE - 0) || (BSP430_CONSOLE_TX_BUFFER_SIZE - 0)
      BSP430_CORE_DISABLE_INTERRUPT();
#endif /* BSP430_CONSOLE_RX_BUFFER_SIZE */
      while (0 <= ((rc = cgetchar()))) {
        cputtext(" rx char ");
        cputu(rc, 10);
        cputtext(" '");
        cputchar(rc);
        cputtext("'");
      }
      cputtext("\nctr ");
      cputu(counter, 10);
#if (APP_VERBOSE - 0)
      cputtext("; tx ");
      cputul(console->num_tx, 10);
      cputtext("; rx ");
      cputul(console->num_rx, 10);
      cputtext("; stat 0x");
#if (configBSP430_SERIAL_USE_USCI - 0)
      cputu(console->hpl.usci->stat, 16);
#endif
#if (configBSP430_SERIAL_USE_USCI5 - 0)
      cputu(console->hpl.usci5->stat, 16);
#endif
#if (configBSP430_SERIAL_USE_EUSCI - 0)
      cputu(console->hpl.euscia->statw, 16);
#endif
#endif /* APP_VERBOSE */
      ++counter;
      vBSP430ledSet(0, -1);
    }
  }
}
