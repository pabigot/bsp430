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
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/** Denote the version of the SuRF board that is being targetted.
 *
 * <ul>
 * <li>A value of 0 indicates a Rev A board.
 * <li>A value of 1 indicates a Rev B board.
 * <li>A value of 2 indicates a Rev B2 board, which was the production version.
 * <ul>
 */
#ifndef BSP430_PLATFORM_SURF_REVISION
#define BSP430_PLATFORM_SURF_REVISION 2
#endif /* BSP430_PLATFORM_SURF_REVISION */

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_SURF 1

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

#else /* BSP430_PLATFORM_SURF_REVISION */

#error BSP430_PLATFORM_SURF_REVISION not supported

#endif /* BSP430_PLATFORM_SURF_REVISION */

/* What to use as a console */
/* !BSP430! module=console subst=module instance=nop */
/* !BSP430! insert=module_startif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_startif] */
#if ((configBSP430_CONSOLE - 0)                                    \
     && ((! defined(configBSP430_CONSOLE_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_CONSOLE_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [module_startif] */
/* !BSP430! end=module_startif */
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE BSP430_PERIPH_USCI5_A0
/* !BSP430! insert=module_endif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_endif] */
#endif /* configBSP430_CONSOLE && need default */
/* END AUTOMATICALLY GENERATED CODE [module_endif] */
/* !BSP430! end=module_endif */

/* How to use ACLK as a capture/compare input source */
/* !BSP430! module=timer feature=ccaclk timer=TA1 cc_index=2 ccis=1 clk_port=PORT2 clk_pin=BIT0 */
/* !BSP430! insert=feature_startif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_startif] */
#if ((configBSP430_TIMER_CCACLK - 0)                                    \
     && ((! defined(configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [feature_startif] */
/* !BSP430! end=feature_startif */
/* !BSP430! insert=feature_ccaclk_decl subst=timer,cc_index,ccis,clk_port,clk_pin */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_ccaclk_decl] */
#define BSP430_TIMER_CCACLK 1
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TA1
#define BSP430_TIMER_CCACLK_CC_INDEX 2
#define BSP430_TIMER_CCACLK_CCIS CCIS_1
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT0
/* END AUTOMATICALLY GENERATED CODE [feature_ccaclk_decl] */
/* !BSP430! end=feature_ccaclk_decl */
/* !BSP430! insert=feature_endif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_endif] */
#endif /* configBSP430_TIMER_CCACLK && need default */
/* END AUTOMATICALLY GENERATED CODE [feature_endif] */
/* !BSP430! end=feature_endif */


#ifndef configBSP430_PLATFORM_SURF_FLASH
/** Enable HPL support for on-board SPI flash
 *
 * The SuRF board has a M25P10-A 1 Mib (128 kiB) serial flash from
 * Micron on board.  It uses USCI_B0 and has CSn on P1.7.  PWR and
 * RSTn are hard-wired.  Defining this constant to a true value
 * enables the HPL interface to port 1, SPI on USCI_B0, and provides
 * constants for applications to use the flash. */
#define configBSP430_PLATFORM_SURF_FLASH 0
#endif /* configBSP430_PLATFORM_SURF_FLASH */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_SURF_FLASH - 0)

/** BSP430 SPI peripheral handle for flash.
 * @dependency #configBSP430_PLATFORM_SURF_FLASH
 */
#define BSP430_PLATFORM_SURF_FLASH_SPI_PERIPH_HANDLE BSP430_PERIPH_USCI5_B0

/** BSP430 peripheral handle for port on which SPI flash chip-select (inverted) is placed.
 * @dependency #configBSP430_PLATFORM_SURF_FLASH
 */
#define BSP430_PLATFORM_SURF_FLASH_CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1

/** Port bit on #BSP430_PLATFORM_SURF_FLASH_PORT_PERIPH_HANDLE for SPI flash CSn
 * @dependency #configBSP430_PLATFORM_SURF_FLASH
 */
#define BSP430_PLATFORM_SURF_FLASH_CSn_PORT_BIT BIT7

#endif /* configBSP430_PLATFORM_SURF_FLASH */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_SURF_H */
