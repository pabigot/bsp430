/** This file is in the public domain.
 *
 * Basic validation of SPI using an RFEM+CC110XEMK or EXP430G2+Anaren
 * AIR to query the CC110X for its part number and version.
 *
 * This also works for CC2500 radios if the EMK hardware is used; the
 * difference will be evident in PARTNUM and VERSION output.
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
#include <bsp430/rf/cc110x.h>

hBSP430halSERIAL spi;

static uint8_t
sendStrobe (uint8_t reg)
{
  uint8_t rc = 0;

  (void)iBSP430spiTxRx_rh(spi, &reg, 1, 0, &rc);
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
  (void)iBSP430spiTxRx_rh(spi, &reg, 1, 1, rxbuf);
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
  (void)iBSP430spiTxRx_rh(spi, txbuf, 2, 0, rxbuf);
  return rxbuf[1];
}

void main ()
{
  int rc = 0;

  /* GDO0 and GDO2 are always interrupt-capable. */
  volatile sBSP430hplPORTIE * gdo0 = xBSP430hplLookupPORTIE(BSP430_RF_CC110X_GDO0_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * gdo2 = xBSP430hplLookupPORTIE(BSP430_RF_CC110X_GDO2_PORT_PERIPH_HANDLE);
  hBSP430halPORT hgdo1 = hBSP430portLookup(BSP430_RF_CC110X_GDO1_PORT_PERIPH_HANDLE);
  hBSP430halPORT hcsn = hBSP430portLookup(BSP430_RF_CC110X_CSn_PORT_PERIPH_HANDLE);

  spi = hBSP430serialLookup(BSP430_RF_CC110X_SPI_PERIPH_HANDLE);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  cprintf("\nccid " __DATE__ " " __TIME__ "\n");

  cprintf("GDO0 %p at %s.%u\n", gdo0, xBSP430portName(BSP430_RF_CC110X_GDO0_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RF_CC110X_GDO0_PORT_BIT));
  cprintf("GDO1 HAL %p HPL %p at %s.%u\n", hgdo1, hgdo1->hpl.any, xBSP430portName(BSP430_RF_CC110X_GDO1_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RF_CC110X_GDO1_PORT_BIT));
  cprintf("GDO2 %p at %s.%u\n", gdo2, xBSP430portName(BSP430_RF_CC110X_GDO2_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RF_CC110X_GDO2_PORT_BIT));
  cprintf("CSn HAL %p HPL %p at %s.%u\n", hcsn, hcsn->hpl.any, xBSP430portName(BSP430_RF_CC110X_CSn_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RF_CC110X_CSn_PORT_BIT));
  cprintf("SPI %p is %s\n", spi, xBSP430serialName(BSP430_RF_CC110X_SPI_PERIPH_HANDLE));
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("SPI Pins: %s\n", xBSP430platformPeripheralHelp(BSP430_RF_CC110X_SPI_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_SPI3));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  cprintf(__DATE__ " " __TIME__ "\n");


  /* Configure the SPI interface, but immediately put it into hold
   * mode so we can check CHIP_RDYn on the MISO/GDO1 input. */
  spi = hBSP430serialOpenSPI(spi, BSP430_RF_CC110X_SPI_CTL0_BYTE, 0, 0);
  if (spi) {
    rc = iBSP430serialSetHold_rh(spi, 1);
    /* GDO1 to input, pull-up */
    BSP430_PORT_HAL_HPL_DIR(hgdo1) &= ~BSP430_RF_CC110X_GDO1_PORT_BIT;
    BSP430_PORT_HAL_HPL_REN(hgdo1) |= BSP430_RF_CC110X_GDO1_PORT_BIT;
    BSP430_PORT_HAL_HPL_OUT(hgdo1) |= BSP430_RF_CC110X_GDO1_PORT_BIT;
  }

  cprintf("SPI device %p hold returned %d\n", spi, rc);
  if (! spi) {
    return;
  }

  /* Configure CSn initial high to ensure we have a falling edge when
   * we first enable the radio. */
  BSP430_PORT_HAL_HPL_SEL(hcsn) &= ~BSP430_RF_CC110X_CSn_PORT_BIT;
  BSP430_PORT_HAL_HPL_OUT(hcsn) |= BSP430_RF_CC110X_CSn_PORT_BIT;
  BSP430_PORT_HAL_HPL_DIR(hcsn) |= BSP430_RF_CC110X_CSn_PORT_BIT;

  /* Now enable the radio */
  BSP430_PORT_HAL_HPL_OUT(hcsn) &= ~BSP430_RF_CC110X_CSn_PORT_BIT;

  /* Spin until GDO1 (CHP_RDYn) is clear indicating radio is responsive */
  while (BSP430_PORT_HAL_HPL_IN(hgdo1) & BSP430_RF_CC110X_GDO1_PORT_BIT) {
    cprintf("Waiting for radio ready\n");
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }

  /* Enable SPI */
  rc = iBSP430serialSetHold_rh(spi, 0);
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
  rc = iBSP430serialSetHold_rh(spi, 1);
  BSP430_PORT_HAL_HPL_OUT(hcsn) |= BSP430_RF_CC110X_CSn_PORT_BIT;

  /* This gets the RF2500T power down to about 120 nA.  Note:
   * Purposefully enter LPM with #GIE off since we do not intend to
   * wake up.*/
  BSP430_CORE_LPM_ENTER(LPM3_bits);
}
