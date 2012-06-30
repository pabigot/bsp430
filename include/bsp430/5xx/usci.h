/* Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>
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
 * Genericized UCSI on 5xx/6xx devices
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_5XX_USCI_H
#define BSP430_5XX_USCI_H

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include <bsp430/5xx/periph.h>

/** Structure holding USCI-related information */
typedef struct bsp430_FreeRTOS_USCI {
	/** Flags indicating various things; primarily whether anybody is
	 * using the device. */
	unsigned int flags;
	
	/** Pointer to the peripheral register structure. */
	volatile bsp430_USCI * const usci;

	/** Queue used to collect input via interrupt.  If null,
	 * interrupts are not used for reception. */
	xQueueHandle rx_queue;

	/** Queue used to transmit output via interrupt.  If null,
	 * interrupts are not used for transmission. */
	xQueueHandle tx_queue;

	/** Total number of received octets */
	unsigned long num_rx;

	/** Total number of transmitted octets */
	unsigned long num_tx;
} bsp430_FreeRTOS_USCI;

/** Find the FreeRTOS USCI structure associated with the given USCI device
 *
 * @return The corresponding bsp430_FreeRTOS_USCI Device, or a null
 * pointer if there is no such device. */
bsp430_FreeRTOS_USCI*
bsp430_usci_lookup (int devid);

/** Request and configure a USCI device in UART mode.
 *
 * @param devnum The device that is being requested, e.g. BSP430_USCI_A0.
 *
 * @param control_word The configuration to be written to the device's
 * ctlw0 word.  Most bit fields will be assigned, but UCSYNC will be
 * cleared, and UCSSELx will be set based on baud rate.
 *
 * @param baud The desired baud rate.  This will be configured
 * based on the current clock setting, using ACLK if the rate is low
 * enough and SMCLK otherwise.
 *
 * @param rx_queue A references to a queue to be used for receiving.
 * A non-null value enables interrupt-driven reception, and data
 * should be read from the queue by the application.
 *
 * @param tx_queue A references to a queue to be used for
 * transmitting.  A non-null value enables interrupt-driven
 * transmission, and the application should add data to the queue for
 * transmission.
 *
 * @return A pointer to the allocated and configured USCI peripheral
 * if successful; a null pointer if something went wrong. */
bsp430_FreeRTOS_USCI*
bsp430_usci_uart_open (int devid,
					   unsigned int control_word,
					   unsigned long baud,
					   xQueueHandle rx_queue,
					   xQueueHandle tx_queue);

bsp430_FreeRTOS_USCI*
bsp430_usci_spi_open (int devid,
					  unsigned int control_word,
					  unsigned int prescaler,
					  xQueueHandle rx_queue,
					  xQueueHandle tx_queue);

/** Release a USCI device.
 *
 * This places the device into reset mode, reset the peripheral pins
 * to port function, and releases any resources allocated when the
 * device was opened.
 *
 * @param device The device to be closed.
 *
 * @return 0 if the close occurred without error. */
int bsp430_usci_close (bsp430_FreeRTOS_USCI* device);

/** Wake up the interrupt-driven transmission if necessary.
 *
 * Normally the transmission infrastructure transmits data that is
 * added to the queue.  However, the infrastructure is disabled when
 * the transmit queue is emptied.  When this has happened, it must be
 * told that more data has been added and the infrastructure
 * re-enabled.
 *
 * For efficiency, this should only be called if it is believed that
 * data is present in the transmit queue but that the transmission
 * infrastructure may be idle.
 *
 * @param device A USCI device which must have a transmit queue.
 */
void bsp430_usci_wakeup_transmit (bsp430_FreeRTOS_USCI* device);

/** Transmit a single character on a queued USCI device.
 *
 * @param device The device on which the character should be
 * transmitted.  It is an error to use this function if the device has
 * a transmit queue; in such a case this call is likely to hang.
 *
 * @param c the character to be written
 *
 * @return @C c. */
static int
__inline__
bsp430_usci_raw_transmit (bsp430_FreeRTOS_USCI* device,
						  int c)
{
	/* Spin until the transmit buffer is available */
	while (! (device->usci->ifg & UCTXIFG)) {
		;
	}
	/* Queue the octet for transmission */
	device->usci->txbuf = c;
	return c;
}

/** Analog to fputc */
int bsp430_uart_putc (int c, bsp430_FreeRTOS_USCI* hsuart);

/** Analog to fputs */
int bsp430_uart_puts (const char* str, bsp430_FreeRTOS_USCI* hsuart);

#endif /* BSP430_5XX_USCI_H */
