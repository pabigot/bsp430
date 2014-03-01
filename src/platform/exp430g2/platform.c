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

#include <bsp430/periph/bc2.h>
#include <bsp430/periph/usci.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/led.h>
#include <bsp430/platform/exp430g2/platform.h>

#include <bsp430/platform/standard.inc>

#if (BSP430_LED - 0)
const sBSP430halLED xBSP430halLED_[] = {
  { .outp = &P1OUT, .bit = BIT0 }, /* Red */
  { .outp = &P1OUT, .bit = BIT6 }, /* Green */
};
const unsigned char nBSP430led = sizeof(xBSP430halLED_) / sizeof(*xBSP430halLED_);
#endif /* BSP430_LED */

int
iBSP430platformConfigurePeripheralPins_ni (tBSP430periphHandle device,
                                           int periph_config,
                                           int enablep)
{
  unsigned int bits;

  if (BSP430_PERIPH_LFXT1 == device) {
    bits = BIT6 | BIT7;
    P2SEL2 &= ~bits;
    if (enablep) {
      P2REN &= ~bits;
      P2SEL |= bits;
    } else {
      P2SEL &= ~bits;
      P2DIR |= bits;
      P2OUT &= ~bits;
    }
    return 0;
  }
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    bits = BIT0 | BIT4;
    if (enablep) {
      P1REN &= ~bits;
      P1SEL |= bits;
    } else {
      P1SEL &= ~bits;
      P1OUT &= ~bits;
    }
    P1DIR |= bits;
    return 0;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_USCI_A0 - 0)
  else if (BSP430_PERIPH_USCI_A0 == device) {
    bits = BIT1 | BIT2;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits |= BIT4;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits |= BIT5;
      }
    }
    if (enablep) {
      P1REN &= ~bits;
      P1SEL |= bits;
      P1SEL2 |= bits;
    } else {
      P1SEL2 &= ~bits;
      P1SEL &= ~bits;
      P1DIR |= bits;
      P1OUT &= ~bits;
    }
    return 0;
  }
#endif /* configBSP430_HPL_USCI_A0 */
#if (configBSP430_HPL_USCI_B0 - 0)
  else if (BSP430_PERIPH_USCI_B0 == device) {
    bits = BIT6 | BIT7;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits |= BIT5;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits |= BIT4;
      }
    }
    if (enablep) {
      P1REN &= ~bits;
      P1SEL |= bits;
      P1SEL2 |= bits;
    } else {
      P1SEL2 &= ~bits;
      P1SEL &= ~bits;
      P1DIR |= bits;
      P1OUT &= ~bits;
    }
    return 0;
  }
#endif /* configBSP430_HPL_USCI_B0 */
  return -1;
}

const char *
xBSP430platformPeripheralHelp (tBSP430periphHandle device,
                               int periph_config)
{
  if (BSP430_PERIPH_LFXT1 == device) {
    return "XIN=P2.6; XOUT=P2.7";
  }
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    return "SMCLK on P1.4; ACLK on P1.0 (red LED)";
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_USCI_A0 - 0)
  if (BSP430_PERIPH_USCI_A0 == device) {
    return "MOSI/TXD=P1.2; MISO/RXD=P1.1; CLK=P1.4; STE=P1.5";
  }
#endif /* configBSP430_HPL_USCI_A0 */
#if (configBSP430_HPL_USCI_B0 - 0)
  if (BSP430_PERIPH_USCI_B0 == device) {
    /* Yes, MISO/MOSI are a different order than other chips */
    return "MOSI/SDA=P1.7; MISO/SCL=P1.6; CLK=P1.5; STE=P1.4";
  }
#endif /* configBSP430_HPL_USCI_B0 */
#if (configBSP430_HPL_TA0 - 0)
  if (BSP430_PERIPH_TA0 == device) {
    return "INCLK=P1.0";
  }
#endif /* configBSP430_HPL_TA0 */
  return NULL;
}

void
vBSP430platformSpinForJumper_ni (void)
{
  /* P1.4 output low, P1.5 configured with pullup */
  P1DIR |= BIT4;
  P1OUT &= ~BIT4;
  P1DIR &= ~BIT5;
  P1REN |= BIT5;
  P1OUT |= BIT5;

  /* Flash LEDs alternately while waiting */
#if BSP430_LED
  vBSP430ledInitialize_ni();
#else /* BSP430_LED */
  P1DIR |= BIT0 | BIT6;
  P1SEL &= ~(BIT0 | BIT6);
#endif /* BSP430_LED */
  P1OUT |= BIT0;
  while (! (P1IN & BIT5)) {
    BSP430_CORE_WATCHDOG_CLEAR();
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 10);
    P1OUT ^= BIT0 | BIT6;
  }

  /* Restore P1.4, P1.5, and LEDs */
  P1OUT &= ~(BIT0 | BIT4 | BIT5 | BIT6);
  P1DIR |= BIT5;
  P1REN &= ~BIT5;
}
