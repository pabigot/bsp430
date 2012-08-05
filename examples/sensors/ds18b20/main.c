/** This file is in the public domain.
 *
 * This demonstrates using the BSP430 OneWire interface to read the
 * temperature from an externally-powered Maxim DS18B20 (or similar)
 * digital thermometer.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
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

/* Where the device can be found */
const struct sBSP430onewireBus ds18b20 = {
  .port = APP_DS18B20_PORT_HAL,
  .bit = APP_DS18B20_BIT,
  .use_ren =
  /* The Fraunchpad can read the sensor whether or not REN is enabled,
   * but can't detect absence of the sensor unless it's enabled.  For
   * undiagnosed reasons but possibly due to capacitive sense support on
   * P1.7, using REN on onewire prevents the LaunchPad from reading from
   * the sensor; we'll live with it not being able to detect absence. */
#if BSP430_PLATFORM_EXP430G2 - 0
  0
#else /* BSP430_PLATFORM_EXP430G2 */
  1
#endif /* BSP430_PLATFORM_EXP430G2 */
};

/* The serial number read from the device on startup */
struct sBSP430onewireSerialNumber serial;

void main ()
{
  int rc;
  unsigned long uptime_ticks_per_sec;
  const struct sBSP430onewireBus * bus = &ds18b20;

  vBSP430platformInitialize_ni();

#if BSP430_PLATFORM_SPIN_FOR_JUMPER - 0
  vBSP430platformSpinForJumper_ni();
#endif /* BSP430_PLATFORM_SPIN_FOR_JUMPER */

  (void)hBSP430consoleInitialize();
  cprintf("\nHere we go...\n");

  uptime_ticks_per_sec = ulBSP430uptimeResolution_Hz_ni();
  cprintf("Uptime now %lu with resolution %lu\n",
          ulBSP430uptime_ni(), uptime_ticks_per_sec);

  cprintf("Monitoring DS18xx on %s.%u bit %x\n",
          xBSP430portName(BSP430_PORT_HAL_GET_PERIPH_HANDLE(APP_DS18B20_PORT_HAL)) ?: "P?",
          iBSP430portBitPosition(APP_DS18B20_BIT),
          APP_DS18B20_BIT);

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
    unsigned int delay_count = 0;
    int t_c;

    start_tck = ulBSP430uptime_ni();
    rc = -1;
    if (0 == iBSP430onewireRequestTemperature_ni(bus)) {
      /* Wait for read to complete.  Each read is nominally 61
       * microseconds.  Conversion time can be as long as 750 ms if
       * 12-bit resolution is used.  (12-bit resolution is the
       * default.) */
      while (! iBSP430onewireReadBit_ni(bus)) {
        ++delay_count;
      }
      rc = iBSP430onewireReadTemperature_ni(bus, &t_c);
    }
    end_tck = ulBSP430uptime_ni();
    duration_ms = 1000 * (end_tck - start_tck) / uptime_ticks_per_sec;

    if (0 == rc) {
      cprintf("Temperature %d dCel or %d d[degF] in %u ms\n",
              (10 * t_c) / 16, BSP430_ONEWIRE_xCel_TO_ddegF(t_c), duration_ms);
    } else {
      cprintf("Measurement failed in %u ms\n", duration_ms);
    }

    /* You'd want to do this if you were going to sleep here */
    vBSP430onewireShutdown_ni(bus);
  }
}
