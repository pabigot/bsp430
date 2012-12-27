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
#include <string.h>

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif /* BSP430_CONSOLE */
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif /* BSP430_UPTIME */

volatile int alarm_fired;
static int
alarmCallback_ni (hBSP430timerAlarm alarm)
{
  (void)alarm;
  alarm_fired = 1;
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM | BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
}

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

struct sBSP430timerAlarm alarm_struct;
sBMP085calibration calib;
hBSP430timerAlarm alarm;
unsigned long alarm_freq_Hz;

#define DELAY_MS_NI(ms_) do {                                              \
    alarm_fired = 0;                                                    \
    rc = iBSP430timerAlarmSet_ni(alarm, ulBSP430uptime_ni() + BSP430_CORE_MS_TO_TICKS(ms_, alarm_freq_Hz)); \
    while (! alarm_fired) {                                             \
      BSP430_CORE_LPM_ENTER_NI(LPM0_bits | GIE);                        \
    }                                                                   \
  } while (0)


#define TEMP_dC_TO_dF(dc_) (320 + ((dc_) * 9) / 5)
#define PRESSURE_Pa_TO_cinHg(pa_) (((pa_) * 100) / 3386)

void
getCalibration (hBSP430halSERIAL i2c,
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
  int rc;

  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();
  cprintf("\nApplication starting\n");

  alarm = hBSP430timerAlarmInitialize(&alarm_struct, BSP430_UPTIME_TIMER_PERIPH_HANDLE, 1, alarmCallback_ni);
  if (NULL == alarm) {
    cprintf("Failed to initialize alarm\n");
    return;
  }
  alarm_freq_Hz = ulBSP430timerFrequency_Hz_ni(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
  rc = iBSP430timerAlarmSetEnabled_ni(alarm, 1);

  cprintf("Alarm enable returned %d\n", rc);

  cprintf("BMP085 I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
          xBSP430serialName(APP_BMP085_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
          i2c, ulBSP430clockSMCLK_Hz_ni() / APP_BMP085_I2C_PRESCALER,
          APP_BMP085_I2C_ADDRESS);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("BMP085 I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_BMP085_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */

  /* XCLR not currently connected */
  /* 10ms delay before first communication */
  DELAY_MS_NI(10);

  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             UCSSEL_2, APP_BMP085_I2C_PRESCALER);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  (void)iBSP430i2cSetAddresses_ni(i2c, -1, APP_BMP085_I2C_ADDRESS);

  getCalibration(i2c, &calib);
  DELAY_MS_NI(10);

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

    DELAY_MS_NI(5);

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

    DELAY_MS_NI(20 + (3 << oss));
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
    cprintf("Raw temperature %lu becomes %d dC and %d dF\n", ut, temp_dC, temp_dF);
    pres_cinHg = PRESSURE_Pa_TO_cinHg(pres_Pa);
    cprintf("Raw pressure %lu becomes %lu Pa or %lu cinHg\n", up, pres_Pa, pres_cinHg);

    DELAY_MS_NI(10000UL);
  } while (1);
  cprintf("Error: RC %d\n", rc);
}

