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
 * @homepage http://github.com/pabigot/bsp430
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

  (void)iBSP430spiTxRx_ni(spi, &reg, 1, 0, &rc);
  return rc;
}

static uint8_t
readRegister (uint8_t reg)
{
  uint8_t rxbuf[2];

  /* If this is a status register add the BURST bit */
  if (0x30 <= reg) {
    reg |= 0x40;
  }
  /* Add the READ bit */
  reg |= 0x80;
  (void)iBSP430spiTxRx_ni(spi, &reg, 1, 1, rxbuf);
  return rxbuf[1];
}

static int
writeRegister (uint8_t reg,
               uint8_t val)
{
  uint8_t txbuf[2];
  uint8_t rxbuf[2];

  txbuf[0] = reg;
  txbuf[1] = val;
  (void)iBSP430spiTxRx_ni(spi, txbuf, 2, 0, rxbuf);
  return rxbuf[1];
}

void main ()
{
  int rc = 0;
  unsigned int ctl0_byte;
  /* GDO0 and GDO2 are always interrupt-capable. */
  volatile sBSP430hplPORTIE * gdo0 = xBSP430hplLookupPORTIE(APP_GDO0_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * gdo2 = xBSP430hplLookupPORTIE(APP_GDO2_PORT_PERIPH_HANDLE);
  hBSP430halPORT hgdo1 = hBSP430portLookup(APP_GDO1_PORT_PERIPH_HANDLE);
  hBSP430halPORT hcsn = hBSP430portLookup(APP_CSn_PORT_PERIPH_HANDLE);

  spi = hBSP430serialLookup(APP_SPI_PERIPH_HANDLE);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("GDO0 %p at %s.%u\n", gdo0, xBSP430portName(APP_GDO0_PORT_PERIPH_HANDLE), iBSP430portBitPosition(APP_GDO0_PORT_BIT));
  cprintf("GDO1 HAL %p HPL %p at %s.%u\n", hgdo1, hgdo1->hpl.any, xBSP430portName(APP_GDO1_PORT_PERIPH_HANDLE), iBSP430portBitPosition(APP_GDO1_PORT_BIT));
  cprintf("GDO2 %p at %s.%u\n", gdo2, xBSP430portName(APP_GDO2_PORT_PERIPH_HANDLE), iBSP430portBitPosition(APP_GDO2_PORT_BIT));
  cprintf("CSn HAL %p HPL %p at %s.%u\n", hcsn, hcsn->hpl.any, xBSP430portName(APP_CSn_PORT_PERIPH_HANDLE), iBSP430portBitPosition(APP_CSn_PORT_BIT));
  cprintf("SPI %p is %s\n", spi, xBSP430serialName(APP_SPI_PERIPH_HANDLE));
  cprintf(__DATE__ " " __TIME__ "\n");


  /* Configure the SPI interface, but immediately put it into hold
   * mode so we can check CHIP_RDYn on the MISO/GDO1 input */
  ctl0_byte = UCCKPH | UCMSB | UCMST;
  cprintf("Initial ctl0 %04x\n", ctl0_byte);
  if (0x100 <= ctl0_byte) {
    ctl0_byte >>= 8;
  }
  spi = hBSP430serialOpenSPI(spi, ctl0_byte, UCSSEL_2, 1);
  if (spi) {
    rc = iBSP430serialSetHold_ni(spi, 1);
    /* GDO1 to input, pull-up */
    BSP430_PORT_HAL_HPL_DIR(hgdo1) &= ~APP_GDO1_PORT_BIT;
    BSP430_PORT_HAL_HPL_REN(hgdo1) |= APP_GDO1_PORT_BIT;
    BSP430_PORT_HAL_HPL_OUT(hgdo1) |= APP_GDO1_PORT_BIT;
  }

  cprintf("SPI device %p hold returned %d\n", spi, rc);
  if (! spi) {
    return;
  }

  /* Configure CSn initial high to ensure we have a falling edge when
   * we first enable the radio. */
  BSP430_PORT_HAL_HPL_SEL(hcsn) &= ~APP_CSn_PORT_BIT;
  BSP430_PORT_HAL_HPL_OUT(hcsn) |= APP_CSn_PORT_BIT;
  BSP430_PORT_HAL_HPL_DIR(hcsn) |= APP_CSn_PORT_BIT;

  /* Now enable the radio */
  BSP430_PORT_HAL_HPL_OUT(hcsn) &= ~APP_CSn_PORT_BIT;

  /* Spin until GDO1 (CHP_RDYn) is clear indicating radio is responsive */
  while (BSP430_PORT_HAL_HPL_IN(hgdo1) & APP_GDO1_PORT_BIT) {
    cprintf("Waiting for radio ready\n");
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }

  /* Enable SPI */
  rc = iBSP430serialSetHold_ni(spi, 0);
  cprintf("Radio is up, hold release %d; sending SRES strobe\n", rc);

  /* Send a reset */
  do {
    rc = sendStrobe(0x30);
    cprintf("Strobe response %#02x\n", rc);
    if (0x0F != rc) {
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
    }
  } while (0x0F != rc);

  cprintf("PARTNUM response %#02x\n", readRegister(0x30));
  cprintf("VERSION response %#02x\n", readRegister(0x31));
  cprintf("IOCFG2 read %#02x\n", readRegister(0x00));
  cprintf("IOCFG1 read %#02x\n", readRegister(0x01));
  cprintf("IOCFG0 read %#02x\n", readRegister(0x02));

  /* ChipCon radios consume 1.4mA when idle.  That goes down to
   * nominally 400 nA if the GDOs are configured to "HW to 0" and the
   * chip is told to power-down on loss of CSn.  On the EXP430F5438
   * the RF PWR header indicates that a CC1101 is using 40 nA in this
   * mode.*/
  rc = writeRegister(0x00, 0x2f);
  rc = writeRegister(0x01, 0x2f);
  rc = writeRegister(0x02, 0x2f);
  cprintf("Cleared IOCFG\n");
  cprintf("IOCFG2 read %#02x\n", readRegister(0x00));
  cprintf("IOCFG1 read %#02x\n", readRegister(0x01));
  cprintf("IOCFG0 read %#02x\n", readRegister(0x02));

  /* SPWD */
  rc = sendStrobe(0x39);
  cprintf("SPWD got %d\n", rc);

  /* Disable SPI before removing CSn otherwise the sequence isn't
   * right. */
  rc = iBSP430serialSetHold_ni(spi, 1);
  BSP430_PORT_HAL_HPL_OUT(hcsn) |= APP_CSn_PORT_BIT;

  /* This gets the RF2500T power down to about 120 nA. */
  BSP430_CORE_LPM_ENTER_NI(LPM4_bits);
}
