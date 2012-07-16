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
 * Definitions for a generic console print capability.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @copyright @link http://www.opensource.org/licenses/BSD-3-Clause BSD 3-Clause @endlink
 */

#include <bsp430/utility/console.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"

static xBSP430uartHandle prvConsole;
static xSemaphoreHandle prvConsoleSemaphore;
static portTickType prvBlockTime;

static int
cputchar (int c)
{
	return iBSP430uartPutChar(c, prvConsole);
}

int
__attribute__((__weak__))
putchar (int c)
{
	return iBSP430uartPutChar(c, prvConsole);
}

portBASE_TYPE
xConsoleConfigure (xBSP430uartHandle xConsole,
				   portTickType xBlockTime)
{
	if (prvConsoleSemaphore) {
		vSemaphoreDelete(prvConsoleSemaphore);
		prvConsoleSemaphore = 0;
	}
	prvConsole = 0;
	prvBlockTime = xBlockTime;
	if (xConsole) {
		vSemaphoreCreateBinary(prvConsoleSemaphore);
		if (! prvConsoleSemaphore) {
			return pdFAIL;
		}
		prvConsole = xConsole;
	}
	return pdPASS;
}

int
__attribute__((__format__(printf, 1, 2)))
cprintf (const char *fmt, ...)
{
	va_list argp;
	int rc;
	
	if (! prvConsoleSemaphore) {
		return -1;
	}
	if (! xSemaphoreTake(prvConsoleSemaphore, prvBlockTime)) {
		return -1;
	}
	va_start (argp, fmt);
	rc = vuprintf (cputchar, fmt, argp);
	va_end (argp);
	xSemaphoreGive(prvConsoleSemaphore);
	return rc;
}
