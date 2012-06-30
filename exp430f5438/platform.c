#include "platform.h"
#include <bsp430/clocks/ucs.h>
#include <bsp430/timers/timerA0.h>
#include <bsp430/utility/led.h>
#include <bsp430/5xx/periph.h>
#include "serial.h"

const xLEDDefn pxLEDDefn[] = {
	{ .pucPxOUT = &P1OUT, .ucBIT = BIT0 }, /* Red */
	{ .pucPxOUT = &P1OUT, .ucBIT = BIT1 }, /* Orange */
};
const unsigned char ucLEDDefnCount = sizeof(pxLEDDefn) / sizeof(*pxLEDDefn);

int
vBSP430platformConfigurePeripheralPins (int devid, int enablep)
{
	uint8_t bits = 0;
	volatile uint8_t* pxsel = 0;
	switch (devid) {
	case BSP430_USCI_A0:
		bits = BIT4 | BIT5;
		pxsel = &P3SEL;
		break;
	case BSP430_USCI_A1:
		bits = BIT6 | BIT7;
		pxsel = &P5SEL;
		break;
	case BSP430_USCI_A2:
		bits = BIT4 | BIT5;
		pxsel = &P9SEL;
		break;
	case BSP430_USCI_A3:
		bits = BIT4 | BIT5;
		pxsel = &P10SEL;
		break;
	default:
		break;
	}
	if (pxsel) {
		if (enablep) {
			*pxsel |= bits;
		} else {
			*pxsel &= ~bits;
		}
		return 0;
	}
	return -1;
}


void vBSP430platformSetup ()
{
	/* Hold off watchdog */
	WDTCTL = WDTPW + WDTHOLD;

	/* Enable XT1 functions and clock */
	P7SEL |= BIT0;
	ulBSP430ucsConfigure( configCPU_CLOCK_HZ, -1 );

	/* Enable basic timer */
	vBSP430timerA0Configure();
}
