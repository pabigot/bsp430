/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

/*
The TSL2561 appears to have an error in its address decoding, which
shows up when you use the I2C block transfer interface to read the
entire memory.  If the start address is below 0x0c the value read at
0x0c is the result for ADC1 (normally at 0x0e) and the value read at
0x0e is the result for ADC0 (normally read at 0x0c).

For example, here's a series of block reads of 16 bytes, incrementing
the start address by one byte at each read.  Note that in the first
read the 16-bit ADC values appear to be 0x0043 and 0x0176.  This
persists until the block read starts at 0x0c, at which point the
correct value for ADC0 (now 0x0187) shows up (and is repeated two
bytes later).  Once the address increments to 0x0e the value for ADC1
is now in the correct location.

Read 90:   33 12 00 00 00 00 00 00  8a 10 50 04 43 00 76 01  3.........P.C.v.
Read 91:   12 00 00 00 00 00 00 8a  11 50 04 45 00 87 01 33  .........P.E...3
Read 92:   00 00 00 00 00 00 8a 12  50 04 45 00 87 01 33 12  ........P.E...3.
Read 93:   00 00 00 00 00 8a 13 50  04 45 00 87 01 33 12 00  .......P.E...3..
Read 94:   00 00 00 00 8a 14 50 04  45 00 87 01 33 12 00 00  ......P.E...3...
Read 95:   00 00 00 8a 15 50 04 45  00 87 01 33 12 00 00 00  .....P.E...3....
Read 96:   00 00 8a 16 50 04 45 00  87 01 33 12 00 00 00 00  ....P.E...3.....
Read 97:   00 8a 17 50 04 45 00 87  01 33 12 00 00 00 00 00  ...P.E...3......
Read 98:   8a 18 50 04 45 00 87 01  33 12 00 00 00 00 00 00  ..P.E...3.......
Read 99:   19 50 04 45 00 87 01 33  12 00 00 00 00 00 00 8a  .P.E...3........
Read 9a:   50 04 45 00 87 01 33 12  00 00 00 00 00 00 8a 1a  P.E...3.........
Read 9b:   04 45 00 87 01 33 12 00  00 00 00 00 00 8a 1b 50  .E...3.........P
Read 9c:   87 01 87 01 33 12 00 00  00 00 00 00 8a 1c 50 04  ....3.........P.
Read 9d:   01 87 01 33 12 00 00 00  00 00 00 8a 1d 50 04 45  ...3.........P.E
Read 9e:   45 00 33 12 00 00 00 00  00 00 8a 1e 50 04 45 00  E.3.........P.E.
Read 9f:   00 33 12 00 00 00 00 00  00 8a 1f 50 04 45 00 88  .3.........P.E..

This may be a residual effect of the SMB API which specifies that you
can read a 32-bit value by accessing address 0x9b.  If you're writing
your own and try to read both ADC values in one I2C transaction, you
need to use an address strictly less than 0x0c and will have to swap the
values.

*/


#include <string.h>
#include <bsp430/platform.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/sys.h>
#include <bsp430/sensors/utility.h>
#include <ctype.h>

/* Sanity check that the features we requested are present */
#if ! (BSP430_CONSOLE - 0)
#error Console is not configured correctly
#endif /* BSP430_CONSOLE */
/* Sanity check that the features we requested are present */
#if ! (BSP430_UPTIME - 0)
#error Uptime is not configured correctly
#endif /* BSP430_UPTIME */

#ifndef RESAMPLE_INTERVAL_MS
#define RESAMPLE_INTERVAL_MS 1000
#endif /* RESAMPLE_INTERVAL_MS */

typedef struct sBSP430sensorsTSL256x {
  uint8_t control;
  uint8_t timing;
  union {
    struct {
      uint8_t threshlowlow;
      uint8_t threshlowhigh;
    };
    uint16_t threshlow_le;
  };
  union {
    struct {
      uint8_t threshhighlow;
      uint8_t threshhighhigh;
    };
    uint16_t threshhigh_le;
  };
  uint8_t interrupt;
  uint8_t reserved_7_;
  uint8_t crc;
  uint8_t reserved_9_;
  uint8_t id;
  uint8_t reserved_b_;
  /* NOTE: data1 placement based on block read from address less than 0x0c */
  union {
    struct {
      uint8_t data1low;
      uint8_t data1high;
    };
    uint16_t data1_le;
  };
  /* NOTE: data0 placement based on block read from address less than 0x0c */
  union {
    struct {
      uint8_t data0low;
      uint8_t data0high;
    };
    uint16_t data0_le;
  };
} sBSP430sensorsTSL256x;

