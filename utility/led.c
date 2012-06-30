/*
  
Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the software nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#include <bsp430/utility/led.h>
#include <task.h>

/** Macro to configure GPIO for a LED within a particular port.
 *
 * I'm sorry if this isn't MISRA compliant, but I'm not replicating
 * this code for each of the 11 potential ports. */
#define prvCONFIG_PORT_X_LED(_x)				\
	do {										\
		if (&P##_x##OUT == pxLED->pucPxOUT)		\
		{										\
			P##_x##DIR |= pxLED->ucBIT;			\
			P##_x##SEL &= ~pxLED->ucBIT;		\
			P##_x##OUT &= ~pxLED->ucBIT;		\
			continue;							\
		}										\
	} while (0)

void vBSP430ledInit( void )
{
	unsigned portBASE_TYPE uxLED;

	for (uxLED = 0; uxLED < ( unsigned portBASE_TYPE ) ucLEDDefnCount; ++uxLED)
	{
		const xLEDDefn * pxLED = pxLEDDefn + uxLED;

		/* Only include the configuration checks if the target MCU has
		 * the corresponding port supported. */
#if defined( __MSP430_HAS_PORT1__ ) || defined( __MSP430_HAS_PORT1_R__ )
		prvCONFIG_PORT_X_LED(1);
#endif /* PORT1 */
#if defined( __MSP430_HAS_PORT2__ ) || defined( __MSP430_HAS_PORT2_R__ )
		prvCONFIG_PORT_X_LED(2);
#endif /* PORT2 */
#if defined( __MSP430_HAS_PORT3__ ) || defined( __MSP430_HAS_PORT3_R__ )
		prvCONFIG_PORT_X_LED(3);
#endif /* PORT3 */
#if defined( __MSP430_HAS_PORT4__ ) || defined( __MSP430_HAS_PORT4_R__ )
		prvCONFIG_PORT_X_LED(4);
#endif /* PORT4 */
#if defined( __MSP430_HAS_PORT5__ ) || defined( __MSP430_HAS_PORT5_R__ )
		prvCONFIG_PORT_X_LED(5);
#endif /* PORT5 */
#if defined( __MSP430_HAS_PORT6__ ) || defined( __MSP430_HAS_PORT6_R__ )
		prvCONFIG_PORT_X_LED(6);
#endif /* PORT6 */
#if defined( __MSP430_HAS_PORT7__ ) || defined( __MSP430_HAS_PORT7_R__ )
		prvCONFIG_PORT_X_LED(7);
#endif /* PORT7 */
#if defined( __MSP430_HAS_PORT8__ ) || defined( __MSP430_HAS_PORT8_R__ )
		prvCONFIG_PORT_X_LED(8);
#endif /* PORT8 */
#if defined( __MSP430_HAS_PORT9__ ) || defined( __MSP430_HAS_PORT9_R__ )
		prvCONFIG_PORT_X_LED(9);
#endif /* PORT9 */
#if defined( __MSP430_HAS_PORT10__ ) || defined( __MSP430_HAS_PORT10_R__ )
		prvCONFIG_PORT_X_LED(10);
#endif /* PORT10 */
#if defined( __MSP430_HAS_PORT11__ ) || defined( __MSP430_HAS_PORT11_R__ )
		prvCONFIG_PORT_X_LED(11);
#endif /* PORT10 */

	}
}

void vBSP430ledSet( unsigned portBASE_TYPE uxLED,
					signed portBASE_TYPE xValue )
{
	if (uxLED < ( unsigned portBASE_TYPE ) ucLEDDefnCount)
	{
		const xLEDDefn * pxLED = pxLEDDefn + uxLED;

		taskENTER_CRITICAL();
		if (xValue > 0)
		{
			*pxLED->pucPxOUT |= pxLED->ucBIT;
		}
		else if (xValue < 0)
		{
			*pxLED->pucPxOUT ^= pxLED->ucBIT;
		}
		else
		{
			*pxLED->pucPxOUT &= ~pxLED->ucBIT;
		}
		taskEXIT_CRITICAL();
	}
}
