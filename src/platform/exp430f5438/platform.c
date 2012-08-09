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

#include <bsp430/platform/exp430f5438/platform.h>
#include <bsp430/periph/ucs.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/usci5.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/port.h>

#if BSP430_LED - 0
const sBSP430halLED xBSP430hal_[] = {
  { .outp = &P1OUT, .bit = BIT0 }, /* Red */
  { .outp = &P1OUT, .bit = BIT1 }, /* Orange */
};
const unsigned char nBSP430led = sizeof(xBSP430hal_) / sizeof(*xBSP430hal_);
#endif /* BSP430_LED */

int
iBSP430platformConfigurePeripheralPins_ni (tBSP430periphHandle device, int enablep)
{
  unsigned char bits = 0;
  unsigned int pba = 0;
  volatile sBSP430hplPORT_5XX_8 * hpl;

  if (BSP430_PERIPH_LFXT1 == device) {
    bits = BIT0 | BIT1;
    pba = BSP430_PERIPH_PORT7_BASEADDRESS_;
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    bits = BIT0 | BIT1 | BIT2;
    pba = BSP430_PERIPH_PORT11_BASEADDRESS_;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_HPL_USCI5_A0 - 0
  else if (BSP430_PERIPH_USCI5_A0 == device) {
    bits = BIT4 | BIT5;
    pba = BSP430_PERIPH_PORT3_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A0 */
#if configBSP430_HPL_USCI5_A1 - 0
  else if (BSP430_PERIPH_USCI5_A1 == device) {
    bits = BIT6 | BIT7;
    pba = BSP430_PERIPH_PORT5_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A1 */
#if configBSP430_HPL_USCI5_A2 - 0
  else if (BSP430_PERIPH_USCI5_A2 == device) {
    bits = BIT4 | BIT5;
    pba = BSP430_PERIPH_PORT9_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A2 */
#if configBSP430_HPL_USCI5_A3 - 0
  else if (BSP430_PERIPH_USCI5_A3 == device) {
    bits = BIT4 | BIT5;
    pba = BSP430_PERIPH_PORT10_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A3 */
#if configBSP430_HPL_USCI5_B0 - 0
  else if (BSP430_PERIPH_USCI5_B0 == device) {
    bits = BIT1 | BIT2 | BIT3;
    pba = BSP430_PERIPH_PORT3_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_B0 */
#if configBSP430_HPL_USCI5_B1 - 0
  else if (BSP430_PERIPH_USCI5_B1 == device) {
    bits = BIT6 | BIT7;
    pba = BSP430_PERIPH_PORT3_BASEADDRESS_;
    if (enablep) {
      P5SEL |= BIT4;
    } else {
      P5OUT &= ~BIT4;
      P5DIR |= BIT4;
      P5SEL &= ~BIT4;
    }
  }
#endif /* configBSP430_HPL_USCI5_B1 */
#if configBSP430_HPL_USCI5_B2 - 0
  else if (BSP430_PERIPH_USCI5_B2 == device) {
    bits = BIT1 | BIT2 | BIT3;
    pba = BSP430_PERIPH_PORT9_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_B2 */
#if configBSP430_HPL_USCI5_B3 - 0
  else if (BSP430_PERIPH_USCI5_B3 == device) {
    bits = BIT1 | BIT2 | BIT3;
    pba = BSP430_PERIPH_PORT10_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_B3 */
  if (0 == pba) {
    return -1;
  }
  hpl = (volatile sBSP430hplPORT_5XX_8 *)pba;
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
    return "XIN=P7.0, XOUT=P7.1";
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    return "Labelled test points below JTAG header";
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_HPL_USCI5_A0 - 0
  if (BSP430_PERIPH_USCI5_A0 == device) {
    return "MOSI/TXD=P3.4; MISO/RXD=P3.5";
  }
#endif /* configBSP430_HPL_USCI5_A0 */
#if configBSP430_HPL_USCI5_A1 - 0
  if (BSP430_PERIPH_USCI5_A1 == device) {
    return "MOSI/TXD=P5.6; MISO/RXD=P5.7";
  }
#endif /* configBSP430_HPL_USCI5_A1 */
#if configBSP430_HPL_USCI5_A2 - 0
  if (BSP430_PERIPH_USCI5_A2 == device) {
    return "MOSI/TXD=P9.4; MISO/RXD=P9.5";
  }
#endif /* configBSP430_HPL_USCI5_A2 */
#if configBSP430_HPL_USCI5_A3 - 0
  if (BSP430_PERIPH_USCI5_A3 == device) {
    return "MOSI/TXD=P10.4; MISO/RXD=P10.5";
  }
#endif /* configBSP430_HPL_USCI5_A3 */
#if configBSP430_HPL_USCI5_B0 - 0
  if (BSP430_PERIPH_USCI5_B0 == device) {
    return "MOSI/SDA=P3.1; MISO/SCL=P3.2; STE=P3.3";
  }
#endif /* configBSP430_HPL_USCI5_B0 */
#if configBSP430_HPL_USCI5_B1 - 0
  if (BSP430_PERIPH_USCI5_B1 == device) {
    return "MOSI/SDA=P3.7; MISO/SCL=P5.4; STE=P5.5";
  }
#endif /* configBSP430_HPL_USCI5_B1 */
#if configBSP430_HPL_USCI5_B2 - 0
  if (BSP430_PERIPH_USCI5_B2 == device) {
    return "MOSI/SDA=P9.1; MISO/SCL=P9.2; STE=P9.0";
  }
#endif /* configBSP430_HPL_USCI5_B2 */
#if configBSP430_HPL_USCI5_B3 - 0
  else if (BSP430_PERIPH_USCI5_B3 == device) {
    return "MOSI/SDA=P10.1; MISO/SCL=P10.2; STE=P10.0";
  }
#endif /* configBSP430_HPL_USCI5_B3 */
  return NULL;
}


void vBSP430platformInitialize_ni (void)
{
  int crystal_ok = 0;
  (void)crystal_ok;

#if ! (configBSP430_CORE_SUPPORT_WATCHDOG - 0)
  /* Hold off watchdog */
  WDTCTL = WDTPW + WDTHOLD;
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

#if (BSP430_PLATFORM_BOOT_CONFIGURE_LED - 0) && (BSP430_LED - 0)
  vBSP430ledInitialize_ni();
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LED */

#if BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 - 0
  /* Enable XT1 functions and clock */
  crystal_ok = iBSP430clockConfigureLFXT1_ni(1, (BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC * BSP430_CLOCK_PUC_MCLK_HZ) / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 */

#if BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS - 0
  iBSP430clockConfigureACLK_ni(BSP430_PLATFORM_BOOT_ACLKSRC);
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
