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

#include <bsp430/platform/exp430f5438.h>
#include <bsp430/periph/ucs.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/usci5.h>
#include <bsp430/utility/uptime.h>
#include <stdint.h>

const xBSP430led pxBSP430leds[] = {
  { .pucPxOUT = &P1OUT, .ucBIT = BIT0 }, /* Red */
  { .pucPxOUT = &P1OUT, .ucBIT = BIT1 }, /* Orange */
};
const unsigned char ucBSP430leds = sizeof(pxBSP430leds) / sizeof(*pxBSP430leds);

int
iBSP430platformConfigurePeripheralPins_ni (xBSP430periphHandle device, int enablep)
{
  uint8_t bits = 0;
  volatile uint8_t * pxsel = 0;
  if (BSP430_PERIPH_XT1 == device) {
    bits = BIT0 | BIT1;
    pxsel = &P7SEL;
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    bits = BIT0 | BIT1 | BIT2;
    pxsel = &P11SEL;
    P11DIR |= bits;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_PERIPH_USCI5_A0 - 0
  else if (BSP430_PERIPH_USCI5_A0 == device) {
    bits = BIT4 | BIT5;
    pxsel = &P3SEL;
  }
#endif
#if configBSP430_PERIPH_USCI5_A1 - 0
  else if (BSP430_PERIPH_USCI5_A1 == device) {
    bits = BIT6 | BIT7;
    pxsel = &P5SEL;
  }
#endif
#if configBSP430_PERIPH_USCI5_A2 - 0
  else if (BSP430_PERIPH_USCI5_A2 == device) {
    bits = BIT4 | BIT5;
    pxsel = &P9SEL;
  }
#endif
#if configBSP430_PERIPH_USCI5_A3 - 0
  else if (BSP430_PERIPH_USCI5_A3 == device) {
    bits = BIT4 | BIT5;
    pxsel = &P10SEL;
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


void vBSP430platformSetup_ni (void)
{
  int rc;

#if ! (configBSP430_CORE_SUPPORT_WATCHDOG - 0)
  /* Hold off watchdog */
  WDTCTL = WDTPW + WDTHOLD;
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

  /* Enable XT1 functions and clock */
  rc = iBSP430clockConfigureXT1_ni(1, 2000000L / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
  iBSP430ucsConfigureACLK_ni(rc ? SELA__XT1CLK : SELA__VLOCLK);

#if 0 < BSP430_CLOCK_NOMINAL_MCLK_HZ
  ulBSP430ucsConfigure_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ, -1);
#endif /* BSP430_CLOCK_NOMINAL_MCLK_HZ */

#if 0 <= BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT
  iBSP430clockConfigureSMCLKDividingShift_ni(BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT);
#endif /* BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT */

#if configBSP430_UPTIME - 0
  vBSP430uptimeStart_ni();
#endif /* configBSP430_UPTIME */
}
