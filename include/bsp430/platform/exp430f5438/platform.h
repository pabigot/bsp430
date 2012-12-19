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

#ifndef BSP430_PLATFORM_EXP430F5438_PLATFORM_H
#define BSP430_PLATFORM_EXP430F5438_PLATFORM_H

/** @file
 *
 * @brief Platform-specific include for <a href="http://www.ti.com/tool/msp-exp430f5438">MSP-EXP430F5438</a>
 *
 * The following platform-specific features are supported:
 * <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni is not implemented on this
 * platform.
 *
 * <li> #BSP430_PERIPH_EXPOSED_CLOCKS Clocks are made available at
 * dedicated labelled test points below the JTAG header.
 *
 * </ul>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_EXP430F5438 1

/* Available button definitions */
#define BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON0_PORT_BIT BIT6
#define BSP430_PLATFORM_BUTTON1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON1_PORT_BIT BIT7

/* Standard LED colors.  In the absense of a green LED, the orange
 * will substitute. */
#define BSP430_LED_RED 0
#define BSP430_LED_ORANGE 1
#define BSP430_LED_GREEN BSP430_LED_ORANGE

/* How to use ACLK as a capture/compare input source */
/* Settings for TB0: T0B6 ccis=1 ; clk P4.7 ; cc0 P4.0 ; cc1 P4.1 */
#ifndef BSP430_TIMER_CCACLK_ACLK_CC
/* NB: Check against BSP430_TIMER_CCACLK_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_ACLK_CC 6
#endif /* BSP430_TIMER_CCACLK_ACLK_CC */
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
#define BSP430_TIMER_CCACLK_CC0_PORT_BIT BIT0
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC1_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC1_PORT_BIT BIT1
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_BIT */

/* RFEM capabilities */
#if (configBSP430_RFEM - 0)
#define BSP430_RFEM 1
#define BSP430_RFEM_SPI0_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0
/* GDO0: P1.7, not available as CC input */
#define BSP430_RFEM_GDO0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_GDO0_PORT_BIT BIT7
/* GDO1: P3.2, not available as CC input */
#define BSP430_RFEM_GDO1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_GDO1_PORT_BIT BIT2
/* GDO2: P1.3, CCI2A on TA0.2 */
#define BSP430_RFEM_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_GDO2_PORT_BIT BIT3
#define BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_SPI0CSn_PORT_BIT BIT0
#endif /* configBSP430_RFEM */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_EXP430F5438_H */
