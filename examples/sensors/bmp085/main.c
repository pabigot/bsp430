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
#include <bsp430/periph/sys.h>
#include <string.h>

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

#define BMP085_REG_CALIBRATION 0xAA
#define BMP085_REG_CMD 0xF4
#define BMP085_REG_DATA 0xF6

#define BMP085_VAL_TEMP 0x2E
#define BMP085_VAL_PRESSURE(oss_) (0x34 + ((0x03 & (oss_)) << 6))

typedef struct sBMP085calibration {
  int16_t ac1;
  int16_t ac2;
  int16_t ac3;
  uint16_t ac4;
  uint16_t ac5;
  uint16_t ac6;
  int16_t b1;
  int16_t b2;
  int16_t mb;
  int16_t mc;
  int16_t md;
} sBMP085calibration;

sBMP085calibration calib;

#define TEMP_dC_TO_dF(dc_) (320 + ((dc_) * 9) / 5)
#define PRESSURE_Pa_TO_cinHg(pa_) (((pa_) * 100) / 3386)

void
getCalibration_ni (hBSP430halSERIAL i2c,
                   sBMP085calibration * cp)
{
  int rc;
  int i;
  uint8_t data[sizeof(*cp)];
  uint16_t * wp;

  data[0] = BMP085_REG_CALIBRATION;
  rc = iBSP430i2cTxData_ni(i2c, data, 1);
  if (0 > rc) {
    cprintf("I2C TX ERROR %d\n", rc);
    return;
  }
  memset(data, 0, sizeof(data));
  rc = iBSP430i2cRxData_ni(i2c, data, sizeof(data));
  if (0 > rc) {
    cprintf("I2C RX ERROR %d\n", rc);
    return;
  }
  cprintf("Read got %d: ", rc);
  for (i = 0; i < rc; ++i) {
    cprintf(" %02x", data[i]);
  }
  cputchar('\n');

  wp = (uint16_t *)cp;
  i = 0;
  while (i < rc) {
    *wp++ = (data[i] << 8) | data[i+1];
    i += 2;
  }
  cprintf("AC1 = %d\n", cp->ac1);
  cprintf("AC2 = %d\n", cp->ac2);
  cprintf("AC3 = %d\n", cp->ac3);
  cprintf("AC4 = %u\n", cp->ac4);
  cprintf("AC5 = %u\n", cp->ac5);
  cprintf("AC6 = %u\n", cp->ac6);
  cprintf("B1 = %d\n", cp->b1);
  cprintf("B2 = %d\n", cp->b2);
  cprintf("MB = %d\n", cp->mb);
  cprintf("MC = %d\n", cp->mc);
  cprintf("MD = %d\n", cp->md);
}

