/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <string.h>
#include <bsp430/platform.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/sys.h>
#include <bsp430/sensors/bmp180.h>
#include <bsp430/sensors/utility.h>

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif /* BSP430_CONSOLE */
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif /* BSP430_UPTIME */

#ifndef RESAMPLE_INTERVAL_MS
#define RESAMPLE_INTERVAL_MS 10000
#endif /* RESAMPLE_INTERVAL_MS */

#ifndef WITH_VALIDATION
#define WITH_VALIDATION 1
#endif /* WITH_VALIDATION */

#if (WITH_VALIDATION - 0)
typedef struct sTestCase {
  sBSP430sensorsBMP180calibration calib;
  sBSP430sensorsBMP180sample sample;
} sTestCase;

const sTestCase tests[] = {
  /* Values from the data sheet, for algorithm validation.
   * Should produce temp_dC = 150, pres_Pa = 69964. */
  { .calib = { .ac1 = 408, .ac2 = -72, .ac3 = -14383, .ac4 = 32741,
               .ac5 = 32757, .ac6 = 23153, .b1 = 6190, .b2 = 4,
               .mb = -32768, .mc = -8711, .md = 2868 },
    .sample = { .oversampling = 0,
                .temperature_uncomp = 27898,
                .temperature_dK = BSP430_SENSORS_CONVERT_dC_TO_dK(150),
                .pressure_uncomp = 23843,
                .pressure_Pa = 69964 } },
};

int run_test_cases ()
{
  const sTestCase * tp = tests;
  const sTestCase * const etp = tests + sizeof(tests)/sizeof(*tests);
  const unsigned int num_tests = 2 * (etp - tp);
  unsigned int passes = 0;

  while (tp < etp) {
    sBSP430sensorsBMP180sample sample = tp->sample;
    sample.temperature_dK = 0;
    sample.pressure_Pa = 0;
    vBSP430sensorsBMP180convertSample(&tp->calib, &sample);
    if (tp->sample.temperature_dK != sample.temperature_dK) {
      cprintf("FAIL TC %u: T %d != %d\n", tp - tests,
              tp->sample.temperature_dK,  sample.temperature_dK);
    } else {
      ++passes;
    }
    if (tp->sample.pressure_Pa != sample.pressure_Pa) {
      cprintf("FAIL TC %u: P %ld != %ld\n", tp - tests,
              tp->sample.pressure_Pa,  sample.pressure_Pa);
    } else {
      ++passes;
    }
    ++tp;
  }
  cprintf("%u passes in %u tests\n", passes, num_tests);
  return (num_tests == passes);
}

#endif /* WITH_VALIDATION */


void main ()
{
  sBSP430sensorsBMP180calibration calib;
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_BMP085_I2C_PERIPH_HANDLE);
  unsigned long resample_wake_utt;
  unsigned long resample_interval_utt;
  int rc;

  vBSP430platformInitialize_ni();
  BSP430_CORE_ENABLE_INTERRUPT();

  resample_interval_utt = BSP430_UPTIME_MS_TO_UTT(RESAMPLE_INTERVAL_MS);

  (void)iBSP430consoleInitialize();
  cprintf("\nApplication starting\n");

  cprintf("BMP085 I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
          xBSP430serialName(APP_BMP085_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
          i2c, (unsigned long)BSP430_SERIAL_I2C_BUS_SPEED_HZ,
          APP_BMP085_I2C_ADDRESS);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("BMP085 I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_BMP085_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */

#if (WITH_VALIDATION - 0)
  run_test_cases();
#endif /* WITH_VALIDATION */

  /* XCLR not currently connected */
  /* 10ms delay before first communication */
  BSP430_UPTIME_DELAY_MS(10, LPM0_bits, 0);

  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  iBSP430serialSetReset_rh(i2c, 1);

  rc = iBSP430sensorsBMP180getCalibration(i2c, &calib);
  if (0 != rc) {
    cprintf("ERR: getCalibration returned %d\n", rc);
    return;
  }
  cprintf("Calibration values:\n");
  cprintf("AC1 = %d\n", calib.ac1);
  cprintf("AC2 = %d\n", calib.ac2);
  cprintf("AC3 = %d\n", calib.ac3);
  cprintf("AC4 = %u\n", calib.ac4);
  cprintf("AC5 = %u\n", calib.ac5);
  cprintf("AC6 = %u\n", calib.ac6);
  cprintf("B1 = %d\n", calib.b1);
  cprintf("B2 = %d\n", calib.b2);
  cprintf("MB = %d\n", calib.mb);
  cprintf("MC = %d\n", calib.mc);
  cprintf("MD = %d\n", calib.md);
  resample_wake_utt = ulBSP430uptime();
  do {
    char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];
    int temp_dF;
    int pres_cinHg;
    sBSP430sensorsBMP180sample sample;
    unsigned long start_utt;
    unsigned long end_utt;

    sample.oversampling = 3;
    start_utt = ulBSP430uptime();
    rc = iBSP430sensorsBMP180getSample(i2c, &sample);
    end_utt = ulBSP430uptime();
    if (0 > rc) {
      break;
    }
    vBSP430sensorsBMP180convertSample(&calib, &sample);
    temp_dF = BSP430_SENSORS_CONVERT_dK_TO_dFahr(sample.temperature_dK);
    pres_cinHg = BSP430_SENSORS_CONVERT_Pa_TO_cinHg(sample.pressure_Pa);
    cprintf("%s: Sample in %u ms\n\tTemperature %u raw or %d dK or %d d[Fahr]\n\tpressure %lu raw or %ld Pa or %d cinHg\n",
            xBSP430uptimeAsText(end_utt, as_text),
            (uint16_t)BSP430_UPTIME_UTT_TO_MS(end_utt - start_utt),
            sample.temperature_uncomp, sample.temperature_dK, temp_dF,
            sample.pressure_uncomp, sample.pressure_Pa, pres_cinHg);

    resample_wake_utt += resample_interval_utt;
    while (0 < lBSP430uptimeSleepUntil(resample_wake_utt, LPM3_bits)) {
    }
  } while (1);
  cprintf("Error: RC %d\n", rc);
}

