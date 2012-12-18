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
 * @brief Platform-specific include for <a href="http://www.ti.com/tool/msp-exp430g2">MSP-EXP430G2 ("LaunchPad")</a>
 *
 * This board is also known as the "LaunchPad"
 *
 * The following platform-specific features are supported: <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni The jumper pair for this
 * platform is P1.4 and P1.5, located in the left header.  Place the
 * jumper across these pins.
 *
 * <li> Since 2xx-family MCUs power-up with the standard XIN/XOUT pins
 * enabled for their peripheral function in P2SEL, you may define
 * #BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 to 0 to explicitly disable
 * the crystal on power-up.  This allows P2.6 and P2.7 to be used for
 * other functions without concern that the clock depends on them.
 *
 * </ul>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_EXP430G2_PLATFORM_H
#define BSP430_PLATFORM_EXP430G2_PLATFORM_H

/* Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_EXP430G2 1

/* Enable if requested */
#if configBSP430_PLATFORM_SPIN_FOR_JUMPER - 0
#define BSP430_PLATFORM_SPIN_FOR_JUMPER 1
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

/* Available button definitions */
#define BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_PLATFORM_BUTTON0_PORT_BIT BIT3

/* Standard LED colors */
#define BSP430_LED_RED 0
#define BSP430_LED_GREEN 1

/* What to use as the uptime timer.  If we're going to pick TA0 for
   CCACLK, try to pick a different timer for uptime. */
#if ((configBSP430_UPTIME - 0)                                  \
     && ((! defined(configBSP430_UPTIME_USE_DEFAULT_RESOURCE))  \
         || (configBSP430_UPTIME_USE_DEFAULT_RESOURCE - 0)))
#if defined(__MSP430G2553__)
#define BSP430_UPTIME_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA1
#endif /* MCU */
#endif /* configBSP430_UPTIME && need default */

/* What to use as a console */
/* !BSP430! module=console subst=module instance=nop */
/* !BSP430! insert=module_startif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_startif] */
#if ((configBSP430_CONSOLE - 0)                                    \
     && ((! defined(configBSP430_CONSOLE_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_CONSOLE_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [module_startif] */
/* !BSP430! end=module_startif */
#if defined(__MSP430G2553__)
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE BSP430_PERIPH_USCI_A0
#endif /* MCU */
/* !BSP430! insert=module_endif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_endif] */
#endif /* configBSP430_CONSOLE && need default */
/* END AUTOMATICALLY GENERATED CODE [module_endif] */
/* !BSP430! end=module_endif */

/* How to use ACLK as a capture/compare input source */
/* !BSP430! module=timer feature=ccaclk */
/* !BSP430! insert=feature_startif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_startif] */
#if ((configBSP430_TIMER_CCACLK - 0)                                    \
     && ((! defined(configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [feature_startif] */
/* !BSP430! end=feature_startif */
#if defined(__MSP430G2553__)
/* !BSP430! module=timer feature=ccaclk timer=TA0 cc_index=0 ccis=1 clk:pp=1.0 cc0:pp=1.1 cc1:pp=1.2 */
/* !BSP430! insert=feature_ccaclk_decl subst=timer,cc_index,ccis,clk_port,clk_pin */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_ccaclk_decl] */
#define BSP430_TIMER_CCACLK 1
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TA0
#define BSP430_TIMER_CCACLK_CC_INDEX 0
#define BSP430_TIMER_CCACLK_CCIS CCIS_1
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT0
/* END AUTOMATICALLY GENERATED CODE [feature_ccaclk_decl] */
/* !BSP430! end=feature_ccaclk_decl */
#endif /* MCU */
/* !BSP430! insert=feature_endif  subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_endif] */
#endif /* configBSP430_TIMER_CCACLK && need default */
/* END AUTOMATICALLY GENERATED CODE [feature_endif] */
/* !BSP430! end=feature_endif */

/* RFEM capabilities */
#if (configBSP430_RFEM - 0)
#define BSP430_RFEM 1
#define BSP430_RFEM_SPI0_PERIPH_HANDLE BSP430_PERIPH_USCI_B0
/* GDO0: P2.6 not available as CC input */
#define BSP430_RFEM_GDO0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_RFEM_GDO0_PORT_BIT BIT6
/* GDO1: P1.6, TA0.1, not available as CC input */
#define BSP430_RFEM_GDO1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_GDO1_PORT_BIT BIT6
/* GDO2: P1.0, TA0.1, not available as CC input */
#define BSP430_RFEM_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_GDO2_PORT_BIT BIT0
#define BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_RFEM_SPI0CSn_PORT_BIT BIT7
#endif /* configBSP430_RFEM */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_EXP430G2_H */
