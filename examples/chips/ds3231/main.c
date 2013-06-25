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
#include <bsp430/chips/ds3231.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

void dumpMemory (const uint8_t * dp,
                 size_t len,
                 unsigned long base)
{
  const uint8_t * const edp = dp + len;
  const uint8_t * adp = dp;

  while (dp < edp) {
    if (0 == (base & 0x0F)) {
      if (adp < dp) {
        cprintf("  ");
        while (adp < dp) {
          cputchar(isprint(*adp) ? *adp : '.');
          ++adp;
        }
      }
      adp = dp;
      cprintf("\n%08lx ", base);
    } else if (0 == (base & 0x07)) {
      cputchar(' ');
    }
    cprintf(" %02x", *dp++);
    ++base;
  }
  if (adp < dp) {
    while (base & 0x0F) {
      if (0 == (base & 0x07)) {
        cputchar(' ');
      }
      cprintf("   ");
      ++base;
    }
    cprintf("  ");
    while (adp < dp) {
      cputchar(isprint(*adp) ? *adp : '.');
      ++adp;
    }
  }
  cputchar('\n');
}

void main ()
{
  sDS3231registers regs;
  struct tm tms;
  int rc;
  hBSP430halSERIAL i2c = hBSP430serialLookup(APP_I2C_PERIPH_HANDLE);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("I2C interface on %s is %p\n", xBSP430serialName(APP_I2C_PERIPH_HANDLE), i2c);
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

  (void)iBSP430i2cSetAddresses_rh(i2c, -1, DS3231_I2C_ADDRESS);

  memset(&regs, 0, sizeof(regs));
#if 1
  regs.hours |= BIT6;
#endif
  memset(&tms, 0, sizeof(tms));
  tms.tm_sec = 55;
  tms.tm_min = 59;
  tms.tm_hour = 11;
  tms.tm_mday = 31;
  tms.tm_mon = 4;
  tms.tm_year = 113;
  tms.tm_wday = 5;
#if 1
  tms.tm_hour = 12;
#endif
#if 0
  tms.tm_hour = 23;
#endif
#if 0
  tms.tm_wday = 2;
  tms.tm_mon = 11;
#endif

  /* Set start address for slave transmitter mode */
  {
    uint8_t data[8];

    memset(data, -1, sizeof(data));
    data[0] = 0;
#if 0
    cprintf("Write time: %s", asctime(&tms));
    xDS3231tmToRegisters(&tms, &regs);
    dumpMemory((uint8_t*)&regs, sizeof(regs), 0);
    xDS3231registersToTm(&regs, &tms);
    cprintf("Converted time: %s", asctime(&tms));
    memcpy(data+1, &regs, offsetof(sDS3231registers, alrm1_sec));
    dumpMemory(data, sizeof(data), 0);
    rc = iBSP430i2cTxData_rh(i2c, data, sizeof(data));
    cprintf("Time write got %d\n", rc);
#endif
    rc = iBSP430i2cTxData_rh(i2c, data, 1);
    cprintf("Address set got %d\n", rc);
  }
  while (1) {
    cprintf("Regs %u long\n", sizeof(regs));
    memset(&regs, 0, sizeof(regs));
    rc = iBSP430i2cRxData_rh(i2c, (uint8_t*)&regs, sizeof(regs));
    if (0 > rc) {
      cprintf("I2C RX ERROR: %d\n", rc);
      break;
    }
    dumpMemory((uint8_t*)&regs, sizeof(regs), 0);

    cputs(asctime(xDS3231registersToTm(&regs, &tms)));
    {
      int t_raw;
      int t_dC;

      t_raw = (regs.temp_msb << 8) | regs.temp_lsb;
      t_raw >>= 6;
      t_dC = (t_raw * 10) / 4;
      cprintf("Temp: %u dC\n", t_dC);
    }
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
