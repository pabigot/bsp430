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

#include <msp430.h>
#include <bsp430/utility/led.h>

#if (BSP430_LED - 0)

/* Some MSP430 MCUs use two selector registers.  At this time, it
 * appears that if an MCU has two selectors for one port, it has two
 * for all, so we'll use the existings of P1SEL0 as the trigger for
 * the variation. */
#if defined(P1SEL0)
#define CLEAR_SEL_X(_x, _bit) do {              \
    P##_x##SEL0 &= ~_bit;                       \
    P##_x##SEL1 &= ~_bit;                       \
  } while(0)
#else /* P1SEL0 */
#define CLEAR_SEL_X(_x, _bit) do {              \
    P##_x##SEL &= ~_bit;                        \
  } while(0)
#endif /* P1SEL0 */

/** Macro to configure GPIO for a LED within a particular port.
 *
 * I'm sorry if this isn't MISRA compliant, but I'm not replicating
 * this code for each of the 11 potential ports. */
#define CONFIG_PORT_X_LED(_x)                   \
  do {                                          \
    if (&P##_x##OUT == lp->outp) {              \
      P##_x##DIR |= lp->bit;                    \
      CLEAR_SEL_X(_x, lp->bit);                 \
      P##_x##OUT &= ~lp->bit;                   \
      continue;                                 \
    }                                           \
  } while (0)

void
vBSP430ledInitialize_ni (void)
{
  unsigned char ucLED;

  for (ucLED = 0; ucLED < nBSP430led; ++ucLED) {
    const sBSP430halLED * lp = xBSP430halLED_ + ucLED;

    /* Only include the configuration checks if the target MCU has
     * the corresponding port supported. */
#if defined( __MSP430_HAS_PORT1__ ) || defined( __MSP430_HAS_PORT1_R__ )
    CONFIG_PORT_X_LED(1);
#endif /* PORT1 */
#if defined( __MSP430_HAS_PORT2__ ) || defined( __MSP430_HAS_PORT2_R__ )
    CONFIG_PORT_X_LED(2);
#endif /* PORT2 */
#if defined( __MSP430_HAS_PORT3__ ) || defined( __MSP430_HAS_PORT3_R__ )
    CONFIG_PORT_X_LED(3);
#endif /* PORT3 */
#if defined( __MSP430_HAS_PORT4__ ) || defined( __MSP430_HAS_PORT4_R__ )
    CONFIG_PORT_X_LED(4);
#endif /* PORT4 */
#if defined( __MSP430_HAS_PORT5__ ) || defined( __MSP430_HAS_PORT5_R__ )
    CONFIG_PORT_X_LED(5);
#endif /* PORT5 */
#if defined( __MSP430_HAS_PORT6__ ) || defined( __MSP430_HAS_PORT6_R__ )
    CONFIG_PORT_X_LED(6);
#endif /* PORT6 */
#if defined( __MSP430_HAS_PORT7__ ) || defined( __MSP430_HAS_PORT7_R__ )
    CONFIG_PORT_X_LED(7);
#endif /* PORT7 */
#if defined( __MSP430_HAS_PORT8__ ) || defined( __MSP430_HAS_PORT8_R__ )
    CONFIG_PORT_X_LED(8);
#endif /* PORT8 */
#if defined( __MSP430_HAS_PORT9__ ) || defined( __MSP430_HAS_PORT9_R__ )
    CONFIG_PORT_X_LED(9);
#endif /* PORT9 */
#if defined( __MSP430_HAS_PORT10__ ) || defined( __MSP430_HAS_PORT10_R__ )
    CONFIG_PORT_X_LED(10);
#endif /* PORT10 */
#if defined( __MSP430_HAS_PORT11__ ) || defined( __MSP430_HAS_PORT11_R__ )
    CONFIG_PORT_X_LED(11);
#endif /* PORT10 */
  }
}

void
vBSP430ledSet (int led_idx,
               int value)
{
  if (led_idx < nBSP430led) {
    const sBSP430halLED * lp = xBSP430halLED_ + led_idx;

    if (value > 0) {
      *lp->outp |= lp->bit;
    } else if (value < 0) {
      *lp->outp ^= lp->bit;
    } else {
      *lp->outp &= ~lp->bit;
    }
  }
}

int
vBSP430ledGet (int led_idx)
{
  int rv = 0;

  if (led_idx < nBSP430led) {
    const sBSP430halLED * lp = xBSP430halLED_ + led_idx;
    rv = lp->bit & *lp->outp;
  }
  return rv;
}

#endif /* BSP430_LED */
