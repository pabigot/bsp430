/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/timer.h>
#include <bsp430/periph/sys.h>
#include <bsp430/sensors/sht21.h>
#include <bsp430/sensors/utility.h>
#include <string.h>

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif /* BSP430_CONSOLE */
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif /* BSP430_UPTIME */

/* Enable the heater and print the temperature and humidity measured
 * at 1Hz for some seconds, then turn the heater off and continue
 * printing at 1 Hz for another some seconds, to demonstrate the sensor
 * works.
 *
 * A couple seconds is adequate to verify behavior: too long and the
 * sensor dries out enough that the humidity is pretty far off by the
 * time real measurement occurs. */
void
do_diagnostics (hBSP430halSERIAL i2c)
{
  const int cfg_diag = BSP430_SENSORS_SHT21_CONFIG_H11T11;
  int cfg_in = iBSP430sensorsSHT21configuration (i2c, -1, -1);
  const int diagnostic_cycles = 7;
  const int heater_off_cycle = diagnostic_cycles / 2;
  int cycle;
  sBSP430sensorsSHT21sample sample;
  sBSP430sensorsSHT21sample first_sample;
  sBSP430sensorsSHT21sample off_sample;
  sBSP430sensorsSHT21sample last_sample;
  int rc;

  memset(&sample, 0, sizeof(sample));
  cprintf("Initial config: %02x\n", cfg_in);
  rc = iBSP430sensorsSHT21configuration (i2c, cfg_diag, 1);
  cprintf("Turned heater on and configured for %x got %x\n", cfg_diag, rc);

  rc = iBSP430sensorsSHT21getSample(i2c, &sample);
  first_sample = sample;
  off_sample = sample;
  last_sample = sample;
  for (cycle = 0; cycle < diagnostic_cycles; ++cycle) {
    unsigned long t0;
    unsigned long t1;

    t0 = ulBSP430uptime_ni();
    rc = iBSP430sensorsSHT21getSample(i2c, &sample);
    t1 = ulBSP430uptime_ni();
    cprintf("[%u] rc=%d, cfg %02x : %u RH %u ; delta T %d RH %d : in %u ms\n",
            cycle, rc, iBSP430sensorsSHT21configuration (i2c, -1, -1),
            sample.temperature_dK, sample.humidity_ppth,
            sample.temperature_dK - last_sample.temperature_dK,
            sample.humidity_ppth - last_sample.humidity_ppth,
            (unsigned int)BSP430_UPTIME_UTT_TO_MS(t1-t0));
    if (cycle == heater_off_cycle) {
      off_sample = sample;
      rc = iBSP430sensorsSHT21configuration (i2c, -1, 0);
      cprintf("Turn heater off got %x\n", rc);
    }
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
    last_sample = sample;
  }
  cprintf("Deltas with heater on: T %d dK RH %d ppth\n",
          off_sample.temperature_dK - first_sample.temperature_dK,
          off_sample.humidity_ppth - first_sample.humidity_ppth);
  cprintf("Deltas with heater off: T %d dK RH %d ppth\n",
          last_sample.temperature_dK - off_sample.temperature_dK,
          last_sample.humidity_ppth - off_sample.humidity_ppth);

  /* Explicitly turn off heater (not affected by soft reset) */
  rc = iBSP430sensorsSHT21configuration (i2c, -1, 0);
  cprintf("Disable heater got %d\n", rc);

  rc = iBSP430sensorsSHT21reset (i2c);
  /* SHT21 wants max 15ms delay to complete reset. */
  BSP430_UPTIME_DELAY_MS_NI(BSP430_SENSORS_SHT21_RESET_DELAY_MS, LPM3_bits, 0);

  cprintf("Reset got %d\n", rc);
}

