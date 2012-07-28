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

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/periph/usci5_.h>
#include "task.h"

/* !BSP430! periph=usci5 */
/* !BSP430! instance=USCI5_A0,USCI5_A1,USCI5_A2,USCI5_A3,USCI5_B0,USCI5_B1,USCI5_B2,USCI5_B3 */

#define COM_PORT_ACTIVE  0x01

/** Convert from a raw peripheral handle to the corresponding USCI5
 * device handle. */
static xBSP430usci5Handle periphToDevice (xBSP430periphHandle periph);

xBSP430usci5Handle
xBSP430usci5OpenUART (xBSP430periphHandle periph,
					 unsigned int control_word,
					 unsigned long baud)
{
	BSP430_CORE_INTERRUPT_STATE_T istate;
	unsigned long brclk_Hz;
	xBSP430usci5Handle device = periphToDevice(periph);
	uint16_t br;
	uint16_t brs;

	configASSERT(NULL != device);

	BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
	BSP430_CORE_DISABLE_INTERRUPT();
	/* Reject invalid baud rates */
	if ((0 == baud) || (1000000UL < baud)) {
		device = NULL;
	}

	/* Reject if the pins can't be configured */
	if ((NULL != device)
		&& (0 != iBSP430platformConfigurePeripheralPins_ni((xBSP430periphHandle)(uintptr_t)(device->usci5), 1))) {
		device = NULL;
	}

	if (NULL != device) {
		/* Assume ACLK <= 20 kHz is VLOCLK and cannot be trusted.  Prefer
		 * 32 kiHz ACLK for rates that are low enough.  Use SMCLK for
		 * anything larger.  */
		brclk_Hz = usBSP430clockACLK_Hz_ni();
		if ((brclk_Hz > 20000) && (brclk_Hz >= (3 * baud))) {
			device->usci5->ctlw0 = UCSWRST | UCSSEL__ACLK;
		} else {
			device->usci5->ctlw0 = UCSWRST | UCSSEL__SMCLK;
			brclk_Hz = ulBSP430clockSMCLK_Hz_ni();
		}
		br = (brclk_Hz / baud);
		brs = (1 + 16 * (brclk_Hz - baud * br) / baud) / 2;

		device->usci5->brw = br;
		device->usci5->mctl = (0 * UCBRF0) | (brs * UCBRS0);

		/* Mark the device active */
		device->num_rx = device->num_tx = 0;
		device->flags |= COM_PORT_ACTIVE;

		/* Release the USCI5 and enable the interrupts.  Interrupts are
		 * disabled and cleared when UCSWRST is set. */
		device->usci5->ctlw0 &= ~UCSWRST;
		if (0 != device->rx_queue) {
			device->usci5->ie |= UCRXIE;
		}
	}
	BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

	return device;
}

int
iBSP430usci5ConfigureQueues (xBSP430usci5Handle device,
							xQueueHandle rx_queue,
							xQueueHandle tx_queue)
{
	BSP430_CORE_INTERRUPT_STATE_T istate;
	int rc = 0;
	
	BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
	BSP430_CORE_DISABLE_INTERRUPT();
	device->usci5->ctlw0 |= UCSWRST;
	if (device->rx_queue || device->tx_queue) {
		rc = -1;
	} else {
		device->rx_queue = rx_queue;
		device->tx_queue = tx_queue;
	}
	/* Release the USCI5 and enable the interrupts.  Interrupts are
	 * disabled and cleared when UCSWRST is set. */
	device->usci5->ctlw0 &= ~UCSWRST;
	if (0 != device->rx_queue) {
		device->usci5->ie |= UCRXIE;
	}
	BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

	return rc;
}

int
iBSP430usci5Close (xBSP430usci5Handle device)
{
	BSP430_CORE_INTERRUPT_STATE_T istate;
	int rc;
	
	BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
	BSP430_CORE_DISABLE_INTERRUPT();
	device->usci5->ctlw0 = UCSWRST;
	rc = iBSP430platformConfigurePeripheralPins_ni ((xBSP430periphHandle)(uintptr_t)(device->usci5), 0);
	device->flags = 0;
	BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);

	return rc;
}

/* If there's data in the transmit queue, and the transmit interrupt
 * is not enabled, then enable the interrupt.  Do NOT muck with TXIFG,
 * since it may be that the ISR just completed draining the queue but
 * the data has not been transmitted, in which case setting TXIFG
 * would cause the in-progress transmission to be corrupted.
 *
 * For this to work, of course, nobody else should ever muck with the
 * TXIFG bit.  Normal management of this bit via UCSWRST is
 * correct. */
