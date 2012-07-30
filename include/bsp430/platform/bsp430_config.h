/* Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>
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
 * <li>#BSP430_PLATFORM_EXP430FG4619</a>
 * <li>#BSP430_PLATFORM_EXP430F5438</a>
 * <li>#BSP430_PLATFORM_EXP430F5529</a>
 * <li>#BSP430_PLATFORM_EXP430FR5739</a>
 * <li>#BSP430_PLATFORM_RF2500T</a>
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

#if defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_UCS_RF__)
#ifndef configBSP430_CLOCK_TRIM_FLL
#define configBSP430_CLOCK_TRIM_FLL 1
#ifndef configBSP430_TIMER_CCACLK
#define configBSP430_TIMER_CCACLK 1
#endif /* configBSP430_TIMER_CCACLK */
#endif /* configBSP430_CLOCK_TRIM_FLL */
#endif /* UCS */

/* !BSP430! instance=exp430f5438,exp430fr5739,exp430fg4618,exp430g2,rf2500t */
/* !BSP430! insert=platform_bsp430_config */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_bsp430_config] */
#if BSP430_PLATFORM_EXP430F5438 - 0
#include <bsp430/platform/exp430f5438_bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430F5438 */

#if BSP430_PLATFORM_EXP430FR5739 - 0
#include <bsp430/platform/exp430fr5739_bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430FR5739 */

#if BSP430_PLATFORM_EXP430FG4618 - 0
#include <bsp430/platform/exp430fg4618_bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430FG4618 */

#if BSP430_PLATFORM_EXP430G2 - 0
#include <bsp430/platform/exp430g2_bsp430_config.h>
#endif /* BSP430_PLATFORM_EXP430G2 */

#if BSP430_PLATFORM_RF2500T - 0
#include <bsp430/platform/rf2500t_bsp430_config.h>
#endif /* BSP430_PLATFORM_RF2500T */

/* END AUTOMATICALLY GENERATED CODE [platform_bsp430_config] */
/* !BSP430! end=platform_bsp430_config */

/** Requirements to support default uptime resource (if that is enabled)
 * @defaulted */
#if ((configBSP430_UPTIME - 0)                                  \
     && ((! defined(configBSP430_UPTIME_USE_DEFAULT_RESOURCE))  \
         ||(configBSP430_UPTIME_USE_DEFAULT_RESOURCE - 0)))
#ifndef configBSP430_HAL_TA0
#define configBSP430_HAL_TA0 1
#endif /* TA0 is default uptime resource */
#endif /* configBSP430_UPTIME */

/** @def BSP430_CONSOLE_SERIAL_PERIPH_HANDLE
 *
 * Provide a default based on the available serial peripheral.
 *
 * @par<b>Peripheral-specific</b>:
 * Based on common use by TI experimenter boards:<ul>
 * <li>USCI selects #BSP430_PERIPH_USCI_A0.
 * <li>USCI5 selects #BSP430_PERIPH_USCI5_A1.
 * <li>EUSCI selects #BSP430_PERIPH_EUSCI_A0.
 * </ul>
 *
 * If a definition was not already provided and the the corresponding
 * peripheral is identified as being available on the target MCU, the
 * macro is defined to the specified handle and the HAL configuration
 * for that instance is defaulted to true.
 *
 * If no supported UART peripheral is identified, no default value is
 * assigned.
 *
 * @defaulted
 */
#ifndef BSP430_CONSOLE_SERIAL_PERIPH_HANDLE
#if defined(BSP430_DOXYGEN)
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE peripheral-specific value
#endif /* BSP430_DOXYGEN */

#if defined(__MSP430_HAS_USCI__)
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE BSP430_PERIPH_USCI_A0
#ifndef configBSP430_HAL_USCI_A0
/** Default to enable use of console device
 * @defaulted */
#define configBSP430_HAL_USCI_A0 1
#endif /* configBSP430_HAL_USCI_A0 */
#endif /* __MSP430_HAS_USCI__ */

#if defined(__MSP430_HAS_USCI_A0__)
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE BSP430_PERIPH_USCI5_A1
#ifndef configBSP430_HAL_USCI5_A1
/** Default to enable use of console device
 * @defaulted */
#define configBSP430_HAL_USCI5_A1 1
#endif /* configBSP430_HAL_USCI5_A1 */
#endif /* __MSP430_HAS_USCI_A0__ */

#if defined(__MSP430_HAS_EUSCI_A0__)
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE BSP430_PERIPH_EUSCI_A0
#ifndef configBSP430_HAL_EUSCI_A0
/** Default to enable use of console device
 * @defaulted */
#define configBSP430_HAL_EUSCI_A0 1
#endif /* configBSP430_HAL_EUSCI_A0 */
#endif /* __MSP430_HAS_EUSCI_A0__ */

#endif /* BSP430_CONSOLE_SERIAL_PERIPH_HANDLE */


#endif /* BSP430_PLATFORM_BSP430_CONFIG_H */
