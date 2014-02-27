/** This file is in the public domain.
 *
 * This demonstrates using the BSP430 OneWire interface to read the
 * temperature from an externally-powered Maxim DS18B20 (or similar)
 * digital thermometer.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/onewire.h>

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif

#ifndef APP_DS18B20_BUS
#define APP_DS18B20_BUS ds18b20
/* Where the device can be found */
const struct sBSP430onewireBus ds18b20 = {
  .port = APP_DS18B20_PORT_HAL,
  .bit = APP_DS18B20_BIT,
};
#endif /* APP_DS18B20_BUS */

/* The serial number read from the device on startup */
struct sBSP430onewireSerialNumber serial;

void main ()
{
  int rc;
  const struct sBSP430onewireBus * const bus = &APP_DS18B20_BUS;

  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();

  BSP430_CORE_ENABLE_INTERRUPT();
  cprintf("\nHere we go...\n");

  cprintf("Uptime now %lu with frequency %lu Hz\n",
          ulBSP430uptime_ni(), ulBSP430uptimeConversionFrequency_Hz_ni());

  cprintf("Monitoring DS18xx on %s.%u bit %x\n",
          xBSP430portName(BSP430_PORT_HAL_GET_PERIPH_HANDLE(bus->port)) ?: "P?",
          iBSP430portBitPosition(bus->bit),
          bus->bit);

  do {
    rc = iBSP430onewireReadSerialNumber(bus, &serial);
    if (0 != rc) {
      cprintf("ERROR: Failed to read serial number from DS18B20: %d\n", rc);
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
    }
  } while (0 != rc);
  cprintf("DS18B20 serial number: %02x%02x%02x%02x%02x%02x\n",
          serial.id[0], serial.id[1], serial.id[2],
          serial.id[3], serial.id[4], serial.id[5]);

  while (1) {
    int rc;
    unsigned long start_tck;
    unsigned long end_tck;
    unsigned int duration_ms;
    int t_c;

    start_tck = ulBSP430uptime_ni();
    rc = -1;
    if (0 == iBSP430onewireRequestTemperature_ni(bus)) {
      /* Wait for read to complete.  Conversion time can be as long as
       * 750 ms if 12-bit resolution is used (this resolution is the
       * default). Timing will be wrong unless interrupts are enabled
       * so uptime overflow events can be handled.  Sleep for 600ms,
       * then test at 10ms intervals until the result is ready.  Each
       * check is nominally 61 microseconds. */
      BSP430_UPTIME_DELAY_MS_NI(600, LPM3_bits, 0);
      while (! iBSP430onewireReadBit_ni(bus)) {
        BSP430_UPTIME_DELAY_MS_NI(10, LPM3_bits, 0);
      }
      rc = iBSP430onewireReadTemperature_ni(bus, &t_c);
    }
    end_tck = ulBSP430uptime_ni();
    duration_ms = BSP430_UPTIME_UTT_TO_MS(end_tck - start_tck);

    cprintf("%s: ", xBSP430uptimeAsText_ni(end_tck));
    if (0 == rc) {
      cprintf("Temperature %d dCel or %d d[degF] in %u ms\n",
              (10 * t_c) / 16, BSP430_ONEWIRE_xCel_TO_ddegF(t_c),
              duration_ms);
    } else {
      cprintf("Measurement failed in %u ms\n", duration_ms);
    }

    /* You'd want to do this if you were going to sleep here */
    vBSP430onewireShutdown_ni(bus);
    BSP430_UPTIME_DELAY_MS_NI(5000, LPM3_bits, 0);
  }
}
