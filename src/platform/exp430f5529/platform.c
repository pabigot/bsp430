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

#include <bsp430/periph/ucs.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/usci5.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/periph/port.h>
#include <bsp430/platform/exp430f5529/platform.h>

#include <bsp430/platform/standard.inc>

#if (BSP430_LED - 0)
const sBSP430halLED xBSP430halLED_[] = {
  { .outp = &P1OUT, .bit = BIT0 }, /* Red */
  { .outp = &P8OUT, .bit = BIT1 }, /* Orange */
  { .outp = &P8OUT, .bit = BIT2 }, /* Green */
  { .outp = &P1OUT, .bit = BIT1 }, /* Blue */
  { .outp = &P1OUT, .bit = BIT2 }, /* Blue */
  { .outp = &P1OUT, .bit = BIT3 }, /* Blue */
  { .outp = &P1OUT, .bit = BIT4 }, /* Blue */
  { .outp = &P1OUT, .bit = BIT5 }, /* Blue */
};
const unsigned char nBSP430led = sizeof(xBSP430halLED_) / sizeof(*xBSP430halLED_);
#endif /* BSP430_LED */

int
iBSP430platformConfigurePeripheralPins_ni (tBSP430periphHandle device,
                                           int periph_config,
                                           int enablep)
{
  unsigned char bits = 0;
  uintptr_t pba = 0;
  volatile sBSP430hplPORT_5XX_8 * hpl;

  if (BSP430_PERIPH_LFXT1 == device) {
    /* Setting P5.4 is sufficient for both XIN and XOUT */
    bits = BIT4;
    pba = BSP430_PERIPH_PORT5_BASEADDRESS_;
  }
#if (configBSP430_PERIPH_XT2 - 0)
  else if (BSP430_PERIPH_XT2 == device) {
    /* Setting P5.2 is sufficient for both XT2IN and XT2OUT */
    bits = BIT2;
    pba = BSP430_PERIPH_PORT5_BASEADDRESS_;
  }
#endif /* configBSP430_PERIPH_XT2 */
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    /* P1.0 ACLK, P2.2 SMCLK, P7.7 MCLK */
    P1DIR |= BIT0;
    P2DIR |= BIT2;
    P7DIR |= BIT7;
    if (enablep) {
      P1REN &= ~BIT0;
      P1SEL |= BIT0;
      P2REN &= ~BIT2;
      P2SEL |= BIT2;
      P7REN &= ~BIT7;
      P7SEL |= BIT7;
    } else {
      P1OUT &= ~BIT0;
      P2OUT &= ~BIT2;
      P7OUT &= ~BIT7;
      P1SEL &= ~BIT0;
      P2SEL &= ~BIT2;
      P7SEL &= ~BIT7;
    }
    return 0;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_USCI5_A0 - 0)
  else if (BSP430_PERIPH_USCI5_A0 == device) {
    bits = BIT3 | BIT4;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      /* UCA0CLK is on P2.7 */
      if (enablep) {
        P2REN &= ~BIT7;
        P2SEL |= BIT7;
      } else {
        P2OUT &= ~BIT7;
        P2DIR |= BIT7;
        P2SEL &= ~BIT7;
      }
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        /* UCA0STE is on P3.2 */
        bits |= BIT2;
      }
    }
    pba = BSP430_PERIPH_PORT3_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A0 */
#if (configBSP430_HPL_USCI5_A1 - 0)
  else if (BSP430_PERIPH_USCI5_A1 == device) {
    /* NOTE: Default port-mapped */
    bits = BIT4 | BIT5;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits |= BIT0;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits |= BIT3;
      }
    }
    pba = BSP430_PERIPH_PORT4_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A1 */
#if (configBSP430_HPL_USCI5_B0 - 0)
  else if (BSP430_PERIPH_USCI5_B0 == device) {
    bits = BIT0 | BIT1;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits |= BIT2;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        /* UCB0STE is on P2.7 */
        if (enablep) {
          P2REN &= ~BIT7;
          P2SEL |= BIT7;
        } else {
          P2OUT &= ~BIT7;
          P2DIR |= BIT7;
          P2SEL &= ~BIT7;
        }
      }
    }
    pba = BSP430_PERIPH_PORT3_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_B0 */
#if (configBSP430_HPL_USCI5_B1 - 0)
  else if (BSP430_PERIPH_USCI5_B1 == device) {
    /* NOTE: Default port-mapped */
    bits = BIT1 | BIT2;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits |= BIT3;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits |= BIT0;
      }
    }
    pba = BSP430_PERIPH_PORT4_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_B1 */
  if (0 == pba) {
    return -1;
  }
  hpl = (volatile sBSP430hplPORT_5XX_8 *)pba;
  if (enablep) {
    hpl->ren &= ~bits;
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
    return "XIN=P5.4, XOUT=P5.5";
  }
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    return "ACLK on P1.0 (J12.1); MCLK on P7.7 (J5.2)";
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_USCI5_A0 - 0)
  if (BSP430_PERIPH_USCI5_A0 == device) {
    return "MOSI/TXD=P3.3; MISO/RXD=P3.4; CLK=P2.7; STE=P3.2";
  }
#endif /* configBSP430_HPL_USCI5_A0 */
#if (configBSP430_HPL_USCI5_A1 - 0)
  if (BSP430_PERIPH_USCI5_A1 == device) {
    return "MOSI/TXD=P4.4; MISO/RXD=P4.5; CLK=P4.0; STE=P4.3";
  }
#endif /* configBSP430_HPL_USCI5_A1 */
#if (configBSP430_HPL_USCI5_B0 - 0)
  if (BSP430_PERIPH_USCI5_B0 == device) {
    return "MOSI/SDA=P3.0; MISO/SCL=P3.1; CLK=P3.2; STE=P2.7";
  }
#endif /* configBSP430_HPL_USCI5_B0 */
#if (configBSP430_HPL_USCI5_B1 - 0)
  if (BSP430_PERIPH_USCI5_B1 == device) {
    return "MOSI/SDA=P4.1; MISO/SCL=P4.2; CLK=P4.3; STE=P4.0";
  }
#endif /* configBSP430_HPL_USCI5_B1 */
  return NULL;
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
