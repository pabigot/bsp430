/* Copyright (c) 2012, Peter A. Bigot
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

#include <bsp430/platform/exp430fg4618/platform.h>
#include <bsp430/periph/fllplus.h>
#include <bsp430/periph/usci.h>
#include <bsp430/utility/led.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/port.h>

#if BSP430_LED - 0
const sBSP430halLED xBSP430hal_[] = {
  { .outp = &P2OUT, .bit = BIT2 }, /* Green (LED1) */
  { .outp = &P2OUT, .bit = BIT1 }, /* Yellow (LED2) */
  /* LED3 is attached to msp430f2013 */
  { .outp = &P5OUT, .bit = BIT1 }, /* Red (LED4) */
};
const unsigned char nBSP430led = sizeof(xBSP430hal_) / sizeof(*xBSP430hal_);
#endif /* BSP430_LED */

int
iBSP430platformConfigurePeripheralPins_ni (tBSP430periphHandle device, int enablep)
{
  unsigned char bits = 0;
  unsigned int pba = 0;
  volatile sBSP430hplPORT_IE_8 * hpl;

  if (BSP430_PERIPH_LFXT1 == device) {
    /* XIN/XOUT are dedicated pins on this device. */
    return 0;
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    bits = BIT1 | BIT4 | BIT5;
    pba = BSP430_PERIPH_PORT1_BASEADDRESS_;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_HPL_USCI_A0 - 0
  else if (BSP430_PERIPH_USCI_A0 == device) {
    bits = BIT4 | BIT5;
    pba = BSP430_PERIPH_PORT2_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI_A0 */
#if configBSP430_HPL_USCI_B0 - 0
  else if (BSP430_PERIPH_USCI_B0 == device) {
    bits = BIT0 | BIT1 | BIT2 | BIT3;
    pba = BSP430_PERIPH_PORT2_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI_B0 */
  if (0 == pba) {
    return -1;
  }
  hpl = (volatile sBSP430hplPORT_IE_8 *)pba;
  if (enablep) {
    hpl->sel |= bits;
  } else {
    hpl->out &= ~bits;
    hpl->dir |= bits;
    hpl->sel &= ~bits;
  }
  return 0;
}

const char *
xBSP430platformPeripheralHelp (tBSP430periphHandle device)
{
  if (BSP430_PERIPH_LFXT1 == device) {
    return "Dedicated";
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    return "H2 below FG4618 JTAG: MCLK on H2.2, SMCLK on H2.5, ACLK on H2.6";
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_HPL_USCI_A0 - 0
  if (BSP430_PERIPH_USCI_A0 == device) {
    return "MOSI/TXD=P2.4; MISO/RXD=P2.5";
  }
#endif /* configBSP430_HPL_USCI_A0 */
#if configBSP430_HPL_USCI_B0 - 0
  else if (BSP430_PERIPH_USCI_B0 == device) {
    return "STE=P3.0; MOSI/SDA=P3.1; MISO/SCL=P3.2; CLK=P3.3";
  }
#endif /* configBSP430_HPL_USCI_B0 */
  return NULL;
}

void vBSP430platformInitialize_ni (void)
{
#if BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG - 0
  /* Hold off watchdog */
  WDTCTL = WDTPW | WDTHOLD;
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

#if (BSP430_PLATFORM_BOOT_CONFIGURE_LEDS - 0) && (BSP430_LED - 0)
  vBSP430ledInitialize_ni();
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LEDS */

#if BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 - 0
  /* Enable XT1 functions and clock */
  (void)iBSP430clockConfigureLFXT1_ni(1, (BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC * BSP430_CLOCK_PUC_MCLK_HZ) / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 */

#if BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS - 0
  ulBSP430clockConfigureMCLK_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  iBSP430clockConfigureSMCLKDividingShift_ni(BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT);
#if configBSP430_CORE_DISABLE_FLL - 0
  __bis_status_register(SCG0);
#endif /* configBSP430_CORE_DISABLE_FLL */
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS */

#if BSP430_UPTIME - 0
  vBSP430uptimeStart_ni();
#endif /* BSP430_UPTIME */
}