void main ()
{
  const int sht21_config = BSP430_SENSORS_SHT21_CONFIG_H12T14;
  uint8_t eic[BSP430_SENSORS_SHT21_EIC_LENGTH];
  int is_htu21d = 0;
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_I2C_PERIPH_HANDLE);
  int rc;

  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();
  cprintf("\nsht21 " __DATE__ " " __TIME__ "\n");

  cprintf("I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
          xBSP430serialName(APP_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
          i2c, (unsigned long)BSP430_SERIAL_I2C_BUS_SPEED_HZ,
          BSP430_SENSORS_SHT21_I2C_ADDRESS);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */

  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  memset(eic, 0, sizeof(eic));
  rc = iBSP430sensorsSHT21eic(i2c, eic);
  cprintf("EIC read got %d:", rc);
  vBSP430consoleDisplayOctets(eic, sizeof(eic));
  cputchar('\n');
  if (0 == rc) {
    /* Diagnose chip type based on EIC SNA code */
    if (BSP430_SENSORS_SHT21_EIC_IS_SHT21(eic)) {
      cprintf("Device is SHT21\n");
    } else if (BSP430_SENSORS_SHT21_EIC_IS_HTU21D(eic)) {
      cprintf("Device is HTU21D\n");
      is_htu21d = 1;
    } else {
      cprintf("Unable to determine device type\n");
    }
  }

  rc = iBSP430sensorsSHT21reset (i2c);
  /* SHT21 wants max 15ms delay to complete reset. */
  BSP430_UPTIME_DELAY_MS_NI(BSP430_SENSORS_SHT21_RESET_DELAY_MS, LPM3_bits, 0);
  cprintf("Soft reset got %d\n", rc);
  rc = iBSP430sensorsSHT21configuration (i2c, -1, -1);
  cprintf("Post-reset device config: %02x\n", rc);

  do_diagnostics(i2c);
  cprintf("Post-diagnostics config: %02x\n", iBSP430sensorsSHT21configuration (i2c, -1, -1));

  rc = iBSP430sensorsSHT21configuration (i2c, sht21_config, 0);
  cprintf("Reconfig got %02x\n", rc);
  rc = iBSP430sensorsSHT21configuration (i2c, -1, -1);
  cprintf("Test run config config: %02x\n", rc);
  if (BSP430_SENSORS_SHT21_CONFIG_HEATER_MASK & rc) {
    cprintf("WARNING: Heater is on!\n");
  }

  /* Need interrupts enabled for uptime overflow, but off when
   * manipulating timers and doing I2C.  Leave disabled except when
   * sleeping. */
  while (1) {
    char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];
    int hold_master;
    unsigned long t0;
    unsigned long t1;
    uint16_t t_raw;
    unsigned int t_ms;
    uint16_t rh_raw;
    unsigned  rh_ms;
    int nhiters = 0;

    /* For temperature we'll use hold-master mode. */
    hold_master = 1;
    t0 = ulBSP430uptime_ni();
    rc = iBSP430sensorsSHT21initiateMeasurement(i2c, hold_master, eBSP430sensorsSHT21measurement_TEMPERATURE);
    if (0 != rc) {
      cprintf("T_HM failed %d\n", rc);
      break;
    }
    if (is_htu21d) {
      BSP430_UPTIME_DELAY_MS_NI(50, LPM0_bits, 0);
    }
    rc = iBSP430sensorsSHT21getMeasurement(i2c, hold_master, &t_raw);
    t1 = ulBSP430uptime_ni();
    (void)iBSP430serialSetReset_rh(i2c, 1);
    t_ms = BSP430_UPTIME_UTT_TO_MS(t1-t0);
    if (0 > rc) {
      break;
    }
    if (0 != rc) {
      cprintf("ERR: Temperature CRC failed %02x\n", rc);
    }

    /* For humidity we'll use no-hold-master mode. */
    hold_master = 0;
    t0 = ulBSP430uptime_ni();
    rc = iBSP430sensorsSHT21initiateMeasurement(i2c, hold_master, eBSP430sensorsSHT21measurement_HUMIDITY);
    if (0 != rc) {
      cprintf("RN_NH failed %d\n", rc);
      break;
    }
    do {
      rc = iBSP430sensorsSHT21getMeasurement(i2c, hold_master, &rh_raw);
      if (0 > rc) {
        /* NACK from I2C means bus should be reset, which we'll do
         * while we wait to try again. */
        (void)iBSP430serialSetReset_rh(i2c, 1);
        BSP430_UPTIME_DELAY_MS_NI(1, LPM0_bits, 0);
        (void)iBSP430serialSetReset_rh(i2c, 0);
        ++nhiters;
      }
    } while (0 > rc);
    if (0 != rc) {
      cprintf("ERR: Humidity CRC failed %02x\n", rc);
    }
    t1 = ulBSP430uptime_ni();
    rh_ms = BSP430_UPTIME_UTT_TO_MS(t1-t0);

    (void)iBSP430serialSetReset_rh(i2c, 1);
    cprintf("%s:\n", xBSP430uptimeAsText(t0, as_text));
    cprintf("\tTemperature %u raw or %u cK or %u dK or %d d[Fahr] in %u ms\n",
            t_raw,
            BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_cK(t_raw),
            BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_dK(t_raw),
            BSP430_SENSORS_CONVERT_dK_TO_dFahr(BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_dK(t_raw)),
            t_ms);
    cprintf("\tHumidity %u raw %u ppth in %u iters %u ms \n",
            rh_raw,
            BSP430_SENSORS_SHT21_HUMIDITY_RAW_TO_ppth(rh_raw),
            nhiters, rh_ms);
    BSP430_UPTIME_DELAY_MS_NI(5000, LPM3_bits, 0);
  }
  cprintf("Aborted due to error result code %d : %x\n", rc, -rc);
}
