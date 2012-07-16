#include <bsp430/platform.h>
#include <bsp430/periph/ucs.h>
#include <bsp430/timers/timerA0.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/usci.h>

const xBSP430led pxBSP430leds[] = {
	{ .pucPxOUT = &P1OUT, .ucBIT = BIT0 }, /* Red */
	{ .pucPxOUT = &P1OUT, .ucBIT = BIT1 }, /* Orange */
};
const unsigned char ucBSP430leds = sizeof(pxBSP430leds) / sizeof(*pxBSP430leds);

int
iBSP430platformConfigurePeripheralPins (xBSP430periphHandle device, int enablep)
{
	uint8_t bits = 0;
	volatile uint8_t* pxsel = 0;
	if (BSP430_PERIPH_XT1 == device) {
		bits = BIT0 | BIT1;
		pxsel = &P7SEL;
	}
#if configBSP430_PERIPH_USCI_A0 - 0
	else if (xBSP430Periph_USCI_A0 == device) {
		bits = BIT4 | BIT5;
		pxsel = &P3SEL;
	}
#endif
#if configBSP430_PERIPH_USCI_A1 - 0
	else if (xBSP430Periph_USCI_A1 == device) {
		bits = BIT6 | BIT7;
		pxsel = &P5SEL;
	}
#endif
#if configBSP430_PERIPH_USCI_A2 - 0
	else if (xBSP430Periph_USCI_A2 == device) {
		bits = BIT4 | BIT5;
		pxsel = &P9SEL;
	}
#endif
#if configBSP430_PERIPH_USCI_A3 - 0
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
	int rc;
	
	/* Hold off watchdog */
	WDTCTL = WDTPW + WDTHOLD;

	/* Enable XT1 functions and clock */
	rc = iBSP430clockConfigureXT1(1, 2000000L / configBSP430_CLOCK_XT1_STABILIZATION_DELAY_CYCLES);
	iBSP430ucsConfigureACLK(rc ? SELA__XT1CLK : SELA__VLOCLK);
	ulBSP430ucsConfigure( configCPU_CLOCK_HZ, -1 );

	/* Enable basic timer */
	vBSP430timerA0Configure();
}
