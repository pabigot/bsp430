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

/* Implementation of tWlanReadInterruptPin for wlan_init() */
static long
readWlanInterruptPin_ (void)
{
  volatile sBSP430hplPORTIE * const spi_irq_port = xBSP430hplLookupPORTIE(BSP430_RFEM_SPI_IRQ_PORT_PERIPH_HANDLE);
  return spi_irq_port->in & BSP430_RFEM_SPI_IRQ_PORT_BIT;
}

/* Implementation of tWlanInterruptEnable for wlan_init() */
static void
wlanInterruptEnable_ (void)
{
  volatile sBSP430hplPORTIE * const spi_irq_port = xBSP430hplLookupPORTIE(BSP430_RFEM_SPI_IRQ_PORT_PERIPH_HANDLE);

  spi_irq_port->ie |= BSP430_RFEM_SPI_IRQ_PORT_BIT;
}

/* Implementation of tWlanInterruptDisable for wlan_init() */
static void
wlanInterruptDisable_ (void)
{
  volatile sBSP430hplPORTIE * const spi_irq_port = xBSP430hplLookupPORTIE(BSP430_RFEM_SPI_IRQ_PORT_PERIPH_HANDLE);
  spi_irq_port->ie &= ~BSP430_RFEM_SPI_IRQ_PORT_BIT;
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
  wlan_init(wlan_cb, firmware_patch_fn, driver_patch_fn, boot_loader_patch_fn,
            readWlanInterruptPin_, wlanInterruptEnable_, wlanInterruptDisable_,
            writeWlanPin_);
  return 0;
}

unsigned char wlan_tx_buffer[BSP430_CC3000SPI_TX_BUFFER_SIZE];

void
SpiOpen (gcSpiHandleRx pfRxHandler)
{
  cprintf("%s SpiOpen(%p)\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), pfRxHandler);
}

void
SpiClose (void)
{
}

long
SpiWrite (unsigned char * tx_buffer,
          unsigned short len)
{
  return 0;
}

void
SpiResumeSpi (void)
{
}
