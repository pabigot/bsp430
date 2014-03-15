/* Copyright 2012-2014, Peter A. Bigot
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
 * @brief CC3000 Host Driver Interface for BSP430
 *
 * This file supports a Host Driver SPI interface for the <a
 * href="http://processors.wiki.ti.com/index.php/CC3000_Wi-Fi_for_MCU">Texas
 * Instruments SimpleLink(TM) CC3000 Wi-Fi SOC</a>.
 *
 * This file provides only the host driver interface.  The host driver
 * itself is a separate library provided by Texas Instruments.  A git
 * repository that repackages that driver in a format suitable for
 * linking with user applications is available at
 * http://github.com/pabigot/cc3000.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RF_CC3000_H
#define BSP430_RF_CC3000_H

#include <bsp430/platform.h>
#include <cc3000/cc3000_common.h>

#ifndef BSP430_CC3000_ENABLE_SMART
/** Define externally to a true value when you want the <a
 * href="http://processors.wiki.ti.com/index.php/CC3000_Smart_Config">CC3000
 * Smart Config</a> process to work.
 *
 * This feature requires that the RX buffer size be a full 1500
 * octets; setting this flag changes the default value for
 * #BSP430_CC3000SPI_RX_BUFFER_SIZE to satisfy that requirement.
 *
 * @defaulted
 * @affects #BSP430_CC3000SPI_RX_BUFFER_SIZE */
#define BSP430_CC3000_ENABLE_SMART 0
#endif /* BSP430_CC3000_ENABLE_SMART */

/** Size of the cc3000spi receive buffer, in octets.
 *
 * The value is an integer between #CC3000_MINIMAL_RX_SIZE=119 and
 * #CC3000_MAXIMAL_RX_SIZE=1520, unless #CC3000_TINY_DRIVER is defined
 * in which case #CC3000_MAXIMAL_RX_SIZE is 44.
 *
 * <cc3000/cc3000_common.h> unconditionally defines a
 * CC3000_RX_BUFFER_SIZE constant, but it is not referenced by the
 * driver and is not relevant to this implementation.
 *
 * @note The current (SP 1.10.1 and later) <a
 * href="http://processors.wiki.ti.com/index.php/CC3000_Smart_Config">CC3000
 * Smart Config</a> process works by encoding data in the length of
 * UDP packets transmitted to/from port 15000 of the gateway.  It is
 * necessary to have at least the RX buffer of the CC3000 at full size
 * in order to receive this information.  (Documentation says use
 * 1500; I have observed packets that are 1498 octets, so this is
 * probably accurate.)
 *
 * @defaulted
 * @dependency #BSP430_CC3000SPI_RX_BUFFER_SIZE
 */
#ifndef BSP430_CC3000SPI_RX_BUFFER_SIZE
#if (BSP430_CC3000_ENABLE_SMART - 0)
#define BSP430_CC3000SPI_RX_BUFFER_SIZE 1500
#else /* BSP430_CC3000_ENABLE_SMART */
#define BSP430_CC3000SPI_RX_BUFFER_SIZE 400
#endif /* BSP430_CC3000_ENABLE_SMART */
#endif /* BSP430_CC3000SPI_RX_BUFFER_SIZE */

/** Size of the cc3000spi transmit buffer, in octets.
 *
 * The value is an integer between #CC3000_MINIMAL_TX_SIZE=119 and
 * #CC3000_MAXIMAL_TX_SIZE=1520, unless #CC3000_TINY_DRIVER is defined
 * in which case #CC3000_MAXIMAL_TX_SIZE is 59.
 *
 * <cc3000/cc3000_common.h> unconditionally defines a
 * CC3000_TX_BUFFER_SIZE constant, but it is not referenced by the
 * driver and is not relevant to this implementation.
 *
 * @defaulted
 */
#ifndef BSP430_CC3000SPI_TX_BUFFER_SIZE
#define BSP430_CC3000SPI_TX_BUFFER_SIZE 400
#endif /* BSP430_CC3000SPI_TX_BUFFER_SIZE */

