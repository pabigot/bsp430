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
 * @brief BSP430 HPL macros for signals corresponding to the CC1125, CC1121, CC1120, etc. radio interface
 *
 * The <a href="http://www.ti.com/product/cc1101">CC1125</a> and
 * related products (CC1121, CC1175, CC1200) are high performance RF
 * narrowband transceivers.  This header provides declarations
 * enabling BSP430 applications to access its resources through
 * platform-specific hardware presentation adaptation.
 *
 * The hardware presentation is probably identifical for the
 * CC112x/CC117x/CC120x variants of the radio.  This module probably
 * supports them all, with differences handled by software drivers.
 *
 * This interface is different from the previous low-power transceiver
 * line, which is supported by <bsp430/rf/cc110x.h>.
 *
 * If TI ever cleans up their model numbering, this header may be
 * renamed to be CC120x, parallel to CC110x.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RF_CC1125_H
#define BSP430_RF_CC1125_H

#include <bsp430/rf/rf.h>

/** Define to a true value to indicate intent to use a <a
 * href="http://www.ti.com/tool/cc1125dk">CC1125 RF Evaluation
 * Module</a> accessed through the RFEM header.
 *
 * A true value requests that <bsp430/platform.h> map
 * platform-specific RFEM resources as described in <bsp430/rf/rfem.h>
 * into the standard names described in the <bsp430/rf/cc1125.h>
 * header.
 *
 * Platforms that have a CC1125 radio hard-wired may instead request
 * this functionality through #configBSP430_PLATFORM_RF.
 *
 * @cppflag
 * @affects #BSP430_RF_CC1125
 * @defaulted
 */
#ifndef configBSP430_RF_CC1125EMK
#define configBSP430_RF_CC1125EMK 0
#endif /* configBSP430_RF_CC1125EMK */

#if defined(BSP430_DOXYGEN)
/** Defined to a true value if a CC1125 radio is expected to be
 * available to the application.
 *
 * Note that from the hardware interface perspective there is probably
 * no difference between a CC1120, CC1121, or CC1125.
 *
 * This may be enabled by #configBSP430_RF_CC1125EMK or
 * #configBSP430_PLATFORM_RF among other means. */
#define BSP430_RF_CC1125 include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN) || (BSP430_RF_CC1125 - 0)
/** SPI initialization constant required for CC1125 radio.
 *
 * This value should be provided as the argument for @a ctl0_byte to
 * hBSP430serialOpenSPI() when configuring SPI for a CC1125 radio.
 */
#define BSP430_RF_CC1125_SPI_CTL0_BYTE BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPH | UCMSB | UCMST)
#endif /* BSP430_RF_CC1125 */

#if defined(BSP430_DOXYGEN)

/** The @link #BSP430_PERIPH_CPPID_NONE CPPID@endlink associated with
 * the serial device serving as the SPI interface to the CC1125
 * radio. */
#define BSP430_RF_CC1125_SPI_PERIPH_CPPID include <bsp430/platform.h>

/** The serial peripheral serving as the SPI interface to the CC1125
 * radio. */
#define BSP430_RF_CC1125_SPI_PERIPH_HANDLE include <bsp430/platform.h>

/* -- Begin material from genrf */

/** BSP430 peripheral handle for port on which CSn is placed.
 *
 * The CSn signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC1125_CSn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO0 is placed.
 *
 * The GPIO0 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC1125_GPIO0_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO1 is placed.
 *
 * The GPIO1 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC1125_GPIO1_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO2 is placed.
 *
 * The GPIO2 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC1125_GPIO2_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO3 is placed.
 *
 * The GPIO3 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC1125_GPIO3_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which RESETn is placed.
 *
 * The RESETn signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC1125_RESETn_PORT_PERIPH_HANDLE include <bsp430/platform.h>
/* *** END OF CUSTOMIZED DECLARATIONS */


/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_CSn_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_CSn_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC1125_CSn_PORT_PERIPH_HANDLE for CSn.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC1125_CSn_PORT_BIT include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_GPIO0_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_GPIO0_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_GPIO0_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_GPIO0_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC1125_GPIO0_PORT_PERIPH_HANDLE for GPIO0.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC1125_GPIO0_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO0.
 *
 * This macro is defined only if GPIO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC1125_GPIO0_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC1125_GPIO0_TIMER_PERIPH_HANDLE
 * corresponding to GPIO0.
 *
 * This macro is defined only if GPIO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC1125_GPIO0_TIMER_CCIS
 */
#define BSP430_RF_CC1125_GPIO0_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC1125 GPIO0.
 *
 * This macro is defined only if GPIO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC1125_GPIO0_TIMER_CCIDX
 */
#define BSP430_RF_CC1125_GPIO0_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_GPIO1_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_GPIO1_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_GPIO1_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_GPIO1_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC1125_GPIO1_PORT_PERIPH_HANDLE for GPIO1.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC1125_GPIO1_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO1.
 *
 * This macro is defined only if GPIO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC1125_GPIO1_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC1125_GPIO1_TIMER_PERIPH_HANDLE
 * corresponding to GPIO1.
 *
 * This macro is defined only if GPIO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC1125_GPIO1_TIMER_CCIS
 */
#define BSP430_RF_CC1125_GPIO1_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC1125 GPIO1.
 *
 * This macro is defined only if GPIO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC1125_GPIO1_TIMER_CCIDX
 */
#define BSP430_RF_CC1125_GPIO1_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_GPIO2_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_GPIO2_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_GPIO2_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_GPIO2_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC1125_GPIO2_PORT_PERIPH_HANDLE for GPIO2.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC1125_GPIO2_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO2.
 *
 * This macro is defined only if GPIO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC1125_GPIO2_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC1125_GPIO2_TIMER_PERIPH_HANDLE
 * corresponding to GPIO2.
 *
 * This macro is defined only if GPIO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC1125_GPIO2_TIMER_CCIS
 */
#define BSP430_RF_CC1125_GPIO2_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC1125 GPIO2.
 *
 * This macro is defined only if GPIO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC1125_GPIO2_TIMER_CCIDX
 */
#define BSP430_RF_CC1125_GPIO2_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_GPIO3_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_GPIO3_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_GPIO3_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO3 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_GPIO3_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC1125_GPIO3_PORT_PERIPH_HANDLE for GPIO3.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC1125_GPIO3_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO3.
 *
 * This macro is defined only if GPIO3 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC1125_GPIO3_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC1125_GPIO3_TIMER_PERIPH_HANDLE
 * corresponding to GPIO3.
 *
 * This macro is defined only if GPIO3 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC1125_GPIO3_TIMER_CCIS
 */
#define BSP430_RF_CC1125_GPIO3_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC1125 GPIO3.
 *
 * This macro is defined only if GPIO3 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC1125_GPIO3_TIMER_CCIDX
 */
#define BSP430_RF_CC1125_GPIO3_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC1125_RESETn_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC1125_RESETn_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC1125_RESETn_PORT_PERIPH_HANDLE for RESETn.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC1125_RESETn_PORT_BIT include <bsp430/platform.h>
/* -- End material from genrf */

#endif /* BSP430_DOXYGEN */

#endif /* BSP430_RF_RFEM_H */
