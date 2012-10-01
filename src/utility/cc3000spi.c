/* Copyright (c) 2012, Peter A. Bigot
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the software nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @file
 *
 * @brief Implementation for the BSP430 CC3000 SPI Host Driver interface
 *
 * This follows the specification at
 * http://processors.wiki.ti.com/index.php/CC3000_Host_Driver_Porting_Guide,
 * informed by the use of these functions in the host driver version
 * 6.10.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/platform.h>
#include <bsp430/utility/cc3000spi.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/port.h>
#include <bsp430/serial.h>
#include <cc3000/cc3000_common.h>
#include <cc3000/wlan.h>
#include <cc3000/spi.h>

unsigned char wlan_tx_buffer[BSP430_CC3000SPI_TX_BUFFER_SIZE];

#define SPIFLAG_INITIALIZED 0x01
#define SPIFLAG_DID_FIRST_WRITE 0x02

static unsigned int spiFlags_;
static hBSP430halSERIAL spi_;
static hBSP430halPORT halCSn_;
static volatile sBSP430hplPORTIE * spiIRQport_;
static gcSpiHandleRx rxCallback_;

#define ASSERT_CS() do {                                                \
    BSP430_PORT_HAL_HPL_OUT(halCSn_) &= ~BSP430_RFEM_SPI0CSn_PORT_BIT;  \
  } while (0)
  
#define DEASSERT_CS() do {                                              \
    BSP430_PORT_HAL_HPL_OUT(halCSn_) |= BSP430_RFEM_SPI0CSn_PORT_BIT;   \
  } while (0)


/* Implementation of tWlanReadInterruptPin for wlan_init() */
static long
readWlanInterruptPin_ (void)
{
  return spiIRQport_->in & BSP430_RFEM_SPI_IRQ_PORT_BIT;
}

/* Implementation of tWlanInterruptEnable for wlan_init() */
static void
wlanInterruptEnable_ (void)
{
  spiIRQport_->ie |= BSP430_RFEM_SPI_IRQ_PORT_BIT;
}

/* Implementation of tWlanInterruptDisable for wlan_init() */
static void
wlanInterruptDisable_ (void)
{
  spiIRQport_->ie &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
}

