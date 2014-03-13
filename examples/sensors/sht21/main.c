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

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif /* BSP430_CONSOLE */
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif /* BSP430_UPTIME */

void main ()
{
  uint8_t eic[BSP430_SENSORS_SHT21_EIC_LENGTH];
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_I2C_PERIPH_HANDLE);
  int i;
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

  rc = iBSP430sensorsSHT21eic(i2c, eic);
  cprintf("EIC read got %d:", rc);
  for (i = 0; i < sizeof(eic); ++i) {
    cprintf(" %02x", eic[i]);
  }
  cputchar('\n');

  cprintf("First config: %02x\n", iBSP430sensorsSHT21configuration (i2c, -1, -1));
  rc = iBSP430sensorsSHT21configuration (i2c, BSP430_SENSORS_SHT21_CONFIG_H11T11, 1);
  cprintf("Reconfig got %02x\n", rc);
  cprintf("After config: %02x\n", iBSP430sensorsSHT21configuration (i2c, -1, -1));

  rc = iBSP430sensorsSHT21reset (i2c);
  /* SHT21 wants max 15ms delay to complete reset. */
  BSP430_UPTIME_DELAY_MS_NI(BSP430_SENSORS_SHT21_RESET_DELAY_MS, LPM3_bits, 0);

  cprintf("Reset got %d\n", rc);
  cprintf("Actual config: %02x\n", iBSP430sensorsSHT21configuration (i2c, -1, -1));

  /* Need interrupts enabled for uptime overflow, but off when
   * manipulating timers and doing I2C.  Leave disabled except when
   * sleeping. */
  while (1) {
    char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];
    unsigned long t0;
    unsigned long t1;
    unsigned int t_raw;
    unsigned int t_ms;
    unsigned int rh_raw;
    unsigned int rh_ms;
    int nhiters = 0;

    t0 = ulBSP430uptime_ni();
    rc = iBSP430sensorsSHT21initiateMeasurement(i2c, 1, eBSP430sensorsSHT21measurement_TEMPERATURE);
    if (0 != rc) {
      cprintf("T_HM failed %d\n", rc);
      break;
    }
    rc = iBSP430sensorsSHT21getSample(i2c, 1, &t_raw);
    t1 = ulBSP430uptime_ni();
    t_ms = BSP430_UPTIME_UTT_TO_MS(t1-t0);
    if (0 > rc) {
      break;
    }
    if (0 != rc) {
      cprintf("ERR: Temperature CRC failed %02x\n", rc);
    }

    t0 = ulBSP430uptime_ni();
    rc = iBSP430sensorsSHT21initiateMeasurement(i2c, 0, eBSP430sensorsSHT21measurement_HUMIDITY);
    if (0 != rc) {
      cprintf("RN_NH failed %d\n", rc);
      break;
    }
    do {
      rc = iBSP430sensorsSHT21getSample(i2c, 0, &rh_raw);
      if (0 > rc) {
        BSP430_UPTIME_DELAY_MS_NI(1, LPM0_bits, 0);
        ++nhiters;
      }
    } while (0 > rc);
    if (0 != rc) {
      cprintf("ERR: Humidity CRC failed %02x\n", rc);
    }
    t1 = ulBSP430uptime_ni();
    rh_ms = BSP430_UPTIME_UTT_TO_MS(t1-t0);

    iBSP430serialSetReset_rh(i2c, 1);
    cprintf("%s:\n", xBSP430uptimeAsText(t0, as_text));
    cprintf("\tTemperature %u raw or %u cK or %u dK or %d d[Fahr] in %u ms\n",
            t_raw,
            BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_cK(t_raw),
            BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_dK(t_raw),
            BSP430_SENSORS_UTILITY_dK_TO_dFahr(BSP430_SENSORS_SHT21_TEMPERATURE_RAW_TO_dK(t_raw)),
            t_ms);
    cprintf("\tHumidity %u raw %u ppth in %u iters %u ms \n",
            rh_raw,
            BSP430_SENSORS_SHT21_HUMIDITY_RAW_TO_ppth(rh_raw),
            nhiters, rh_ms);
    BSP430_UPTIME_DELAY_MS_NI(5000, LPM3_bits, 0);
  }
  cprintf("Aborted due to error result code %d : %x\n", rc, -rc);
}
