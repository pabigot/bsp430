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
iBSP430platformConfigurePeripheralPins (xBSP430Periph device, int enablep)
{
	uint8_t bits = 0;
	volatile uint8_t* pxsel = 0;
	if (0) {
	}
#if configBSP430_PERIPH_USE_USCI_A0 - 0
	else if (xBSP430Periph_USCI_A0 == device) {
		bits = BIT4 | BIT5;
		pxsel = &P3SEL;
	}
#endif
#if configBSP430_PERIPH_USE_USCI_A1 - 0
	else if (xBSP430Periph_USCI_A1 == device) {
		bits = BIT6 | BIT7;
		pxsel = &P5SEL;
	}
#endif
#if configBSP430_PERIPH_USE_USCI_A2 - 0
	else if (xBSP430Periph_USCI_A2 == device) {
		bits = BIT4 | BIT5;
		pxsel = &P9SEL;
	}
#endif
#if configBSP430_PERIPH_USE_USCI_A3 - 0
	else if (xBSP430Periph_USCI_A3 == device) {
		bits = BIT4 | BIT5;
		pxsel = &P10SEL;
	}
#endif
	else {
		return -1;
	}
	if (enablep) {
		*pxsel |= bits;
	} else {
		*pxsel &= ~bits;
	}
	return 0;
}


void vBSP430platformSetup ()
{
	/* Hold off watchdog */
	WDTCTL = WDTPW + WDTHOLD;

	/* Enable XT1 functions and clock */
	P7SEL |= BIT0 | BIT1;
	ulBSP430ucsConfigure( configCPU_CLOCK_HZ, -1 );

	/* Enable basic timer */
	vBSP430timerA0Configure();
}
