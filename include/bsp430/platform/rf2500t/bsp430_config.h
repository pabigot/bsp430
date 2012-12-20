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
 * @brief Platform-specific BSP430 configuration directives for <a href="http://www.ti.com/tool/ez430-rf2500t">EZ430-RF2500T</a>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_RF2500T_BSP430_CONFIG_H
#define BSP430_PLATFORM_RF2500T_BSP430_CONFIG_H

/** @cond DOXYGEN_EXCLUDE */

/** Platform does not support a crystal
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC
#define BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC 0
#endif /* BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC */

/* Use native USCI for genericized serial port unless told not to */
#ifndef configBSP430_SERIAL_USE_USCI
#define configBSP430_SERIAL_USE_USCI 1
#endif /* configBSP430_SERIAL_USE_USCI */

/* What to use as a console */
#if configBSP430_CONSOLE - 0
#ifndef BSP430_CONSOLE_SERIAL_PERIPH_CPPID
#define BSP430_CONSOLE_SERIAL_PERIPH_CPPID BSP430_PERIPH_CPPID_USCI_A0
#endif /* BSP430_CONSOLE_SERIAL_PERIPH_CPPID */
#endif /* configBSP430_CONSOLE */

/* How to use ACLK as a capture/compare input source */
/* Settings for TB0: T0B2 ccis=1 ; clk P4.7 ; cc0 P4.0 ; cc1 P4.1 */
#if configBSP430_TIMER_CCACLK - 0
#ifndef BSP430_TIMER_CCACLK_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_PERIPH_CPPID BSP430_PERIPH_CPPID_TB0
#endif /* BSP430_TIMER_CCACLK_PERIPH_CPPID */
#ifndef BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT4
#endif /* BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_CPPID */
#ifndef BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT4
#endif /* BSP430_TIMER_CCACLK_CC0_PORT_PERIPH_CPPID */
#ifndef BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID
#define BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID BSP430_PERIPH_CPPID_PORT4
#endif /* BSP430_TIMER_CCACLK_CC1_PORT_PERIPH_CPPID */
#endif /* configBSP430_TIMER_CCACLK */

/* Enable buttons as requested */
#if configBSP430_PLATFORM_BUTTON0 - 0
#if !defined(configBSP430_HAL_PORT1)
#define configBSP430_HAL_PORT1 1
#else /* configBSP430_HAL_PORT1 */
#if !defined(configBSP430_HPL_PORT1)
#define configBSP430_HPL_PORT1 1
#endif /* configBSP430_HPL_PORT1 */
#endif /* configBSP430_HAL_PORT1 */
#endif /* configBSP430_PLATFORM_BUTTON0 */

/* Enable RFEM resources if requested */
#if (configBSP430_RFEM - 0)
#define configBSP430_SERIAL_ENABLE_SPI 1
#define configBSP430_HAL_USCI_B0 1
#define configBSP430_HAL_PORT2 1
#define configBSP430_HAL_PORT3 1
#endif /* configBSP430_RFEM */

/** @endcond */

#endif /* BSP430_PLATFORM_RF2500T_BSP430_CONFIG_H */
