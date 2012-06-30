#include <bsp430/5xx/usci.h>
#include <bsp430/clocks/ucs.h>
#include <bsp430/common/platform.h>
#include "task.h"

enum {
#if defined(__MSP430_HAS_USCI_A0__)
	DEVIDX_USCI_A0,
#endif /* __MSP430_HAS_USCI_A0__ */
#if defined(__MSP430_HAS_USCI_A1__)
	DEVIDX_USCI_A1,
#endif /* __MSP430_HAS_USCI_A1__ */
#if defined(__MSP430_HAS_USCI_A2__)
	DEVIDX_USCI_A2,
#endif /* __MSP430_HAS_USCI_A2__ */
#if defined(__MSP430_HAS_USCI_A3__)
	DEVIDX_USCI_A3,
#endif /* __MSP430_HAS_USCI_A3__ */
#if defined(__MSP430_HAS_USCI_B0__)
	DEVIDX_USCI_B0,
#endif /* __MSP430_HAS_USCI_B0__ */
#if defined(__MSP430_HAS_USCI_B1__)
	DEVIDX_USCI_B1,
#endif /* __MSP430_HAS_USCI_B1__ */
#if defined(__MSP430_HAS_USCI_B2__)
	DEVIDX_USCI_B2,
#endif /* __MSP430_HAS_USCI_B2__ */
#if defined(__MSP430_HAS_USCI_B3__)
	DEVIDX_USCI_B3,
#endif /* __MSP430_HAS_USCI_B3__ */
};

#define DEVID_TO_USCI(_devid) ((volatile bsp430_USCI *)(_devid))
#define DEVICE_TO_DEVID(_device) ((int)((_device)->usci))

bsp430_FreeRTOS_USCI usci_devices[] = {
#if defined(__MSP430_HAS_USCI_A0__)
	{ .usci = DEVID_TO_USCI(__MSP430_BASEADDRESS_USCI_A0__) },
#endif /* __MSP430_HAS_USCI_A0__ */
#if defined(__MSP430_HAS_USCI_A1__)
	{ .usci = DEVID_TO_USCI(__MSP430_BASEADDRESS_USCI_A1__) },
#endif /* __MSP430_HAS_USCI_A1__ */
#if defined(__MSP430_HAS_USCI_A2__)
	{ .usci = DEVID_TO_USCI(__MSP430_BASEADDRESS_USCI_A2__) },
#endif /* __MSP430_HAS_USCI_A2__ */
#if defined(__MSP430_HAS_USCI_A3__)
	{ .usci = DEVID_TO_USCI(__MSP430_BASEADDRESS_USCI_A3__) },
#endif /* __MSP430_HAS_USCI_A3__ */
#if defined(__MSP430_HAS_USCI_B0__)
	{ .usci = DEVID_TO_USCI(__MSP430_BASEADDRESS_USCI_B0__) },
#endif /* __MSP430_HAS_USCI_B0__ */
#if defined(__MSP430_HAS_USCI_B1__)
	{ .usci = DEVID_TO_USCI(__MSP430_BASEADDRESS_USCI_B1__) },
#endif /* __MSP430_HAS_USCI_B1__ */
#if defined(__MSP430_HAS_USCI_B2__)
	{ .usci = DEVID_TO_USCI(__MSP430_BASEADDRESS_USCI_B2__) },
#endif /* __MSP430_HAS_USCI_B2__ */
#if defined(__MSP430_HAS_USCI_B3__)
	{ .usci = DEVID_TO_USCI(__MSP430_BASEADDRESS_USCI_B3__) },
#endif /* __MSP430_HAS_USCI_B3__ */
};
const bsp430_FreeRTOS_USCI* const end_usci_devices = usci_devices + sizeof(usci_devices)/sizeof(*usci_devices);

#define COM_PORT_ACTIVE  0x01

bsp430_FreeRTOS_USCI*
bsp430_usci_lookup (int devid)
{
	bsp430_FreeRTOS_USCI* device = usci_devices;
	
	while (device < end_usci_devices) {
		if (device->usci == DEVID_TO_USCI(devid)) {
			return device;
		}
		++device;
	}
	return NULL;
}

