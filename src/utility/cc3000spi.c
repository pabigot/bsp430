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
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/port.h>
#include <bsp430/serial.h>
#include <cc3000/cc3000_common.h>
#include <cc3000/wlan.h>
#include <cc3000/hci.h>
#include <cc3000/spi.h>

/** Globally visible array wherein CC3000 host driver will place data
 * to be written to the device.  Poor API design, but what can you
 * do? */
#if defined(__MSP430FR5739__)
__attribute__((__section__(".rodata")))
#endif /* __MSP430FR5739__ */
unsigned char wlan_tx_buffer[BSP430_CC3000SPI_TX_BUFFER_SIZE];

/** Not globally visible array for local collection of incoming data
 * from the CC3000. */
#if defined(__MSP430FR5739__)
__attribute__((__section__(".rodata")))
#endif /* __MSP430FR5739__ */
static unsigned char wlan_rx_buffer[BSP430_CC3000SPI_RX_BUFFER_SIZE];

/** Bit set in spiFlags_ when the CC3000 has powered up and is
 * attentive (via an IRQ after a power-up). */
#define SPIFLAG_INITIALIZED 0x01

/** Bit set in spiFlags_ after the first SPI write has been completed.
 * Per
 * http://processors.wiki.ti.com/index.php/CC3000_Host_Programming_Guide#First_Host_Write_Operation,
 * this first operation has special timing requirements. */
#define SPIFLAG_DID_FIRST_WRITE 0x02

/** The CC3000 transaction header opcode used when writing to the
 * device. */
#define CC3000_SPI_OPCODE_WRITE 1

/** The CC3000 transaction header opcode used when reading from the
 * device. */
#define CC3000_SPI_OPCODE_READ 3

static unsigned int spiFlags_;
static hBSP430halSERIAL spi_;
static hBSP430halPORT halCSn_;
static volatile sBSP430hplPORTIE * spiIRQport_;
static gcSpiHandleRx rxCallback_ni_;

/* Assert chip select by clearing CSn */
#define CS_ASSERT() do {                                                \
    BSP430_PORT_HAL_HPL_OUT(halCSn_) &= ~BSP430_RFEM_SPI0CSn_PORT_BIT;  \
  } while (0)
  
/* De-assert chip select by setting CSn */
#define CS_DEASSERT() do {                                              \
    BSP430_PORT_HAL_HPL_OUT(halCSn_) |= BSP430_RFEM_SPI0CSn_PORT_BIT;   \
  } while (0)

/* Implementation of tWlanReadInterruptPin for wlan_init().
 * 
 * This is used in wlan_start() to detect that the CC3000 has
 * successfully powered up. */
static long
readWlanInterruptPin_ (void)
{
  return spiIRQport_->in & BSP430_RFEM_SPI_IRQ_PORT_BIT;
}

/* Implementation of tWlanInterruptEnable for wlan_init().
 *
 * Required by the interface, but not used by the host driver
 * implementation.  Used internally. */
static void
wlanInterruptEnable_ (void)
{
  spiIRQport_->ie |= BSP430_RFEM_SPI_IRQ_PORT_BIT;
}

/* Implementation of tWlanInterruptDisable for wlan_init().
 *
 * Required by the interface, but not used by the host driver
 * implementation.  Used internally.  */
static void
wlanInterruptDisable_ (void)
{
  spiIRQport_->ie &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
}

/* Implementation of tWriteWlanPin for wlan_init().
 *
 * This is used in wlan_start() to power-up the CC3000, and in
 * wlan_stop() to shut it down again. */
static void
writeWlanPin_ (unsigned char val)
{
  volatile sBSP430hplPORTIE * const pwr_en_port = xBSP430hplLookupPORTIE(BSP430_RFEM_PWR_EN_PORT_PERIPH_HANDLE);

  if (val) {
    pwr_en_port->out |= BSP430_RFEM_PWR_EN_PORT_BIT;
  } else {
    pwr_en_port->out &= ~BSP430_RFEM_PWR_EN_PORT_BIT;
    spiFlags_ &= ~SPIFLAG_INITIALIZED;
  }
}

