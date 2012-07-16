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
 * Declarations for a generic console print capability.
 *
 * cprintf* is like printf* except that it incorporates a semaphore to
 * ensure the output is not interleaved when multiple tasks are
 * printing at the same time.  It also assumes a console device which
 * uses non-blocking output.
 * 
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_CONSOLE_H
#define BSP430_UTILITY_CONSOLE_H

#include <bsp430/uart.h>

/** Like printf, but to platform console.
 *
 * Calls block until access to the console is obtained. */
int
cprintf (const char *string, ...)
__attribute__((__format__(printf, 1, 2)));

/** Configure a console device.
 *
 * @param xUART a serial device, preferably one that does non-blocking
 * writes.  Pass zero to disable the console print infrastructure.
 *
 * @param xBlockTime the duration to wait for any other active users
 * of cprintf to exit
 *
 * @return pdSUCCESS if the console could be configured, pdFAIL
 * otherwise. */
portBASE_TYPE xConsoleConfigure (xBSP430uartHandle xUART,
								 portTickType xBlockTime);

#endif /* BSP430_UTILITY_CONSOLE_H */
