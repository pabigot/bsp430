/** This file is in the public domain.
 *
 * Basic validation of SPI using the RF2500T to query the CC2500 for its
 * part number and version.
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
  uint8_t rc;
  
  (void)iBSP430serialSynchronousTransmitReceive_ni(spi, &reg, 1, 0, &rc);
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
  (void)iBSP430serialSynchronousTransmitReceive_ni(spi, &reg, 1, 1, rxbuf);
  return rxbuf[1];
}

void main ()
{
  uint8_t rc;
  
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("Initializing port for USCI_B0 and CSn\n");
  cprintf(__DATE__ " " __TIME__ "\n");
  spi = hBSP430serialLookup(BSP430_PERIPH_USCI_B0);
  cprintf("SPI device originally %p\n", spi);
  spi = hBSP430serialOpenSPI(spi, UCCKPH | UCMSB | UCMST, UCSSEL_2, 1);
  cprintf("SPI device %p: CTL0 %02x ; CTL1 %02x ; P3SEL %02x\n", spi, UCB0CTL0, UCB0CTL1, P3SEL);
  
  /* Configure for GPIO use: CS=P3.0 */
  P3SEL &= ~BIT0;
  P3DIR |= BIT0;
  P3OUT |= BIT0;
  
  /* Configure USCI_B0 */
  
  /* Release USCI_B0 to run */
  cprintf("Resetting radio\n");
  P3OUT &= ~BIT0;

  /* Spin until MISO (CHP_RDYn) is clear */
  while (P3IN & BIT2) {
    cprintf("Waiting for radio ready\n");
  }
  cprintf("Radio is up; sending SRES strobe\n");

  /* Send a reset */
  do {
    rc = sendStrobe(0x30);
    cprintf("Strobe response %#02x\n", rc);
  } while (0x0F != rc);

  cprintf("PARTNUM response %#02x\n", readRegister(0x30));
  cprintf("VERSION response %#02x\n", readRegister(0x31));
}
