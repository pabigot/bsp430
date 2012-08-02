/** This file is in the public domain.
 *
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

#include <bsp430/platform.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/onewire.h>

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif

#ifndef APP_SAMPLE_PERIOD_SEC
#define APP_SAMPLE_PERIOD_SEC 1
#endif /* APP_SAMPLE_PERIOD_SEC */

/* Where the device can be found */
const struct xBSP430onewireBus ds18b20 = {
        .port = APP_DS18B20_PORT,
        .bit = APP_DS18B20_BIT
};
/* The serial number read from the device on startup */
struct xBSP430onewireSerialNumber serial;


void main ()
{
  int rc;
  unsigned long uptime_ticks_per_sec;
  const struct xBSP430onewireBus * bus = &ds18b20;
  
  vBSP430platformInitialize_ni();
  (void)xBSP430consoleInitialize();
  uptime_ticks_per_sec = ulBSP430uptimeResolution_Hz_ni();
  cprintf("Here we go...\n");
  cprintf("Uptime now %lu with resolution %lu\n",
          ulBSP430uptime_ni(), uptime_ticks_per_sec);
  rc = iBSP430onewireReadSerialNumber(bus, &serial);
  if (0 != rc) {
    cprintf("ERROR: Failed to read serial number from DS18B20: %d\n", rc);
    return;
  }
  cprintf("DS18B20 serial number: %02x%02x%02x%02x%02x%02x\n",
          serial.id[0], serial.id[1], serial.id[2], 
          serial.id[3], serial.id[4], serial.id[5]);
  while (1) {
    int valid;
    unsigned long start_tck;
    unsigned long end_tck;
    unsigned int duration_ms;
    unsigned int delay_count = 0;
    int t_c;
    int t_f = 0;
    
    start_tck = ulBSP430uptime_ni();
    if ((valid = iBSP430onewireReset_ni(bus))) {
      vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_SKIP_ROM);
      vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_CONVERT_T);
        
      /* Wait for read to complete.  Each read is nominally 61
       * microseconds.  Conversion time can be as long as 750 ms if
       * 12-bit resolution is used.  (12-bit resolution is the
       * default.) */
      while (! iBSP430onewireReadBit_ni(bus)) {
        ++delay_count;
      }

      iBSP430onewireReset_ni(bus);
      vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_SKIP_ROM);
      vBSP430onewireWriteByte_ni(bus, BSP430_ONEWIRE_CMD_READ_SCRATCHPAD);
      t_c = iBSP430onewireReadByte_ni(bus);
      t_c |= (iBSP430onewireReadByte_ni(bus) << 8);
      t_f = 320 + (int)((9 * t_c * 10L) / (5 * 16));
    }
    vBSP430onewireShutdown_ni(bus);
    end_tck = ulBSP430uptime_ni();
    duration_ms = 1000 * (end_tck - start_tck) / uptime_ticks_per_sec;
    cprintf("Done in %u, %d\n", duration_ms, t_f);
  }
}
