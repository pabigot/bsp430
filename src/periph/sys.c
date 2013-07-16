/* Copyright 2012-2013, Peter A. Bigot
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
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/periph/sys.h>

#if (BSP430_MODULE_SYS - 0)

/* The list of relevant system reset vector values can be obtained by
 * filtering the MCU-specific headers using this pipe:

 grep -h 'define.*SYSRSTIV' * \
   | sort \
   | grep : \
   | grep -v Reserved \
   | uniq \
   | sort -b -k +3

Values through security violation (SYSRSTIV_SECYV) reflect a BOR;
greater values reflect a lesser reset.

Passing the result through the following filter removes cases where
the constant differs but the symbol and description are the same:

   | sed -e 's@(0x....) *@@' \
   | sort \
   | uniq \

The initializer for the table of expected values can be obtained by
passing that output through:

   | sed -e 's@^[^ ]* \([^ ]*\) .*: \(.*\) \*\/@#ifdef \1%{ .iv = \1, .desc = "\2" },%#endif /\* \1 *\/@' \
   | tr '%\015' '\012\012'
   | dos2unix
*/

#define EMIT_SYSRSTIV(iv_,txt_) { iv: iv_, desc: txt_ },

typedef struct sysrstiv_pair_t {
  unsigned char iv;
  const char * desc;
} sysrstiv_pair_t;

static const sysrstiv_pair_t sysrstiv_pairs[] = {

#ifdef SYSRSTIV_BOR
  { .iv = SYSRSTIV_BOR, .desc = "BOR" },
#endif /* SYSRSTIV_BOR */

#ifdef SYSRSTIV_CCSKEY
  { .iv = SYSRSTIV_CCSKEY, .desc = "CCS Key violation" },
#endif /* SYSRSTIV_CCSKEY */

#ifdef SYSRSTIV_CSKEY
  { .iv = SYSRSTIV_CSKEY, .desc = "CSKEY violation" },
#endif /* SYSRSTIV_CSKEY */

#ifdef SYSRSTIV_CSPW
  { .iv = SYSRSTIV_CSPW, .desc = "CS Password violation" },
#endif /* SYSRSTIV_CSPW */

#ifdef SYSRSTIV_DBDIFG
  { .iv = SYSRSTIV_DBDIFG, .desc = "FRAM Double bit Error" },
#endif /* SYSRSTIV_DBDIFG */

#ifdef SYSRSTIV_DOBOR
  { .iv = SYSRSTIV_DOBOR, .desc = "Do BOR" },
#endif /* SYSRSTIV_DOBOR */

#ifdef SYSRSTIV_DOPOR
  { .iv = SYSRSTIV_DOPOR, .desc = "Do POR" },
#endif /* SYSRSTIV_DOPOR */

#ifdef SYSRSTIV_FLLUL
  { .iv = SYSRSTIV_FLLUL, .desc = "FLL unlock" },
#endif /* SYSRSTIV_FLLUL */

#ifdef SYSRSTIV_FRCTLPW
  { .iv = SYSRSTIV_FRCTLPW, .desc = "FRAM Key violation" },
#endif /* SYSRSTIV_FRCTLPW */

#ifdef SYSRSTIV_KEYV
  { .iv = SYSRSTIV_KEYV, .desc = "Flash Key violation" },
#endif /* SYSRSTIV_KEYV */

#ifdef SYSRSTIV_LPM5WU
  { .iv = SYSRSTIV_LPM5WU, .desc = "Port LPM5 Wake Up" },
#endif /* SYSRSTIV_LPM5WU */

#ifdef SYSRSTIV_MPUKEY
  { .iv = SYSRSTIV_MPUKEY, .desc = "MPUKEY violation" },
#endif /* SYSRSTIV_MPUKEY */

#ifdef SYSRSTIV_MPUPW
  { .iv = SYSRSTIV_MPUPW, .desc = "MPU Password violation" },
#endif /* SYSRSTIV_MPUPW */

#ifdef SYSRSTIV_MPUSEG1IFG
  { .iv = SYSRSTIV_MPUSEG1IFG, .desc = "MPUSEG1IFG violation" },
#endif /* SYSRSTIV_MPUSEG1IFG */

#ifdef SYSRSTIV_MPUSEG2IFG
  { .iv = SYSRSTIV_MPUSEG2IFG, .desc = "MPUSEG2IFG violation" },
#endif /* SYSRSTIV_MPUSEG2IFG */

#ifdef SYSRSTIV_MPUSEG3IFG
  { .iv = SYSRSTIV_MPUSEG3IFG, .desc = "MPUSEG3IFG violation" },
#endif /* SYSRSTIV_MPUSEG3IFG */

#ifdef SYSRSTIV_MPUSEGIIFG
  { .iv = SYSRSTIV_MPUSEGIIFG, .desc = "MPUSEGIIFG violation" },
#endif /* SYSRSTIV_MPUSEGIIFG */

#ifdef SYSRSTIV_MPUSEGPIFG
  { .iv = SYSRSTIV_MPUSEGPIFG, .desc = "MPUSEGPIFG violation" },
#endif /* SYSRSTIV_MPUSEGPIFG */

#ifdef SYSRSTIV_PERF
  { .iv = SYSRSTIV_PERF, .desc = "peripheral/config area fetch" },
#endif /* SYSRSTIV_PERF */

#ifdef SYSRSTIV_PLLUL
  { .iv = SYSRSTIV_PLLUL, .desc = "PLL unlock" },
#endif /* SYSRSTIV_PLLUL */

#ifdef SYSRSTIV_PMMKEY
  { .iv = SYSRSTIV_PMMKEY, .desc = "PMMKEY violation" },
#endif /* SYSRSTIV_PMMKEY */

#ifdef SYSRSTIV_PMMPW
  { .iv = SYSRSTIV_PMMPW, .desc = "PMM Password violation" },
#endif /* SYSRSTIV_PMMPW */

#ifdef SYSRSTIV_PSSKEY
  { .iv = SYSRSTIV_PSSKEY, .desc = "PMMKEY violation" },
#endif /* SYSRSTIV_PSSKEY */

#ifdef SYSRSTIV_RSTNMI
  { .iv = SYSRSTIV_RSTNMI, .desc = "RST/NMI" },
#endif /* SYSRSTIV_RSTNMI */

#ifdef SYSRSTIV_SECYV
  { .iv = SYSRSTIV_SECYV, .desc = "Security violation" },
#endif /* SYSRSTIV_SECYV */

#ifdef SYSRSTIV_SVMBOR
  { .iv = SYSRSTIV_SVMBOR, .desc = "SVMBOR" },
#endif /* SYSRSTIV_SVMBOR */

#ifdef SYSRSTIV_SVMH_OVP
  { .iv = SYSRSTIV_SVMH_OVP, .desc = "SVMH_OVP" },
#endif /* SYSRSTIV_SVMH_OVP */

#ifdef SYSRSTIV_SVML_OVP
  { .iv = SYSRSTIV_SVML_OVP, .desc = "SVML_OVP" },
#endif /* SYSRSTIV_SVML_OVP */

#ifdef SYSRSTIV_SVSH
  { .iv = SYSRSTIV_SVSH, .desc = "SVSH" },
#endif /* SYSRSTIV_SVSH */

#ifdef SYSRSTIV_SVSHIFG
  { .iv = SYSRSTIV_SVSHIFG, .desc = "SVSHIFG" },
#endif /* SYSRSTIV_SVSHIFG */

#ifdef SYSRSTIV_SVSL
  { .iv = SYSRSTIV_SVSL, .desc = "SVSL" },
#endif /* SYSRSTIV_SVSL */

#ifdef SYSRSTIV_SVSLIFG
  { .iv = SYSRSTIV_SVSLIFG, .desc = "SVSLIFG" },
#endif /* SYSRSTIV_SVSLIFG */

#ifdef SYSRSTIV_UBDIFG
  { .iv = SYSRSTIV_UBDIFG, .desc = "FRAM Uncorrectable bit Error" },
#endif /* SYSRSTIV_UBDIFG */

#ifdef SYSRSTIV_WDTKEY
  { .iv = SYSRSTIV_WDTKEY, .desc = "WDTKEY violation" },
#endif /* SYSRSTIV_WDTKEY */

#ifdef SYSRSTIV_WDTTO
  { .iv = SYSRSTIV_WDTTO, .desc = "WDT Time out" },
#endif /* SYSRSTIV_WDTTO */
};

