/** This file is in the public domain.
 *
 * This demonstrates using the BSP430 OneWire interface to read the
 * temperature from an externally-powered Maxim DS18B20 (or similar)
 * digital thermometer.
 *
 * For parasitic power: Keep the 4K7 resistor between DQ and Vcc,
 * connect DS18B20 Vdd to GND, and specify a second bit on the device
 * port that will supply power during temperature conversions.
 *
 * There is perhaps 0.2uA discernable difference in idle current
 * consumption between parasitic and external power supply, but this
 * is at the extreme limit of what my test equipment can measure.
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

#ifndef APP_DS18B20_POWER_BIT
/* Bit on APP_DS18B20_BUS.port that supplies power during temperature
 * conversions. */
#define APP_DS18B20_POWER_BIT 0
#endif /* APP_DS18B20_POWER_BIT */

/* The serial number read from the device on startup */
struct sBSP430onewireSerialNumber serial;

void main ()
{
  int rc;
  int external_power;
  static const char * const supply_type[] = { "parasitic", "external" };
  const struct sBSP430onewireBus * const bus = &APP_DS18B20_BUS;

  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();

  BSP430_CORE_ENABLE_INTERRUPT();
  cprintf("\nHere we go...\n");

  cprintf("Uptime now %lu with frequency %lu Hz\n",
          ulBSP430uptime_ni(), ulBSP430uptimeConversionFrequency_Hz());

  cprintf("Monitoring DS18xx on %s.%u ; ",
          xBSP430portName(BSP430_PORT_HAL_GET_PERIPH_HANDLE(bus->port)) ?: "P?",
          iBSP430portBitPosition(bus->bit));
  if (APP_DS18B20_POWER_BIT) {
    cprintf("parasitic conversion power from %s.%u\n",
            xBSP430portName(BSP430_PORT_HAL_GET_PERIPH_HANDLE(bus->port)) ?: "P?",
            iBSP430portBitPosition(APP_DS18B20_POWER_BIT));
    BSP430_PORT_HAL_HPL_DIR(bus->port) &= ~APP_DS18B20_POWER_BIT;
  } else {
    cprintf("no parasitic power boost\n");
  }

  external_power = iBSP430onewireReadPowerSupply(bus);
  cprintf("Power supply: %s\n", supply_type[external_power]);
  if (0 > external_power) {
    cprintf("ERROR: Device not present?\n");
  } else if ((! external_power) && (! (APP_DS18B20_POWER_BIT))) {
    cprintf("ERROR: Parasitic power without conversion boost?\n");
  }

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
      if (external_power) {
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
      } else {
        /* Output high on the parasitic power boost line for 750ms, to
         * power the conversion.  Then switch that signal back to
         * input so the data can flow over the same circuit. */
        BSP430_PORT_HAL_HPL_OUT(bus->port) |= APP_DS18B20_POWER_BIT;
        BSP430_PORT_HAL_HPL_DIR(bus->port) |= APP_DS18B20_POWER_BIT;
        BSP430_UPTIME_DELAY_MS_NI(750, LPM3_bits, 0);
        BSP430_PORT_HAL_HPL_DIR(bus->port) &= ~APP_DS18B20_POWER_BIT;
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
    BSP430_UPTIME_DELAY_MS_NI(30000, LPM3_bits, 0);
  }
}
