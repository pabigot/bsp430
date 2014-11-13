/** This file is in the public domain.
 *
 * This program demonstrates interacting with the BOOSTXL-BATTPACK described at
 * http://www.element14.com/community/docs/DOC-55618/l/element14-fuel-tank-boosterpack-for-ti-launchpad
 *
 * It wakes every RESAMPLE_INTERVAL_S (10) seconds to display the
 * status of the battery.  The BQ24210 can be enabled and disabled by
 * pressing ! (bang) and the full state from the BQ27510 can be
 * displayed by pressing the space bar.
 *
 * The battery pack itself is pretty useless because it consumes ~15mA
 * continuously diddling itself.  See PS # 2 at the end of
 * http://hwtourist.blogspot.com/2013/12/element14s-fuel-tank-boosterpack.html
 * for an explanation.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <string.h>
#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>

#ifndef RESAMPLE_INTERVAL_S
#define RESAMPLE_INTERVAL_S 10
#endif /* RESAMPLE_INTERVAL_S */

typedef struct sBSP430bq24210 {
  volatile sBSP430hplPORT * chg_port;
  volatile sBSP430hplPORT * en_port;
  volatile sBSP430hplPORT * pg_port;
  unsigned char chg_bit;
  unsigned char en_bit;
  unsigned char pg_bit;
} sBSP430bq24210;

typedef struct sBQ27510 {
  uint16_t device_id;
  int16_t atRate_mA;
  uint16_t atRatetimeToEmpty_min;
  uint16_t temperature_dK;
  uint16_t voltage_mV;
  uint16_t flags;
  uint16_t nominalAvailableCapacity_mAh;
  uint16_t fullAvailableCapacity_mAh;
  uint16_t remainingCapacity_mAh;
  uint16_t fullChargeCapacity_mAh;
  int16_t averageCurrent_mA;
  uint16_t timeToEmpty_min;
  int16_t standbyCurrent_mA;
  uint16_t standbyTimeToEmpty_min;
  uint16_t stateOfHealth_ppcpx;
  uint16_t cycleCount;
  uint16_t stateOfCharge_ppc;
  int16_t instantaneousCurrent_mA;
  uint16_t internalTemperature_dK;
  uint16_t reistanceScale;
  uint16_t operationConfiguration;
  uint16_t designCapacity_mAh;
} sBQ27510;

int
readBQ27510 (hBSP430halSERIAL i2c,
             uint8_t address,
             int count,
             uint16_t * dest)
{
  int rc;
  uint16_t u16;
  uint8_t * bp = (uint8_t *)dest;
  uint16_t * dp = dest;
  const uint16_t * const dpe = dp + count;

  if (0 >= count) {
    return -1;
  }
  rc = iBSP430i2cTxData_rh(i2c, &address, sizeof(address));
  if (sizeof(address) != rc) {
    return -2;
  }
  BSP430_CORE_DELAY_CYCLES(BSP430_CORE_US_TO_TICKS(66, BSP430_CLOCK_NOMINAL_MCLK_HZ));
  rc = iBSP430i2cRxData_rh(i2c, bp, count * sizeof(*dest));
  if (count * sizeof(*dest) != rc) {
    return -3;
  }
  while (dp != dpe) {
    u16 = bp[0] | (bp[1] << 8);
    *dp++ = u16;
    bp += 2;
  }
  return count;
}

#define FLG_TOGGLE_ENABLE 0x01
#define FLG_DUMP_STATE 0x02
#define FLG_UPDATE_INTERVAL 0x04

