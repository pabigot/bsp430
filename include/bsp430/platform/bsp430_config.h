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
 * @brief BSP430 configuration directives for generic platform support
 *
 * This simplifies management of application-specific bsp430_config.h
 * files.  First define anything for your application, then include
 * this file.
 *
 * This file will first include any platform-specific files.
 * Platform-specific configuration headers are included based on the
 * definitions of:
 * <ul>
 * <li>#BSP430_PLATFORM_EXP430G2</a>
 * <li>#BSP430_PLATFORM_EXP430FG4618</a>
 * <li>#BSP430_PLATFORM_EXP430F5438</a>
 * <li>#BSP430_PLATFORM_EXP430F5529</a>
 * <li>#BSP430_PLATFORM_EXP430FR5739</a>
 * <li>#BSP430_PLATFORM_RF2500T</a>
 * <li>#BSP430_PLATFORM_CUSTOM</a>
 * </ul>
 * After platform-specific configuration has been read, this file may
 * provide additional BSP430-generic configuration.
 *
 * Each inclusion will respect previous configuration values.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_BSP430_CONFIG_H
#define BSP430_PLATFORM_BSP430_CONFIG_H

#include <msp430.h>

/* UCS has no facility other than BSP430_UCS_TRIM_DCOCLKDIV to set the
 * MCLK, so enable it by default. */
#if defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__)
#ifndef configBSP430_UCS_TRIM_DCOCLKDIV
#define configBSP430_UCS_TRIM_DCOCLKDIV 1
#ifndef configBSP430_TIMER_CCACLK
#define configBSP430_TIMER_CCACLK 1
#endif /* configBSP430_TIMER_CCACLK */
#endif /* configBSP430_UCS_TRIM_DCOCLKDIV */
#endif /* UCS */

/* BC2 feature request also requires CCACLK be enabled */
#if configBSP430_BC2_TRIM_TO_MCLK - 0
#ifndef configBSP430_TIMER_CCACLK
#define configBSP430_TIMER_CCACLK 1
#endif /* configBSP430_TIMER_CCACLK */
#endif /* configBSP430_BC2_TRIM_TO_MCLK */

/* !BSP430! instance=exp430f5438,exp430f5529,exp430fr5739,exp430fg4618,exp430g2,rf2500t */
/* !BSP430! insert=platform_bsp430_config */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_bsp430_config] */
#if BSP430_PLATFORM_EXP430F5438 - 0
#include <bsp430/platform/exp430f5438/bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430F5438 */

#if BSP430_PLATFORM_EXP430F5529 - 0
#include <bsp430/platform/exp430f5529/bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430F5529 */

#if BSP430_PLATFORM_EXP430FR5739 - 0
#include <bsp430/platform/exp430fr5739/bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430FR5739 */

#if BSP430_PLATFORM_EXP430FG4618 - 0
#include <bsp430/platform/exp430fg4618/bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430FG4618 */

#if BSP430_PLATFORM_EXP430G2 - 0
#include <bsp430/platform/exp430g2/bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430G2 */

#if BSP430_PLATFORM_RF2500T - 0
#include <bsp430/platform/rf2500t/bsp430_config.h>
#endif /* BSP430_PLATFORM_RF2500T */

/* END AUTOMATICALLY GENERATED CODE [platform_bsp430_config] */
/* !BSP430! end=platform_bsp430_config */

#if BSP430_PLATFORM_CUSTOM - 0
#include <bsp430/platform/custom/bsp430_config.h>
#endif /* BSP430_PLATFORM_CUSTOM */

/* Requirements to support default uptime resource (if that is
 * enabled, and nobody's done it already) */
#if ((configBSP430_UPTIME - 0)                                  \
     && ((! defined(configBSP430_UPTIME_USE_DEFAULT_RESOURCE))  \
         || (configBSP430_UPTIME_USE_DEFAULT_RESOURCE - 0)))
#define configBSP430_HAL_TA0 1
#if configBSP430_UPTIME_USE_DEFAULT_CC0_ISR - 0
#define configBSP430_HAL_TA0_CC0_ISR 1
#endif /* enable uptime CC0 ISR */
#endif /* configBSP430_UPTIME */

#endif /* BSP430_PLATFORM_BSP430_CONFIG_H */
