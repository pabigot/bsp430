/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */
#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <string.h>

/** Raw number is a 16 bit value.  First 14 bits represent the
 * temperature as a count of 0.03125C values.  Shifting by 2 gets us
 * the 14-bit value; multiply by 0.3125 = 5/16. */
#define TMP106_RAW_TO_dC_(raw_) ((5 * ((raw_) >> 2)) / 16)
#define TMP106_RAW_TO_dC(raw_) ((0 <= (int)(raw_)) ? TMP106_RAW_TO_dC_(raw_) : -TMP106_RAW_TO_dC_(-(int)(raw_)))
#define dC_TO_dF(dC_) (320 + 9 * (dC_) / 5)

/** Raw number is a 16 bit value with 156.25 nV per count.  .15625 is
 * 10/64. */
#define TMP106_VRAW_TO_uV_(raw_) ((10L * (raw_)) / 64)
#define TMP106_VRAW_TO_uV(raw_) ((0 <= (int)(raw_)) ? TMP106_VRAW_TO_uV_(raw_) : -TMP106_VRAW_TO_uV_(-(int)(raw_)))

static uint16_t
read_register(hBSP430halSERIAL i2c,
              uint8_t regno)
{
  uint8_t data[2];
  int rc;

  rc = iBSP430i2cTxData_rh(i2c, &regno, sizeof(regno));
  rc = iBSP430i2cRxData_rh(i2c, data, sizeof(data));
  (void)rc;
  return (data[0] << 8) | data[1];
}

void main ()
{
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_TMP106_I2C_PERIPH_HANDLE);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("I2C interface on %s is %p\n", xBSP430serialName(APP_TMP106_I2C_PERIPH_HANDLE), i2c);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("TMP106 I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_TMP106_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  (void)iBSP430i2cSetAddresses_rh(i2c, -1, APP_TMP106_I2C_ADDRESS);

  cprintf("Manuf Id: %04x ; Device Id: %04x\n",
          read_register(i2c, 0xfe),
          read_register(i2c, 0xff));

  while (1) {
    uint16_t cfg;
    uint16_t v_obj;
    uint16_t t_amb;
    int t_dC;

    do {
      cfg = read_register(i2c, 2);
    } while (! (cfg & 0x80));
    v_obj = read_register(i2c, 0);
    t_amb = read_register(i2c, 1);

    t_dC = TMP106_RAW_TO_dC(t_amb);
    /* Actually calculating the temperature involves complex
     * mathematics not suited to an MSP430 */
    cprintf("[%04x] T_amb: %d dC %d dF ; V_obj: %ld uV\n", cfg, t_dC, dC_TO_dF(t_dC), TMP106_VRAW_TO_uV(v_obj));
  }
}
