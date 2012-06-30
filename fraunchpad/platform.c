#include "platform.h"
#include <bsp430/clocks/ucs.h>
#include <bsp430/timers/timerA0.h>
#include <bsp430/utility/led.h>
#include <bsp430/5xx/periph.h>
#include "serial.h"
#include "task.h"

/* exp430fr5739 LEDs are PJ.0 to PJ.3 and PB.4 to PB.7.  PJ is not
 * byte addressable, so we need to have a custom implementation of the
 * LED interface. */
void vBSP430ledInit (void)
{
	PJDIR |= 0x0F;
	PJOUT &= ~0x0F;
	PJSEL0 &= ~0x0F;
	PJSEL1 &= ~0x0F;
	PBDIR |= 0xF0;
	PBOUT &= ~0xF0;
	PBSEL0 &= ~0xF0;
	PBSEL1 &= ~0xF0;
}

void vBSP430ledSet (unsigned char ucLED,
					signed portBASE_TYPE xValue)
{
	unsigned int bit;
	volatile unsigned int * pxout;
	if (8 <= ucLED) {
		return;
	}
	bit = 1 << ucLED;
	if (4 <= ucLED) {
		pxout = &PBOUT;
	} else {
		pxout = &PJOUT;
	}
	taskENTER_CRITICAL();
	if (xValue > 0)	{
		*pxout |= bit;
	} else if (xValue < 0) {
		*pxout ^= bit;
	} else {
		*pxout &= ~bit;
	}
	taskEXIT_CRITICAL();
}

int
iBSP430platformConfigurePeripheralPins (xBSP430Periph device, int enablep)
{
	unsigned char bits = 0;
	if (0) {
	}
#if configBSP430_PERIPH_USE_EUSCI_A0 - 0
	else if (xBSP430Periph_EUSCI_A0 == device) {
		bits = BIT0 | BIT1;
		if (enablep) {
			P2SEL0 &= ~bits;
			P2SEL1 |= bits;
		} else {
			P2SEL0 &= ~bits;
			P2SEL1 &= ~bits;
			P2DIR |= bits;
		}
		return 0;
	}
#endif
	(void)bits;
#if configBSP430_PERIPH_USE_EUSCI_A1 - 0
	else if (xBSP430Periph_EUSCI_A1 == device) {
	}
#endif
#if configBSP430_PERIPH_USE_EUSCI_B0 - 0
	else if (xBSP430Periph_EUSCI_B0 == device) {
	}
#endif
	return -1;
}

void vBSP430platformSetup ()
{
	/* Hold off watchdog */
	WDTCTL = WDTPW + WDTHOLD;

	/* Enable XT1 functions and clock */
	PJSEL0 |= BIT4 | BIT5;
	PJSEL1 &= ~(BIT4 | BIT5);
#if 0
	ulBSP430ucsConfigure(configCPU_CLOCK_HZ, -1);
#endif
	/* Enable basic timer */
	vBSP430timerA0Configure();
}
