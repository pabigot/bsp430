/* Copyright 2013, Peter A. Bigot
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
 * @brief Declarations related to platform-specific u8glib support
 *
 * <a href="http://code.google.com/p/u8glib/">Universal Graphics
 * Library for 8 Bit Embedded Systems</a> is portable library with
 * text and graphical support for LCDs commonly used on
 * microcontroller platforms.
 *
 * u8glib is highly influenced by Arduino, and adding another target
 * platform to the library itself would be messy; however, it's fairly
 * easy to create bridge support in each platform to provide the
 * interface from hardware to the library.  This module defines the
 * shared material related to the BSP430-provided interfaces.
 *
 * The majority of the peripheral resource identifiers and constants
 * are platform-specific;
 * e.g. #BSP430_PLATFORM_TRXEB_LCD_SPI_PERIPH_HANDLE.  The interface
 * for u8glib does not required that these be abstracted and made
 * available generically.
 *
 * Access to the platform-specific interface requires adding @c
 * u8g_bsp430.c to the set of source files compiled into the program,
 * along with the necessary pieces of u8glib itself including any
 * desired fonts.  Also include <tt>\$(PLATFORM_U8G_PB_C)</tt> which is
 * defined in the platform-specific @c Makefile.common to select the
 * correct low-level implementation based on the display orientation
 * and color support.
 *
 * @see @ref ex_utility_u8glib
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_U8GLIB_H
#define BSP430_UTILITY_U8GLIB_H

#include <bsp430/core.h>
#include <bsp430/serial.h>

/** Define to request that platform enable its U8GLIB adaptation.
 *
 * Some platforms have an on-board LCD for which a bridge to u8glib is
 * available.  Defining this macro in your <bsp430_config.h> will
 * register a request for that device.
 *
 * @cppflag
 * @affects #BSP430_UTILITY_U8GLIB
 * @defaulted
 */
#ifndef configBSP430_UTILITY_U8GLIB
#define configBSP430_UTILITY_U8GLIB 0
#endif /* configBSP430_UTILITY_U8GLIB */

/** Indicate that an U8GLIB LCD interface is available on the
 * platform.  This is set by the platform-specific header when
 * #configBSP430_UTILITY_U8GLIB is true and the platform supports an
 * U8GLIB device.
 *
 * If this define evaluates to false either the u8glib adaptation was
 * not requested, or there is none provided by the platform.
 *
 * See @ref ex_utility_u8glib for an example of use.
 *
 * @cppflag
 * @dependency #configBSP430_UTILITY_U8GLIB
 * @defaulted */
#if defined(BSP430_DOXYGEN)
#define BSP430_UTILITY_U8GLIB include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN) || (configBSP430_UTILITY_U8GLIB - 0)
/** @cond DOXYGEN_EXCLUDE */
#include <u8g.h>
/** @endcond */

/** The platform-provided device for the on-board LCD.
 *
 * @dependency #BSP430_UTILITY_U8GLIB
 */
extern u8g_dev_t xBSP430u8gDevice;
#endif /* BSP430_DOXYGEN */

/** Provide access to the SPI device used to communicate with the u8glib device.
 *
 * @return The SPI device for LCD communication.  Only valid after
 * u8g_Init() has been invoked.  If null is returned, the device is
 * not usable.
 */
hBSP430halSERIAL hBSP430u8gSPI (void);

#endif /* BSP430_UTILITY_U8GLIB_H */
