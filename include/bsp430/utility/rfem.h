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
 * @brief RF Evaluation Model connector support
 *
 * Many MSP430 evaluation boards support an RF interface, often in the
 * form of the RF1/RF2 Evaluation Model connector and/or its companion
 * RF3 header.  In some cases, such as the #BSP430_PLATFORM_RF2500T,
 * the radio is built into the platform.  This header provides
 * standard names for the resources required to use these interfaces
 * pins, allowing the platform-specific headers to provide definitions
 * for them.  Each pin is identified by its @link tBSP430periphHandle
 * port peripheral handle @endlink and the bit within that port.
 *
 * Although the standard RFEM connector comprises two 20-pin header
 * blocks, not all EM boards make use of all pins.  The majority of
 * the RFEM kits available to the developer are for ChipCon radios, so
 * the names associated with the pins correspond to the names of the
 * signals on that pin for ChipCon radios.
 *
 * The developer also has the <a
 * href="http://processors.wiki.ti.com/index.php?title=PAN1315EMK_User_Guide">PAN
 * 1323 EMK</a>, which uses additional pins.  These are normally
 * available on RF3, but need to be connected to RF2 on the board to
 * support the evaluation module.  These pins are also supported,
 * using the names corresponding to their function in the PAN module.
 *
 * EM Header Pin    | BSP430 Resources                       | Comments
 * :------------    | :---------------                       | :----------
 *  RF1.3 (RF3.4)   | BT_HCI_CTS                             | aka SPI1_CSn |
 *  RF1.5 (RF3.6)   | SLOW_CLK                               | |
 *  RF1.7 (RF3.8)   | BT_HCI_RX                              | aka SPI1_MISO/UART_TXD |                  
 *  RF1.9 (RF3.10)  | BT_HCI_TX                              | aka SPI1_MOSI/UART_RXD |
 *  RF1.10          | #BSP430_RFEM_GDO0_PORT_PERIPH_HANDLE   | |
 *  RF1.12          | #BSP430_RFEM_GDO2_PORT_PERIPH_HANDLE   | |
 *  RF1.14          | #BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE| |
 *  RF1.16          | #BSP430_RFEM_SPI0_PERIPH_HANDLE        | SCLK |
 *  RF1.18          | #BSP430_RFEM_SPI0_PERIPH_HANDLE        | MOSI |
 *  RF1.20          | #BSP430_RFEM_SPI0_PERIPH_HANDLE        | MISO |
 *  RF2.8           | BT_AUDIO_OUT                           | |
 *  RF2.10          | BT_AUDIO_IN                            | |
 *  RF2.11          | BT_AUDIO_FSYNC                         | |
 *  RF2.17          | BT_AUDIO_CLK                           | |
 *  RF2.18 (RF3.11) | BT_HCI_RTS, #BSP430_RFEM_GPIO3_PORT_PERIPH_HANDLE | aka SPI1_CLK
 * 
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_RFEM_H
#define BSP430_UTILITY_RFEM_H

#include <bsp430/core.h>

/** @def configBSP430_RFEM
 *
 * Define to a true value to indicate intent to use the RFEM
 * interface.  If supported by the platform, this will provide
 * definitions for the relevant ports and pins, and request the
 * relevant peripherals.
 *
 * @cppflag
 * @affects #BSP430_RFEM
 * @defaulted
 */
#ifndef configBSP430_RFEM
#define configBSP430_RFEM 0
#endif /* configBSP430_RFEM */

/* Most of what follows is documentation for definitions provided in
 * platform-specific headers. */
#if defined(BSP430_DOXYGEN)

/** @def BSP430_RFEM
 *
 * Indicate that the RFEM interface is available on the platform.
 * This is normally set by the platform-specific header included by
 * <bsp430/platform.h> when #configBSP430_RFEM is true.  If it is
 * unset or false, the platform does not support the RFEM interface
 * and RFEM identifiers in this module should not be referenced.
 *
 * This flag is defined only if #configBSP430_RFEM is true.
 *
 * @dependency #configBSP430_RFEM
 * @platformdefault */
#define BSP430_RFEM include <bsp430/platform.h>

/** BSP430 port peripheral for ChipCon radio GDO0 signal.
 *
 * This general digital output is often used to signal the MCU when
 * event occurs.
 *
 * @note The @link hBSP430halPORT HAL port interface @endlink for this
 * port will be enabled.
 *
 * @note On CC112x radios this is known as GPIO0.
 *
 * @platformdefault */
#define BSP430_RFEM_GDO0_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** Bit mask within #BSP430_RFEM_GDO0_PORT_PERIPH_HANDLE for GDO0 signal */
#define BSP430_RFEM_GDO0_PORT_BIT include <bsp430/platform.h>