#define USCI5_WAKEUP_TRANSMIT_FROM_ISR(device) do {			\
		if ((! xQueueIsQueueEmptyFromISR(device->tx_queue)) \
			&& (! (device->usci5->ie & UCTXIE))) {			\
			device->usci5->ie |= UCTXIE;						\
		}													\
	} while (0)

void
vBSP430usci5WakeupTransmit (xBSP430usci5Handle device)
{
	BSP430_CORE_INTERRUPT_STATE_T istate;
	BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
	BSP430_CORE_DISABLE_INTERRUPT();
	USCI5_WAKEUP_TRANSMIT_FROM_ISR(device);
	BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

#define RAW_TRANSMIT(_periph, _c) do {			\
		while (! (_periph->ifg & UCTXIFG)) {	\
			;									\
		}										\
		_periph->txbuf = _c;					\
	} while (0)

int
iBSP430usci5PutChar (int c, xBSP430usci5Handle device)
{
	const portTickType MAX_DELAY = portMAX_DELAY; // 2000;
	portTickType delay = 0;
	int passp;

	if (device->tx_queue) {
		do {
			passp = xQueueSendToBack(device->tx_queue, &c, delay);
			vBSP430usci5WakeupTransmit(device);
			if (! passp) {
				delay = MAX_DELAY;
			}
		} while (! passp);
	} else {
		RAW_TRANSMIT(device->usci5, c);
	}
	return c;
}

int
iBSP430usci5PutString (const char* str, xBSP430usci5Handle device)
{
	const portTickType MAX_DELAY = portMAX_DELAY; // 2000;
	portTickType delay = 0;
	const char * in_string = str;
	
	if (device->tx_queue) {
		while (*str) {
			if (xQueueSendToBack(device->tx_queue, str, delay)) {
				++str;
				if (delay) {
					vBSP430usci5WakeupTransmit(device);
					delay = 0;
				}
			} else {
				vBSP430usci5WakeupTransmit(device);
				delay = MAX_DELAY;
			}
		}
		vBSP430usci5WakeupTransmit(device);
	} else {
		while (*str) {
			RAW_TRANSMIT(device->usci5, *str);
			++str;
		}
	}
	return str - in_string;
}

/* Since the interrupt code is the same for all peripherals, on MCUs
 * with multiple USCI5 devices it is more space efficient to share it.
 * This does add an extra call/return for some minor cost in stack
 * space.
 *
 * Making the implementation function __c16__ ensures it's legitimate
 * to use portYIELD_FROM_ISR().
 *
 * Adding __always_inline__ supports maintainability by having a
 * single implementation but speed by forcing the implementation into
 * each handler.  It's a lot cleaner than defining the body as a
 * macro.  GCC will normally inline the code if there's only one call
 * point; there should be a configPORT_foo option to do so in other
 * cases. */
#if ((configBSP430_HAL_USCI5_A0_ISR - 0) \
	 || (configBSP430_HAL_USCI5_A1_ISR - 0) \
	 || (configBSP430_HAL_USCI5_A2_ISR - 0) \
	 || (configBSP430_HAL_USCI5_A3_ISR - 0) \
	 || (configBSP430_HAL_USCI5_B0_ISR - 0) \
	 || (configBSP430_HAL_USCI5_B1_ISR - 0) \
	 || (configBSP430_HAL_USCI5_B2_ISR - 0) \
	 || (configBSP430_HAL_USCI5_B3_ISR - 0) \
	 )
static void
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usci5_isr (xBSP430usci5Handle device)
{
	portBASE_TYPE yield = pdFALSE;
	int rv = 1;

	switch (device->usci5->iv) {
	default:
	case USCI_NONE:
		break;
	case USCI_UCTXIFG:
		if (device->tx_queue) {
			rv = xQueueReceiveFromISR(device->tx_queue, &device->tx_byte, &yield);
			if (xQueueIsQueueEmptyFromISR(device->tx_queue)) {
				device->usci5->ie &= ~UCTXIE;
			}
		}
		if (rv) {
			++device->num_tx;
			device->usci5->txbuf = device->tx_byte;
		}
		break;
	case USCI_UCRXIFG:
		device->rx_byte = device->usci5->rxbuf;
		++device->num_rx;
		if (device->rx_queue) {
			rv = xQueueSendToBackFromISR(device->rx_queue, &device->rx_byte, &yield);
		}
		break;
	}
	portYIELD_FROM_ISR(yield);
}
#endif  /* HAL ISR */

