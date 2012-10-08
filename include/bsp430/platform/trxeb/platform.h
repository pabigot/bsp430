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

#ifndef BSP430_PLATFORM_TRXEB_PLATFORM_H
#define BSP430_PLATFORM_TRXEB_PLATFORM_H

/** @file
 *
 * @brief Platform-specific include for <a href="http://www.ti.com/tool/cc1125dk">MSP-TRXEB</a>
 *
 * The TrxEB is a transceiver evaluation board for TI RF modules.  It
 * is not easy to locate, but can be purchased with certain RF
 * evaluation models, and is documented in <a
 * href="http://www.ti.com/litv/pdf/swru294a">SWRU294</a>.
 *
 * The following platform-specific features are supported:
 * <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni is not implemented on this
 * platform.
 *
 * <li> #BSP430_PERIPH_EXPOSED_CLOCKS Clocks are made available at
 * dedicated labelled test points above the LCD.
 *
 * </ul>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_TRXEB 1

/* Available button definitions */
/* Button0 is LEFT */
#define BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON0_PORT_BIT BIT1
#define BSP430_PLATFORM_BUTTON_LEFT_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_LEFT_PORT_BIT BSP430_PLATFORM_BUTTON0_PORT_BIT
/* Button1 is RIGHT */
#define BSP430_PLATFORM_BUTTON1_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON1_PORT_BIT BIT2
#define BSP430_PLATFORM_BUTTON_RIGHT_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON1_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_RIGHT_PORT_BIT BSP430_PLATFORM_BUTTON1_PORT_BIT
/* Button2 is SELECT */
#define BSP430_PLATFORM_BUTTON2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON2_PORT_BIT BIT3
#define BSP430_PLATFORM_BUTTON_SELECT_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON2_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_SELECT_PORT_BIT BSP430_PLATFORM_BUTTON2_PORT_BIT
/* Button3 is UP */
#define BSP430_PLATFORM_BUTTON3_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON3_PORT_BIT BIT4
#define BSP430_PLATFORM_BUTTON_UP_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON3_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_UP_PORT_BIT BSP430_PLATFORM_BUTTON3_PORT_BIT
/* Button4 is DOWN */
#define BSP430_PLATFORM_BUTTON4_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT2
#define BSP430_PLATFORM_BUTTON4_PORT_BIT BIT5
#define BSP430_PLATFORM_BUTTON_DOWN_PORT_PERIPH_HANDLE BSP430_PLATFORM_BUTTON4_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON_DOWN_PORT_BIT BSP430_PLATFORM_BUTTON4_PORT_BIT

/* I think LED1 is yellow.  It may be green. */
#define BSP430_LED_RED 0
#define BSP430_LED_YELLOW 1
#define BSP430_LED_GREEN 2
#define BSP430_LED_RED1 3

/* What to use as a console */
/* !BSP430! module=console subst=module instance=nop */
/* !BSP430! insert=module_startif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_startif] */
#if ((configBSP430_CONSOLE - 0)                                    \
     && ((! defined(configBSP430_CONSOLE_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_CONSOLE_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [module_startif] */
/* !BSP430! end=module_startif */
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE BSP430_PERIPH_USCI5_A1
/* !BSP430! insert=module_endif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_endif] */
#endif /* configBSP430_CONSOLE && need default */
/* END AUTOMATICALLY GENERATED CODE [module_endif] */
/* !BSP430! end=module_endif */

/* How to use ACLK as a capture/compare input source */
/* !BSP430! module=timer feature=ccaclk timer=TB0 cc_index=6 ccis=1 clk_port=PORT4 clk_pin=BIT7 */
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
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TB0
#define BSP430_TIMER_CCACLK_CC_INDEX 6
#define BSP430_TIMER_CCACLK_CCIS CCIS_1
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT4
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT7
/* END AUTOMATICALLY GENERATED CODE [feature_ccaclk_decl] */
/* !BSP430! end=feature_ccaclk_decl */
/* !BSP430! insert=feature_endif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_endif] */
#endif /* configBSP430_TIMER_CCACLK && need default */
/* END AUTOMATICALLY GENERATED CODE [feature_endif] */
/* !BSP430! end=feature_endif */

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
/* GDO2: P1.3, CCI2A on TA0 */
#define BSP430_RFEM_GDO2_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_RFEM_GDO2_PORT_BIT BIT3
#define BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT3
#define BSP430_RFEM_SPI0CSn_PORT_BIT BIT0
#endif /* configBSP430_RFEM */

#ifndef configBSP430_PLATFORM_TRXEB_ALS
/** Enable HPL support for on-board ambient light sensor
 *
 * The TrxEB has an SFH 5711 ambient light sensor from Osram on board,
 * positioned to the right and below the LCD.  It receives power
 * through P6.1, and provides an analog logarithmic measure of ambient
 * light over P6.2 which is A2 for the on-board ADC.  Defining this
 * constant to a true value enables the configuration of port 6 as
 * well as provides constants for applications to use the sensor. */
#define configBSP430_PLATFORM_TRXEB_ALS 0
#endif /* configBSP430_PLATFORM_TRXEB_ALS */

#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_TRXEB_ALS - 0)
/** BSP430 port HPL reference on which ambient light sensor is placed.
 * @dependency #configBSP430_PLATFORM_TRXEB_ALS
 */
#define BSP430_PLATFORM_TRXEB_ALS_PORT_HPL BSP430_HPL_PORT6

/** Port bit on #BSP430_PLATFORM_TRXEB_ALS_PORT_HPL for ALS power
 * @dependency #configBSP430_PLATFORM_TRXEB_ALS
 */
#define BSP430_PLATFORM_TRXEB_ALS_PWR_PORT_BIT BIT1

/** Port bit on #BSP430_PLATFORM_TRXEB_ALS_PORT_HPL for ALS output
 * @dependency #configBSP430_PLATFORM_TRXEB_ALS
 */
#define BSP430_PLATFORM_TRXEB_ALS_OUT_PORT_BIT BIT2

/** ADC12 channel for ALS output
 * @dependency #configBSP430_PLATFORM_TRXEB_ALS
 */
#define BSP430_PLATFORM_TRXEB_ALS_OUT_INCH ADC12INCH_2
#endif /* configBSP430_PLATFORM_TRXEB_ALS */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_TRXEB_H */