void main ()
{
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_TSL2561_I2C_PERIPH_HANDLE);
  sBSP430sensorsTSL256x state;
  unsigned long resample_wake_utt;
  unsigned long resample_interval_utt;
  int rc;
  uint8_t setgain[] = { 0x81, 0x12 };

  vBSP430platformInitialize_ni();
  BSP430_CORE_ENABLE_INTERRUPT();

  resample_interval_utt = BSP430_UPTIME_MS_TO_UTT(RESAMPLE_INTERVAL_MS);

  (void)iBSP430consoleInitialize();
  cprintf("\nApplication starting\n");

  cprintf("TSL2561 I2C on %s at %p, bus rate %lu Hz, address 0x%02x\n",
          xBSP430serialName(APP_TSL2561_I2C_PERIPH_HANDLE) ?: "UNKNOWN",
          i2c, (unsigned long)BSP430_SERIAL_I2C_BUS_SPEED_HZ,
          APP_TSL2561_I2C_ADDRESS);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("TSL2561 I2C Pins: %s\n", xBSP430platformPeripheralHelp(APP_TSL2561_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */

  i2c = hBSP430serialOpenI2C(i2c,
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    cprintf("I2C open failed.\n");
    return;
  }

  iBSP430serialSetReset_rh(i2c, 1);
  rc = iBSP430i2cSetAddresses_rh(i2c, -1, APP_TSL2561_I2C_ADDRESS);
  cprintf("Set address %x got %d\n", APP_TSL2561_I2C_ADDRESS, rc);
  iBSP430serialSetReset_rh(i2c, 0);

  {
    uint8_t wakeup[] = { 0x80, 0x03 };

    rc = iBSP430i2cTxData_rh(i2c, wakeup, sizeof(wakeup));
    cprintf("Wakeup got %d\n", rc);
    memset(&state, 0, sizeof(state));
    rc = iBSP430i2cRxData_rh(i2c, (uint8_t*)&state, sizeof(state));
    cprintf("State got %d: ctrl %02x timing %02x intr %02x id %02x\n",
            rc, state.control, state.timing, state.interrupt, state.id);
    vBSP430consoleDisplayMemory((uint8_t*)&state, sizeof(state), 0);
  }

#if 1
  cprintf("Setting high (16x) gain\n");
  rc = iBSP430i2cTxData_rh(i2c, setgain, sizeof(setgain));
#endif

#if 1
  {
    uint8_t readb;
    uint8_t memory[16];
    int offset;
    rc = iBSP430i2cTxData_rh(i2c, setgain, sizeof(setgain));
    for (offset = 0; offset < 0x10; ++offset) {
      readb = 0x90 | offset;
      rc = iBSP430i2cTxData_rh(i2c, &readb, sizeof(readb));
      rc = iBSP430i2cRxData_rh(i2c, memory, sizeof(memory));
      cprintf("Read %02x: ", readb);
      vBSP430consoleDisplayOctets(memory, sizeof(memory));
      cputchar('\n');
    }
  }
#endif

  resample_wake_utt = ulBSP430uptime();
  do {
    char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];
    uint16_t adc[2];
    unsigned long start_utt;
    unsigned long end_utt;

    start_utt = ulBSP430uptime();
    {
      uint8_t readcmd = 0x9c;

      rc = iBSP430i2cTxData_rh(i2c, &readcmd, sizeof(readcmd));
      rc = iBSP430i2cRxData_rh(i2c, (uint8_t*)(adc+0), sizeof(adc[0]));
      readcmd += 2;
      rc = iBSP430i2cTxData_rh(i2c, &readcmd, sizeof(readcmd));
      rc = iBSP430i2cRxData_rh(i2c, (uint8_t*)(adc+1), sizeof(adc[1]));
    }
    end_utt = ulBSP430uptime();
    if (0 > rc) {
      break;
    }
    cprintf("%s: Sample in %u ms rc %d ; D0 %u ; D1 %u\n",
            xBSP430uptimeAsText(end_utt, as_text),
            (uint16_t)BSP430_UPTIME_UTT_TO_MS(end_utt - start_utt), rc,
            adc[0], adc[1]);
    resample_wake_utt += resample_interval_utt;
    while (0 < lBSP430uptimeSleepUntil(resample_wake_utt, LPM3_bits)) {
    }
  } while (1);
  cprintf("Error: RC %d\n", rc);
}
