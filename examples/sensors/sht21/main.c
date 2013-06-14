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

/* Address for the thing. */
#define APP_I2C_ADDRESS 0x40

#define SHT21_TRIGGER_T_HM 0xE3
#define SHT21_TRIGGER_RH_HM 0xE5
#define SHT21_TRIGGER_T (0x10 | SHT21_TRIGGER_T_HM)
#define SHT21_SOFT_RESET 0xFE

/** Convert a raw humidity value to per-mille */
#define HUMIDITY_RAW_TO_PPT(raw_) (unsigned int)(((1250UL * (raw_)) >> 16) - 60)
/** Convert a raw temperature value to deci-degrees Celcius */
#define TEMPERATURE_RAW_TO_dC(raw_) ((-937 + (int)((((raw_) * 4393L) / 5) >> 14)) / 2)
#define TEMPERATURE_dC_TO_dF(dc_) (320 + ((9 * (dc_)) / 5))

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif /* BSP430_CONSOLE */
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif /* BSP430_UPTIME */

int
iSHT21ComputeCRC (const unsigned char * romp,
                        int len)
{
  static const uint16_t SHT_CRC_POLY = 0x131;
  uint8_t crc = 0;

  while (0 < len--) {
    int bi;

    crc ^= *romp++;
    for (bi = 8; bi > 0; --bi) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ SHT_CRC_POLY;
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

int
iSHT21readMeasurement_ni (hBSP430halSERIAL i2c,
                          unsigned int * rawp)
{
  int rc;
  uint8_t data[3];

  rc = iBSP430i2cRxData_ni(i2c, data, sizeof(data));
  if (sizeof(data) != rc) {
    return rc;
  }
  if (0 != iSHT21ComputeCRC(data, sizeof(data))) {
    cprintf("CRC failed\n");
    return -1;
  }
#if 0
  cprintf("Data: %02x %02x %02x\n", data[0], data[1], data[2]);
#endif
  *rawp = (data[0] << 8) | data[1];
  *rawp &= ~0x03;
  return 0;
}

void main ()
{
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_I2C_PERIPH_HANDLE);
  int rc;

  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();
  cprintf("\nApplication starting\n");

  cprintf("I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
          xBSP430serialName(APP_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
          i2c, (unsigned long)BSP430_SERIAL_I2C_BUS_SPEED_HZ,
          APP_I2C_ADDRESS);
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

  vBSP430serialSetReset_ni(i2c, 1);
  (void)iBSP430i2cSetAddresses_ni(i2c, -1, APP_I2C_ADDRESS);

  {
    uint8_t cmd = SHT21_SOFT_RESET;
    vBSP430serialSetReset_ni(i2c, 0);
    rc = iBSP430i2cTxData_ni(i2c, &cmd, sizeof(cmd));
    vBSP430serialSetReset_ni(i2c, 1);
    cprintf("Reset got %d\n", rc);
  }

  /* SHT21 wants max 15ms on power-up. */
  BSP430_UPTIME_DELAY_MS_NI(15, LPM3_bits, 0);

  /* Need interrupts enabled for uptime overflow, but off when
   * manipulating timers and doing I2C.  Leave disabled except when
   * sleeping. */
  while (1) {
    unsigned long t0;
    unsigned long t1;
    unsigned int t_raw;
    unsigned int t_ms;
    int t_dC;
    uint8_t cmd;
    unsigned int rh_raw;
    unsigned int rh_ms;
    int rh_ppt;

    vBSP430serialSetReset_ni(i2c, 0);
    t0 = ulBSP430uptime_ni();
    cmd = SHT21_TRIGGER_T_HM;
    rc = iBSP430i2cTxData_ni(i2c, &cmd, sizeof(cmd));
    if (1 != rc) {
      cprintf("T_HM failed %d\n", rc);
      break;
    }
    rc = iSHT21readMeasurement_ni(i2c, &t_raw);
    if (0 > rc) {
      break;
    }
    t1 = ulBSP430uptime_ni();
    t_ms = BSP430_UPTIME_UTT_TO_MS(t1-t0);

    t0 = ulBSP430uptime_ni();
    cmd = SHT21_TRIGGER_RH_HM;
    rc = iBSP430i2cTxData_ni(i2c, &cmd, sizeof(cmd));
    if (1 != rc) {
      cprintf("RH_HM failed %d\n", rc);
      break;
    }
    rc = iSHT21readMeasurement_ni(i2c, &rh_raw);
    if (0 > rc) {
      break;
    }
    t1 = ulBSP430uptime_ni();
    rh_ms = BSP430_UPTIME_UTT_TO_MS(t1-t0);

    vBSP430serialSetReset_ni(i2c, 1);
    t_dC = TEMPERATURE_RAW_TO_dC(t_raw);
    rh_ppt = HUMIDITY_RAW_TO_PPT(rh_raw);
    cprintf("%s: ", xBSP430uptimeAsText_ni(t0));
    cprintf("T %u raw %d dC %d dF %u ms ; ", t_raw, t_dC, TEMPERATURE_dC_TO_dF(t_dC), t_ms);
    cprintf("RH %u raw %u ppt %u ms\n", rh_raw, rh_ppt, rh_ms);
    BSP430_UPTIME_DELAY_MS_NI(5000, LPM3_bits, 0);
  }
  cprintf("Aborted due to error result code %d : %x\n", rc, -rc);
}
