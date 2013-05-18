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
 *
 * @brief RF Evaluation Module connector support
 *
 * Many MSP430 evaluation boards support an RF interface, often in the
 * form of the RF1/RF2 Evaluation Module connector and/or its
 * companion RF3 header (also known as the eZ-RF header).  This header
 * describes standard names associating the platform-specific GPIO
 * ports and secondary functions with each pin.  These in turn can be
 * used to associate signals from specific RF EMK boards (e.g. the
 * CC1101EMK or CC2520EMK) with the MCU pins using the RFEM pins as a
 * bridge.
 *
 * Although the standard RFEM connector comprises two 20-pin header
 * blocks, not all EM boards provide connections for all pins.
 *
 * For simplicity only pin 20 on the RF1 header is fully described
 * here.  The remaining 19 pins on RF1, 20 pins on RF2, and 18 pins on
 * RF3 are used analogously.
 *
 * In standard SPI usage, SCLK is in RF1P16, SI is RF1P18, SO is
 * RF1P20, and CSn is RF1P14.  The platform-specific serial device
 * associated with these RFEM pins is
 * #BSP430_RFEM_SERIAL_PERIPH_HANDLE.
 *
 * @note It is never necessary to include this file directly.  It
 * serves entirely as a point of documentation for macros that are
 * defined through <bsp430/platform.h>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RF_RFEM_H
#define BSP430_RF_RFEM_H

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

/** The @link #BSP430_PERIPH_CPPID_NONE CPPID@endlink associated with
 * the serial device serving as the primary SPI interface on the RF
 * headers. */
#define BSP430_RFEM_SERIAL_PERIPH_CPPID include <bsp430/platform.h>

/** The serial peripheral serving as the primary SPI interface on the RF
 * headers. */
#define BSP430_RFEM_SERIAL_PERIPH_HANDLE include <bsp430/platform.h>

/** The @link #BSP430_PERIPH_CPPID_NONE CPPID@endlink associated with
 * the port peripheral controlling pin 20 on the RF1 header on a
 * target board.
 *
 * Defined analogously by each platform for all connected pins on RF1,
 * RF2, and RF3.  If the macro for a specific header pin is not
 * defined by a platform, that header pin is not connected to the MCU.
 */
#define BSP430_RFEM_RF1P20_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** A specific port handle, such as #BSP430_PERIPH_PORT1, associated
 * with the port peripheral controlling pin 20 on the RF1 header on a
 * target board.
 *
 * Defined analogously by each platform for all connected pins on RF1,
 * RF2, and RF3.  If the macro for a specific header pin is not
 * defined by a platform, that header pin is not connected to the MCU.
 *
 * @see #BSP430_RFEM_RF1P20_PORT_BIT.
 */
#define BSP430_RFEM_RF1P20_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** The port bit (mask) associated with the GPIO port peripheral pin
 * connected to RF1 header pin 20 on a target board.
 *
 * Defined analogously by each platform for all connected pins on RF1,
 * RF2, and RF3.  If the macro for a specific header pin is not
 * defined by a platform, that header pin is not connected to the MCU.
 *
 * @see #BSP430_RFEM_RF1P20_PORT_PERIPH_HANDLE.
 */
#define BSP430_RFEM_RF1P20_PORT_BIT include <bsp430/platform.h>

/** The @link #BSP430_PERIPH_CPPID_NONE CPPID@endlink associated with a
 * timer peripheral available as the primary peripheral function on
 * pin 20 on the RF1 header on a target board.
 *
 * Defined analogously by each platform for all connected pins on RF1,
 * RF2, and RF3.  If the macro for a specific header pin is not
 * defined by a platform, that header pin is not connected to a timer
 * peripheral on the MCU.
 *
 * @see #BSP430_RFEM_RF1P20_TIMER_PERIPH_HANDLE
 */
#define BSP430_RFEM_RF1P20_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** A specific timer handle, such as #BSP430_PERIPH_TA0, associated
 * with a timer peripheral available as the primary peripheral
 * function on pin 20 on the RF1 header on a target board.
 *
 * ("primary peripheral function" means that the corresponding
 * #BSP430_RFEM_RF1P20_PORT_BIT is set on the @link
 * sBSP430hplPORT_5XX_8::sel sel@endlink register of the
 * #BSP430_RFEM_RF1P20_PORT_PERIPH_HANDLE port peripheral associated
 * with the RF header pin, while that bit is cleared on any secondary
 * selection registers such as @link sBSP430hplPORT_5XX_8::sel2
 * sel2@endlink.)
 *
 * Defined analogously by each platform for all connected pins on RF1,
 * RF2, and RF3.  If the macro for a specific header pin is not
 * defined by a platform, that header pin is not connected to a timer
 * peripheral on the MCU.
 */
#define BSP430_RFEM_RF1P20_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on
 * #BSP430_RFEM_RF1P20_TIMER_PERIPH_HANDLE that is connected to pin 20
 * on the RF1 header of a target board.
 *
 * Defined analogously by each platform for all connected pins on RF1,
 * RF2, and RF3.  If the macro for a specific header pin is not
 * defined by a platform, that header pin is not connected to a timer
 * peripheral on the MCU.
 *
 * @see #BSP430_RFEM_RF1P20_TIMER_PERIPH_HANDLE
 */
#define BSP430_RFEM_RF1P20_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture on
 * #BSP430_RFEM_RF1P20_TIMER_PERIPH_HANDLE any signal changes
 * associated with pin 20 on the RF1 header of a target board.
 *
 * Defined analogously by each platform for all connected pins on RF1,
 * RF2, and RF3.  If the macro for a specific header pin is not
 * defined by a platform, that header pin is not connected to a timer
 * peripheral on the MCU.
 *
 * @see #BSP430_RFEM_RF1P20_TIMER_PERIPH_HANDLE
 * @see #BSP430_RFEM_RF1P20_TIMER_CCIDX
 */
#define BSP430_RFEM_RF1P20_TIMER_CCIS include <bsp430/platform.h>

#endif /* BSP430_DOXYGEN */

#endif /* BSP430_RF_RFEM_H */
