/* Copyright 2012-2014, Peter A. Bigot
 * Copyright 2012, Texas Instruments Incorporated
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
 * * Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
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
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Portions Copyright
 * 2012, Texas Instruments Incorporated.  Licensed under <a
 * href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/periph/flash.h>

#if (BSP430_MODULE_FLASH - 0)

/* Fix API inconsistency between FLASH2 and FLASH modules */
#if defined(__MSP430_HAS_FLASH2__)
#define FWPW FWKEY
#endif /* FWPW */

int
iBSP430flashEraseSegment_ni (const void * addr)
{
  BSP430_CORE_WATCHDOG_CLEAR();
  while (BUSY & FCTL3) {
    ;
  }
  BSP430_CORE_WATCHDOG_CLEAR();
  FCTL3 = FWPW;
  FCTL1 = FWPW | ERASE;
  *(char *)addr = 0;
  while (BUSY & FCTL3) {
    ;
  }
  FCTL3 = FWPW | LOCK;
  return 0;
}

int
iBSP430flashWriteData_ni (void * dest,
                          const void * src,
                          size_t len)
{
  char * dp = (char *)dest;
  const char * sp = (const char *)src;
  const char * const esp = sp + len/sizeof(*dp);

  BSP430_CORE_WATCHDOG_CLEAR();
  while (BUSY & FCTL3) {
    ;
  }
  BSP430_CORE_WATCHDOG_CLEAR();
  FCTL3 = FWPW;
  FCTL1 = FWPW | WRT;
  /* Yes, we could go through contortions to write word and
   * double-word values at a time.  Then we'd have to validate the
   * implementation in the case of misaligned data.  Besides, on 5xx
   * it's 4x faster to write 128 bytes as single bytes than to write
   * 64 words */
  while (sp < esp) {
    *dp++ = *sp++;
  }
  while (BUSY & FCTL3) {
    ;
  }
  FCTL3 = FWPW | LOCK;
  return len;
}

#endif /* BSP430_MODULE_FLASH */