void main ()
{
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_BMP085_I2C_PERIPH_HANDLE);
  unsigned long resample_wake_utt;
  unsigned long resample_interval_utt;
  int rc;

  vBSP430platformInitialize_ni();
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

  /* XCLR not currently connected */
  /* 10ms delay before first communication */
  BSP430_UPTIME_DELAY_MS_NI(10, LPM0_bits, 0);

  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  (void)iBSP430i2cSetAddresses_ni(i2c, -1, APP_BMP085_I2C_ADDRESS);

  getCalibration_ni(i2c, &calib);
  resample_wake_utt = ulBSP430uptime_ni();
  do {
    sBMP085calibration * const cp = &calib;
    uint8_t data[4];
    int32_t ut;
    int32_t up;
    int temp_dC;
    int temp_dF;
    long pres_Pa;
    long pres_cinHg;
    int oss = 3;

    data[0] = BMP085_REG_CMD;
    data[1] = BMP085_VAL_TEMP;
    rc = iBSP430i2cTxData_ni(i2c, data, 2);
    if (0 > rc) {
      break;
    }

    /* 4.5 ms but make it 5 */
    BSP430_UPTIME_DELAY_MS_NI(5, LPM0_bits, 0);

    data[0] = BMP085_REG_DATA;
    rc = iBSP430i2cTxData_ni(i2c, data, 1);
    if (0 > rc) {
      break;
    }
    rc = iBSP430i2cRxData_ni(i2c, data, 2);
    if (0 > rc) {
      break;
    }
    ut = data[0];
    ut = (ut << 8) | data[1];

    data[0] = BMP085_REG_CMD;
    data[1] = BMP085_VAL_PRESSURE(oss);
    rc = iBSP430i2cTxData_ni(i2c, data, 2);
    if (0 > rc) {
      break;
    }

    /* 3 ms plus 1.5 ms for each sample. */
    BSP430_UPTIME_DELAY_MS_NI(3 + (2 << oss), LPM0_bits, 0);
    data[0] = BMP085_REG_DATA;
    rc = iBSP430i2cTxData_ni(i2c, data, 1);
    if (0 > rc) {
      break;
    }
    rc = iBSP430i2cRxData_ni(i2c, data, 3);
    if (0 > rc) {
      break;
    }
    up = data[0];
    up = (up << 8) | data[1];
    up = (up << 8) | data[2];
    up >>= 8 - oss;

#if 0
    /* Values from the data sheet, for algorithm validation.
     * Should produce temp_dC = 150, pres_Pa = 69964. */
    oss = 0;
    cp->ac1 = 408;
    cp->ac2 = -72;
    cp->ac3 = -14383;
    cp->ac4 = 32741;
    cp->ac5 = 32757;
    cp->ac6 = 23153;
    cp->b1 = 6190;
    cp->b2 = 4;
    cp->mb = -32768;
    cp->mc = -8711;
    cp->md = 2868;

    ut = 27898;
    up = 23843;
#endif

    do {
      int32_t x1;
      int32_t x2;
      int32_t x3;
      int32_t b3;
      uint32_t b4;
      int32_t b5;
      int32_t b6;
      uint32_t b7;
      int32_t p;

      x1 = (((int32_t)ut - cp->ac6) * cp->ac5) >> 15;
      x2 = ((int32_t)cp->mc << 11) / (x1 + cp->md);
      b5 = x1 + x2;
      temp_dC = (b5 + 8) >> 4;
      b6 = b5 - 4000;
      x1 = (cp->b2 * ((b6 * b6) >> 12)) >> 11;
      x2 = (cp->ac2 * b6) >> 11;
      x3 = x1 + x2;
      b3 = ((((cp->ac1 * 4) + x3) << oss) + 2) / 4;
      x1 = (cp->ac3 * b6) >> 13;
      x2 = (cp->b1 * ((b6 * b6) >> 12)) >> 16;
      x3 = ((x1 + x2) + 2) >> 2;
      b4 = (cp->ac4 * (uint32_t)(x3 + 32768UL)) >> 15;
      b7 = ((uint32_t)up - b3) * (50000 >> oss);
      if (0x80000000UL > b7) {
        p = (b7 * 2) / b4;
      } else {
        p = (b7 / b4) * 2;
      }
      x1 = (p >> 8) * (p >> 8);
      x1 = (x1 * 3038) >> 16;
      x2 = (-7357 * p) >> 16;
      pres_Pa = p + ((x1 + x2 + 3791) >> 4);
    } while (0);

    temp_dF = TEMP_dC_TO_dF(temp_dC);
    cprintf("%s: Raw temperature %lu becomes %d dC and %d dF\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), ut, temp_dC, temp_dF);
    pres_cinHg = PRESSURE_Pa_TO_cinHg(pres_Pa);
    cprintf("\tRaw pressure %lu becomes %lu Pa or %lu cinHg\n", up, pres_Pa, pres_cinHg);

    resample_wake_utt += resample_interval_utt;
    while (0 < lBSP430uptimeSleepUntil_ni(resample_wake_utt, LPM3_bits)) {
    }
  } while (1);
  cprintf("Error: RC %d\n", rc);
}

