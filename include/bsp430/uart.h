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

#ifndef BSP430_COMMON_UART_H
#define BSP430_COMMON_UART_H

#include "FreeRTOS.h"

/** @typedef xBSP430UARTHandle
 *
 * Alias for an underlying peripheral handle that serves as the UART
 * facility for the application. */

#ifndef configBSP430_UART_USE_5XX_USCI
/** Define to true value to select xBSP430USCI as the underlying UART
 * implementation. */
#define configBSP430_UART_USE_5XX_USCI 0
#endif /* configBSP430_UART_USE_5XX_USCI */

#if configBSP430_UART_USE_5XX_USCI - 0
#include <bsp430/5xx/usci.h>

/** Alias for device providing UART capabilities */
typedef xBSP430USCIHandle xBSP430UARTHandle;

static __inline__ xBSP430UARTHandle
xBSP430UARTOpen (xBSP430Periph xPeriph,
				 unsigned int control_word,
				 unsigned long baud,
				 xQueueHandle rx_queue,
				 xQueueHandle tx_queue)
{
	return xBSP430USCIOpenUART(xPeriph, control_word, baud, rx_queue, tx_queue);
}

static __inline__ int
iBSP430UARTClose (xBSP430UARTHandle xUART)
{
	return iBSP430USCIClose(xUART);
}

static __inline__ int
iBSP430UARTputc (int c, xBSP430UARTHandle xUART)
{
	return iBSP430USCIputc(c, xUART);
}

static __inline__ int
iBSP430UARTputs (const char * str, xBSP430UARTHandle xUART)
{
	return iBSP430USCIputs(str, xUART);
}

#endif /* configBSP430_UART_USE_5XX_USCI */

#ifndef configBSP430_UART_USE_EUSCI
/** Define to true value to select xBSP430eUSCI_A as the underlying
 * UART implementation. */
#define configBSP430_UART_USE_EUSCI 0
#endif /* configBSP430_UART_USE_EUSCI */

#if configBSP430_UART_USE_EUSCI - 0

#include <bsp430/5xx/eusci_a.h>

/** Alias for device providing UART capabilities */
typedef xBSP430eUSCIAHandle xBSP430UARTHandle;

static __inline__ xBSP430UARTHandle
xBSP430UARTOpen (xBSP430Periph xPeriph,
				 unsigned int control_word,
				 unsigned long baud,
				 xQueueHandle rx_queue,
				 xQueueHandle tx_queue)
{
	return xBSP430eUSCIAOpenUART(xPeriph, control_word, baud, rx_queue, tx_queue);
}

static __inline__ int
iBSP430UARTClose (xBSP430UARTHandle xUART)
{
	return iBSP430eUSCIAClose(xUART);
}

static __inline__ int
iBSP430UARTputc (int c, xBSP430UARTHandle xUART)
{
	return iBSP430eUSCIAputc(c, xUART);
}

static __inline__ int
iBSP430UARTputs (const char * str, xBSP430UARTHandle xUART)
{
	return iBSP430eUSCIAputs(str, xUART);
}

#endif /* configBSP430_UART_USE_EUSCI */

#endif /* BSP430_COMMON_UART_H */
