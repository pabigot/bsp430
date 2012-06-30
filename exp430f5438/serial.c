#include "platform.h"
#include "portSerial.h"
#include "queue.h"
#include "semphr.h"
#include <bsp430/clocks/ucs.h>
#include <bsp430/5xx/usci.h>

typedef bsp430_USCI xUSCI_A_UART;

#define COM_PORT_ACTIVE  0x01

typedef bsp430_FreeRTOS_USCI xComPort;

static xComPort*
configurePort_ (xComPort* port,
				unsigned long baud,
				size_t bufsiz)
{
	xComPort* configured_port = NULL;
	xQueueHandle rx_queue = NULL;
	xQueueHandle tx_queue = NULL;

	/* Reject if requested queue could not be allocated */
	if (0 < bufsiz) {
		rx_queue = xQueueCreate(bufsiz, sizeof(uint8_t));
		if (NULL == rx_queue) {
			goto failed;
		}
		tx_queue = xQueueCreate(bufsiz, sizeof(uint8_t));
		if (NULL == tx_queue) {
			goto failed;
		}
	}

	configured_port = bsp430_usci_uart_open((int)(port->usci), 0, baud,
											rx_queue, tx_queue);

	if (NULL != configured_port) {
		return configured_port;
	}

 failed:
	if (NULL != tx_queue) {
		vQueueDelete(tx_queue);
	}
	if (NULL != rx_queue) {
		vQueueDelete(rx_queue);
	}
	return NULL;
}

static void
unconfigurePort_ (xComPort* port)
{
	xQueueHandle rx_queue = port->rx_queue;
	xQueueHandle tx_queue = port->tx_queue;
	(void)bsp430_usci_close(port);
	if (tx_queue) {
		vQueueDelete(tx_queue);
	}
	if (rx_queue) {
		vQueueDelete(rx_queue);
	}
}

static xComPort*
portToDevice (eCOMPort ePort)
{
	int devid;

	switch (ePort) {
	case serCOM1: devid = BSP430_USCI_A0; break;
	case serCOM2: devid = BSP430_USCI_A1; break;
	case serCOM3: devid = BSP430_USCI_A2; break;
	case serCOM4: devid = BSP430_USCI_A3; break;
	default:
		devid = 0;
		break;
	}
	return bsp430_usci_lookup(devid);
}

static unsigned long
prvBaudEnumToValue (eBaud baud_enum)
{
	switch (baud_enum) {
	case ser50: return 50;
	case ser75: return 75;
	case ser110: return 110;
	case ser134: return 134;
	case ser150: return 150;
	case ser200: return 200;
	case ser300: return 300;
	case ser600: return 600;
	case ser1200: return 1200;
	case ser1800: return 1800;
	case ser2400: return 2400;
	case ser4800: return 4800;
	case ser9600: return 9600;
	case ser19200: return 19200;
	case ser38400: return 38400;
	case ser57600: return 57600;
	case ser115200: return 115200;
	default: return 0;
	}
	return 0;
}

xComPortHandle
xSerialPortInit (eCOMPort ePort, eBaud eWantedBaud, eParity eWantedParity, eDataBits eWantedDataBits, eStopBits eWantedStopBits, unsigned portBASE_TYPE uxBufferLength)
{
	xComPort* port = portToDevice(ePort);
	if (! port) {
		return NULL;
	}
	if (port->flags & COM_PORT_ACTIVE) {
		unconfigurePort_(port);
	}
	port = configurePort_(port, prvBaudEnumToValue(eWantedBaud), uxBufferLength);
	return (xComPortHandle)port;
}

void
vSerialPutString (xComPortHandle pxPort, const signed char * const pcString, unsigned short usStringLength)
{
	unsigned short i;
	for (i = 0; i < usStringLength; ++i) {
		xSerialPutChar(pxPort, pcString[i], 0);
	}
}

signed portBASE_TYPE
xSerialGetChar (xComPortHandle pxPort, signed char *pcRxedChar, portTickType xBlockTime)
{
	xComPort* port = (xComPort*)pxPort;

	if (NULL == port) {
		return pdFAIL;
	}
	if (0 == port->rx_queue) {
		return pdFAIL;
	}
	return xQueueReceive (port->rx_queue, pcRxedChar, xBlockTime);
}

signed portBASE_TYPE
xSerialPutChar (xComPortHandle pxPort, signed char cOutChar, portTickType xBlockTime)
{
	xComPort* port = (xComPort*)pxPort;
	portBASE_TYPE rv;

	if (NULL == port) {
		return pdFAIL;
	}
	if (0 == port->tx_queue) {
		(void)bsp430_usci_raw_transmit(port, cOutChar);
		return pdPASS;
	}
	rv = xQueueSendToBack(port->tx_queue, &cOutChar, xBlockTime);
	if (pdTRUE == rv) {
		(void)bsp430_usci_wakeup_transmit(port);
	}
	return pdTRUE == rv;
}

long ulSerialNumRx (xComPortHandle xPort)
{
	xComPort* port = (xComPort*)xPort;
	return port ? port->num_rx : -1L;
}

long ulSerialNumTx (xComPortHandle xPort)
{
	xComPort* port = (xComPort*)xPort;
	return port ? port->num_tx : -1L;
}


portBASE_TYPE
xSerialWaitForSemaphore (xComPortHandle xPort)
{
	return 0;
}

void
vSerialClose (xComPortHandle xPort)
{
	xComPort* port = (xComPort*)xPort;
	if (NULL != xPort) {
		unconfigurePort_(port);
	}
}

