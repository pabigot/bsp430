/* Copyright 2014, Peter A. Bigot
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
 * @brief Prototypes for BSP430 system enhancements to newlib
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_NEWLIB_SYSTEM_H
#define BSP430_NEWLIB_SYSTEM_H

#include <bsp430/core.h>
#include <unistd.h>

/** The system function used to allocate memory for use by libc heap
 * memory management.
 *
 * @ref newlib_sys_sbrk documents how to select a specific _sbrk()
 * implementation from among the available policies by which
 * allocation is done, and how to supply your own implementation.
 *
 * @note All BSP430 policies invoke _bsp430_sbrk_error() if allocation
 * fails, allowing an application to control response to the failure.
 *
 * @param increment the number of bytes of additional memory that libc
 * needs in order to perform additional allocations.
 *
 * @return a pointer to the new end-of-memory, or <tt>(void*)-1</tt>
 * if no allocation can be performed. */
void * _sbrk (intptr_t increment);

/** This function is invoked whenever _sbrk() runs out of memory.
 *
 * @c libbsp430.a provides a weak definition is that disables
 * interrupts and enters an infinite loop, allowing diagnosis through
 * a debugger.  The application may provide an alternative
 * implementation that is more diagnostic or that returns the
 * responsibility of handling out-of-memory to the application
 * (i.e. requires the application to check allocation return values).
 *
 * @param brk the current program break
 *
 * @param current total number of bytes allocated by previous
 * successful invocations of _sbrk() (i.e., allocated bytes preceding
 * @p brk)
 *
 * @param increment the number of bytes in the request that _sbrk()
 * cannot satisfy
 *
 * @return This implementation does not return.  If superseded, an
 * implementation that does return must set @c errno to @c ENOMEM and
 * return <tt>(void*)-1</tt>. */
void * _bsp430_sbrk_error (void * brk,
                           ptrdiff_t current,
                           ptrdiff_t increment);

/** Variable used by default nosys implementations to record which
 * system calls are invoked.
 *
 * Stub routines in @c newlib/nosys set bits in this variable when
 * they are invoked.  A non-zero value indicates that some stub system
 * interface was invoked in the course of execution; further analysis
 * can identify the functions and determine why they were invoked and
 * whether supplying a non-stub implementation is worthwhile.
 *
 * This is a diagnostic aid that may not be worth maintaining. */
extern unsigned long ulBSP430newlibNoSysInvoked;

#endif /* BSP430_NEWLIB_SYSTEM_H */
