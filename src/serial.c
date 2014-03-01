/* Copyright 2012-2014, Peter A. Bigot
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
 * @brief Definitions for shared HAL serial support.
 *
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/serial.h>
#include <bsp430/clock.h>
#include <limits.h>

const char *
xBSP430serialName (tBSP430periphHandle periph)
{
  const char * rv = NULL;
#if (configBSP430_SERIAL_USE_USCI - 0)
  if (NULL == rv) {
    rv = xBSP430usciName(periph);
  }
#endif /* configBSP430_SERIAL_USE_USCI */
#if (configBSP430_SERIAL_USE_USCI5 - 0)
  if (NULL == rv) {
    rv = xBSP430usci5Name(periph);
  }
#endif /* configBSP430_SERIAL_USE_USCI5 */
#if (configBSP430_SERIAL_USE_EUSCI - 0)
  if (NULL == rv) {
    rv = xBSP430eusciName(periph);
  }
#endif /* configBSP430_SERIAL_USE_EUSCI */
  return rv;
}

unsigned int
uiBSP430serialSMCLKPrescaler (unsigned long freq_Hz)
{
  unsigned long smclk_Hz = ulBSP430clockSMCLK_Hz();
  unsigned long prescaler;

  if ((0 == freq_Hz) || (freq_Hz >= smclk_Hz)) {
    return 1U;
  }
  prescaler = (smclk_Hz + freq_Hz - 1) / freq_Hz;
  if (prescaler > UINT_MAX) {
    return UINT_MAX;
  }
  return (unsigned int)prescaler;
}
