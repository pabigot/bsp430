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

#include <bsp430/periph/ucs.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/usci5.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/port.h>
#include <bsp430/platform/em430/platform.h>

#include <bsp430/platform/standard.inc>

#if BSP430_LED - 0
const sBSP430halLED xBSP430halLED_[] = {
  { .outp = &P1OUT, .bit = BIT0 }, /* Green */
  { .outp = &P3OUT, .bit = BIT6 }, /* Red */
};
const unsigned char nBSP430led = sizeof(xBSP430halLED_) / sizeof(*xBSP430halLED_);
#endif /* BSP430_LED */

int
iBSP430platformConfigurePeripheralPins_ni (tBSP430periphHandle device,
                                           int periph_config,
                                           int enablep)
{
  unsigned char bits = 0;
  unsigned int pba = 0;
  volatile sBSP430hplPORT_5XX_8 * hpl;

  if (BSP430_PERIPH_LFXT1 == device) {
    bits = BIT0 | BIT1;
    pba = BSP430_PERIPH_PORT5_BASEADDRESS_;
  }
#if (configBSP430_PERIPH_XT2 - 0)
  else if (BSP430_PERIPH_XT2 == device) {
    /* No configuration necessary; RF_XIN and RF_XOUT are dedicated pins */
    return 0;
  }
#endif /* configBSP430_PERIPH_XT2 */
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    /* Note: The datasheet is incorrect in its claim that PxDIR is
     * don't care: it must be set to output for the clocks to appear.
     * (At least on my board, which might have experimental
     * silicon.) */
    if (enablep) {
      P3DIR |= BIT7;
      P2DIR |= BIT6;
      P3SEL |= BIT7;
      P2SEL |= BIT6;
    } else {
      P3OUT &= ~BIT7;
      P2OUT &= ~BIT6;
      P3DIR |= BIT7;
      P2DIR |= BIT6;
      P3SEL &= ~BIT7;
      P2SEL &= ~BIT6;
    }
    return 0;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_HPL_USCI5_A0 - 0
  else if (BSP430_PERIPH_USCI5_A0 == device) {
    bits = BIT5 | BIT6;
    if (BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config) {
      bits |= BIT7;             /* UCA0CLK */
    } else if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
      bits |= BIT4 | BIT7;      /* UCA0CLK + UCA0STE */
    }
    pba = BSP430_PERIPH_PORT1_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A0 */
#if configBSP430_HPL_USCI5_B0 - 0
  else if (BSP430_PERIPH_USCI5_B0 == device) {
    bits = BIT2 | BIT3;
    if (BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config) {
      bits |= BIT4;             /* UCB0CLK */
    } else if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
      bits |= BIT4 | BIT7;      /* UCB0STE + UCB0CLK */
    }
    pba = BSP430_PERIPH_PORT1_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_B0 */
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
xBSP430platformPeripheralHelp (tBSP430periphHandle device,
                               int periph_config)
{
  if (BSP430_PERIPH_LFXT1 == device) {
    return "XIN=P5.0, XOUT=P5.1";
  }
#if configBSP430_PERIPH_EXPOSED_CLOCKS - 0
  if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    return "SMCLK=P3.7; ACLK=P2.6";
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if configBSP430_HPL_USCI5_A0 - 0
  if (BSP430_PERIPH_USCI5_A0 == device) {
    return "MOSI/TXD=P1.6; MISO/RXD=P1.5; CLK=P1.7; STE=P1.4";
  }
#endif /* configBSP430_HPL_USCI5_A0 */
#if configBSP430_HPL_USCI5_B0 - 0
  if (BSP430_PERIPH_USCI5_B0 == device) {
    return "MOSI/SDA=P1.3; MISO/SCL=P1.2; CLK=P1.4; STE=P1.7";
  }
#endif /* configBSP430_HPL_USCI5_B0 */
  return NULL;
}