int
iBSP430cc3000spiInitialize (tWlanCB wlan_cb,
                            tFWPatches firmware_patch_fn,
                            tDriverPatches driver_patch_fn,
                            tBootLoaderPatches boot_loader_patch_fn)
{
  if (NULL == hBSP430serialLookup(BSP430_RFEM_SPI0_PERIPH_HANDLE)) {
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
  if (spiFlags_ & SPIFLAG_INITIALIZED) {
    int rv;
    const unsigned char opcode = CC3000_SPI_OPCODE_READ;
    unsigned char * rp;
    unsigned int len;
    
    /* This is a signal that data is available.  Read it.  Yes, right
     * here in the ISR.  The host driver API would need to be
     * integrated with a multi-tasking OS to do it elsewhere, since
     * it's not event-driven at the user level. */
    CS_ASSERT();
    /* Read the header.  From that we'll extract the length. */
    rp = wlan_rx_buffer;
    rv = iBSP430spiTxRx_ni(spi_, &opcode, sizeof(opcode), SPI_HEADER_SIZE-sizeof(opcode), rp);
    if (SPI_HEADER_SIZE == rv) {
      len = (wlan_rx_buffer[HCI_PACKET_LENGTH_OFFSET] << 8) | wlan_rx_buffer[1 + HCI_PACKET_LENGTH_OFFSET];
      rp += SPI_HEADER_SIZE;
      rv = iBSP430spiTxRx_ni(spi_, NULL, 0, len, rp);
    } else {
      rv = -1;
    }
    CS_DEASSERT();
    if (0 < rv) {
      /* Turn off the interrupt while we process this message.  The
       * driver will invoke SpiResume() to turn it back on. */
      wlanInterruptDisable_();

      /* Pass the packet off to the driver.  Again, yes, right here in
       * the ISR. */
      rxCallback_ni_(rp);
    }
  } else {
    /* If not initialized, this is the IRQ raised by the CC3000 to
     * notify the MCU that it is now powered up and active.  No data
     * to read, no callback to invoke. */
    spiFlags_ |= SPIFLAG_INITIALIZED;
  }
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
  do {
    if (spi_) {
      break;
    }

    /* Open the device for 3-wire SPI.  Per
     * http://processors.wiki.ti.com/index.php/CC3000_Host_Programming_Guide#CC3000_SPI_Operation
     * the CC3000 can accept clock speeds up to 26MHz, so we might as
     * well just use undivided SMCLK. */
    spi_ = hBSP430serialOpenSPI(hBSP430serialLookup(BSP430_RFEM_SPI0_PERIPH_HANDLE),
                                BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMSB | UCMST),
                                UCSSEL_2, 1);
    if (! spi_) {
      break;
    }

    /* Clear the SPI flags */
    spiFlags_ = 0;

    /* Preserve the callback pointer */
    rxCallback_ni_ = pfRxHandler;

    /* Configure CC3000 power-enable pin and turn off power.  It'll be
     * turned on in wlan_start(). */
    pwr_en_port->out &= ~BSP430_RFEM_PWR_EN_PORT_BIT;
    pwr_en_port->dir |= BSP430_RFEM_PWR_EN_PORT_BIT;

    /* Clear chip select (set value before making it an output) */
    CS_DEASSERT();
    BSP430_PORT_HAL_HPL_DIR(halCSn_) |= BSP430_RFEM_SPI0CSn_PORT_BIT;

    /* Hook into the interrupt vector for the SPI IRQ, then enable
     * interrupts on falling edge.  When wlan_start() turns on power,
     * we'll get an interrupt indicating the chip is ready (wlan_start()
     * also busy-waits internally for this event). */
    spi_irq_cb.next_ni = spi_irq_hal->pin_cbchain_ni[spi_irq_pin];
    spi_irq_hal->pin_cbchain_ni[spi_irq_pin] = &spi_irq_cb;
    spiIRQport_->ies |= BSP430_RFEM_SPI_IRQ_PORT_BIT;
    spiIRQport_->dir &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
    spiIRQport_->ifg &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
    wlanInterruptEnable_();
  } while (0);
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
  do {
    if (! spi_) {
      break;
    }

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
    CS_DEASSERT();
    
    /* Power down the CC3000.  In fact, this was already done by
     * wlan_stop(). */
    pwr_en_port->out &= ~BSP430_RFEM_PWR_EN_PORT_BIT;

    rxCallback_ni_ = NULL;
    spi_ = NULL;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

long
SpiWrite (unsigned char * tx_buffer,
          unsigned short len)
{
  int rv;
  unsigned char * tp = tx_buffer;
  BSP430_CORE_INTERRUPT_STATE_T istate;

  /* Total length of packet must be an even number of octets.  Packet
   * length is user-provided length plus the length of the SPI header,
   * which happens to be odd.  Pad the payload if necessary. */
  if (0x01 & (len + SPI_HEADER_SIZE)) {
    ++len;
  }

  /* The caller-provided tx_buffer includes SPI_HEADER_SIZE octets at
   * the front, ready for our use. */
  *tp++ = CC3000_SPI_OPCODE_WRITE;
  *tp++ = len >> 8;
  *tp++ = len & 0x0FF;
  *tp++ = 0;
  *tp++ = 0;
  len += SPI_HEADER_SIZE;
  
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();

  CS_ASSERT();
  /* Spin waiting for acknowledgement, then clear the flag that would
   * look like a read request when we exit. */
  while (0 != readWlanInterruptPin_()) {
    ;
  }
  spiIRQport_->ifg &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;

  /* Special handling for first write */
  if (! (spiFlags_ & SPIFLAG_DID_FIRST_WRITE)) {
    BSP430_CORE_DELAY_CYCLES((50 * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000UL);
    tp = tx_buffer;
    rv = iBSP430spiTxRx_ni(spi_, tp, 4, 0, NULL);
    tp += 4;
    len -= 4;
    BSP430_CORE_DELAY_CYCLES((50 * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000UL);
  }
  rv = iBSP430spiTxRx_ni(spi_, tp, len, 0, NULL);
  CS_DEASSERT();

  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

  return (0 <= rv) ? rv : 0;
}

/* Driver calls this to restore interrupts after processing an
 * incoming message. */
void
SpiResumeSpi (void)
{
  wlanInterruptEnable_();
}