/* !BSP430! insert=hal_ba_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_ba_defn] */
#if configBSP430_HAL_USCI5_A0 - 0
static struct xBSP430usci5State state_USCI5_A0_ = {
	.usci5 = (xBSP430periphUSCI5 *)_BSP430_PERIPH_USCI5_A0_BASEADDRESS
};

xBSP430usci5Handle const xBSP430usci5_USCI5_A0 = &state_USCI5_A0_;
#endif /* configBSP430_HAL_USCI5_A0 */

#if configBSP430_HAL_USCI5_A1 - 0
static struct xBSP430usci5State state_USCI5_A1_ = {
	.usci5 = (xBSP430periphUSCI5 *)_BSP430_PERIPH_USCI5_A1_BASEADDRESS
};

xBSP430usci5Handle const xBSP430usci5_USCI5_A1 = &state_USCI5_A1_;
#endif /* configBSP430_HAL_USCI5_A1 */

#if configBSP430_HAL_USCI5_A2 - 0
static struct xBSP430usci5State state_USCI5_A2_ = {
	.usci5 = (xBSP430periphUSCI5 *)_BSP430_PERIPH_USCI5_A2_BASEADDRESS
};

xBSP430usci5Handle const xBSP430usci5_USCI5_A2 = &state_USCI5_A2_;
#endif /* configBSP430_HAL_USCI5_A2 */

#if configBSP430_HAL_USCI5_A3 - 0
static struct xBSP430usci5State state_USCI5_A3_ = {
	.usci5 = (xBSP430periphUSCI5 *)_BSP430_PERIPH_USCI5_A3_BASEADDRESS
};

xBSP430usci5Handle const xBSP430usci5_USCI5_A3 = &state_USCI5_A3_;
#endif /* configBSP430_HAL_USCI5_A3 */

#if configBSP430_HAL_USCI5_B0 - 0
static struct xBSP430usci5State state_USCI5_B0_ = {
	.usci5 = (xBSP430periphUSCI5 *)_BSP430_PERIPH_USCI5_B0_BASEADDRESS
};

xBSP430usci5Handle const xBSP430usci5_USCI5_B0 = &state_USCI5_B0_;
#endif /* configBSP430_HAL_USCI5_B0 */

#if configBSP430_HAL_USCI5_B1 - 0
static struct xBSP430usci5State state_USCI5_B1_ = {
	.usci5 = (xBSP430periphUSCI5 *)_BSP430_PERIPH_USCI5_B1_BASEADDRESS
};

xBSP430usci5Handle const xBSP430usci5_USCI5_B1 = &state_USCI5_B1_;
#endif /* configBSP430_HAL_USCI5_B1 */

#if configBSP430_HAL_USCI5_B2 - 0
static struct xBSP430usci5State state_USCI5_B2_ = {
	.usci5 = (xBSP430periphUSCI5 *)_BSP430_PERIPH_USCI5_B2_BASEADDRESS
};

xBSP430usci5Handle const xBSP430usci5_USCI5_B2 = &state_USCI5_B2_;
#endif /* configBSP430_HAL_USCI5_B2 */

#if configBSP430_HAL_USCI5_B3 - 0
static struct xBSP430usci5State state_USCI5_B3_ = {
	.usci5 = (xBSP430periphUSCI5 *)_BSP430_PERIPH_USCI5_B3_BASEADDRESS
};

xBSP430usci5Handle const xBSP430usci5_USCI5_B3 = &state_USCI5_B3_;
#endif /* configBSP430_HAL_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [hal_ba_defn] */
/* !BSP430! end=hal_ba_defn */

/* !BSP430! uscifrom=usci5 insert=hal_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_defn] */
#if configBSP430_HAL_USCI5_A0_ISR - 0
static void
__attribute__((__interrupt__(USCI_A0_VECTOR)))
isr_USCI5_A0 (void)
{
	usci5_isr(xBSP430usci5_USCI5_A0);
}
#endif /* configBSP430_HAL_USCI5_A0_ISR */

#if configBSP430_HAL_USCI5_A1_ISR - 0
static void
__attribute__((__interrupt__(USCI_A1_VECTOR)))
isr_USCI5_A1 (void)
{
	usci5_isr(xBSP430usci5_USCI5_A1);
}
#endif /* configBSP430_HAL_USCI5_A1_ISR */

