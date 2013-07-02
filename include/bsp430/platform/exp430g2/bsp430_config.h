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

/** @file
 * @brief Platform-specific BSP430 configuration directives for <a href="http://www.ti.com/tool/msp-exp430g2">MSP-EXP430G2 ("LaunchPad")</a>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_EXP430G2_BSP430_CONFIG_H
#define BSP430_PLATFORM_EXP430G2_BSP430_CONFIG_H

/** @cond DOXYGEN_EXCLUDE */

/* Use native USCI for genericized serial port unless told not to */
#ifndef configBSP430_SERIAL_USE_USCI
#if defined(__MSP430G2553__)
#define configBSP430_SERIAL_USE_USCI 1
#endif /* MCU */
#endif /* configBSP430_SERIAL_USE_USCI */

/** Provide a platform default for the uptime timer */
#if (configBSP430_UPTIME - 0)
#ifndef BSP430_UPTIME_TIMER_PERIPH_CPPID
#if defined(__MSP430G2553__)
#define BSP430_UPTIME_TIMER_PERIPH_CPPID BSP430_PERIPH_CPPID_TA1
#endif /* MCU */
#endif /* BSP430_UPTIME_TIMER_PERIPH_CPPID */
#endif /* configBSP430_UPTIME */

/* What to use as a console */
#if (configBSP430_CONSOLE - 0)
#ifndef BSP430_CONSOLE_SERIAL_PERIPH_CPPID
#if defined(__MSP430G2553__)
#define BSP430_CONSOLE_SERIAL_PERIPH_CPPID BSP430_PERIPH_CPPID_USCI_A0
#endif /* MCU */
#endif /* BSP430_CONSOLE_SERIAL_PERIPH_CPPID */
#endif /* configBSP430_CONSOLE */

/* How to use ACLK as a capture/compare input source */
/* Settings for TA0: T0A0 ccis=1 ; clk P1.0 ; cc0 P1.1 ; cc1 P1.2 */
#if (configBSP430_TIMER_CCACLK - 0)
#if defined(__MSP430G2553__)
#ifndef BSP430_TIMER_CCACLK_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_PERIPH_CPPID BSP430_PERIPH_CPPID_TA0
#endif /* BSP430_TIMER_CCACLK_PERIPH_CPPID */
#ifndef BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT1
#endif /* BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID */
#ifndef BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT1
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID */
#ifndef BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT1
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID */
#endif /* MCU */
#endif /* configBSP430_TIMER_CCACLK */

/* Enable buttons as requested */
#if (configBSP430_PLATFORM_BUTTON0 - 0)
#if !defined(configBSP430_HAL_PORT1)
#define configBSP430_HAL_PORT1 1
#else /* configBSP430_HAL_PORT1 */
#if !defined(configBSP430_HPL_PORT1)
#define configBSP430_HPL_PORT1 1
#endif /* configBSP430_HPL_PORT1 */
#endif /* configBSP430_HAL_PORT1 */
#endif /* configBSP430_PLATFORM_BUTTON0 */

/* !BSP430! insert=emk_config emk=anaren emktag=anaren_cc110l mcu=msp430g2553 spi=UCB0SOMI tag=cc110x gpio=GDO0,GDO1,GDO2 */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [emk_config] */
#if (configBSP430_RF_ANAREN_CC110L - 0)
#define BSP430_RF_CC110X_SPI_PERIPH_CPPID BSP430_PERIPH_CPPID_USCI_B0
#define configBSP430_SERIAL_ENABLE_SPI 1
#define configBSP430_HAL_USCI_B0 1
#define BSP430_RF_CC110X_GDO2_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT1
#define BSP430_RF_CC110X_GDO0_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT2
#define BSP430_RF_CC110X_CSn_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT2
#define BSP430_RF_CC110X_GDO1_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT1
/* Request HAL (and HPL) for all GPIO ports */
#define BSP430_WANT_CONFIG_HAL 1
#define BSP430_WANT_PERIPH_CPPID BSP430_RF_CC110X_GDO2_PORT_PERIPH_CPPID
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_PERIPH_CPPID
#define BSP430_WANT_PERIPH_CPPID BSP430_RF_CC110X_GDO0_PORT_PERIPH_CPPID
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_PERIPH_CPPID
#define BSP430_WANT_PERIPH_CPPID BSP430_RF_CC110X_GDO1_PORT_PERIPH_CPPID
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_PERIPH_CPPID
#undef BSP430_WANT_CONFIG_HAL
/* Request HPL for all non-GPIO ports */
#define BSP430_WANT_CONFIG_HPL 1
#define BSP430_WANT_PERIPH_CPPID BSP430_RF_CC110X_CSn_PORT_PERIPH_CPPID
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_PERIPH_CPPID
#undef BSP430_WANT_CONFIG_HPL
/* Request HAL for GPIO timers */
#define BSP430_WANT_CONFIG_HAL 1

