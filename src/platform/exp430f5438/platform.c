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
#include <bsp430/platform/exp430f5438/platform.h>

#include <bsp430/platform/standard.inc>

#if (BSP430_LED - 0)
const sBSP430halLED xBSP430halLED_[] = {
  { .outp = &P1OUT, .bit = BIT0 }, /* Red */
  { .outp = &P1OUT, .bit = BIT1 }, /* Orange */
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
    bits = BIT0 | BIT1;
    pba = BSP430_PERIPH_PORT7_BASEADDRESS_;
  }
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  else if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    bits = BIT0 | BIT1 | BIT2;
    pba = BSP430_PERIPH_PORT11_BASEADDRESS_;
    /* SEL is not sufficient to bring out signal; need DIR */
    P11DIR |= bits;
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_USCI5_A0 - 0)
  else if (BSP430_PERIPH_USCI5_A0 == device) {
    bits = BIT4 | BIT5;
    if (BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config) {
      bits |= BIT0;             /* UCA0CLK */
    } else if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
      bits |= BIT0 | BIT3;      /* UCA0CLK + UCA0STE */
    }
    pba = BSP430_PERIPH_PORT3_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A0 */
#if (configBSP430_HPL_USCI5_A1 - 0)
  else if (BSP430_PERIPH_USCI5_A1 == device) {
    bits = BIT6 | BIT7;
    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      /* UCA1CLK is on P3.6 */
      if (enablep) {
        P3SEL |= BIT6;
      } else {
        P3OUT &= ~BIT6;
        P3DIR |= BIT6;
        P3SEL &= ~BIT6;
      }
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits |= BIT4;      /* UCA1STE */
      }
    }
    pba = BSP430_PERIPH_PORT5_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A1 */
#if (configBSP430_HPL_USCI5_A2 - 0)
  else if (BSP430_PERIPH_USCI5_A2 == device) {
    bits = BIT4 | BIT5;
    if (BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config) {
      bits |= BIT0;             /* UCA2CLK */
    } else if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
      bits |= BIT0 | BIT3;      /* UCA2CLK + UCA2STE */
    }
    pba = BSP430_PERIPH_PORT9_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A2 */
#if (configBSP430_HPL_USCI5_A3 - 0)
  else if (BSP430_PERIPH_USCI5_A3 == device) {
    bits = BIT4 | BIT5;
    if (BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config) {
      bits |= BIT0;             /* UCA3CLK */
    } else if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
      bits |= BIT0 | BIT3;      /* UCA3CLK + UCA3STE */
    }
    pba = BSP430_PERIPH_PORT10_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_A3 */
#if (configBSP430_HPL_USCI5_B0 - 0)
  else if (BSP430_PERIPH_USCI5_B0 == device) {
    bits = BIT1 | BIT2;
    if (BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config) {
      bits |= BIT3;             /* UCB0CLK */
    } else if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
      bits |= BIT0 | BIT3;      /* UCB0STE + UCB0CLK */
    }
    pba = BSP430_PERIPH_PORT3_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_B0 */
#if (configBSP430_HPL_USCI5_B1 - 0)
  else if (BSP430_PERIPH_USCI5_B1 == device) {
    /* MISO(SCL): P5.4; CLK: P5.5; MOSI(SDA): P3.7; STE: P3.6 */
    unsigned char bits3 = BIT7;
    unsigned char bits5 = BIT4;

    if ((BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config)
        || (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config)) {
      bits5 |= BIT5;
      if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
        bits3 |= BIT6;
      }
    }
    if (enablep) {
      P3SEL |= bits3;
      P5SEL |= bits5;
    } else {
      P3OUT &= ~bits3;
      P5OUT &= ~bits5;
      P3DIR |= bits3;
      P5DIR |= bits5;
      P3SEL &= ~bits3;
      P5SEL &= ~bits5;
    }
    return 0;
  }
