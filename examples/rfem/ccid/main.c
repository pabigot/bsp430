/** This file is in the public domain.
 *
 * Basic validation of SPI using the RF2500T to query the CC2500 for its
 * part number and version.
 *
 * Recognized MCUs/platforms are:
 *
 * | MCU            | EXP          | GDO0 | GDO2 | CSn  | SCL  | MOSI | MISO/GDO1 |
 * | -------------- | ------------ | ---- | ---- | ---- | ---- | ---- | --------- |
 * |                |  RF1 Header  |  P10 |  P12 |  P14 |  P16 |  P18 |  P20      |
 * | msp430f2274    | RF2500T      | P2.6 | P2.7 | P3.0 | P3.3 | P3.1 | P3.2      |
 * | msp430f5438(a) | EXP430F5438  | P1.7 | P1.3 | P3.0 | P3.3 | P3.1 | P3.2      |
 * | msp430fr5739   | EXP430FR5739 | P4.1 | P2.3 | P1.3 | P2.2 | P1.6 | P1.7      |
 * | msp430f5529    | EXP430F5529  | P2.3 | P2.4 | P2.6 | P3.2 | P3.0 | P3.1      |
 * | msp430g2553    | Anaren Air   | P2.6 | P1.0 | P2.7 | P1.5 | P1.7 | P1.6      |
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>

hBSP430halSERIAL spi;

static uint8_t
sendStrobe (uint8_t reg)
{
  uint8_t rc = 0;
  
  (void)iBSP430serialSPITxRx_ni(spi, &reg, 1, 0, &rc);
  return rc;
}

static uint8_t
readRegister (uint8_t reg)
{
  uint8_t rxbuf[2];

  /* Add the READ bit, and if this is a status register the BURST bit */
  reg |= 0x80;
  if (0x30 <= reg) {
    reg |= 0x40;
  }
  (void)iBSP430serialSPITxRx_ni(spi, &reg, 1, 1, rxbuf);
  return rxbuf[1];
}

void main ()
{
  uint8_t rc;
  unsigned int ctl0_byte;
  
  volatile sBSP430hplPORTIE * gdo0 = xBSP430hplLookupPORTIE(APP_GDO0_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * gdo1 = xBSP430hplLookupPORTIE(APP_GDO1_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * gdo2 = xBSP430hplLookupPORTIE(APP_GDO2_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * csn = xBSP430hplLookupPORTIE(APP_CSn_PORT_PERIPH_HANDLE);
  spi = hBSP430serialLookup(APP_SPI_PERIPH_HANDLE);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("GDO0 %p at %s.%u\n", gdo0, xBSP430portName(APP_GDO0_PORT_PERIPH_HANDLE), iBSP430portBitPosition(APP_GDO0_PORT_BIT));
  cprintf("GDO1 %p at %s.%u\n", gdo1, xBSP430portName(APP_GDO1_PORT_PERIPH_HANDLE), iBSP430portBitPosition(APP_GDO1_PORT_BIT));
  cprintf("GDO2 %p at %s.%u\n", gdo2, xBSP430portName(APP_GDO2_PORT_PERIPH_HANDLE), iBSP430portBitPosition(APP_GDO2_PORT_BIT));
  cprintf("CSn %p at %s.%u\n", csn, xBSP430portName(APP_CSn_PORT_PERIPH_HANDLE), iBSP430portBitPosition(APP_CSn_PORT_BIT));
  cprintf("SPI %p is %s\n", spi, xBSP430serialName(APP_SPI_PERIPH_HANDLE));
  cprintf(__DATE__ " " __TIME__ "\n");
  ctl0_byte = UCCKPH | UCMSB | UCMST;
  cprintf("Initial ctl0 %04x\n", ctl0_byte);
  if (0x100 <= ctl0_byte) {
    ctl0_byte >>= 8;
  }

  spi = hBSP430serialOpenSPI(spi, ctl0_byte, UCSSEL_2, 1);

  cprintf("SPI device %p: CTL0 %02x ; CTL1 %02x\n", spi, UCB0CTL0, UCB0CTL1);

  /* Configure for enable with radio disabled to ensure we have a
   * falling edge. */
  csn->sel &= ~APP_CSn_PORT_BIT;
  csn->out |= APP_CSn_PORT_BIT;
  csn->dir |= APP_CSn_PORT_BIT;

  /* Now enable the radio */
  csn->out &= ~APP_CSn_PORT_BIT;

  /* Spin until MISO (CHP_RDYn) is clear */
  while (gdo1->in & APP_GDO1_PORT_BIT) {
    cprintf("Waiting for radio ready\n");
  }

  /* Enable SPI */
  UCB0CTL1 &= ~UCSWRST;
  cprintf("Radio is up; sending SRES strobe\n");

  /* Send a reset */
  do {
    rc = sendStrobe(0x30);
    cprintf("Strobe response %#02x\n", rc);
  } while (0x0F != rc);

  cprintf("PARTNUM response %#02x\n", readRegister(0x30));
  cprintf("VERSION response %#02x\n", readRegister(0x31));
}