#if (configBSP430_RF_CC110X_GDO2_TIMER - 0) && defined(BSP430_RF_CC110X_GDO2_TIMER_PERIPH_CPPID)
#define BSP430_WANT_PERIPH_CPPID BSP430_RF_CC110X_GDO2_TIMER_PERIPH_CPPID
#if 0 == BSP430_RF_CC110X_GDO2_TIMER_CCIDX
#define BSP430_WANT_CONFIG_HAL_CC0_ISR 1
#else /* BSP430_RF_CC110X_GDO2_TIMER_CCIDX */
#define BSP430_WANT_CONFIG_HAL_ISR 1
#endif /* BSP430_RF_CC110X_GDO2_TIMER_CCIDX */
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL_ISR
#undef BSP430_WANT_CONFIG_HAL_CC0_ISR
#undef BSP430_WANT_PERIPH_CPPID
#endif /* config BSP430_RF_CC110X_GDO2 timer */

#if (configBSP430_RF_CC110X_GDO0_TIMER - 0) && defined(BSP430_RF_CC110X_GDO0_TIMER_PERIPH_CPPID)
#define BSP430_WANT_PERIPH_CPPID BSP430_RF_CC110X_GDO0_TIMER_PERIPH_CPPID
#if 0 == BSP430_RF_CC110X_GDO0_TIMER_CCIDX
#define BSP430_WANT_CONFIG_HAL_CC0_ISR 1
#else /* BSP430_RF_CC110X_GDO0_TIMER_CCIDX */
#define BSP430_WANT_CONFIG_HAL_ISR 1
#endif /* BSP430_RF_CC110X_GDO0_TIMER_CCIDX */
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL_ISR
#undef BSP430_WANT_CONFIG_HAL_CC0_ISR
#undef BSP430_WANT_PERIPH_CPPID
#endif /* config BSP430_RF_CC110X_GDO0 timer */

#if (configBSP430_RF_CC110X_GDO1_TIMER - 0) && defined(BSP430_RF_CC110X_GDO1_TIMER_PERIPH_CPPID)
#define BSP430_WANT_PERIPH_CPPID BSP430_RF_CC110X_GDO1_TIMER_PERIPH_CPPID
#if 0 == BSP430_RF_CC110X_GDO1_TIMER_CCIDX
#define BSP430_WANT_CONFIG_HAL_CC0_ISR 1
#else /* BSP430_RF_CC110X_GDO1_TIMER_CCIDX */
#define BSP430_WANT_CONFIG_HAL_ISR 1
#endif /* BSP430_RF_CC110X_GDO1_TIMER_CCIDX */
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL_ISR
#undef BSP430_WANT_CONFIG_HAL_CC0_ISR
#undef BSP430_WANT_PERIPH_CPPID
#endif /* config BSP430_RF_CC110X_GDO1 timer */
#undef BSP430_WANT_CONFIG_HAL
#endif /* configBSP430_RF_ANAREN_CC110L */
/* END AUTOMATICALLY GENERATED CODE [emk_config] */
/* !BSP430! end=emk_config */

/** @endcond */

#endif /* BSP430_PLATFORM_EXP430G2_BSP430_CONFIG_H */
