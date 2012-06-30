/** @file
 *
 * Definitions for a generic console print capability.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @copyright @link http://www.opensource.org/licenses/BSD-3-Clause BSD 3-Clause @endlink
 */

#include "platform.h"
#include "serial.h"
#include <bsp430/utility/console.h>
#include "semphr.h"
#include "stdio.h"

static xComPortHandle prvConsole;
static xSemaphoreHandle prvConsoleSemaphore;
static portTickType prvBlockTime;

static int
cputchar (int c)
{
	return xSerialPutChar(prvConsole, c, 0) ? c : -1;
}

int
__attribute__((__weak__))
putchar (int c)
{
	return xSerialPutChar(prvConsole, c, 0) ? c : -1;
}

portBASE_TYPE
xConsoleConfigure (xComPortHandle xConsole,
				   portTickType xBlockTime)
{
	if (prvConsoleSemaphore) {
		vSemaphoreDelete(prvConsoleSemaphore);
		prvConsoleSemaphore = 0;
	}
	prvConsole = 0;
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
