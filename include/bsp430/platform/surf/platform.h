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

#ifndef BSP430_PLATFORM_SURF_PLATFORM_H
#define BSP430_PLATFORM_SURF_PLATFORM_H

/** @file
 *
 * @brief Platform-specific include for SuRF board.
 *
 * The SuRF board was a development platform for the <a
 * href="http://www.ti.com/product/cc430f5137">CC430F5137</a>,
 * produced by <a href="http://www.peoplepowerco.com">PeoplePower</a>
 * prior to their focus on a cloud-based software and mobile
 * applications.
 *
 * The following platform-specific features are supported:
 * <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni is not implemented on this
 * platform.
 *
 * </ul>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/** Denote the version of the SuRF board that is being targetted.
 *
 * <ul>
 * <li>A value of 0 indicates a Rev A board.
 * <li>A value of 1 indicates a Rev B board.
 * <li>A value of 2 indicates a Rev B2 board, which was the production version.
 * </ul>
 */
#ifndef BSP430_PLATFORM_SURF_REVISION
#define BSP430_PLATFORM_SURF_REVISION 2
#endif /* BSP430_PLATFORM_SURF_REVISION */

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_SURF 1

/** SuRF has a 26MHz XT2 for the RF1A */
#define BSP430_CLOCK_NOMINAL_XT2CLK_HZ 26000000UL

/** @cond DOXYGEN_EXCLUDE */

#if 2 == BSP430_PLATFORM_SURF_REVISION

/* Available button definitions. */
#define BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_PLATFORM_BUTTON0_PORT_BIT BIT0
#define BSP430_PLATFORM_BUTTON1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_PLATFORM_BUTTON1_PORT_BIT BIT1
#define BSP430_PLATFORM_BUTTON2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON2_PORT_BIT BIT6
#define BSP430_PLATFORM_BUTTON3_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON3_PORT_BIT BIT7

/* Standard LED colors */
#define BSP430_LED_GREEN 0
#define BSP430_LED_RED 1
#define BSP430_LED_ORANGE 2
#define BSP430_LED_WHITE 3
#define BSP430_LED_BLUE 4

/* DS1825 on P3.7 */
#define BSP430_PLATFORM_SURF_DS1825_PORT_HAL BSP430_HAL_PORT3
#define BSP430_PLATFORM_SURF_DS1825_PORT_BIT BIT7

#else /* BSP430_PLATFORM_SURF_REVISION */

#error BSP430_PLATFORM_SURF_REVISION not supported

#endif /* BSP430_PLATFORM_SURF_REVISION */

/* How to use ACLK as a capture/compare input source */
/* Settings for TA1: T1A2 ccis=1 ; clk P2.0 ; cc0 P2.1 ; cc1 P2.2 */
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
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT0
#endif /* BSP430_TIMER_CCACLK_CLK_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC0_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC0_PORT_BIT BIT1
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_BIT */
#ifndef BSP430_TIMER_CCACLK_CC1_PORT_BIT
/* NB: Check against BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID in bsp430_config.h */
#define BSP430_TIMER_CCACLK_CC1_PORT_BIT BIT2
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_BIT */

/** @endcond */

/** Enable HPL support for on-board SPI flash
 *
 * The SuRF board has a M25P10-A 1 Mib (128 kiB) serial flash from
 * Micron on board.  It uses USCI_B0 and has CSn on P1.7.  PWR and
 * RSTn are hard-wired.  Defining this constant to a true value
 * enables the HPL interface to port 1, SPI on USCI_B0, and provides
 * constants for applications to use the flash. */
#ifndef configBSP430_PLATFORM_M25P
#define configBSP430_PLATFORM_M25P 0
#endif /* configBSP430_PLATFORM_M25P */

#if (configBSP430_PLATFORM_M25P - 0)

/** BSP430 SPI peripheral handle for flash.
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0

/** BSP430 peripheral handle for port on which SPI flash chip-select (inverted) is placed.
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1

/** Port bit on #BSP430_PLATFORM_M25P_CSn_PORT_PERIPH_HANDLE for SPI flash CSn
 * @dependency #configBSP430_PLATFORM_M25P
 */
#define BSP430_PLATFORM_M25P_CSn_PORT_BIT BIT7

/* M25P10A does not support #BSP430_M25P_CMD_PE or
 * #BSP430_M25P_CMD_PW, and does not support subsectors. */

/* M25P10A has four 32 kiB sectors */
#define BSP430_PLATFORM_M25P_SECTOR_SIZE 0x8000L

/* M25P10A has four 32 kiB sectors */
#define BSP430_PLATFORM_M25P_SECTOR_COUNT 4

#endif /* configBSP430_PLATFORM_M25P */

/** Define to true to provide definitions for the DS1825 one-wire interface.
 *
 * If true, this allocates the port HAL required to interact with the
 * DS1825 temperature sensor on P3.7 and adds the declaration for
 * #xBSP430surfDS1825.
 *
 * @defaulted
 */
#ifndef configBSP430_PLATFORM_SURF_DS1825
#define configBSP430_PLATFORM_SURF_DS1825 0
#endif /* configBSP430_PLATFORM_SURF_DS1825 */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_SURF_DS1825 - 0)
#include <bsp430/utility/onewire.h>

/** The 1-Wire(R) interface for the DS1825 temperature sensor.
 *
 * @dependency #configBSP430_PLATFORM_SURF_DS1825
 */
extern const sBSP430onewireBus xBSP430surfDS1825;
#endif /* configBSP430_PLATFORM_SURF_DS1825 */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_SURF_H */
