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

#include <bsp430/periph/cs4.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/eusci.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/platform/exp430fr4133/platform.h>

#include <bsp430/platform/standard.inc>

#if (BSP430_LED - 0)
const sBSP430halLED xBSP430halLED_[] = {
  { .outp = &P1OUT, .bit = BIT0 }, /* Red (jumper) */
  { .outp = &P4OUT, .bit = BIT0 }, /* Green (direct) */
};
const unsigned char nBSP430led = sizeof(xBSP430halLED_) / sizeof(*xBSP430halLED_);
#endif /* BSP430_LED */

int
iBSP430platformConfigurePeripheralPins_ni (tBSP430periphHandle device,
                                           int periph_config,
                                           int enablep)
{
  if (BSP430_PERIPH_LFXT1 == device) {
    unsigned char bits4 = BIT1 | BIT2;
    if (enablep) {
      P4REN &= ~bits4;
      P4SEL0 |= bits4;
    } else {
      P4OUT &= ~bits4;
      P4DIR |= bits4;
      P4SEL0 &= ~bits4;
    }
    return 0;
  }
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    /* SMCLK on P8.0, MCLK on P1.4 ACLK on P8.1 */
    unsigned char bits8 = BIT0 | BIT1;
    unsigned char bits1 = BIT4;
    P8REN &= ~bits8;
    P1REN &= ~bits1;
    if (enablep) {
      P8DIR |= bits8;
      P8SEL0 |= bits8;
      P1DIR |= bits1;
      P1SEL0 |= bits1;
    } else {
      P8SEL0 &= ~bits8;
      P8DIR &= ~bits8;
    }
    return 0;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_EUSCI_A0 - 0)
  else if (BSP430_PERIPH_EUSCI_A0 == device) {
    unsigned char bits1 = BIT0 | BIT1;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits1 |= BIT2;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits1 |= BIT3;
      }
    }
    P1REN &= ~bits1;
    if (enablep) {
      P1SEL0 |= bits1;
    } else {
      P1SEL0 &= ~bits1;
      P1DIR &= ~bits1;
    }
    return 0;
  }
#endif /* configBSP430_HPL_EUSCI_A0 */
#if (configBSP430_HPL_EUSCI_B0 - 0)
  else if (BSP430_PERIPH_EUSCI_B0 == device) {
    unsigned char bits5 = BIT2 | BIT3;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits5 |= BIT1;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits5 |= BIT0;
      }
    }
    P5REN &= ~bits5;
    if (enablep) {
      P5SEL0 |= bits5;
    } else {
      P5SEL0 &= ~bits5;
      P5DIR &= ~bits5;
    }
    return 0;
  }
#endif /* configBSP430_HPL_EUSCI_B0 */
  return 0;
}

const char *
xBSP430platformPeripheralHelp (tBSP430periphHandle device,
                               int periph_config)
{
  if (BSP430_PERIPH_LFXT1 == device) {
    return "XIN=PJ.4, XOUT=PJ.5";
  }
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    return "SMCLK on P8.0, MCLK on P1.4 ACLK on P8.1";
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_EUSCI_A0 - 0)
  if (BSP430_PERIPH_EUSCI_A0 == device) {
    return "MOSI/TXD=P1.0; MISO/RXD=P1.1; CLK=P1.2; STE=P1.3";
  }
#endif /* configBSP430_HPL_EUSCI_A0 */
#if (configBSP430_HPL_EUSCI_B0 - 0)
  if (BSP430_PERIPH_EUSCI_B0 == device) {
    return "MOSI/SDA=P5.2; MISO/SCL=P5.3; CLK=P5.1; STE=P5.0";
  }
#endif /* configBSP430_HPL_EUSCI_B0 */
  return NULL;
}
