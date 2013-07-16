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

#include <bsp430/periph/bc2.h>
#include <bsp430/periph/usci.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/port.h>
#include <bsp430/platform/rf2500t/platform.h>

/* Ensure CC2500 CSn is held high to avoid issues with use of I2C. */
#define BSP430_PLATFORM_STANDARD_INITIALIZE_EPILOGUE do {       \
    P3DIR |= BIT0;                                              \
    P3OUT |= BIT0;                                              \
  } while (0)

#include <bsp430/platform/standard.inc>

#if (BSP430_LED - 0)
const sBSP430halLED xBSP430halLED_[] = {
  { .outp = &P1OUT, .bit = BIT0 }, /* Red */
  { .outp = &P1OUT, .bit = BIT1 }, /* Green */
};
const unsigned char nBSP430led = sizeof(xBSP430halLED_) / sizeof(*xBSP430halLED_);
#endif /* BSP430_LED */

int
iBSP430platformConfigurePeripheralPins_ni (tBSP430periphHandle device,
                                           int periph_config,
                                           int enablep)
{
  unsigned int bits;
  volatile sBSP430hplPORT_IE_8 * hplie = NULL;
  volatile sBSP430hplPORT_8 * hpl = NULL;

  /* This platform does not support a crystal */
  if (0) {
  }
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    bits = BIT0 | BIT1;
    hplie = (volatile sBSP430hplPORT_IE_8 *)BSP430_PERIPH_PORT2_BASEADDRESS_;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_USCI_A0 - 0)
  else if (BSP430_PERIPH_USCI_A0 == device) {
    bits = BIT4 | BIT5;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits |= BIT0;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits |= BIT3;
      }
    }
#if (BSP430_PORT_SUPPORTS_REN - 0)
    if (enablep) {
      P3REN &= ~bits;
    }
#endif /* BSP430_PORT_SUPPORTS_REN */
    hpl = (volatile sBSP430hplPORT_8 *)BSP430_PERIPH_PORT3_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI_A0 */
#if (configBSP430_HPL_USCI_B0 - 0)
  else if (BSP430_PERIPH_USCI_B0 == device) {
    bits = BIT1 | BIT2;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits |= BIT3;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits |= BIT0;
      }
    }
#if (BSP430_PORT_SUPPORTS_REN - 0)
    if (enablep) {
      P3REN &= ~bits;
    }
#endif /* BSP430_PORT_SUPPORTS_REN */
    hpl = (volatile sBSP430hplPORT_8 *)BSP430_PERIPH_PORT3_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI_B0 */
  if (NULL != hplie) {
    if (enablep) {
      hplie->ren &= ~bits;
      hplie->sel |= bits;
    } else {
      hplie->out &= ~bits;
      hplie->dir |= bits;
      hplie->sel &= ~bits;
    }
    return 0;
  }
  if (NULL != hpl) {
    if (enablep) {
      hpl->sel |= bits;
    } else {
      hpl->out &= ~bits;
      hpl->dir |= bits;
      hpl->sel &= ~bits;
    }
    return 0;
  }
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
    return "SMCLK on P2.1 (P4); ACLK on P2.0 (P3)";
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_USCI_A0 - 0)
  if (BSP430_PERIPH_USCI_A0 == device) {
    return "MOSI/TXD=P3.4; MISO/RXD=P3.5; CLK=P3.0; STE=P3.3";
  }
#endif /* configBSP430_HPL_USCI_A0 */
#if (configBSP430_HPL_USCI_B0 - 0)
  if (BSP430_PERIPH_USCI_B0 == device) {
    return "MOSI/SDA=P3.1; MISO/SCL=P3.2; CLK=P3.3; STE=P3.0";
  }
#endif /* configBSP430_HPL_USCI_B0 */
  return NULL;
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
    P1OUT ^= BIT0 | BIT1;
  }
  /* Restore P1.2 */
  P1OUT &= ~(BIT0 | BIT1 | BIT2);
  P1DIR |= BIT2;
  P1REN &= ~BIT2;
}
