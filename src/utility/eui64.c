/* Copyright 2013, Peter A. Bigot
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

#include <bsp430/platform.h>
#include <bsp430/utility/eui64.h>
#include <bsp430/utility/tlv.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#if (BSP430_EUI64 - 0)

#if (configBSP430_EUI64_USE_PLATFORM - 0) && (configBSP430_EUI64_USE_GENERIC - 0)
int
iBSP430eui64 (hBSP430eui64 eui64)
{
  int rv = -1;

  memset(eui64->bytes, 0xFF, sizeof(eui64->bytes));
  do {
    const uint8_t * sp = (const uint8_t *)TLV_START;
    const uint8_t * esp = (const uint8_t *)TLV_END;

    if (0 != sp) {
      uint8_t * bp = eui64->bytes;
      const uint8_t * ebp = bp + sizeof(eui64->bytes);

      while (sp < esp) {
        *bp ^= *sp++;
        if (++bp == ebp) {
          bp -= sizeof(eui64->bytes);
        }
      }
      rv = BSP430_EUI64_STOLEN;
    }

#if 0
    /* Don't do this.  Without introducing peripheral dependencies
     * into this module to set the random seed, every node is going to
     * generate the same random number. */
    if (0 > rv) {
      unsigned int next = 1; /* Seed value */
      int i;

      for (i = 0; i < sizeof(eui64->bytes); ++i) {
        eui64->bytes[i] = rand_r(&next);
      }
      rv = BSP430_EUI64_RANDOM;
    }
#endif /* 0 */
  } while (0);
  if (BSP430_EUI64_PERM < rv) {
    /* Mark generated value as local and individual */
    eui64->bytes[0] |= BSP430_EUI64_MASK_LOCAL;
    eui64->bytes[0] &= ~BSP430_EUI64_MASK_GROUP;
  }
  return rv;
}
#endif /* configBSP430_EUI64_USE_PLATFORM */

char *
xBSP430eui64AsText (hBSP430eui64 eui64,
                    char * buffer)
{
  char * rv = buffer;
  int ei;

  if (NULL == eui64) {
    return NULL;
  }
  buffer += sprintf(buffer, "%02X", eui64->bytes[0]);
  for (ei = 1; ei < sizeof(eui64->bytes); ++ei) {
    buffer += sprintf(buffer, "-%02X", eui64->bytes[ei]);
  }
  return rv;
}

int
iBSP430eui64Parse (const char * cfp,
                   size_t len,
                   hBSP430eui64 eui64)
{
  const char * const cfpe = cfp + len;
  int ei = 0;
  int nn = 0;

  if (NULL == eui64) {
    return -1;
  }
  memset(eui64, 0, sizeof(*eui64));
  while ((ei < sizeof(eui64->bytes)) && (cfp < cfpe) && (0 <= nn)) {
    int c = *cfp++;
    int v;
    c = tolower(c);
    if ('-' == c) {
      nn = 0;
      ++ei;
    } else if ((nn < 2) && (isdigit(c) || (('a' <= c) && (c <= 'f')))) {
      v = isdigit(c) ? (c - '0') : (10 + c - 'a');
      eui64->bytes[ei] = (eui64->bytes[ei] << 4) + v;
      ++nn;
    } else {
      break;
    }
  }
  if (((sizeof(eui64->bytes) - 1) != ei) || (cfp != cfpe)) {
    return -1;
  }
  return 0;
}

#endif /* BSP430_EUI64 */
