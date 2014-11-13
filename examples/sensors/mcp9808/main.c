/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/timer.h>
#include <bsp430/periph/sys.h>
#include <bsp430/sensors/utility.h>
#include <bsp430/sensors/mcp9808.h>
#include <stdio.h>
#include <string.h>

int
read_r16_rh (hBSP430halSERIAL i2c,
             uint8_t regno, uint16_t * rp)
{
  int rc;
  uint8_t buf[2];

  rc = iBSP430i2cTxData_rh(i2c, &regno, sizeof(regno));
  if (0 < rc) {
    rc = iBSP430i2cRxData_rh(i2c, buf, sizeof(buf));
  }
  if (0 < rc) {
    *rp = ((uint16_t)buf[0] << 8) | buf[1];
    return 0;
  }
  return rc;
}

int
read_reg_rh (hBSP430halSERIAL i2c,
             uint8_t regno)
{
  uint16_t v;
  int rc = read_r16_rh(i2c, regno, &v);
  if (0 == rc) {
    return v;
  }
  printf("ERR: Got %d reading r%u\n", rc, regno);
  while (1);
}

int
write_r16_rh (hBSP430halSERIAL i2c,
              uint8_t regno,
              uint16_t v)
{
  int rc;
  uint8_t buf[3];

  buf[0] = regno;
  buf[1] = v >> 8;
  buf[2] = v;
  rc = iBSP430i2cTxData_rh(i2c, buf, sizeof(buf));
  return -(sizeof(buf) != rc);
}

int
read_r8_rh (hBSP430halSERIAL i2c,
            uint8_t regno)
{
  int rc;
  uint8_t rv;

  rc = iBSP430i2cTxData_rh(i2c, &regno, sizeof(regno));
  if (0 < rc) {
    rc = iBSP430i2cRxData_rh(i2c, &rv, sizeof(rv));
  }
  return (0 < rc) ? rv : -1;
}

int
write_r8_rh (hBSP430halSERIAL i2c,
             uint8_t regno,
             uint8_t v)
{
  int rc;
  uint8_t buf[2];

  buf[0] = regno;
  buf[1] = v;
  rc = iBSP430i2cTxData_rh(i2c, buf, sizeof(buf));
  return -(sizeof(buf) != rc);
}

void
main ()
{
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_MCP9808_I2C_PERIPH_HANDLE);
  uint16_t cfg;
  int rc = -1;

  vBSP430platformInitialize_ni();

  printf("\nApplication starting\n");
  printf("MCP9808 I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
         xBSP430serialName(APP_MCP9808_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
         i2c, (unsigned long)BSP430_SERIAL_I2C_BUS_SPEED_HZ,
         APP_MCP9808_I2C_ADDRESS);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  printf("MCP9808 I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_MCP9808_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */

  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    printf("I2C open failed.\n");
    return;
  }

  iBSP430serialSetReset_rh(i2c, 1);
  (void)iBSP430i2cSetAddresses_rh(i2c, -1, APP_MCP9808_I2C_ADDRESS);

  iBSP430serialSetReset_rh(i2c, 0);
  printf("Device ID %04x:%04x\n",
         read_reg_rh(i2c, BSP430_SENSORS_MCP9808_REGNO_MFGID),
         read_reg_rh(i2c, BSP430_SENSORS_MCP9808_REGNO_DEVID));
  rc = read_r16_rh(i2c, BSP430_SENSORS_MCP9808_REGNO_CONFIG, &cfg);
  printf("Config %04x resolution 1/%d\n", cfg,
         2 << read_r8_rh(i2c, BSP430_SENSORS_MCP9808_REGNO_RESOLUTION));
  iBSP430serialSetReset_rh(i2c, 1);
  cfg &= ~BSP430_SENSORS_MCP9808_CONFIG_SHDN;

  /* Need interrupts enabled for uptime overflow, but off when
   * manipulating timers and doing I2C.  Leave disabled except when
   * sleeping. */
  while (1) {
    uint16_t ta;
    iBSP430serialSetReset_rh(i2c, 0);
    rc = read_r16_rh(i2c, BSP430_SENSORS_MCP9808_REGNO_TA, &ta);
    iBSP430serialSetReset_rh(i2c, 1);
    if (0 == rc) {
      int cK = iBSP430sensorsMCP9808tempFromRaw_cK(ta);
      printf("Raw %04x is %d cK = %d cFahr\n", ta, cK, BSP430_SENSORS_CONVERT_cK_TO_cFahr(cK));
    } else {
      printf("Problem: %d\n", rc);
      break;
    }
    BSP430_UPTIME_DELAY_MS(5000, LPM3_bits, 0);
  }
}
