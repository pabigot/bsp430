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
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_BSP430_CONFIG_H
#define BSP430_PLATFORM_BSP430_CONFIG_H

/** Platform support should normally include uptime
 * @defaulted */
#ifndef configBSP430_UPTIME
#define configBSP430_UPTIME 1
#endif /* configBSP430_UPTIME */

/** Requirements to support uptime, if desired
 * @defaulted */
#if configBSP430_UPTIME - 0
#define configBSP430_HAL_TA0 1
#endif /* configBSP430_UPTIME */

/** @def BSP430_CONSOLE_UART_PERIPH_HANDLE
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
 * If a definition was not already provided and tf the corresponding
 * peripheral is identified as being available on the target MCU, the
 * macro is defined to the specified handle and the HAL configuration
 * for that instance is defaulted to true.
 *
 * If no supported UART peripheral is identified, no default value is
 * assigned.
 */
#ifndef BSP430_CONSOLE_UART_PERIPH_HANDLE
#if defined(BSP430_DOXYGEN)
#define BSP430_CONSOLE_UART_PERIPH_HANDLE peripheral-specific value
#endif /* BSP430_DOXYGEN */

#if defined(__MSP430_HAS_USCI__)
#define BSP430_CONSOLE_UART_PERIPH_HANDLE BSP430_PERIPH_USCI_A0
#ifndef configBSP430_HAL_USCI_A0
/** Default to enable use of console device */
#define configBSP430_HAL_USCI_A0 1
#endif /* configBSP430_HAL_USCI_A0 */
#endif /* __MSP430_HAS_USCI__ */

#if defined(__MSP430_HAS_USCI_A0__)
#define BSP430_CONSOLE_UART_PERIPH_HANDLE BSP430_PERIPH_USCI5_A1
#ifndef configBSP430_HAL_USCI5_A1
/** Default to enable use of console device */
#define configBSP430_HAL_USCI5_A1 1
#endif /* configBSP430_HAL_USCI5_A1 */
#endif /* __MSP430_HAS_USCI_A0__ */

#if defined(__MSP430_HAS_EUSCI_A0__)
#define BSP430_CONSOLE_UART_PERIPH_HANDLE BSP430_PERIPH_EUSCI_A0
#ifndef configBSP430_HAL_EUSCI_A0
/** Default to enable use of console device */
#define configBSP430_HAL_EUSCI_A0 1
#endif /* configBSP430_HAL_EUSCI_A0 */
#endif /* __MSP430_HAS_EUSCI_A0__ */

#endif /* BSP430_CONSOLE_UART_PERIPH_HANDLE */

#endif /* BSP430_PLATFORM_BSP430_CONFIG_H */
