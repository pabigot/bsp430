#include <bsp430/5xx/usci.h>
#include <bsp430/clocks/ucs.h>
#include <bsp430/common/platform.h>
#include "task.h"

#define COM_PORT_ACTIVE  0x01

static xBSP430USCIHandle periphToDevice (xBSP430Periph periph);

xBSP430USCIHandle
xBSP430USCIOpenUART (xBSP430Periph periph,
					 unsigned int control_word,
					 unsigned long baud,
					 xQueueHandle rx_queue,
					 xQueueHandle tx_queue)
{
	unsigned long brclk_hz;
	xBSP430USCIHandle device = periphToDevice(periph);
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
		brclk_hz = ulBSP430ucsSMCLK_Hz();
	}
	br = (brclk_hz / baud);
	brs = (1 + 16 * (brclk_hz - baud * br) / baud) / 2;

	device->usci->brw = br;
	device->usci->mctl = (0 * UCBRF_1) | (brs * UCBRS_1);

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
iBSP430USCIClose (xBSP430USCIHandle device)
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
vBSP430USCIWakeupTransmit (xBSP430USCIHandle device)
{
	taskENTER_CRITICAL();
	USCI_WAKEUP_TRANSMIT_FROM_ISR(device);
	taskEXIT_CRITICAL();
}

int
iBSP430USCIputc (int c, xBSP430USCIHandle hsuart)
{
	const portTickType MAX_DELAY = portMAX_DELAY; // 2000;
	portTickType delay = 0;
	int passp;

	do {
		passp = xQueueSendToBack(hsuart->tx_queue, &c, delay);
		vBSP430USCIWakeupTransmit(hsuart);
		if (! passp) {
			delay = MAX_DELAY;
		}
	} while (! passp);
	return c;
}

