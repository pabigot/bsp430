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
  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  (void)iBSP430i2cSetAddresses_rh(i2c, -1, APP_TMP102_I2C_ADDRESS);

  /** Raw number is a 16 bit value.  First 12 bits represent the
   * temperature as a count of 0.0625C values.  (If the LSB is 1, then
   * an extended temperature is used and the 13th bit represents a
   * half count.)  0.625 = 5/8; shifting by 3 gets us the 13-bit
   * value; dividing by 2 accounts for the half-count in extended
   * temperature mode. */
#define TMP102_RAW_TO_dC_(raw_) (5 * ((raw_) >> 3) / 16)
#define TMP102_RAW_TO_dC(raw_) ((0 <= (int)(raw_)) ? TMP102_RAW_TO_dC_(raw_) : -TMP102_RAW_TO_dC_(-(int)(raw_)))
#define dC_TO_dF(dC_) (320 + 9 * (dC_) / 5)

#if 0
  {
    unsigned int data[] = { 0x7FF0, 0x4B00, 0x1900, 0xFFC0, 0xE700, 0xC908 };
    int i;

    for (i = 0; i < sizeof(data)/sizeof(*data); ++i) {
      int temp_dC = TMP102_RAW_TO_dC(data[i]);
      cprintf("temp 0x%04x = %d dC = %d d[degF]\n", data[i], temp_dC, dC_TO_dF(temp_dC));
    }
  }
#endif

  while (1) {
    int rc;
    uint8_t data[2];
    uint16_t raw;

    rc = iBSP430i2cTxData_rh(i2c, &pr, 1);
    if (0 > rc) {
      cprintf("I2C TX ERROR\n");
      break;
    }
    memset(data, 0, sizeof(data));
    rc = iBSP430i2cRxData_rh(i2c, data, sizeof(data));
    if (0 > rc) {
      cprintf("I2C RX ERROR\n");
      break;
    }
    raw = data[1] | (data[0] << 8);
    if (0 == pr) {
      int temp_dC = TMP102_RAW_TO_dC(raw);
      cprintf("temp 0x%04x = %d dC = %d d[degF]\n", raw, temp_dC, dC_TO_dF(temp_dC));
    } else {
      cprintf("reg %d is 0x%04x\n", pr, raw);
    }
    pr = (pr + 1) & 0x03;
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