/** BSP430 port peripheral for ChipCon radio GDO1 signal.
 *
 * This general digital output is often used to signal the MCU when
 * event occurs.  It shares a pin with the SPI interface MISO signal,
 * so is only available for its GPIO purpose when the radio interface
 * is not being used.
 *
 * @note The @link hBSP430halPORT HAL port interface @endlink for this
 * port will be enabled.
 *
 * @note On CC112x radios this is known as GPIO1.
 *
 * @platformdefault */
#define BSP430_RFEM_GDO1_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** Bit mask within #BSP430_RFEM_GDO1_PORT_PERIPH_HANDLE for GDO1 signal */
#define BSP430_RFEM_GDO1_PORT_BIT include <bsp430/platform.h>

/** BSP430 port peripheral for ChipCon radio GDO2 signal.
 *
 * This general digital output is often used to signal the MCU when
 * event occurs.
 *
 * @note The @link hBSP430halPORT HAL port interface @endlink for this
 * port will be enabled.
 *
 * @note On CC112x radios this is known as GPIO2.
 *
 * @platformdefault */
#define BSP430_RFEM_GDO2_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** Bit mask within #BSP430_RFEM_GDO2_PORT_PERIPH_HANDLE for GDO2 signal */
#define BSP430_RFEM_GDO2_PORT_BIT include <bsp430/platform.h>

/** BSP430 port peripheral for ChipCon 112x radio GPIO3 signal.
 *
 * This general digital output is often used to signal the MCU when
 * event occurs.
 *
 * @note The @link hBSP430halPORT HAL port interface @endlink for this
 * port will be enabled.
 *
 * @note There is no analogous signal on previous ChipCon radios
 *
 * @note If the platform does not support this signal, this identifier
 * will remain undefined.
 *
 * @platformdefault */
#define BSP430_RFEM_GPIO3_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** Bit mask within #BSP430_RFEM_GPIO3_PORT_PERIPH_HANDLE for GPIO3 signal */
#define BSP430_RFEM_GPIO3_PORT_BIT include <bsp430/platform.h>

/** BSP430 port peripheral for RFEM SPI0 chip select signal.
 *
 * @note Only the @link sBSP430hplPORT HPL interface @endlink is
 * enabled by its reference for this purpose.  The @link
 * hBSP430halPORT HAL port interface @endlink will be enabled only if
 * it is required for other signals on the same port.  In most cases,
 * the port for chip select is also used for a GDO signal, so the HAL
 * interface is almost certainly available.
 *
 * This pin enables the radio to operate on the SPI bus when set low.
 * When set high, #BSP430_RFEM_GDO1_PORT_PERIPH_HANDLE operates as a
 * signal from the radio to the MCU.
 *
 * @platformdefault */
#define BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** Bit mask within #BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE for the CSn signal */
#define BSP430_RFEM_SPI0CSn_PORT_BIT include <bsp430/platform.h>

/** The BSP430 peripheral handle for the primary SPI interface.
 *
 * On most evaluation boards, this corresponds to
 * #BSP430_PERIPH_USCI_B0 or its family-specific equivalent.
 *
 * @platformdefault */
#define BSP430_RFEM_SPI0_PERIPH_HANDLE include <bsp430/platform.h>

#endif /* BSP430_DOXYGEN */

/** Alias to GDO0 for CC112x radios */
#define BSP430_RFEM_GPIO0_PORT_PERIPH_HANDLE BSP430_RFEM_GDO0_PORT_PERIPH_HANDLE
/** Alias to GDO0 for CC112x radios */
#define BSP430_RFEM_GPIO0_PORT_BIT BSP430_RFEM_GDO0_PORT_BIT
/** Alias to GDO1 for CC112x radios */
#define BSP430_RFEM_GPIO1_PORT_PERIPH_HANDLE BSP430_RFEM_GDO1_PORT_PERIPH_HANDLE
/** Alias to GDO1 for CC112x radios */
#define BSP430_RFEM_GPIO1_PORT_BIT BSP430_RFEM_GDO1_PORT_BIT
/** Alias to GDO2 for CC112x radios */
#define BSP430_RFEM_GPIO2_PORT_PERIPH_HANDLE BSP430_RFEM_GDO2_PORT_PERIPH_HANDLE
/** Alias to GDO2 for CC112x radios */
#define BSP430_RFEM_GPIO2_PORT_BIT BSP430_RFEM_GDO2_PORT_BIT

#endif /* BSP430_UTILITY_RFEM_H */