int
iBSP430USCIputs (const char* str, xBSP430USCIHandle hsuart)
{
	const portTickType MAX_DELAY = portMAX_DELAY; // 2000;
	portTickType delay = 0;
	const char * in_string = str;
	
	while (*str) {
		if (xQueueSendToBack(hsuart->tx_queue, str, delay)) {
			++str;
			if (delay) {
				vBSP430USCIWakeupTransmit(hsuart);
				delay = 0;
			}
		} else {
			vBSP430USCIWakeupTransmit(hsuart);
			delay = MAX_DELAY;
		}
	}
	vBSP430USCIWakeupTransmit(hsuart);
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
usci_irq (xBSP430USCIHandle device)
{
	portBASE_TYPE yield = pdFALSE;
	portBASE_TYPE rv = pdFALSE;
	uint8_t c;

	switch (device->usci->iv) {
	default:
	case USCI_NONE:
		break;
	case USCI_UCTXIFG:
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
	case USCI_UCRXIFG:
		c = device->usci->rxbuf;
		++device->num_rx;
		rv = xQueueSendToBackFromISR(device->rx_queue, &c, &yield);
		break;
	}
	portYIELD_FROM_ISR(yield);
}

#if configBSP430_PERIPH_USE_USCI_A0 - 0
static struct xBSP430USCI xBSP430USCI_A0_ = {
	.usci = (xBSP430Periph_USCI *)__MSP430_BASEADDRESS_USCI_A0__
};

xBSP430USCIHandle const xBSP430USCI_A0 = &xBSP430USCI_A0_;

static void
__attribute__((__interrupt__(USCI_A0_VECTOR)))
irq_USCI_A0 (void)
{
	usci_irq(xBSP430USCI_A0);
}

#endif /* configBSP430_PERIPH_USE_USCI_A0 */
#if configBSP430_PERIPH_USE_USCI_A1 - 0
static struct xBSP430USCI xBSP430USCI_A1_ = {
	.usci = (xBSP430Periph_USCI *)__MSP430_BASEADDRESS_USCI_A1__
};

xBSP430USCIHandle const xBSP430USCI_A1 = &xBSP430USCI_A1_;

static void
__attribute__((__interrupt__(USCI_A1_VECTOR)))
irq_USCI_A1 (void)
{
	usci_irq(xBSP430USCI_A1);
}

#endif /* configBSP430_PERIPH_USE_USCI_A1 */
#if configBSP430_PERIPH_USE_USCI_A2 - 0
static struct xBSP430USCI xBSP430USCI_A2_ = {
	.usci = (xBSP430Periph_USCI *)__MSP430_BASEADDRESS_USCI_A2__
};

xBSP430USCIHandle const xBSP430USCI_A2 = &xBSP430USCI_A2_;

static void
__attribute__((__interrupt__(USCI_A2_VECTOR)))
irq_USCI_A2 (void)
{
	usci_irq(xBSP430USCI_A2);
}

#endif /* configBSP430_PERIPH_USE_USCI_A2 */
#if configBSP430_PERIPH_USE_USCI_A3 - 0
static struct xBSP430USCI xBSP430USCI_A3_ = {
	.usci = (xBSP430Periph_USCI *)__MSP430_BASEADDRESS_USCI_A3__
};

xBSP430USCIHandle const xBSP430USCI_A3 = &xBSP430USCI_A3_;

static void
__attribute__((__interrupt__(USCI_A3_VECTOR)))
irq_USCI_A3 (void)
{
	usci_irq(xBSP430USCI_A3);
}

#endif /* configBSP430_PERIPH_USE_USCI_A3 */
#if configBSP430_PERIPH_USE_USCI_B0 - 0
static struct xBSP430USCI xBSP430USCI_B0_ = {
	.usci = (xBSP430Periph_USCI *)__MSP430_BASEADDRESS_USCI_B0__
};

xBSP430USCIHandle const xBSP430USCI_B0 = &xBSP430USCI_B0_;

static void
__attribute__((__interrupt__(USCI_B0_VECTOR)))
irq_USCI_B0 (void)
{
	usci_irq(xBSP430USCI_B0);
}

#endif /* configBSP430_PERIPH_USE_USCI_B0 */
#if configBSP430_PERIPH_USE_USCI_B1 - 0
static struct xBSP430USCI xBSP430USCI_B1_ = {
	.usci = (xBSP430Periph_USCI *)__MSP430_BASEADDRESS_USCI_B1__
};

xBSP430USCIHandle const xBSP430USCI_B1 = &xBSP430USCI_B1_;

static void
__attribute__((__interrupt__(USCI_B1_VECTOR)))
irq_USCI_B1 (void)
{
	usci_irq(xBSP430USCI_B1);
}

#endif /* configBSP430_PERIPH_USE_USCI_B1 */
#if configBSP430_PERIPH_USE_USCI_B2 - 0
static struct xBSP430USCI xBSP430USCI_B2_ = {
	.usci = (xBSP430Periph_USCI *)__MSP430_BASEADDRESS_USCI_B2__
};

xBSP430USCIHandle const xBSP430USCI_B2 = &xBSP430USCI_B2_;

static void
__attribute__((__interrupt__(USCI_B2_VECTOR)))
irq_USCI_B2 (void)
{
	usci_irq(xBSP430USCI_B2);
}

#endif /* configBSP430_PERIPH_USE_USCI_B2 */
#if configBSP430_PERIPH_USE_USCI_B3 - 0
static struct xBSP430USCI xBSP430USCI_B3_ = {
	.usci = (xBSP430Periph_USCI *)__MSP430_BASEADDRESS_USCI_B3__
};

xBSP430USCIHandle const xBSP430USCI_B3 = &xBSP430USCI_B3_;

static void
__attribute__((__interrupt__(USCI_B3_VECTOR)))
irq_USCI_B3 (void)
{
	usci_irq(xBSP430USCI_B3);
}

#endif /* configBSP430_PERIPH_USE_USCI_B3 */

static xBSP430USCIHandle periphToDevice (xBSP430Periph periph)
{
#if configBSP430_PERIPH_USE_USCI_A0 - 0
	if (xBSP430Periph_USCI_A0 == periph) {
		return xBSP430USCI_A0;
	}
#endif /* configBSP430_PERIPH_USE_USCI_A0 */
#if configBSP430_PERIPH_USE_USCI_A1 - 0
	if (xBSP430Periph_USCI_A1 == periph) {
		return xBSP430USCI_A1;
	}
#endif /* configBSP430_PERIPH_USE_USCI_A1 */
#if configBSP430_PERIPH_USE_USCI_A2 - 0
	if (xBSP430Periph_USCI_A2 == periph) {
		return xBSP430USCI_A2;
	}
#endif /* configBSP430_PERIPH_USE_USCI_A2 */
#if configBSP430_PERIPH_USE_USCI_A3 - 0
	if (xBSP430Periph_USCI_A3 == periph) {
		return xBSP430USCI_A3;
	}
#endif /* configBSP430_PERIPH_USE_USCI_A3 */
#if configBSP430_PERIPH_USE_USCI_B0 - 0
	if (xBSP430Periph_USCI_B0 == periph) {
		return xBSP430USCI_B0;
	}
#endif /* configBSP430_PERIPH_USE_USCI_B0 */
#if configBSP430_PERIPH_USE_USCI_B1 - 0
	if (xBSP430Periph_USCI_B1 == periph) {
		return xBSP430USCI_B1;
	}
#endif /* configBSP430_PERIPH_USE_USCI_B1 */
#if configBSP430_PERIPH_USE_USCI_B2 - 0
	if (xBSP430Periph_USCI_B2 == periph) {
		return xBSP430USCI_B2;
	}
#endif /* configBSP430_PERIPH_USE_USCI_B2 */
#if configBSP430_PERIPH_USE_USCI_B3 - 0
	if (xBSP430Periph_USCI_B3 == periph) {
		return xBSP430USCI_B3;
	}
#endif /* configBSP430_PERIPH_USE_USCI_B3 */
	return NULL;
}