#endif /* configBSP430_HPL_USCI5_B1 */
#if (configBSP430_HPL_USCI5_B2 - 0)
  else if (BSP430_PERIPH_USCI5_B2 == device) {
    bits = BIT1 | BIT2;
    if (BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config) {
      bits |= BIT3;             /* UCB2CLK */
    } else if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
      bits |= BIT0 | BIT3;      /* UCB2STE + UCB2CLK */
    }
    pba = BSP430_PERIPH_PORT9_BASEADDRESS_;
  }
#endif /* configBSP430_HPL_USCI5_B2 */
#if (configBSP430_HPL_USCI5_B3 - 0)
  else if (BSP430_PERIPH_USCI5_B3 == device) {
    bits = BIT1 | BIT2;
    if (BSP430_PERIPHCFG_SERIAL_SPI3 == periph_config) {
      bits |= BIT3;             /* UCB3CLK */
    } else if (BSP430_PERIPHCFG_SERIAL_SPI4 == periph_config) {
      bits |= BIT0 | BIT3;      /* UCB3STE + UCB3CLK */
    }
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
xBSP430platformPeripheralHelp (tBSP430periphHandle device,
                               int periph_config)
{
  if (BSP430_PERIPH_LFXT1 == device) {
    return "XIN=P7.0, XOUT=P7.1";
  }
#if (configBSP430_PERIPH_EXPOSED_CLOCKS - 0)
  if (BSP430_PERIPH_EXPOSED_CLOCKS == device) {
    return "Labelled test points below JTAG header";
  }
#endif /* configBSP430_PERIPH_EXPOSED_CLOCKS */
#if (configBSP430_HPL_USCI5_A0 - 0)
  if (BSP430_PERIPH_USCI5_A0 == device) {
    return "MOSI/TXD=P3.4; MISO/RXD=P3.5; CLK=P3.0; STE=P3.3";
  }
#endif /* configBSP430_HPL_USCI5_A0 */
#if (configBSP430_HPL_USCI5_A1 - 0)
  if (BSP430_PERIPH_USCI5_A1 == device) {
    return "MOSI/TXD=P5.6; MISO/RXD=P5.7; CLK=P3.6; STE=P5.4";
  }
#endif /* configBSP430_HPL_USCI5_A1 */
#if (configBSP430_HPL_USCI5_A2 - 0)
  if (BSP430_PERIPH_USCI5_A2 == device) {
    return "MOSI/TXD=P9.4; MISO/RXD=P9.5; CLK=P9.0; STE=P9.3";
  }
#endif /* configBSP430_HPL_USCI5_A2 */
#if (configBSP430_HPL_USCI5_A3 - 0)
  if (BSP430_PERIPH_USCI5_A3 == device) {
    return "MOSI/TXD=P10.4; MISO/RXD=P10.5; CLK=P10.0; STE=P10.3";
  }
#endif /* configBSP430_HPL_USCI5_A3 */
#if (configBSP430_HPL_USCI5_B0 - 0)
  if (BSP430_PERIPH_USCI5_B0 == device) {
    return "MOSI/SDA=P3.1; MISO/SCL=P3.2; CLK=P3.3; STE=P3.0";
  }
#endif /* configBSP430_HPL_USCI5_B0 */
#if (configBSP430_HPL_USCI5_B1 - 0)
  if (BSP430_PERIPH_USCI5_B1 == device) {
    return "MOSI/SDA=P3.7; MISO/SCL=P5.4; CLK=P5.5; STE=P3.6";
  }
#endif /* configBSP430_HPL_USCI5_B1 */
#if (configBSP430_HPL_USCI5_B2 - 0)
  if (BSP430_PERIPH_USCI5_B2 == device) {
    return "MOSI/SDA=P9.1; MISO/SCL=P9.2; CLK=P9.3; STE=P9.0";
  }
#endif /* configBSP430_HPL_USCI5_B2 */
#if (configBSP430_HPL_USCI5_B3 - 0)
  else if (BSP430_PERIPH_USCI5_B3 == device) {
    return "MOSI/SDA=P10.1; MISO/SCL=P10.2; CLK=P10.3; STE=P10.0";
  }
#endif /* configBSP430_HPL_USCI5_B3 */
  return NULL;
}
