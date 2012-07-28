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

#include <bsp430/platform/exp430fg4618.h>
#include <bsp430/periph/fllplus.h>
#include <bsp430/utility/led.h>

const xBSP430led pxBSP430leds[] = {
  { .pucPxOUT = &P5OUT, .ucBIT = BIT1 }, /* Red */
  { .pucPxOUT = &P2OUT, .ucBIT = BIT2 }, /* Green */
  { .pucPxOUT = &P2OUT, .ucBIT = BIT1 }, /* Yellow */
};
const unsigned char ucBSP430leds = sizeof(pxBSP430leds) / sizeof(*pxBSP430leds);

int
iBSP430platformConfigurePeripheralPins_ni (xBSP430periphHandle device, int enablep)
{
  uint8_t bits = 0;
  volatile uint8_t* pxsel = 0;
  if (BSP430_PERIPH_XT1 == device) {
    /* XIN/XOUT are dedicated pins on this device. */
    return 0;
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    bits = BIT1 | BIT4 | BIT5;
    P1DIR |= bits;
    if (enablep) {
      P1SEL |= bits;
    } else {
      P1SEL &= ~bits;
    }
    return 0;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
  return -1;
}

void vBSP430platformSetup_ni (void)
{
  /* Hold off watchdog */
  WDTCTL = WDTPW + WDTHOLD;

  /* Enable XT1 functions and clock */
  (void)iBSP430clockConfigureXT1_ni(1, 2000000L / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);

#if 0 != BSP430_CLOCK_NOMINAL_MCLK_HZ
  //ulBSP430fllplusConfigureMCLK(BSP430_CLOCK_NOMINAL_MCLK_HZ);
#endif /* BSP430_CLOCK_NOMINAL_MCLK_HZ */

#if 0 <= BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT
  iBSP430clockConfigureSMCLKDividingShift_ni(BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT);
#endif /* BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT */

#if configBSP430_UPTIME - 0
  vBSP430uptimeStart_ni();
#endif
}
