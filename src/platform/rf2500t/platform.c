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

#include <bsp430/platform/rf2500t.h>
#include <bsp430/periph/bc2.h>
#include <bsp430/periph/usci.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/led.h>

const xBSP430led pxBSP430leds[] = {
  { .pucPxOUT = &P1OUT, .ucBIT = BIT0 }, /* Red */
  { .pucPxOUT = &P1OUT, .ucBIT = BIT1 }, /* Green */
};
const unsigned char ucBSP430leds = sizeof(pxBSP430leds) / sizeof(*pxBSP430leds);

int
iBSP430platformConfigurePeripheralPins_ni (xBSP430periphHandle device, int enablep)
{
  unsigned int bits;

  /* This platform does not support a crystal */
  if (0) {
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    bits = BIT0 | BIT1;
    if (enablep) {
      P2SEL |= bits;
    } else {
      P2SEL &= ~bits;
    }
    P2DIR |= bits;
    return 0;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_PERIPH_USCI_A0 - 0
  else if (BSP430_PERIPH_USCI_A0 == device) {
    bits = BIT4 | BIT5;
    if (enablep) {
      P3SEL |= bits;
    } else {
      P3SEL &= ~bits;
    }
    return 0;
  }
#endif /* configBSP430_PERIPH_USCI_A0 */

  return -1;
}

void vBSP430platformInitialize_ni (void)
{
#if ! (configBSP430_CORE_SUPPORT_WATCHDOG - 0)
  /* Hold off watchdog */
  WDTCTL = WDTPW + WDTHOLD;
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

#if BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 - 0
  /* Attempt to stabilize the crystal */
  (void)iBSP430clockConfigureLFXT1_ni(1, (BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC * BSP430_CLOCK_PUC_MCLK_HZ) / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 */

#if BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS - 0
  iBSP430clockConfigureACLK_ni(BSP430_PLATFORM_BOOT_ACLKSRC);
  ulBSP430clockConfigureMCLK_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  iBSP430clockConfigureSMCLKDividingShift_ni(BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT);
#if configBSP430_CLOCK_DISABLE_FLL - 0
  __bis_status_register(SCG0);
#endif /* configBSP430_CLOCK_DISABLE_FLL */
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS */

#if BSP430_UPTIME - 0
  vBSP430uptimeStart_ni();
#endif /* BSP430_UPTIME */
}

void
vBSP430platformSpinForJumper_ni (void)
{
  /* P1.2 input with pullup */
  P1SEL &= ~BIT2;
  P1DIR &= ~BIT2;
  P1REN |= BIT2;
  P1OUT |= BIT2;
#if BSP430_LED
  vBSP430ledInitialize_ni();
#else /* BSP430_LED */
  P1DIR |= (BIT0 | BIT1);
  P1SEL &= ~(BIT0 | BIT1);
#endif /* BSP430_LED */
  P1OUT |= BIT0;
  while (! (P1IN & BIT2)) {
    BSP430_CORE_WATCHDOG_CLEAR();
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 10);
    P1OUT ^= BIT0 + BIT1;
  }
  /* Restore P1.2 */
  P1OUT &= ~(BIT0 | BIT1 | BIT2);
  P1DIR |= BIT2;
  P1REN &= ~BIT2;
}
