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

/** @file
 *
 * Genericized UART interface
 *
 * Select from supported devices and provide adaptation layer so that
 * other modules can be ignorant of the device type.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UART_H
#define BSP430_UART_H

#include <bsp430/common.h>

#ifndef configBSP430_UART_USE_5XX_USCI
/** Define to true value to select xBSP430usci as the underlying UART
 * implementation. */
#define configBSP430_UART_USE_5XX_USCI 0
#endif /* configBSP430_UART_USE_5XX_USCI */

#if configBSP430_UART_USE_5XX_USCI - 0
#include <bsp430/periph/usci.h>

/** Alias for device providing UART capabilities */
typedef xBSP430usciHandle xBSP430uartHandle;

static __inline__ xBSP430uartHandle
xBSP430uartOpen (xBSP430periphHandle xPeriph,
				 unsigned int control_word,
				 unsigned long baud)
{
	return xBSP430usciOpenUART(xPeriph, control_word, baud);
}

static __inline__ int
iBSP430uartConfigureQueues (xBSP430uartHandle xUART,
							xQueueHandle rx_queue,
							xQueueHandle tx_queue)
{
	return iBSP430usciConfigureQueues(xUART, rx_queue, tx_queue);
}

static __inline__ int
iBSP430uartClose (xBSP430uartHandle xUART)
{
	return iBSP430usciClose(xUART);
}

static __inline__ int
iBSP430uartPutChar (int c, xBSP430uartHandle xUART)
{
	return iBSP430usciPutChar(c, xUART);
}

static __inline__ int
iBSP430uartPutString (const char * str, xBSP430uartHandle xUART)
{
	return iBSP430usciPutString(str, xUART);
}

#endif /* configBSP430_UART_USE_5XX_USCI */

#ifndef configBSP430_UART_USE_EUSCIA
/** Define to true value to select eUSCI_A as the underlying UART
 * implementation. */
#define configBSP430_UART_USE_EUSCIA 0
#endif /* configBSP430_UART_USE_EUSCIA */

#if configBSP430_UART_USE_EUSCIA - 0

#include <bsp430/periph/euscia.h>

/** Alias for device providing UART capabilities */
typedef xBSP430eusciaHandle xBSP430uartHandle;

static __inline__ xBSP430uartHandle
xBSP430uartOpen (xBSP430periphHandle xPeriph,
				 unsigned int control_word,
				 unsigned long baud)
{
	return xBSP430eusciaOpenUART(xPeriph, control_word, baud);
}

static __inline__ int
iBSP430uartConfigureQueues (xBSP430uartHandle xUART,
							xQueueHandle rx_queue,
							xQueueHandle tx_queue)
{
	return iBSP430eusciaConfigureQueues(xUART, rx_queue, tx_queue);
}

static __inline__ int
iBSP430uartClose (xBSP430uartHandle xUART)
{
	return iBSP430eusciaClose(xUART);
}

static __inline__ int
iBSP430uartPutChar (int c, xBSP430uartHandle xUART)
{
	return iBSP430eusciaPutc(c, xUART);
}

static __inline__ int
iBSP430uartPutString (const char * str, xBSP430uartHandle xUART)
{
	return iBSP430eusciaPuts(str, xUART);
}

#endif /* configBSP430_UART_USE_EUSCI */

#endif /* BSP430_UART_H */
