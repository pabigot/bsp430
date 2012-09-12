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

unsigned int temp_xCel;

void main ()
{
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_TMP102_I2C_PERIPH_HANDLE);
  uint8_t pr = 0;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("I2C interface on %s is %p\n", xBSP430serialName(APP_TMP102_I2C_PERIPH_HANDLE), i2c);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("TMP102 I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_TMP102_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  /* Ugliness to accomodate value change in UCMST with eUSCI */
  i2c = hBSP430serialOpenI2C(i2c, (0x100 <= UCMST) ? (UCMST >> 8) : UCMST,
                             UCSSEL_2, APP_TMP102_I2C_PRESCALER);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  (void)iBSP430i2cSetAddresses_ni(i2c, -1, APP_TMP102_I2C_ADDRESS);

#define BSP430_TMP_xCel_TO_ddegF(xcel_) (320 + ((9 * (xcel_ >> 1)) / (4 >> (1 & (xcel_)))))

  while (1) {
    int rc;
    uint8_t data[2];
    uint16_t temp_xCel;

    rc = iBSP430i2cTxData_ni(i2c, &pr, 1);
    if (0 > rc) {
      cprintf("I2C TX ERROR\n");
      continue;
    }
    memset(data, 0, sizeof(data));
    rc = iBSP430i2cRxData_ni(i2c, data, sizeof(data));
    if (0 > rc) {
      cprintf("I2C RX ERROR\n");
      continue;
    }
    temp_xCel = data[1] | (data[0] << 8);
    if (0 == pr) {
      cprintf("temp 0x%04x = %d d[degF]\n", temp_xCel, BSP430_TMP_xCel_TO_ddegF(temp_xCel));
    } else {
      cprintf("reg %d is 0x%04x\n", pr, temp_xCel);
    }
    pr = (pr + 1) & 0x03;
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
