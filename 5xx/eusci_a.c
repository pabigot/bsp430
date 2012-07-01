#include <bsp430/5xx/eusci_a.h>
#include <bsp430/clocks/ucs.h>
#include <bsp430/common/platform.h>
#include "task.h"

#define COM_PORT_ACTIVE  0x01

/* Work around missing vector definitions for EUSCI devices */
#define EUSCI_A0_VECTOR USCI_A0_VECTOR 
#define EUSCI_A1_VECTOR USCI_A1_VECTOR 
#define EUSCI_A2_VECTOR USCI_A2_VECTOR 
#define EUSCI_B0_VECTOR USCI_B0_VECTOR 

/** Convert from a raw peripheral handle to the corresponding USCI
 * device handle. */
static xBSP430eUSCIAHandle periphToDevice (xBSP430Periph periph);

xBSP430eUSCIAHandle
xBSP430eUSCIAOpenUART (xBSP430Periph periph,
					   unsigned int control_word,
					   unsigned long baud,
					   xQueueHandle rx_queue,
					   xQueueHandle tx_queue)
{
	unsigned long brclk_hz;
	xBSP430eUSCIAHandle device = periphToDevice(periph);
	uint16_t br;
	uint16_t brs;

	configASSERT(NULL != device);

	/* Reject invalid baud rates */
	if ((0 == baud) || (1000000UL < baud)) {
		return NULL;
	}

	/* Reject if the pins can't be configured */
	if (0 != iBSP430platformConfigurePeripheralPins((xBSP430Periph)(device->usci), 1)) {
		return NULL;
	}

	device->rx_queue = rx_queue;
	device->tx_queue = tx_queue;

	/* Prefer ACLK for rates that are low enough.  Use SMCLK for
	 * anything larger. */
	if (portACLK_FREQUENCY_HZ >= (3 * baud)) {
		device->usci->ctlw0 = UCSWRST | UCSSEL__ACLK;
		brclk_hz = portACLK_FREQUENCY_HZ;
	} else {
		device->usci->ctlw0 = UCSWRST | UCSSEL__SMCLK;
		brclk_hz = 0; // ulBSP430ucsSMCLK_Hz();
	}
	br = (brclk_hz / baud);
	brs = (1 + 16 * (brclk_hz - baud * br) / baud) / 2;

	device->usci->brw = br;
	device->usci->mctlw = (0 * UCBRF0) | (brs * UCBRS0);

	/* Mark the device active */
	device->num_rx = device->num_tx = 0;
	device->flags |= COM_PORT_ACTIVE;

	/* Release the USCI and enable the interrupts.  Interrupts are
	 * disabled and cleared when UCSWRST is set. */
	device->usci->ctlw0 &= ~UCSWRST;
	if (0 != device->rx_queue) {
		device->usci->ie |= UCRXIE;
	}

	return device;
}

int
iBSP430eUSCIAClose (xBSP430eUSCIAHandle device)
{
	device->usci->ctlw0 = UCSWRST;
	iBSP430platformConfigurePeripheralPins ((xBSP430Periph)(device->usci), 0);
	device->tx_queue = 0;
	device->rx_queue = 0;
	device->flags = 0;
	return 0;
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
#define USCI_WAKEUP_TRANSMIT_FROM_ISR(device) do {			\
		if ((! xQueueIsQueueEmptyFromISR(device->tx_queue)) \
			&& (! (device->usci->ie & UCTXIE))) {			\
			device->usci->ie |= UCTXIE;						\
		}													\
	} while (0)

void
vBSP430eUSCIAWakeupTransmit (xBSP430eUSCIAHandle device)
{
	taskENTER_CRITICAL();
	USCI_WAKEUP_TRANSMIT_FROM_ISR(device);
	taskEXIT_CRITICAL();
}

#define RAW_TRANSMIT(_periph, _c) do {			\
		while (! (_periph->ifg & UCTXIFG)) {	\
			;									\
		}										\
		_periph->txbuf = _c;					\
	} while (0)

int
iBSP430eUSCIAputc (int c, xBSP430eUSCIAHandle device)
{
	const portTickType MAX_DELAY = portMAX_DELAY; // 2000;
	portTickType delay = 0;
	int passp;

	if (device->tx_queue) {
		do {
			passp = xQueueSendToBack(device->tx_queue, &c, delay);
			vBSP430eUSCIAWakeupTransmit(device);
			if (! passp) {
				delay = MAX_DELAY;
			}
		} while (! passp);
	} else {
		RAW_TRANSMIT(device->usci, c);
	}
	return c;
}

int
iBSP430eUSCIAputs (const char* str, xBSP430eUSCIAHandle device)
{
	const portTickType MAX_DELAY = portMAX_DELAY; // 2000;
	portTickType delay = 0;
	const char * in_string = str;
	
	if (device->tx_queue) {
		while (*str) {
			if (xQueueSendToBack(device->tx_queue, str, delay)) {
				++str;
				if (delay) {
					vBSP430eUSCIAWakeupTransmit(device);
					delay = 0;
				}
			} else {
				vBSP430eUSCIAWakeupTransmit(device);
				delay = MAX_DELAY;
			}
		}
		vBSP430eUSCIAWakeupTransmit(device);
	} else {
		while (*str) {
			RAW_TRANSMIT(device->usci, *str);
			++str;
		}
	}
	return str - in_string;
}

