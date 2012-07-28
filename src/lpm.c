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
 * @brief Definitions for low power mode support.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @copyright @link http://www.opensource.org/licenses/BSD-3-Clause BSD 3-Clause @endlink
 */

#include <bsp430/lpm.h>

void
vBSP430lpmConfigurePortsForLPM_ni (void)
{
#define CONFIGURE_FOR_LPM(_p) do {              \
    P##_p##DIR = ~0;                            \
    P##_p##OUT = 0;                             \
  } while (0)

#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#if defined(__MSP430_HAS_PORTA__) || defined(__MSP430_HAS_PORTA_R__)
  CONFIGURE_FOR_LPM(A);
#endif /* PORTA */
#if defined(__MSP430_HAS_PORTB__) || defined(__MSP430_HAS_PORTB_R__)
  CONFIGURE_FOR_LPM(B);
#endif /* PORTB */
#if defined(__MSP430_HAS_PORTC__) || defined(__MSP430_HAS_PORTC_R__)
  CONFIGURE_FOR_LPM(C);
#endif /* PORTC */
#if defined(__MSP430_HAS_PORTD__) || defined(__MSP430_HAS_PORTD_R__)
  CONFIGURE_FOR_LPM(D);
#endif /* PORTD */
#if defined(__MSP430_HAS_PORTE__) || defined(__MSP430_HAS_PORTE_R__)
  CONFIGURE_FOR_LPM(E);
#endif /* PORTE */
#if defined(__MSP430_HAS_PORTF__) || defined(__MSP430_HAS_PORTF_R__)
  CONFIGURE_FOR_LPM(F);
#endif /* PORTF */
#else /* CPUXV2 */
#if defined(__MSP430_HAS_PORT1__) || defined(__MSP430_HAS_PORT1_R__)
  CONFIGURE_FOR_LPM(1);
#endif /* PORT1 */
#if defined(__MSP430_HAS_PORT2__) || defined(__MSP430_HAS_PORT2_R__)
  CONFIGURE_FOR_LPM(2);
#endif /* PORT2 */
#if defined(__MSP430_HAS_PORT3__) || defined(__MSP430_HAS_PORT3_R__)
  CONFIGURE_FOR_LPM(3);
#endif /* PORT3 */
#if defined(__MSP430_HAS_PORT4__) || defined(__MSP430_HAS_PORT4_R__)
  CONFIGURE_FOR_LPM(4);
#endif /* PORT4 */
#if defined(__MSP430_HAS_PORT5__) || defined(__MSP430_HAS_PORT5_R__)
  CONFIGURE_FOR_LPM(5);
#endif /* PORT5 */
#if defined(__MSP430_HAS_PORT6__) || defined(__MSP430_HAS_PORT6_R__)
  CONFIGURE_FOR_LPM(6);
#endif /* PORT6 */
#if defined(__MSP430_HAS_PORT7__) || defined(__MSP430_HAS_PORT7_R__)
  CONFIGURE_FOR_LPM(7);
#endif /* PORT7 */
#if defined(__MSP430_HAS_PORT8__) || defined(__MSP430_HAS_PORT8_R__)
  CONFIGURE_FOR_LPM(8);
#endif /* PORT8 */
#if defined(__MSP430_HAS_PORT9__) || defined(__MSP430_HAS_PORT9_R__)
  CONFIGURE_FOR_LPM(9);
#endif /* PORT9 */
#if defined(__MSP430_HAS_PORT10__) || defined(__MSP430_HAS_PORT10_R__)
  CONFIGURE_FOR_LPM(10);
#endif /* PORT10 */
#if defined(__MSP430_HAS_PORT11__) || defined(__MSP430_HAS_PORT11_R__)
  CONFIGURE_FOR_LPM(11);
#endif /* PORT11 */
#if defined(__MSP430_HAS_PORT12__) || defined(__MSP430_HAS_PORT12_R__)
  CONFIGURE_FOR_LPM(12);
#endif /* PORT12 */
#endif /* CPUXV2 */
#if defined(__MSP430_HAS_PORTJ__)
  CONFIGURE_FOR_LPM(J);
#endif /* PORTJ */
#undef CONFIGURE_FOR_LPM
}