/** Define to a true value to indicate intent to use a <a
 * href="http://www.ti.com/tool/cc3000boost">CC3000BOOST</a>
 * boosterpack.
 *
 * This selects the boosterpack pin mapping in preference to the
 * default RFEM pin mapping.
 *
 * @cppflag
 * @defaulted
 */
#ifndef configBSP430_RF_CC3000BOOST
#define configBSP430_RF_CC3000BOOST 0
#endif /* configBSP430_RF_CC3000BOOST */

#if defined(BSP430_DOXYGEN)

/** The @link #BSP430_PERIPH_CPPID_NONE CPPID@endlink associated with
 * the serial device serving as the SPI interface to the CC3000
 * radio. */
#define BSP430_RF_CC3000_SPI_PERIPH_CPPID include <bsp430/platform.h>

/** The serial peripheral serving as the SPI interface to the CC3000
 * radio. */
#define BSP430_RF_CC3000_SPI_PERIPH_HANDLE include <bsp430/platform.h>

/* -- Begin material from genrf */

/** BSP430 peripheral handle for port on which CSn is placed.
 *
 * The CSn signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC3000_CSn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which IRQn is placed.
 *
 * The IRQn signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC3000_IRQn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which PWR_EN is placed.
 *
 * The PWR_EN signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC3000_PWR_EN_PORT_PERIPH_HANDLE include <bsp430/platform.h>
/* *** END OF CUSTOMIZED DECLARATIONS */


/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC3000_CSn_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC3000_CSn_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC3000_CSn_PORT_PERIPH_HANDLE for CSn.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC3000_CSn_PORT_BIT include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC3000_IRQn_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC3000_IRQn_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC3000_IRQn_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if IRQn is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC3000_IRQn_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC3000_IRQn_PORT_PERIPH_HANDLE for IRQn.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC3000_IRQn_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to IRQn.
 *
 * This macro is defined only if IRQn is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC3000_IRQn_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC3000_IRQn_TIMER_PERIPH_HANDLE
 * corresponding to IRQn.
 *
 * This macro is defined only if IRQn is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC3000_IRQn_TIMER_CCIS
 */
#define BSP430_RF_CC3000_IRQn_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC3000 IRQn.
 *
 * This macro is defined only if IRQn is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC3000_IRQn_TIMER_CCIDX
 */
#define BSP430_RF_CC3000_IRQn_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC3000_PWR_EN_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC3000_PWR_EN_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC3000_PWR_EN_PORT_PERIPH_HANDLE for PWR_EN.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC3000_PWR_EN_PORT_BIT include <bsp430/platform.h>

/* -- End material from genrf */

#endif /* BSP430_DOXYGEN */

/** BSP430 wrapper around CC3000 wlan_init().
 *
 * This interface provides access to user-level callbacks, and adds
 * the driver-level callbacks internally.
 *
 * @note While this function may be called with interrupts disabled,
 * it does nothing but store some configuration information.  All
 * other CC3000 operations including wlan_start() should be called
 * with interrupts enabled.
 *
 * @param wlan_cb see wlan_init() documentation.
 *
 * @param firmware_patch_fn see wlan_init() documentation.  Passing @c
 * NULL is generally appropriate.
 *
 * @param driver_patch_fn see wlan_init() documentation.  Passing @c
 * NULL is generally appropriate.
 *
 * @param boot_loader_patch_fn see wlan_init() documentation.  Passing
 * @c NULL is generally appropriate.
 *
 * @return 0 if all goes well; -1 if one of the underlying BSP430
 * resources could not be obtained.
 */
int iBSP430cc3000spiInitialize (tWlanCB wlan_cb,
                                tFWPatches firmware_patch_fn,
                                tDriverPatches driver_patch_fn,
                                tBootLoaderPatches boot_loader_patch_fn);

#endif /* BSP430_RF_CC3000_H */
