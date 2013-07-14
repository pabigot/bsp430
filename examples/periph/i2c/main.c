/** This file is in the public domain.
 *
 * This demonstrates introspection into I2C bus operations.  The
 * primary goal is to demonstrate that an I2C peripheral can show
 * UCBUSY even when it is not responsible for the bus activity, making
 * the current implementation of iBSP430serialFlush_ni() incorrect for
 * I2C.
 *
 * A secondary role demonstrates fragility: if the underlying
 * peripherals of i2c_a and i2c_b are swapped, USCI_B2(=i2c_a) is
 * marked as losing arbitration and the transmission fails.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/serial.h>

void main ()
{
  hBSP430halSERIAL i2c_a = hBSP430serialLookup(APP_I2C_BUSA);
  hBSP430halSERIAL i2c_b = hBSP430serialLookup(APP_I2C_BUSB);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("I2C bus A on %s at %p, bus rate %lu Hz\n",
          xBSP430serialName(APP_I2C_BUSA) ?: "UNKNOWN",
          i2c_a, (unsigned long)BSP430_SERIAL_I2C_BUS_SPEED_HZ);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("I2C bus A pins: %s\n", xBSP430platformPeripheralHelp(APP_I2C_BUSA, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  i2c_a = hBSP430serialOpenI2C(i2c_a,
                               BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMM | UCMST),
                               0, 0);
  if (NULL == i2c_a) {
    cprintf("I2C configuration failed\n");
  }

  cprintf("I2C bus B on %s at %p, bus rate %lu Hz\n",
          xBSP430serialName(APP_I2C_BUSB) ?: "UNKNOWN",
          i2c_b, (unsigned long)BSP430_SERIAL_I2C_BUS_SPEED_HZ);
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("I2C bus B pins: %s\n", xBSP430platformPeripheralHelp(APP_I2C_BUSB, BSP430_PERIPHCFG_SERIAL_I2C));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  i2c_b = hBSP430serialOpenI2C(i2c_b,
                               BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMM | UCMST),
                               0, 0);
  if (NULL == i2c_b) {
    cprintf("I2C configuration failed\n");
  }

#define SHOW_STATE(msg_) do {                                           \
    cprintf("%s\n\tA: CTL0 %02x CTL1 %02x STAT %02x IFG %02x SA %02x OA %02x\n" \
            "\tB: CTL0 %02x CTL1 %02x STAT %02x IFG %02x SA %02x OA %02x\n", \
            msg_,                                                       \
            i2c_a->hpl.usci5->ctl0, i2c_a->hpl.usci5->ctl1,            \
            i2c_a->hpl.usci5->stat, i2c_a->hpl.usci5->ifg,             \
            i2c_a->hpl.usci5->i2csa, i2c_a->hpl.usci5->i2coa,          \
            i2c_b->hpl.usci5->ctl0, i2c_b->hpl.usci5->ctl1,            \
            i2c_b->hpl.usci5->stat, i2c_b->hpl.usci5->ifg,             \
            i2c_b->hpl.usci5->i2csa, i2c_b->hpl.usci5->i2coa);         \
  } while (0)

  iBSP430serialSetReset_rh(i2c_a, 1);
  iBSP430serialSetReset_rh(i2c_b, 1);
  iBSP430i2cSetAddresses_rh(i2c_a, 'A', 'B');
  iBSP430i2cSetAddresses_rh(i2c_b, 'B', 'A');
  i2c_a->hpl.usci5->ctl1 &= ~(UCTR | UCTXNACK | UCTXSTP | UCTXSTT);
  i2c_b->hpl.usci5->ctl1 &= ~(UCTR | UCTXNACK | UCTXSTP | UCTXSTT);
  i2c_a->hpl.usci5->ctl0 |= UCMST;
  i2c_b->hpl.usci5->ctl0 |= UCMST;
  iBSP430serialSetReset_rh(i2c_a, 0);
  iBSP430serialSetReset_rh(i2c_b, 0);

  SHOW_STATE("initial");
  i2c_a->hpl.usci5->ctl1 |= UCTR | UCTXSTT;
  SHOW_STATE("A TX STT");
  while (! (i2c_a->hpl.usci5->ifg & UCTXIFG)) {
  }
  while (i2c_b->hpl.usci5->ctl0 & UCMST) {
  }
  SHOW_STATE("Pre A TX");
  i2c_a->hpl.usci5->txbuf = 0xA5;
  while (! (i2c_a->hpl.usci5->ifg & UCTXIFG)) {
    SHOW_STATE("Pre A STP");
  }
  i2c_a->hpl.usci5->ctl1 |= UCTXSTP;
  SHOW_STATE("A TX 0xA5 + STP");
  do {
    SHOW_STATE("spin B rxifg");
  } while (! (i2c_b->hpl.usci5->ifg & UCRXIFG));
  cprintf("B RX 0x%02x\n", i2c_b->hpl.usci5->rxbuf);
  SHOW_STATE("B RX");
  do {
    SHOW_STATE("spin A STP");
  } while (i2c_a->hpl.usci5->ctl1 & UCTXSTP);
  i2c_b->hpl.usci5->ifg &= ~(UCALIFG | UCSTTIFG);
  i2c_b->hpl.usci5->ctl0 |= UCMST;
  SHOW_STATE("B MST !AL !STT");
}
