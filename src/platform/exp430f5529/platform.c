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

#include <bsp430/platform/exp430f5529/platform.h>
#include <bsp430/periph/ucs.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/usci5.h>
#include <bsp430/utility/uptime.h>
#include <stdint.h>

const xBSP430led pxBSP430leds[] = {
  { .pucPxOUT = &P1OUT, .ucBIT = BIT0 }, /* Red */
  { .pucPxOUT = &P8OUT, .ucBIT = BIT1 }, /* Orange */
  { .pucPxOUT = &P8OUT, .ucBIT = BIT2 }, /* Green */
  { .pucPxOUT = &P1OUT, .ucBIT = BIT1 }, /* Blue */
  { .pucPxOUT = &P1OUT, .ucBIT = BIT2 }, /* Blue */
  { .pucPxOUT = &P1OUT, .ucBIT = BIT3 }, /* Blue */
  { .pucPxOUT = &P1OUT, .ucBIT = BIT4 }, /* Blue */
  { .pucPxOUT = &P1OUT, .ucBIT = BIT5 }, /* Blue */
};
const unsigned char ucBSP430leds = sizeof(pxBSP430leds) / sizeof(*pxBSP430leds);

int
iBSP430platformConfigurePeripheralPins_ni (xBSP430periphHandle device, int enablep)
{
  uint8_t bits = 0;
  volatile uint8_t * pxsel = 0;
  if (BSP430_PERIPH_LFXT1 == device) {
    /* Setting P5.4 is sufficient for both XIN and XOUT */
    bits = BIT4;
    pxsel = &P5SEL;
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    /* P1.0 ACLK, P2.2 SMCLK, P7.7 MCLK */
    if (enablep) {
      P1SEL |= BIT0;
      P2SEL |= BIT2;
      P7SEL |= BIT7;
    } else {
      P1SEL &= ~BIT0;
      P2SEL &= ~BIT2;
      P7SEL &= ~BIT7;
    }
    P1DIR |= BIT0;
    P2DIR |= BIT2;
    P7DIR |= BIT7;
    return 0;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_PERIPH_USCI5_A0 - 0
  else if (BSP430_PERIPH_USCI5_A0 == device) {
    bits = BIT3 | BIT4;
    pxsel = &P3SEL;
  }
#endif
#if configBSP430_PERIPH_USCI5_A1 - 0
  else if (BSP430_PERIPH_USCI5_A1 == device) {
    bits = BIT4 | BIT5;
    pxsel = &P4SEL;
  }
#endif
  else {
    return -1;
  }
  if (enablep) {
    *pxsel |= bits;
  } else {
    *pxsel &= ~bits;
  }
  return 0;
}

void
vBSP430platformSpinForJumper_ni (void)
{
  const unsigned char led_bits = BIT1 | BIT2 | BIT3 | BIT4 | BIT5;
  /* P7.7 input configured with pullup */
  P7DIR &= ~BIT7;
  P7REN |= BIT7;
  P7OUT |= BIT7;

  /* Flash LEDs alternately while waiting */
#if BSP430_LED
  vBSP430ledInitialize_ni();
#else /* BSP430_LED */
  P1DIR |= led_bits;
  P1SEL &= ~led_bits;
#endif /* BSP430_LED */
  P1OUT &= ~(BIT2 | BIT4);
  P1OUT |= BIT1 | BIT3 | BIT5;
  while (! (P7IN & BIT7)) {
    BSP430_CORE_WATCHDOG_CLEAR();
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 10);
    P1OUT ^= led_bits;
  }

  /* Restore P1.0 and LEDs */
  P1OUT &= ~led_bits;
  P7OUT &= ~BIT7;
  P7DIR |= BIT7;
  P7REN &= ~BIT7;
}

void vBSP430platformInitialize_ni (void)
{
  int crystal_ok = 0;
  (void)crystal_ok;

#if ! (configBSP430_CORE_SUPPORT_WATCHDOG - 0)
  /* Hold off watchdog */
  WDTCTL = WDTPW + WDTHOLD;
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

#if BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 - 0
  /* Enable XT1 functions and clock */
  crystal_ok = iBSP430clockConfigureLFXT1_ni(1, (BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC * BSP430_CLOCK_PUC_MCLK_HZ) / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 */

#if BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS - 0
  iBSP430clockConfigureACLK_ni(BSP430_PLATFORM_BOOT_ACLKSRC);
  ulBSP430clockConfigureMCLK_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  iBSP430clockConfigureSMCLKDividingShift_ni(BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT);
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS */

#if BSP430_UPTIME - 0
  vBSP430uptimeStart_ni();
#endif /* BSP430_UPTIME */
}
