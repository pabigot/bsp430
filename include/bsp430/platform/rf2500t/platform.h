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

/** @file
 *
 * @brief Platform-specific include for <a href="http://www.ti.com/tool/ez430-rf2500t">EZ430-RF2500T</a>
 *
 * The following platform-specific features are supported: <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni The "jumper pair" for this
 * platform is the button.  Hold it down.
 *
 * <li> #BSP430_CONSOLE_BAUD_RATE defaults to 2400 for this
 * platform, since no crystal is available and the DCO accuracy is far
 * enough off to disrupt reliable communications with the Linux
 * CDC/ACM driver.
 *
 * </ul>
 *
 * @note The RF2500T has a CC2500 radio built-in, with its chip select
 * on P3.0.  At power-up, the port registers may be set so that the
 * CC2500 is receiving a low signal on that line, causing it to
 * interfere with pull-ups on I2C devices that use USCI_B0.  This
 * platform reconfigures P3.0 to output high during
 * vBSP430platformInitialize_ni().
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_RF2500T_PLATFORM_H
#define BSP430_PLATFORM_RF2500T_PLATFORM_H

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_RF2500T 1

/** @cond DOXYGEN_EXCLUDE */

/** ACM interface on Linux through eZ430 poor */
#ifndef BSP430_CONSOLE_BAUD_RATE
#define BSP430_CONSOLE_BAUD_RATE 2400
#endif /* BSP430_CONSOLE_BAUD_RATE */

/* Enable if requested */
#if (configBSP430_PLATFORM_SPIN_FOR_JUMPER - 0)
#define BSP430_PLATFORM_SPIN_FOR_JUMPER 1
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

/* Available button definitions */
#define BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_PLATFORM_BUTTON0_PORT_BIT BIT2

/* Standard LED colors */
#define BSP430_LED_RED 0
#define BSP430_LED_GREEN 1

/* How to use ACLK as a capture/compare input source */
/* Settings for TB0: T0B2 ccis=1 ; clk P4.7 ; cc0 P4.3 (CCI0B) ; cc1 P4.1 */
#ifndef BSP430_TIMER_CCACLK_ACLK_CCIDX
/* NB: Check against BSP430_TIMER_CCACLK_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_ACLK_CCIDX 2
#endif /* BSP430_TIMER_CCACLK_ACLK_CCIDX */
#ifndef BSP430_TIMER_CCACLK_ACLK_CCIS
/* NB: Check against BSP430_TIMER_CCACLK_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_ACLK_CCIS CCIS_1
#endif /* BSP430_TIMER_CCACLK_ACLK_CCIS */
#ifndef BSP430_TIMER_CCACLK_CLK_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT7
#endif /* BSP430_TIMER_CCACLK_CLK_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC0_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC0_PORT_BIT BIT3
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC0_CCIS
#define BSP430_TIMER_CCACLK_CC0_CCIS CCIS_1
#endif /* BSP430_TIMER_CCACLK_CC0_CCIS */
#ifndef BSP430_TIMER_CCACLK_CC1_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC1_PORT_BIT BIT1
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_BIT */

/* !BSP430! insert=emk_platform emk=rf2500t mcu=msp430f2274 spi=UCB0SOMI tag=cc110x gpio=GDO0,GDO1,GDO2 */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [emk_platform] */
#if (configBSP430_RF_RF2500T - 0)
#define BSP430_RF_CC110X 1
#define BSP430_RF_CC110X_SPI_PERIPH_HANDLE BSP430_PERIPH_USCI_B0
#define BSP430_RF_CC110X_GDO0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_RF_CC110X_GDO0_PORT_BIT BIT6
#define BSP430_RF_CC110X_GDO1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RF_CC110X_GDO1_PORT_BIT BIT2
#define BSP430_RF_CC110X_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_RF_CC110X_GDO2_PORT_BIT BIT7
#define BSP430_RF_CC110X_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RF_CC110X_CSn_PORT_BIT BIT0
#endif /* configBSP430_RF_RF2500T */
/* END AUTOMATICALLY GENERATED CODE [emk_platform] */
/* !BSP430! end=emk_platform */

/** @endcond */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_RF2500T_H */
