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
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_HIH6130_I2C_PERIPH_HANDLE);
  int rc;

  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();
  cprintf("\nApplication starting\n");

  cprintf("HIH6130 I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
          xBSP430serialName(APP_HIH6130_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
          i2c, ulBSP430clockSMCLK_Hz_ni() / APP_HIH6130_I2C_PRESCALER,
          APP_HIH6130_I2C_ADDRESS);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("HIH6130 I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_HIH6130_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */

  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             UCSSEL_2, APP_HIH6130_I2C_PRESCALER);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  (void)iBSP430i2cSetAddresses_ni(i2c, -1, APP_HIH6130_I2C_ADDRESS);

  /* HIH-613x wants max 60ms on power-up. */
  BSP430_UPTIME_DELAY_MS_NI(60, LPM4_bits, 0);

#define DENOMINATOR ((1 << 14) - 1)
#define HUMIDITY_RAW_TO_PPT(raw_) ((unsigned int)((1000UL * (raw_)) / DENOMINATOR))
  /** Convert a raw temperature value to deci-degrees Celcius */
#define TEMPERATURE_RAW_TO_dC(raw_) ((int)(-400 + (1650L * (raw_)) / DENOMINATOR))
#define TEMPERATURE_dC_TO_dF(dc_) (320 + ((9 * (dc_)) / 5))

  /* Need interrupts enabled for uptime overflow, but off when
   * manipulating timers and doing I2C.  Leave disabled except when
   * sleeping. */
  while (1) {
    uint8_t data[4];
    unsigned long t0;
    unsigned long t1;
    unsigned int status;
    unsigned int hum_raw;
    unsigned int temp_raw;

    t0 = ulBSP430uptime_ni();
    rc = iBSP430i2cTxData_ni(i2c, NULL, 0);
    if (0 != rc) {
      cprintf("ERROR IN REQUEST: %d\n", rc);
      break;
    }

    /* HIH-6130 gets pissy if you're impatient.  App note says it
     * typically takes 36.65ms to complete a combined temperature and
     * humidity reading.  Short it so we can verify that, but don't
     * ask more than once per millisecond. */
    BSP430_UPTIME_DELAY_MS_NI(30, LPM4_bits, 0);
    do {
      rc = iBSP430i2cRxData_ni(i2c, data, sizeof(data));
      status = 0x03 & (data[0] >> 6);
      hum_raw = data[1] | (data[0] & 0x3F) << 8;
      temp_raw = (data[2] << 6) | (data [3] >> 2);
      if (1 < status) {
        cprintf("BOGUS STATUS: %d\n", status);
        break;
      }
      if (0 == status) {
        break;
      }
      BSP430_UPTIME_DELAY_MS_NI(1, LPM4_bits, 0);
    } while (1 == status);
    t1 = ulBSP430uptime_ni();
    cprintf("%s: ", xBSP430uptimeAsText_ni(t0));
    cprintf("Temp %d dF, humidity %u ppt, in %s\n", TEMPERATURE_dC_TO_dF(TEMPERATURE_RAW_TO_dC(temp_raw)),
            HUMIDITY_RAW_TO_PPT(hum_raw),
            xBSP430uptimeAsText_ni(t1 - t0));
    BSP430_UPTIME_DELAY_MS_NI(5000, LPM4_bits, 0);
  }
}