/* Implementation of tWriteWlanPin for wlan_init() */
static void
writeWlanPin_ (unsigned char val)
{
  volatile sBSP430hplPORTIE * const pwr_en_port = xBSP430hplLookupPORTIE(BSP430_RFEM_PWR_EN_PORT_PERIPH_HANDLE);
  cprintf("%s writeWlanPin_(%d)\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), val);
  if (val) {
    pwr_en_port->out |= BSP430_RFEM_PWR_EN_PORT_BIT;
  } else {
    pwr_en_port->out &= ~BSP430_RFEM_PWR_EN_PORT_BIT;
  }
}

int
iBSP430cc3000spiInitialize (tWlanCB wlan_cb,
                            tFWPatches firmware_patch_fn,
                            tDriverPatches driver_patch_fn,
                            tBootLoaderPatches boot_loader_patch_fn)
{
  spi_ = hBSP430serialLookup(BSP430_RFEM_SPI0_PERIPH_HANDLE);
  if (NULL == spi_) {
    return -1;
  }
  
  spiIRQport_ = xBSP430hplLookupPORTIE(BSP430_RFEM_SPI_IRQ_PORT_PERIPH_HANDLE);
  if (NULL == spiIRQport_) {
    return -1;
  }
  
  halCSn_ = hBSP430portLookup(BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE);
  if (NULL == halCSn_) {
    return -1;
  }

  wlan_init(wlan_cb, firmware_patch_fn, driver_patch_fn, boot_loader_patch_fn,
            readWlanInterruptPin_, wlanInterruptEnable_, wlanInterruptDisable_,
            writeWlanPin_);
  return 0;
}

static int
processSpiIRQ_ (const struct sBSP430halISRIndexedChainNode * cb,
                void * context,
                int idx)
{
  cprintf("%s processSpiIRQ_()\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()));
  spiFlags_ |= SPIFLAG_INITIALIZED;
  return 0;
}

static sBSP430halISRIndexedChainNode spi_irq_cb = { .callback = processSpiIRQ_ };

void
SpiOpen (gcSpiHandleRx pfRxHandler)
{
  hBSP430halPORT spi_irq_hal = hBSP430portLookup(BSP430_RFEM_SPI_IRQ_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * const pwr_en_port = xBSP430hplLookupPORTIE(BSP430_RFEM_PWR_EN_PORT_PERIPH_HANDLE);
  unsigned int spi_irq_pin = iBSP430portBitPosition(BSP430_RFEM_SPI_IRQ_PORT_BIT);
  BSP430_CORE_INTERRUPT_STATE_T istate;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();

  cprintf("%s SpiOpen(%p)\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), pfRxHandler);

  /* Clear the SPI flags */
  spiFlags_ = 0;

  /* Preserve the callback pointer */
  rxCallback_ = pfRxHandler;

  /* Configure CC3000 power-enable pin and turn off power.  It'll be
   * turned on in wlan_start(). */
  pwr_en_port->out &= ~BSP430_RFEM_PWR_EN_PORT_BIT;
  pwr_en_port->dir |= BSP430_RFEM_PWR_EN_PORT_BIT;

  /* Clear chip select (set value before making it an output) */
  DEASSERT_CS();
  BSP430_PORT_HAL_HPL_DIR(halCSn_) |= BSP430_RFEM_SPI0CSn_PORT_BIT;

  /* Open the device for 3-wire SPI.  Per
   * http://processors.wiki.ti.com/index.php/CC3000_Host_Programming_Guide#CC3000_SPI_Operation
   * the CC3000 can accept clock speeds up to 26MHz, so we might as
   * well just use undivided SMCLK. */
  spi_ = hBSP430serialOpenSPI(spi_,
                              BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMSB | UCMST),
                              UCSSEL_2, 1);

  /* Hook into the interrupt vector for the SPI IRQ, then enable
   * interrupts on falling edge.  When wlan_start() turns on power,
   * we'll get an interrupt indicating the chip is ready (wlan_start()
   * also busy-waits internally for this event). */
  spi_irq_cb.next_ni = spi_irq_hal->pin_cbchain_ni[spi_irq_pin];
  spi_irq_hal->pin_cbchain_ni[spi_irq_pin] = &spi_irq_cb;
  spiIRQport_->ies |= BSP430_RFEM_SPI_IRQ_PORT_BIT;
  spiIRQport_->dir &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
  spiIRQport_->ifg &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
  tSLInformation.WlanInterruptEnable();

  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

void
SpiClose (void)
{
  hBSP430halPORT spi_irq_hal = hBSP430portLookup(BSP430_RFEM_SPI_IRQ_PORT_PERIPH_HANDLE);
  volatile sBSP430hplPORTIE * const pwr_en_port = xBSP430hplLookupPORTIE(BSP430_RFEM_PWR_EN_PORT_PERIPH_HANDLE);
  unsigned int spi_irq_pin = iBSP430portBitPosition(BSP430_RFEM_SPI_IRQ_PORT_BIT);
  BSP430_CORE_INTERRUPT_STATE_T istate;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();

  cprintf("%s SpiClose()\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()));

  /* Disable interrupts, clear pending interrupt, and unhook from
   * the ISR chain.  Set the GPIO to output low. */
  tSLInformation.WlanInterruptDisable();
  spiIRQport_->ifg &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
  spi_irq_hal->pin_cbchain_ni[spi_irq_pin] = spi_irq_cb.next_ni;
  spiIRQport_->out &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
  spiIRQport_->dir |= BSP430_RFEM_SPI_IRQ_PORT_BIT;

  /* Close down SPI peripheral */
  (void)iBSP430serialClose(spi_);

  /* Release the chip select line, just for completeness. */
  DEASSERT_CS();

  /* Power down the CC3000.  In fact, this was already done by
   * wlan_stop(). */
  pwr_en_port->out &= ~BSP430_RFEM_PWR_EN_PORT_BIT;

  rxCallback_ = NULL;

  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

long
SpiWrite (unsigned char * tx_buffer,
          unsigned short len)
{
  cprintf("%s SpiWrite(%p, %u)\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), tx_buffer, len);
  return 0;
}

void
SpiResumeSpi (void)
{
  cprintf("%s SpiResume()\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()));
}