/* Since the interrupt code is the same for all peripherals, on MCUs
 * with multiple USCI devices it is more space efficient to share it.
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

static void
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
usci_irq (xBSP430eUSCIAHandle device)
{
	portBASE_TYPE yield = pdFALSE;
	portBASE_TYPE rv = pdFALSE;
	uint8_t c;

	switch (device->usci->iv) {
	default:
	case USCI_NONE:
		break;
	case USCI_UART_UCTXIFG: /* == USCI_SPI_UCTXIFG */
		rv = xQueueReceiveFromISR(device->tx_queue, &c, &yield);
		if (xQueueIsQueueEmptyFromISR(device->tx_queue)) {
			signed portBASE_TYPE sema_yield = pdFALSE;
			device->usci->ie &= ~UCTXIE;
			yield |= sema_yield;
		}
		if (rv) {
			++device->num_tx;
			device->usci->txbuf = c;
		}
		break;
	case USCI_UART_UCRXIFG: /* == USCI_SPI_UCRXIFG */
		c = device->usci->rxbuf;
		++device->num_rx;
		rv = xQueueSendToBackFromISR(device->rx_queue, &c, &yield);
		break;
	}
	portYIELD_FROM_ISR(yield);
}

/* BEGIN EMBED eusci_a_defn */
/* AUTOMATICALLY GENERATED CODE---DO NOT MODIFY */

#if configBSP430_PERIPH_USE_EUSCI_A0 - 0
static struct xBSP430eUSCIA xBSP430EUSCI_A0_ = {
	.usci = (xBSP430Periph_eUSCI_A *)__MSP430_BASEADDRESS_EUSCI_A0__
};

xBSP430eUSCIAHandle const xBSP430EUSCI_A0 = &xBSP430EUSCI_A0_;

static void
__attribute__((__interrupt__(EUSCI_A0_VECTOR)))
irq_EUSCI_A0 (void)
{
	usci_irq(xBSP430EUSCI_A0);
}
#endif /* configBSP430_PERIPH_USE_EUSCI_A0 */

#if configBSP430_PERIPH_USE_EUSCI_A1 - 0
static struct xBSP430eUSCIA xBSP430EUSCI_A1_ = {
	.usci = (xBSP430Periph_eUSCI_A *)__MSP430_BASEADDRESS_EUSCI_A1__
};

xBSP430eUSCIAHandle const xBSP430EUSCI_A1 = &xBSP430EUSCI_A1_;

static void
__attribute__((__interrupt__(EUSCI_A1_VECTOR)))
irq_EUSCI_A1 (void)
{
	usci_irq(xBSP430EUSCI_A1);
}
#endif /* configBSP430_PERIPH_USE_EUSCI_A1 */

#if configBSP430_PERIPH_USE_EUSCI_A2 - 0
static struct xBSP430eUSCIA xBSP430EUSCI_A2_ = {
	.usci = (xBSP430Periph_eUSCI_A *)__MSP430_BASEADDRESS_EUSCI_A2__
};

xBSP430eUSCIAHandle const xBSP430EUSCI_A2 = &xBSP430EUSCI_A2_;

static void
__attribute__((__interrupt__(EUSCI_A2_VECTOR)))
irq_EUSCI_A2 (void)
{
	usci_irq(xBSP430EUSCI_A2);
}
#endif /* configBSP430_PERIPH_USE_EUSCI_A2 */

/* END EMBED eusci_a_defn: AUTOMATICALLY GENERATED CODE */

static xBSP430eUSCIAHandle periphToDevice (xBSP430Periph periph)
{
/* BEGIN EMBED eusci_a_demux */
/* AUTOMATICALLY GENERATED CODE---DO NOT MODIFY */

#if configBSP430_PERIPH_USE_EUSCI_A0 - 0
	if (xBSP430Periph_EUSCI_A0 == periph) {
		return xBSP430EUSCI_A0;
	}
#endif /* configBSP430_PERIPH_USE_EUSCI_A0 */
#if configBSP430_PERIPH_USE_EUSCI_A1 - 0
	if (xBSP430Periph_EUSCI_A1 == periph) {
		return xBSP430EUSCI_A1;
	}
#endif /* configBSP430_PERIPH_USE_EUSCI_A1 */
#if configBSP430_PERIPH_USE_EUSCI_A2 - 0
	if (xBSP430Periph_EUSCI_A2 == periph) {
		return xBSP430EUSCI_A2;
	}
#endif /* configBSP430_PERIPH_USE_EUSCI_A2 */
/* END EMBED eusci_a_demux: AUTOMATICALLY GENERATED CODE */
	return NULL;
}