const char *
xBSP430sysSYSRSTIVDescription (unsigned int sysrstiv)
{
  const sysrstiv_pair_t * pp = sysrstiv_pairs;
  const sysrstiv_pair_t * epp = pp + sizeof(sysrstiv_pairs)/sizeof(*sysrstiv_pairs);

  while (pp < epp) {
    if (pp->iv == sysrstiv) {
      return pp->desc;
    }
    ++pp;
  }
  return NULL;
}

unsigned int
uiBSP430sysSYSRSTGenerator_ni (unsigned int * puiResetFlags)
{
  unsigned int iv = SYSRSTIV;

  if (SYSRSTIV_NONE == iv) {
    return 0;
  }
  if (NULL != puiResetFlags) {
    if (iv <= SYSRSTIV_SECYV) {
      *puiResetFlags |= BSP430_SYS_FLAG_SYSRST_BOR;
    }
    if (iv == SYSRSTIV_LPM5WU) {
      *puiResetFlags |= BSP430_SYS_FLAG_SYSRST_LPM5WU;
    }
    if (iv <= SYSRSTIV_DOPOR) {
      *puiResetFlags |= BSP430_SYS_FLAG_SYSRST_POR;
    }
    *puiResetFlags |= BSP430_SYS_FLAG_SYSRST_PUC;
  }
  return iv;
}

#endif /* BSP430_MODULE_SYS */
