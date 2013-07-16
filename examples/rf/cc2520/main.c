/** This file is in the public domain.
 *
 * Basic validation of SPI using the RFEM+CC2520EMK to query the
 * CC2520 for its part number.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>
#include <bsp430/rf/cc2520.h>

#define CS_ASSERT() do {                                                \
    volatile sBSP430hplPORT * port = xBSP430hplLookupPORT(BSP430_RF_CC2520_CSn_PORT_PERIPH_HANDLE); \
    port->out &= ~BSP430_RF_CC2520_CSn_PORT_BIT;                    \
  } while (0)

#define CS_DEASSERT() do {                                              \
    volatile sBSP430hplPORT * port = xBSP430hplLookupPORT(BSP430_RF_CC2520_CSn_PORT_PERIPH_HANDLE); \
    port->out |= BSP430_RF_CC2520_CSn_PORT_BIT;                     \
  } while (0)

#define RESET_ASSERT() do {                                             \
    volatile sBSP430hplPORT * port = xBSP430hplLookupPORT(BSP430_RF_CC2520_RESETn_PORT_PERIPH_HANDLE); \
    port->out &= ~BSP430_RF_CC2520_RESETn_PORT_BIT;                     \
  } while (0)

#define RESET_DEASSERT() do {                                           \
    volatile sBSP430hplPORT * port = xBSP430hplLookupPORT(BSP430_RF_CC2520_RESETn_PORT_PERIPH_HANDLE); \
    port->out |= BSP430_RF_CC2520_RESETn_PORT_BIT;                      \
  } while (0)

hBSP430halSERIAL spi;

static uint8_t
sendStrobe (uint8_t reg)
{
  uint8_t rc = 0;

  (void)iBSP430spiTxRx_rh(spi, &reg, 1, 0, &rc);
  return rc;
}

static uint8_t
readRegister (unsigned int addr)
{
  uint8_t txbuf[2];
  uint8_t rxbuf[sizeof(txbuf) + 1];

  /* MEMRD command */
  txbuf[0] = 0x10 | (0x03 & (addr >> 8));
  txbuf[1] = (addr & 0xFF);
  (void)iBSP430spiTxRx_rh(spi, txbuf, sizeof(txbuf), 1, rxbuf);
  return rxbuf[sizeof(txbuf)];
}

#if 0
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
#endif

void main ()
{
  int rc = 0;
  volatile sBSP430hplPORT * so = xBSP430hplLookupPORT(BSP430_RF_CC2520_SO_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORT * csn = xBSP430hplLookupPORT(BSP430_RF_CC2520_CSn_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORT * rstn = xBSP430hplLookupPORT(BSP430_RF_CC2520_RESETn_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORT * gpio0 = xBSP430hplLookupPORT(BSP430_RF_CC2520_GPIO0_PORT_PERIPH_HANDLE);
  spi = hBSP430serialLookup(BSP430_RF_CC2520_SPI_PERIPH_HANDLE);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  cprintf("\nccid " __DATE__ " " __TIME__ "\n");

  cprintf("CSn on %s.%u at %p\n", xBSP430portName(BSP430_RF_CC2520_CSn_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RF_CC2520_CSn_PORT_BIT), csn);
  cprintf("RESETn on %s.%u at %p\n", xBSP430portName(BSP430_RF_CC2520_RESETn_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RF_CC2520_RESETn_PORT_BIT), rstn);
  cprintf("SO on %s.%u at %p\n", xBSP430portName(BSP430_RF_CC2520_SO_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RF_CC2520_SO_PORT_BIT), so);
  cprintf("GPIO0 at %s.%u\n", xBSP430portName(BSP430_RF_CC2520_GPIO0_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RF_CC2520_GPIO0_PORT_BIT));
  cprintf("SPI %p is %s\n", spi, xBSP430serialName(BSP430_RF_CC2520_SPI_PERIPH_HANDLE));
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("SPI Pins: %s\n", xBSP430platformPeripheralHelp(BSP430_RF_CC2520_SPI_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_SPI3));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  cprintf(__DATE__ " " __TIME__ "\n");

  /* SO to input, pull-up */
  so->dir &= ~BSP430_RF_CC2520_SO_PORT_BIT;
  BSP430_PORT_HPL_SET_REN(so, BSP430_RF_CC2520_SO_PORT_BIT, BSP430_PORT_REN_PULL_UP);

  /* CSnn to output, high */
  csn->dir |= BSP430_RF_CC2520_CSn_PORT_BIT;
  csn->out |= BSP430_RF_CC2520_CSn_PORT_BIT;

  /* RSTn to output, low */
  rstn->dir |= BSP430_RF_CC2520_RESETn_PORT_BIT;
  rstn->out &= ~BSP430_RF_CC2520_RESETn_PORT_BIT;

  /* GPIO0 as input, pull down */
  gpio0->dir &= ~BSP430_RF_CC2520_GPIO0_PORT_BIT;
  BSP430_PORT_HPL_SET_REN(gpio0, BSP430_RF_CC2520_GPIO0_PORT_BIT, BSP430_PORT_REN_PULL_DOWN);

  cprintf("Power-up using RESETn\n");
  RESET_ASSERT();
  /* Delay 100 usec */
  BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 10000);
  RESET_DEASSERT();
  /* Delay 200 usec */
  BSP430_CORE_DELAY_CYCLES((2 * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 10000);
  (void)rc;

  CS_ASSERT();
  do {
    cprintf("Waiting for OSC stabilization\n");
  } while (! (so->in & BSP430_RF_CC2520_SO_PORT_BIT));
  CS_DEASSERT();

  /* Configure the SPI interface, but immediately put it into hold
   * mode so we can check CHIP_RDYn on the MISO/GDO1 input.
   *
   * Note: you might want to use a slow clock rate here so you can
   * follow along with a logic analyzer. */
  spi = hBSP430serialOpenSPI(spi, BSP430_RF_CC2520_SPI_CTL0_BYTE, 0, 0);
  cprintf("SPI device %p hold returned %d\n", spi, rc);
  if (! spi) {
    return;
  }
  CS_ASSERT();
  cprintf("SNOP gets %02x\n", sendStrobe(0));
  cprintf("Read CHIPID gets %02x should be 0x84\n", readRegister(0x40));
  CS_DEASSERT();

  RESET_ASSERT();
}
