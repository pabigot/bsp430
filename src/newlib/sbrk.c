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
 * @brief Alternative implementations of sbrk(2) for BSP430
 *
 * This file contains implementations of sbrk(2) that follow a
 * variety of policies.  The desired implementation is selected at
 * link time by defining the _sbrk symbol to refer to one of these.
 * If @c -ffunction-sections is used, exactly one implementation is
 * included.
 *
 * The code assumes the standard TI linker scripts are used.  This
 * organizes RAM from lower addresses to upper, with initialized and
 * uninitialized data at the bottom, the end marked with the symbol @c
 * end.  There are no allocated heap or stack sections in these
 * standard scripts, thus there are no sbrk(2) implementations for
 * fixed heap or reserved stack policies.  End of RAM is indicated by
 * symbol @c __stack.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/* We're providing system call implementation here, so ensure we have
 * visible prototypes that match what newlib is expecting. */
#define _COMPILING_NEWLIB
#include <sys/unistd.h>

#include <bsp430/newlib/system.h>
#include <sys/types.h>
#include <errno.h>

void *
__attribute__((__weak__))
_bsp430_sbrk_error (void * brk,
                    ptrdiff_t current,
                    ptrdiff_t increment)
{
  BSP430_CORE_DISABLE_INTERRUPT();
  while (1) {
    /* spin */
  }
}

/* Implement allocation with a policy-dependent upper bound. */
static BSP430_CORE_INLINE_FORCED
void *
common_sbrk (char * const upper_bound,
             ptrdiff_t increment)
{
  static char * brk;        /* the current program break */
  extern char end;          /* symbol at which heap starts */
  char * nbrk;
  void * rv;

  if (0 == brk) {
    brk = &end;
  }
  nbrk = increment + brk;
  if (upper_bound < nbrk) {
    return _bsp430_sbrk_error(brk, brk - &end, increment);
  }
  rv = brk;
  brk = nbrk;
  return rv;
}

/** An sbrk() implementation that rejects any attempt to allocate
 * memory dynamically. */
void *
_bsp430_sbrk_fatal (ptrdiff_t increment)
{
  return _bsp430_sbrk_error(0, 0, increment);
}

/** An sbrk() implementation that allows heap (growing up) and stack
 * (growing down) to share a region of memory.
 *
 * An error is indicated if the new break point would encroach into
 * the current stack space.
 *
 * @note Like _bsp430_sbrk_unlimited(), but eliminating the minimum
 * reserved stack.  Not sure why this would be worth doing, but for
 * completeness.... */
void *
_bsp430_sbrk_dynstack (ptrdiff_t increment)
{
  return common_sbrk((char*)(intptr_t)_get_SP_register(), increment);
}

/* Provide a weak alias that will resolve to the unlimitedstack
 * implementation in the case where _sbrk() is requested.  This
 * matches the nosys behavior of newlib. */
void * sbrk (ptrdiff_t increment) __attribute__((__weak__,__alias__("_bsp430_sbrk_dynstack")));
