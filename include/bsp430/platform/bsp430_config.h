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
 * @brief BSP430 application and platform configuration directives
 *
 * This infrastructure supports management of @link bsp430_config
 * application-specific bsp430_config.h files@endlink.  In your
 * application's @c bsp430_config.h header define your specific
 * requests then include <bsp430/platform/bsp430_config.h>.
 *
 * This file will first include any platform-specific @c
 * bsp430_config.h files.  Platform-specific configuration headers are
 * included based on the definitions of:
 *
 * <ul>
 * <li>#BSP430_PLATFORM_EXP430G2</a>
 * <li>#BSP430_PLATFORM_EXP430FG4618</a>
 * <li>#BSP430_PLATFORM_EXP430F5438</a>
 * <li>#BSP430_PLATFORM_EXP430F5529</a>
 * <li>#BSP430_PLATFORM_EXP430FR5739</a>
 * <li>#BSP430_PLATFORM_EM430</a>
 * <li>#BSP430_PLATFORM_RF2500T</a>
 * <li>#BSP430_PLATFORM_SURF</a>
 * <li>#BSP430_PLATFORM_TRXEB</a>
 * <li>#BSP430_PLATFORM_WOLVERINE</a>
 * <li>#BSP430_PLATFORM_CUSTOM</a>
 * </ul>
 *
 * After platform-specific configuration has been read, this file may
 * provide additional BSP430-generic configuration such as processing
 * requests for functional resources.
 *
 * Each inclusion will respect previous configuration values.
 *
 * @see @ref grp_config_core
 * @see @ref grp_config_functional
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 *
 * @defgroup grp_config BSP430 Configuration
 * @brief Documentation for @link bsp430_config bsp430_config.h@endlink options
 *
 * @defgroup grp_config_core Configuration of Core Peripheral Resources
 * @ingroup grp_config
 * @brief Flag macros used to enabling core resources (@HAL, @HPL, etc.).
 *
 * Core resources are MSP430 peripherals that are instances of a class
 * of peripheral, such as @link bsp430/periph/port.h digital i/o
 * ports@endlink, @link bsp430/periph/timers.h timers@endlink, and
 * @link bsp430/serial.h serial interfaces@endlink.  For the purposes
 * of exposition, we will use #BSP430_PERIPH_TA0 as an exemplary
 * resource.  The spelling of the options for other peripherals is the
 * same, substituting their short tag for @c TA0 in the flag macros.
 *
 * #configBSP430_HPL_TA0 enables the @link HPL Hardware Presentation
 * Layer @endlink for the peripheral.  This defines the
 * #BSP430_PERIPH_TA0 unique handle, and the #BSP430_HPL_TA0 pointer
 * to the peripheral register map.  It also allows you to use
 * xBSP430hplLookupTIMER() to get the type-cast pointer using the
 * handle.  It allocates no memory and introduces no code.
 *
 * #configBSP430_HAL_TA0 enables the @link HAL Hardware Adaptation
 * Layer @endlink object.  This is a global object, accessed through
 * #BSP430_HAL_TA0, containing information about the resource
 * including references to peripheral registers that are not
 * contiguous with the @HPL registers and control of any associated
 * infrastructure-provided interrupt callbacks.  Use
 * hBSP430timerLookup() to get a pointer to the object from its
 * handle.  Enabling #configBSP430_HAL_TA0 automatically enables
 * #configBSP430_HPL_TA0.
 *
 * #configBSP430_HAL_TA0_ISR provides a shared handler for any primary
 * interrupt capability associated with the peripheral.  Enabling
 * #configBSP430_HAL_TA0 will default enable this feature, but if you
 * don't need interrupts or intend to provide your own handler
 * implementation you can set the flag macro value to zero to exclude
 * it.
 *
 * Some peripherals use variant names for the interrupt capability
 * (e.g., #configBSP430_HAL_USCI_AB0RX_ISR for receive interrupts
 * shared between #BSP340_PERIPH_USCI_A0 and #BSP430_PERIPH_USCI_B0).
 *
 * Some peripherals have additional names for extended interrupt
 * capability (e.g., #configBSP430_HAL_TA0_CC0_ISR for the dicated CC0
 * interrupt handler).  Although the primary interrupt handler is
 * default-enabled when the @HAL is enabled, secondary interrupt
 * capabilities like this may need to be explicitly requested.
 *
 * @defgroup grp_config_functional Configuration of Functional Resources
 * @ingroup grp_config

 * @brief Flag macros used to enhance functional resources.
 *
 * BSP430 provides an indirection facility that allows applications to
 * use a functional peripheral handle while delegating selection of
 * the specific peripheral to the application or platform.  The
 * following functional resources are supported by the infrastructure;
 * options which influence secondary capabilities associated with
 * those resources are documented below.
 *
 * @li #configBSP430_UPTIME provides an uptime timer via
 * #BSP430_UPTIME_TIMER_PERIPH_HANDLE with the corresponding @HPL,
 * @HAL, and primary interrupt capabilities enabled.
 *
 * @li #configBSP430_TIMER_CCACLK provides a secondary timer via
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE.  See @ref grp_timer_ccaclk for
 * more details.
 *
 * @li #configBSP430_CONSOLE provides a serial console via
 * #BSP430_CONSOLE_SERIAL_PERIPH_HANDLE with the corresponding @HPL,
 * @HAL, and interrupt capabilities enabled.  There are no optional
 * functional resources associated with the console.
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

/* !BSP430! instance=exp430f5438,exp430f5529,exp430fr5739,exp430fg4618,exp430g2,em430,rf2500t,wolverine,trxeb,surf */
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

#if BSP430_PLATFORM_EM430 - 0
#include <bsp430/platform/em430/bsp430_config.h>
#endif /* BSP430_PLATFORM_EM430 */

#if BSP430_PLATFORM_RF2500T - 0
#include <bsp430/platform/rf2500t/bsp430_config.h>
#endif /* BSP430_PLATFORM_RF2500T */

#if BSP430_PLATFORM_WOLVERINE - 0
#include <bsp430/platform/wolverine/bsp430_config.h>
#endif /* BSP430_PLATFORM_WOLVERINE */

#if BSP430_PLATFORM_TRXEB - 0
#include <bsp430/platform/trxeb/bsp430_config.h>
#endif /* BSP430_PLATFORM_TRXEB */

#if BSP430_PLATFORM_SURF - 0
#include <bsp430/platform/surf/bsp430_config.h>
#endif /* BSP430_PLATFORM_SURF */

/* END AUTOMATICALLY GENERATED CODE [platform_bsp430_config] */
/* !BSP430! end=platform_bsp430_config */

#if BSP430_PLATFORM_CUSTOM - 0
#include <bsp430/platform/custom/bsp430_config.h>
#endif /* BSP430_PLATFORM_CUSTOM */

/* Propagate requests for functional resources.  Note that because we
 * need the feature resource identified in order to request it,
 * default values for BSP430-supported features must be provided in
 * the platform-specific bsp430_config.h not the platform.h file. */

#if configBSP430_UPTIME - 0
/* Set a default resource if none encountered so far */
#ifndef BSP430_UPTIME_TIMER_PERIPH_CPPID
#define BSP430_UPTIME_TIMER_PERIPH_CPPID BSP430_PERIPH_CPPID_TA0
#endif /* BSP430_UPTIME_TIMER_PERIPH_CPPID */

/* Enable HPL and HAL and primary ISR.  Control CC0 ISR only if
 * explicitly requested. */
#define BSP430_WANT_PERIPH_CPPID BSP430_UPTIME_TIMER_PERIPH_CPPID
#define BSP430_WANT_CONFIG_HPL 1
#define BSP430_WANT_CONFIG_HAL 1
#define BSP430_WANT_CONFIG_HAL_ISR 1
#ifdef configBSP430_UPTIME_TIMER_HAL_CC0_ISR
#define BSP430_WANT_CONFIG_HAL_CC0_ISR (configBSP430_UPTIME_TIMER_HAL_CC0_ISR - 0)
#endif /* configBSP430_UPTIME_TIMER_HAL_CC0_ISR */
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL_CC0_ISR
#undef BSP430_WANT_CONFIG_HAL_ISR
#undef BSP430_WANT_CONFIG_HAL
#undef BSP430_WANT_CONFIG_HPL
#undef BSP430_WANT_PERIPH_CPPID
#endif /* configBSP430_UPTIME */

#if configBSP430_CONSOLE - 0
/* Set a default resource if none encountered so far */
#ifndef BSP430_CONSOLE_SERIAL_PERIPH_CPPID
#define BSP430_CONSOLE_SERIAL_PERIPH_CPPID BSP430_PERIPH_CPPID_NONE
#endif /* BSP430_CONSOLE_SERIAL_PERIPH_CPPID */

/* Enable HPL, HAL, and ISR. */
#define BSP430_WANT_PERIPH_CPPID BSP430_CONSOLE_SERIAL_PERIPH_CPPID
#define BSP430_WANT_CONFIG_HPL 1
#define BSP430_WANT_CONFIG_HAL 1
#define BSP430_WANT_CONFIG_HAL_ISR 1
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL_ISR
#undef BSP430_WANT_CONFIG_HAL
#undef BSP430_WANT_CONFIG_HPL
#undef BSP430_WANT_PERIPH_CPPID
#endif /* configBSP430_CONSOLE */

#if configBSP430_TIMER_CCACLK - 0

#ifndef BSP430_TIMER_CCACLK_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_PERIPH_CPPID BSP430_PERIPH_CPPID_NONE
#endif /* BSP430_TIMER_CCACLK_PERIPH_CPPID */

/* Enable HPL.  Control HAL and ISRs only if explicitly requested. */
#define BSP430_WANT_PERIPH_CPPID BSP430_TIMER_CCACLK_PERIPH_CPPID
#define BSP430_WANT_CONFIG_HPL 1
#ifdef configBSP430_TIMER_CCACLK_HAL
#define BSP430_WANT_CONFIG_HAL (configBSP430_TIMER_CCACLK_HAL - 0)
#endif /* configBSP430_TIMER_CCACLK_HAL */
#ifdef configBSP430_TIMER_CCACLK_HAL_ISR
#define BSP430_WANT_CONFIG_HAL_ISR (configBSP430_TIMER_CCACLK_HAL_ISR - 0)
#endif /* configBSP430_TIMER_CCACLK_HAL_ISR */
#ifdef configBSP430_TIMER_CCACLK_HAL_CC0_ISR
#define BSP430_WANT_CONFIG_HAL_CC0_ISR (configBSP430_TIMER_CCACLK_HAL_CC0_ISR - 0)
#endif /* configBSP430_TIMER_CCACLK_HAL_CC0_ISR */
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HAL_CC0_ISR
#undef BSP430_WANT_CONFIG_HAL_ISR
#undef BSP430_WANT_CONFIG_HAL
#undef BSP430_WANT_CONFIG_HPL
#undef BSP430_WANT_PERIPH_CPPID

#if configBSP430_TIMER_CCACLK_CLK_PORT - 0

#ifndef BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_NONE
#endif /* BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID */

/* Enable HPL.  Control HAL only if requested.  No control of ISRs. */
#define BSP430_WANT_PERIPH_CPPID BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID
#define BSP430_WANT_CONFIG_HPL 1
#ifdef configBSP430_TIMER_CCACL_CLK_PORT_HAL
#define BSP430_WANT_CONFIG_HAL (configBSP430_TIMER_CCACLK_CLK_PORT_HAL - 0)
#endif /* configBSP430_TIMER_CCACL_CLK_PORT_HAL */
#include <bsp430/periph/want_.h>
#undef BSP430_WANT_CONFIG_HPL
#undef BSP430_WANT_CONFIG_HAL
#undef BSP430_WANT_PERIPH_CPPID
#endif /* configBSP430_TIMER_CCACLK_CLK_PORT */

#if configBSP430_TIMER_CCACLK_CC0_PORT - 0
#ifndef BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_NONE
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID */
#endif /* configBSP430_TIMER_CCACLK_CC0_PORT */

#if configBSP430_TIMER_CCACLK_CC1_PORT - 0
#ifndef BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_NONE
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID */
#endif /* configBSP430_TIMER_CCACLK_CC1_PORT */

#endif /* configBSP430_TIMER_CCACLK */

#endif /* BSP430_PLATFORM_BSP430_CONFIG_H */
