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

/** @file
 *
 * @brief Platform-specific include for <a href="http://www.ti.com/tool/msp-exp430fr5739">MSP-EXP430FR5739 ("Fraunchpad")</a>
 *
 * This board is also known as the "Fraunchpad"
 *
 * The following platform-specific features are supported: <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni The jumper for this platform
 * is P4.0, located on the right of the board at the top.  Place the
 * jumper between P4.0 and GND.
 *
 * <li> #BSP430_PERIPH_EXPOSED_CLOCKS Clocks share LED lines, and can
 * be measured at the test points below the LEDs.  Proceeding from the
 * rightmost LED: TP10 provides SMCLK, TP11 provides MCLK, and TP12
 * provides ACLK.
 *
 * </ul>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */
#ifndef BSP430_PLATFORM_EXP430FR5739_PLATFORM_H
#define BSP430_PLATFORM_EXP430FR5739_PLATFORM_H

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_EXP430FR5739 1

/** @cond DOXYGEN_EXCLUDE */

/* Enable if requested */
#if configBSP430_PLATFORM_SPIN_FOR_JUMPER - 0
#define BSP430_PLATFORM_SPIN_FOR_JUMPER 1
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

/* Available button definitions */
#define BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4
#define BSP430_PLATFORM_BUTTON0_PORT_BIT BIT0
#define BSP430_PLATFORM_BUTTON1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4
#define BSP430_PLATFORM_BUTTON1_PORT_BIT BIT1

/* Standard LED colors.  Board has nothing but eight blue LEDs, so
 * assign the first two to the red and green functions,
 * respectively. */
#define BSP430_LED_RED 0
#define BSP430_LED_GREEN 1
#define BSP430_LED_BLUE 2
#define BSP430_LED_BLUE1 3
#define BSP430_LED_BLUE2 4
#define BSP430_LED_BLUE3 5
#define BSP430_LED_BLUE4 6
#define BSP430_LED_BLUE5 7

/* How to use ACLK as a capture/compare input source */
/* Settings for TA1: T1A2 ccis=1 ; clk P1.1 ; cc0 P1.7 ; cc1 P1.2 */
#ifndef BSP430_TIMER_CCACLK_ACLK_CC
/* NB: Check against BSP430_TIMER_CCACLK_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_ACLK_CC 2
#endif /* BSP430_TIMER_CCACLK_ACLK_CC */
#ifndef BSP430_TIMER_CCACLK_ACLK_CCIS
/* NB: Check against BSP430_TIMER_CCACLK_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_ACLK_CCIS CCIS_1
#endif /* BSP430_TIMER_CCACLK_ACLK_CCIS */
#ifndef BSP430_TIMER_CCACLK_CLK_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT1
#endif /* BSP430_TIMER_CCACLK_CLK_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC0_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC0_PORT_BIT BIT7
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC1_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC1_PORT_BIT BIT2
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_BIT */

/* RFEM capabilities */
#if (configBSP430_RFEM - 0)
#define BSP430_RFEM 1
#define BSP430_RFEM_SPI0_PERIPH_HANDLE BSP430_PERIPH_EUSCI_B0
/* GDO0: P4.1, not available as CC input */
#define BSP430_RFEM_GDO0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4
#define BSP430_RFEM_GDO0_PORT_BIT BIT1
/* GDO1: P1.7, CCI2A on TB1.2; CCI0A on TA1.0 */
#define BSP430_RFEM_GDO1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_GDO1_PORT_BIT BIT7
/* GDO2: P2.3, CCI0B on TA0.0 */
#define BSP430_RFEM_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_RFEM_GDO2_PORT_BIT BIT3
#define BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_SPI0CSn_PORT_BIT BIT3
#endif /* configBSP430_RFEM */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

/** @endcond */

#endif /* BSP430_PLATFORM_EXP430FR5739_H */