bsp430_FreeRTOS_USCI*
bsp430_usci_uart_open (int devid,
					   unsigned int control_word,
					   unsigned long baud,
					   xQueueHandle rx_queue,
					   xQueueHandle tx_queue)
{
	bsp430_FreeRTOS_USCI* device;
	unsigned long brclk_hz;
	uint16_t br;
	uint16_t brs;

	/* Reject invalid baud rates */
	if ((0 == baud) || (1000000UL < baud)) {
		return NULL;
	}

	/* Reject non-existent devices */
	device = bsp430_usci_lookup(devid);
	if (! device) {
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

	vBSP430platformConfigurePeripheralPins (DEVICE_TO_DEVID(device), 1);

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
bsp430_usci_close (bsp430_FreeRTOS_USCI* device)
{
	device->usci->ctlw0 = UCSWRST;
	vBSP430platformConfigurePeripheralPins (DEVICE_TO_DEVID(device), 0);
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
bsp430_usci_wakeup_transmit (bsp430_FreeRTOS_USCI* device)
{
	taskENTER_CRITICAL();
	USCI_WAKEUP_TRANSMIT_FROM_ISR(device);
	taskEXIT_CRITICAL();
}

int
bsp430_uart_putc (int c, bsp430_FreeRTOS_USCI* hsuart)
{
	const portTickType MAX_DELAY = portMAX_DELAY; // 2000;
	portTickType delay = 0;
	int passp;

	do {
		passp = xQueueSendToBack(hsuart->tx_queue, &c, delay);
		bsp430_usci_wakeup_transmit(hsuart);
		if (! passp) {
			delay = MAX_DELAY;
		}
	} while (! passp);
	return c;
}

int
bsp430_uart_puts (const char* str, bsp430_FreeRTOS_USCI* hsuart)
{
	const portTickType MAX_DELAY = portMAX_DELAY; // 2000;
	portTickType delay = 0;
	const char * in_string = str;
	
	while (*str) {
		if (xQueueSendToBack(hsuart->tx_queue, str, delay)) {
			++str;
			if (delay) {
				bsp430_usci_wakeup_transmit(hsuart);
				delay = 0;
			}
		} else {
			bsp430_usci_wakeup_transmit(hsuart);
			delay = MAX_DELAY;
		}
	}
	bsp430_usci_wakeup_transmit(hsuart);
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
usci_irq (bsp430_FreeRTOS_USCI *port)
{
	portBASE_TYPE yield = pdFALSE;
	portBASE_TYPE rv = pdFALSE;
	uint8_t c;

	switch (port->usci->iv) {
	default:
	case USCI_NONE:
		break;
	case USCI_UCTXIFG:
		rv = xQueueReceiveFromISR(port->tx_queue, &c, &yield);
		if (xQueueIsQueueEmptyFromISR(port->tx_queue)) {
			signed portBASE_TYPE sema_yield = pdFALSE;
			port->usci->ie &= ~UCTXIE;
			yield |= sema_yield;
		}
		if (rv) {
			++port->num_tx;
			port->usci->txbuf = c;
		}
		break;
	case USCI_UCRXIFG:
		c = port->usci->rxbuf;
		++port->num_rx;
		rv = xQueueSendToBackFromISR(port->rx_queue, &c, &yield);
		break;
	}
	portYIELD_FROM_ISR(yield);
}

/* No current MCU has more than 4 USCI_B instances */

#if defined(__MSP430_HAS_USCI_A0__)
static void
__attribute__((__interrupt__(USCI_A0_VECTOR)))
usci_a0_irq (void)
{
	usci_irq(usci_devices + DEVIDX_USCI_A0);
}
#endif /* __MSP430_HAS_USCI_A0__ */
#if defined(__MSP430_HAS_USCI_A1__)
static void
__attribute__((__interrupt__(USCI_A1_VECTOR)))
usci_a1_irq (void)
{
	usci_irq(usci_devices + DEVIDX_USCI_A1);
}
#endif /* __MSP430_HAS_USCI_A1__ */
#if defined(__MSP430_HAS_USCI_A2__)
static void
__attribute__((__interrupt__(USCI_A2_VECTOR)))
usci_a2_irq (void)
{
	usci_irq(usci_devices + DEVIDX_USCI_A2);
}
#endif /* __MSP430_HAS_USCI_A2__ */
#if defined(__MSP430_HAS_USCI_A3__)
static void
__attribute__((__interrupt__(USCI_A3_VECTOR)))
usci_a3_irq (void)
{
	usci_irq(usci_devices + DEVIDX_USCI_A3);
}
#endif /* __MSP430_HAS_USCI_A3__ */
#if defined(__MSP430_HAS_USCI_B0__)
static void
__attribute__((__interrupt__(USCI_B0_VECTOR)))
usci_b0_irq (void)
{
	usci_irq(usci_devices + DEVIDX_USCI_B0);
}
#endif /* __MSP430_HAS_USCI_B0__ */
#if defined(__MSP430_HAS_USCI_B1__)
static void
__attribute__((__interrupt__(USCI_B1_VECTOR)))
usci_b1_irq (void)
{
	usci_irq(usci_devices + DEVIDX_USCI_B1);
}
#endif /* __MSP430_HAS_USCI_B1__ */
#if defined(__MSP430_HAS_USCI_B2__)
static void
__attribute__((__interrupt__(USCI_B2_VECTOR)))
usci_b2_irq (void)
{
	usci_irq(usci_devices + DEVIDX_USCI_B2);
}
#endif /* __MSP430_HAS_USCI_B2__ */
#if defined(__MSP430_HAS_USCI_B3__)
static void
__attribute__((__interrupt__(USCI_B3_VECTOR)))
usci_b3_irq (void)
{
	usci_irq(usci_devices + DEVIDX_USCI_B3);
}
#endif /* __MSP430_HAS_USCI_B3__ */