void main ()
{
  hBSP430halSERIAL i2c;
  sBSP430bq24210 bq24210;
  union {
    sBQ27510 state;
    uint16_t raw[1];
  } u;
  const int nwords = sizeof(u.state)/sizeof(u.raw[0]);
  unsigned long resample_interval_utt;
  unsigned long resample_wake_utt;
  unsigned int flags;
  int rc;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\nbattpack " __DATE__ " " __TIME__ "\n");

  bq24210.chg_port = xBSP430hplLookupPORT(APP_CHGn_PORT_PERIPH_HANDLE);
  bq24210.en_port = xBSP430hplLookupPORT(APP_ENn_PORT_PERIPH_HANDLE);
  bq24210.pg_port = xBSP430hplLookupPORT(APP_PGn_PORT_PERIPH_HANDLE);
  bq24210.chg_bit = APP_CHGn_PORT_BIT;
  bq24210.en_bit = APP_ENn_PORT_BIT;
  bq24210.pg_bit = APP_PGn_PORT_BIT;

  cprintf("CHGn on %s.%u\n",
          xBSP430portName(xBSP430periphFromHPL(bq24210.chg_port)),
          iBSP430portBitPosition(bq24210.chg_bit));
  cprintf("ENn on %s.%u\n",
          xBSP430portName(xBSP430periphFromHPL(bq24210.en_port)),
          iBSP430portBitPosition(bq24210.en_bit));
  cprintf("PGn on %s.%u\n",
          xBSP430portName(xBSP430periphFromHPL(bq24210.pg_port)),
          iBSP430portBitPosition(bq24210.pg_bit));
  if (! (bq24210.chg_port && bq24210.en_port && bq24210.pg_port)) {
    cprintf("One of the ports is missing\n");
    return;
  }

  /* Charge signal is an input (active low) to the MCU.  Configure as
   * input with internal pull-up. */
  bq24210.chg_port->dir &= ~bq24210.chg_bit;
  bq24210.chg_port->out |= bq24210.chg_bit;
  bq24210.chg_port->ren |= bq24210.chg_bit;

  /* Power-good signal is an input (active low) to the MCU.  Configure
   * as input with internal pull-up. */
  bq24210.pg_port->dir &= ~bq24210.pg_bit;
  bq24210.pg_port->out |= bq24210.pg_bit;
  bq24210.pg_port->ren |= bq24210.pg_bit;

  /* Enable signal is an output (active low) from the MCU.  Start
   * active. */
  bq24210.en_port->out &= ~bq24210.en_bit;
  bq24210.en_port->dir |= bq24210.en_bit;

  cprintf("I2C on %s at address 0x%02x\nPins: %s\n",
          xBSP430serialName(APP_BQ27510_I2C_PERIPH_HANDLE),
          APP_BQ27510_I2C_ADDRESS,
          xBSP430platformPeripheralHelp(APP_BQ27510_I2C_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_I2C));

  /* NOTE: At default BSP430_SERIAL_I2C_BUS_SPEED_HZ 400kHz this
   * devices supports only single-byte write operations.  Further,
   * ensure a 66us delay between packets. */
  i2c = hBSP430serialOpenI2C(hBSP430serialLookup(APP_BQ27510_I2C_PERIPH_HANDLE),
                             BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMST),
                             0, 0);
  if (! i2c) {
    cprintf("I2C open failed\n");
    return;
  }
  (void)iBSP430i2cSetAddresses_rh(i2c, -1, APP_BQ27510_I2C_ADDRESS);

  resample_interval_utt = BSP430_UPTIME_MS_TO_UTT(1000UL * RESAMPLE_INTERVAL_S);
  resample_wake_utt = ulBSP430uptime_ni();
  flags = FLG_DUMP_STATE | FLG_UPDATE_INTERVAL;

  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    char astext_buf[BSP430_UPTIME_AS_TEXT_LENGTH];
    uint16_t temperature_dC;

    if (FLG_DUMP_STATE & flags) {
      memset(&u, 0, sizeof(u));
      rc = readBQ27510(i2c, 0, nwords, u.raw);
      cprintf("Device ID %04x rc %d\n", u.state.device_id, rc);
      cprintf("%.30s = %d\n", "atRate_mA", u.state.atRate_mA);
      cprintf("%.30s = %u\n", "atRatetimeToEmpty_min", u.state.atRatetimeToEmpty_min);
      cprintf("%.30s = %u\n", "temperature_dK", u.state.temperature_dK);
      cprintf("%.30s = %u\n", "voltage_mV", u.state.voltage_mV);
      cprintf("%.30s = 0x%04x\n", "flags", u.state.flags);
      cprintf("%.30s = %u\n", "nominalAvailableCapacity_mAh", u.state.nominalAvailableCapacity_mAh);
      cprintf("%.30s = %u\n", "fullAvailableCapacity_mAh", u.state.fullAvailableCapacity_mAh);
      cprintf("%.30s = %u\n", "remainingCapacity_mAh", u.state.remainingCapacity_mAh);
      cprintf("%.30s = %u\n", "fullChargeCapacity_mAh", u.state.fullChargeCapacity_mAh);
      cprintf("%.30s = %d\n", "averageCurrent_mA", u.state.averageCurrent_mA);
      cprintf("%.30s = %u\n", "timeToEmpty_min", u.state.timeToEmpty_min);
      cprintf("%.30s = %d\n", "standbyCurrent_mA", u.state.standbyCurrent_mA);
      cprintf("%.30s = %u\n", "standbyTimeToEmpty_min", u.state.standbyTimeToEmpty_min);
      cprintf("%.30s = %u\n", "stateOfHealth_ppcpx", u.state.stateOfHealth_ppcpx);
      cprintf("%.30s = %u\n", "cycleCount", u.state.cycleCount);
      cprintf("%.30s = %u\n", "stateOfCharge_ppc", u.state.stateOfCharge_ppc);
      cprintf("%.30s = %d\n", "instantaneousCurrent_mA", u.state.instantaneousCurrent_mA);
      cprintf("%.30s = %u\n", "internalTemperature_dK", u.state.internalTemperature_dK);
      cprintf("%.30s = %u\n", "reistanceScale", u.state.reistanceScale);
      cprintf("%.30s = %u\n", "operationConfiguration", u.state.operationConfiguration);
      cprintf("%.30s = %u\n", "designCapacity_mAh", u.state.designCapacity_mAh);
      cprintf("flags %02x ; ENn state %d\n", flags, (bq24210.en_port->out & bq24210.en_bit));
      flags &= ~FLG_DUMP_STATE;
    }
    if (FLG_TOGGLE_ENABLE & flags) {
      bq24210.en_port->out ^= bq24210.en_bit;
      flags &= ~FLG_TOGGLE_ENABLE;
    }
    vBSP430ledSet(BSP430_LED_GREEN, !(bq24210.en_port->out & bq24210.en_bit));

    rc = readBQ27510(i2c, 0, nwords, u.raw);
    temperature_dC = u.state.temperature_dK - 2733;

    cprintf("%s: %c%c%c % 2d.%dC  %4dmV ; SoC %u%% ; Cap %4d / %4d ; %dmA ~ %dmA / %u\n",
            xBSP430uptimeAsText(ulBSP430uptime(), astext_buf),
            (bq24210.en_port->out & bq24210.en_bit) ? ' ' : 'E',
            (bq24210.chg_port->in & bq24210.chg_bit) ? ' ' : 'C',
            (bq24210.pg_port->in & bq24210.pg_bit) ? ' ' : 'P',
            (temperature_dC / 10), (temperature_dC % 10),
            u.state.voltage_mV,
            u.state.stateOfCharge_ppc,
            u.state.remainingCapacity_mAh,
            u.state.fullAvailableCapacity_mAh,
            u.state.instantaneousCurrent_mA,
            u.state.averageCurrent_mA,
            u.state.cycleCount
           );
    if (FLG_UPDATE_INTERVAL & flags) {
      resample_wake_utt += resample_interval_utt;
      flags &= ~FLG_UPDATE_INTERVAL;
    }
    flags = 0;
    while (! flags) {
      iBSP430consoleFlush();
      if (0 >= lBSP430uptimeSleepUntil(resample_wake_utt, LPM3_bits)) {
        flags |= FLG_UPDATE_INTERVAL;
      }
      while (0 <= ((rc = cgetchar()))) {
        if ('!' == rc) {
          flags |= FLG_TOGGLE_ENABLE;
        } else if (' ' == rc) {
          flags |= FLG_DUMP_STATE;
        }
      }
    }

  }
}
