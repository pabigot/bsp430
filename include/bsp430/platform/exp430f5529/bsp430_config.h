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
 * @brief Platform-specific BSP430 configuration directives for <a href="http://www.ti.com/tool/msp-exp430f5529">MSP-EXP430F5529</a>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_EXP430F5529_BSP430_CONFIG_H
#define BSP430_PLATFORM_EXP430F5529_BSP430_CONFIG_H

/* Use native USCI5 for genericized serial port unless told not to */
#ifndef configBSP430_SERIAL_USE_USCI5
#define configBSP430_SERIAL_USE_USCI5 1
#endif /* configBSP430_SERIAL_USE_USCI5 */

/* Enable buttons as requested */
#if configBSP430_PLATFORM_BUTTON0 - 0
#if !defined(configBSP430_HAL_PORT1)
#define configBSP430_HAL_PORT1 1
#else /* configBSP430_HAL_PORT1 */
#define configBSP430_HPL_PORT1 1
#endif /* configBSP430_HAL_PORT1 */
#endif /* configBSP430_PLATFORM_BUTTON0 */
#if configBSP430_PLATFORM_BUTTON1 - 0
#if !defined(configBSP430_HAL_PORT2)
#define configBSP430_HAL_PORT2 1
#else /* configBSP430_HAL_PORT2 */
#define configBSP430_HPL_PORT2 1
#endif /* configBSP430_HAL_PORT2 */
#endif /* configBSP430_PLATFORM_BUTTON1 */

/* What to use as a console */
#if configBSP430_CONSOLE - 0
#ifndef BSP430_CONSOLE_SERIAL_PERIPH_CPPID
#define BSP430_CONSOLE_SERIAL_PERIPH_CPPID BSP430_PERIPH_CPPID_USCI5_A1
#endif /* BSP430_CONSOLE_SERIAL_PERIPH_CPPID */
#endif /* configBSP430_CONSOLE */

/* How to use ACLK as a capture/compare input source */
/* !BSP430! module=timer feature=ccaclk timer=TB0 cc_index=6 ccis=1 clk:pp=7.7 cc0:pp=5.6 cc1:pp=5.7*/
/* !BSP430! insert=feature_startif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_startif] */
#if ((configBSP430_TIMER_CCACLK - 0)                                    \
     && ((! defined(configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [feature_startif] */
/* !BSP430! end=feature_startif */
/* !BSP430! insert=feature_ccaclk_cfg subst=timer,cc_index,ccis,clk_port,clk_pin */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_ccaclk_cfg] */
#if configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL - 0
#if !defined(configBSP430_HAL_TB0)
#define configBSP430_HAL_TB0 1
#endif /* configBSP430_HAL_TB0 */
#if configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR - 0
#if !defined(configBSP430_HAL_TB0_CC0_ISR)
#define configBSP430_HAL_TB0_CC0_ISR 1
#endif /* configBSP430_HAL_TB0_CC0_ISR */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR */
#else /* configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL */
#if !defined(configBSP430_HPL_TB0)
#define configBSP430_HPL_TB0 1
#endif /* configBSP430_HPL_TB0 */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL */

#if configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL - 0
#if !defined(configBSP430_HAL_PORT7)
#define configBSP430_HAL_PORT7 1
#endif /* configBSP430_HAL_PORT7 */
#else /* configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL */
#if !defined(configBSP430_HPL_PORT7)
#define configBSP430_HPL_PORT7 1
#endif /* configBSP430_HPL_PORT7 */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL */
/* END AUTOMATICALLY GENERATED CODE [feature_ccaclk_cfg] */
/* !BSP430! end=feature_ccaclk_cfg */
/* !BSP430! insert=feature_endif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_endif] */
#endif /* configBSP430_TIMER_CCACLK && need default */
/* END AUTOMATICALLY GENERATED CODE [feature_endif] */
/* !BSP430! end=feature_endif */

/* Enable RFEM resources if requested */
#if (configBSP430_RFEM - 0)
#define configBSP430_SERIAL_ENABLE_SPI 1
#define configBSP430_HAL_USCI5_B0 1
#define configBSP430_HAL_PORT2 1
#define configBSP430_HAL_PORT3 1
#endif /* configBSP430_RFEM */

#endif /* BSP430_PLATFORM_EXP430F5529_BSP430_CONFIG_H */