#if configBSP430_HAL_USCI5_A2_ISR - 0
static void
__attribute__((__interrupt__(USCI_A2_VECTOR)))
isr_USCI5_A2 (void)
{
	usci5_isr(xBSP430usci5_USCI5_A2);
}
#endif /* configBSP430_HAL_USCI5_A2_ISR */

#if configBSP430_HAL_USCI5_A3_ISR - 0
static void
__attribute__((__interrupt__(USCI_A3_VECTOR)))
isr_USCI5_A3 (void)
{
	usci5_isr(xBSP430usci5_USCI5_A3);
}
#endif /* configBSP430_HAL_USCI5_A3_ISR */

#if configBSP430_HAL_USCI5_B0_ISR - 0
static void
__attribute__((__interrupt__(USCI_B0_VECTOR)))
isr_USCI5_B0 (void)
{
	usci5_isr(xBSP430usci5_USCI5_B0);
}
#endif /* configBSP430_HAL_USCI5_B0_ISR */

#if configBSP430_HAL_USCI5_B1_ISR - 0
static void
__attribute__((__interrupt__(USCI_B1_VECTOR)))
isr_USCI5_B1 (void)
{
	usci5_isr(xBSP430usci5_USCI5_B1);
}
#endif /* configBSP430_HAL_USCI5_B1_ISR */

#if configBSP430_HAL_USCI5_B2_ISR - 0
static void
__attribute__((__interrupt__(USCI_B2_VECTOR)))
isr_USCI5_B2 (void)
{
	usci5_isr(xBSP430usci5_USCI5_B2);
}
#endif /* configBSP430_HAL_USCI5_B2_ISR */

#if configBSP430_HAL_USCI5_B3_ISR - 0
static void
__attribute__((__interrupt__(USCI_B3_VECTOR)))
isr_USCI5_B3 (void)
{
	usci5_isr(xBSP430usci5_USCI5_B3);
}
#endif /* configBSP430_HAL_USCI5_B3_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_defn] */
/* !BSP430! end=hal_isr_defn */

static xBSP430usci5Handle periphToDevice (xBSP430periphHandle periph)
{
/* !BSP430! insert=periph_hal_demux */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if configBSP430_HAL_USCI5_A0 - 0
	if (BSP430_PERIPH_USCI5_A0 == periph) {
		return xBSP430usci5_USCI5_A0;
	}
#endif /* configBSP430_PERIPH_USCI5_A0 */

#if configBSP430_HAL_USCI5_A1 - 0
	if (BSP430_PERIPH_USCI5_A1 == periph) {
		return xBSP430usci5_USCI5_A1;
	}
#endif /* configBSP430_PERIPH_USCI5_A1 */

#if configBSP430_HAL_USCI5_A2 - 0
	if (BSP430_PERIPH_USCI5_A2 == periph) {
		return xBSP430usci5_USCI5_A2;
	}
#endif /* configBSP430_PERIPH_USCI5_A2 */

#if configBSP430_HAL_USCI5_A3 - 0
	if (BSP430_PERIPH_USCI5_A3 == periph) {
		return xBSP430usci5_USCI5_A3;
	}
#endif /* configBSP430_PERIPH_USCI5_A3 */

#if configBSP430_HAL_USCI5_B0 - 0
	if (BSP430_PERIPH_USCI5_B0 == periph) {
		return xBSP430usci5_USCI5_B0;
	}
#endif /* configBSP430_PERIPH_USCI5_B0 */

#if configBSP430_HAL_USCI5_B1 - 0
	if (BSP430_PERIPH_USCI5_B1 == periph) {
		return xBSP430usci5_USCI5_B1;
	}
#endif /* configBSP430_PERIPH_USCI5_B1 */

#if configBSP430_HAL_USCI5_B2 - 0
	if (BSP430_PERIPH_USCI5_B2 == periph) {
		return xBSP430usci5_USCI5_B2;
	}
#endif /* configBSP430_PERIPH_USCI5_B2 */

#if configBSP430_HAL_USCI5_B3 - 0
	if (BSP430_PERIPH_USCI5_B3 == periph) {
		return xBSP430usci5_USCI5_B3;
	}
#endif /* configBSP430_PERIPH_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
/* !BSP430! end=periph_hal_demux */
	return NULL;
}
