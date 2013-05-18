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
 * @brief BSP430 HPL macros for signals corresponding to the CC2520 radio interface
 *
 * The <a href="http://www.ti.com/product/cc2520">CC2520</a> is a
 * 2.4GHz RF transceiver supporting ZigBee/IEEE 802.15.4 wireless
 * applications.  This header provides declarations enabling BSP430
 * applications to access its resources through platform-specific
 * hardware presentation adaptation.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RF_CC2520_H
#define BSP430_RF_CC2520_H

#include <bsp430/core.h>

/** @def configBSP430_RF_CC2520EMK
 *
 * Define to a true value to indicate intent to use a <a
 * href="http://www.ti.com/tool/cc2500emk">CC2520 RF Evaluation
 * Module</a> accessed through the RFEM header.
 *
 * A true value requests that <bsp430/platform.h> map
 * platform-specific RFEM resources as described in <bsp430/rf/rfem.h>
 * into the standard names described in the <bsp430/rf/cc2500.h>
 * header.
 *
 * Platforms that have a CC2520 radio hard-wired may instead request
 * this functionality through #configBSP430_PLATFORM_RF.
 *
 * @cppflag
 * @affects #BSP430_RF_CC2520
 * @defaulted
 */
#ifndef configBSP430_RF_CC2520EMK
#define configBSP430_RF_CC2520EMK 0
#endif /* configBSP430_RF_CC2520EMK */

#if defined(BSP430_DOXYGEN)
/** Defined to a true value if a CC2520 radio is expected to be
 * available to the application.
 *
 * This may be enabled by #configBSP430_RF_CC2520EMK or
 * #configBSP430_PLATFORM_RF among other means. */
#define BSP430_RF_CC2520 include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN) || (BSP430_RF_CC2520 - 0)
/** SPI initialization constant required for CC2520 radio.
 *
 * This value should be provided as the argument for @a ctl0_byte to
 * hBSP430serialOpenSPI() when configuring SPI for a CC2520 radio.
 */
#define BSP430_RF_CC2520_SPI_CTL0_BYTE BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPH | UCMSB | UCMST)
#endif /* BSP430_RF_CC2520 */

#if defined(BSP430_DOXYGEN)

/* -- Begin material from genrf */

/** BSP430 peripheral handle for port on which CSn is placed.
 *
 * The CSn signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_CSn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO0 is placed.
 *
 * The GPIO0 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_GPIO0_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO1 is placed.
 *
 * The GPIO1 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_GPIO1_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO2 is placed.
 *
 * The GPIO2 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_GPIO2_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO3 is placed.
 *
 * The GPIO3 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_GPIO3_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO4 is placed.
 *
 * The GPIO4 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_GPIO4_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GPIO5 is placed.
 *
 * The GPIO5 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_GPIO5_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which RESETn is placed.
 *
 * The RESETn signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_RESETn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which SO is placed.
 *
 * The SO signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2520_SO_PORT_PERIPH_HANDLE include <bsp430/platform.h>
/* *** END OF CUSTOMIZED DECLARATIONS */


/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_CSn_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_CSn_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_CSn_PORT_PERIPH_HANDLE for CSn.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_CSn_PORT_BIT include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO0_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO0_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO0_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO0_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_GPIO0_PORT_PERIPH_HANDLE for GPIO0.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_GPIO0_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO0.
 *
 * This macro is defined only if GPIO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2520_GPIO0_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2520_GPIO0_TIMER_PERIPH_HANDLE
 * corresponding to GPIO0.
 *
 * This macro is defined only if GPIO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO0_TIMER_CCIS
 */
#define BSP430_RF_CC2520_GPIO0_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2520 GPIO0.
 *
 * This macro is defined only if GPIO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO0_TIMER_CCIDX
 */
#define BSP430_RF_CC2520_GPIO0_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO1_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO1_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO1_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO1_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_GPIO1_PORT_PERIPH_HANDLE for GPIO1.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_GPIO1_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO1.
 *
 * This macro is defined only if GPIO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2520_GPIO1_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2520_GPIO1_TIMER_PERIPH_HANDLE
 * corresponding to GPIO1.
 *
 * This macro is defined only if GPIO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO1_TIMER_CCIS
 */
#define BSP430_RF_CC2520_GPIO1_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2520 GPIO1.
 *
 * This macro is defined only if GPIO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO1_TIMER_CCIDX
 */
#define BSP430_RF_CC2520_GPIO1_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO2_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO2_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO2_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO2_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_GPIO2_PORT_PERIPH_HANDLE for GPIO2.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_GPIO2_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO2.
 *
 * This macro is defined only if GPIO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2520_GPIO2_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2520_GPIO2_TIMER_PERIPH_HANDLE
 * corresponding to GPIO2.
 *
 * This macro is defined only if GPIO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO2_TIMER_CCIS
 */
#define BSP430_RF_CC2520_GPIO2_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2520 GPIO2.
 *
 * This macro is defined only if GPIO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO2_TIMER_CCIDX
 */
#define BSP430_RF_CC2520_GPIO2_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO3_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO3_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO3_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO3 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO3_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_GPIO3_PORT_PERIPH_HANDLE for GPIO3.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_GPIO3_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO3.
 *
 * This macro is defined only if GPIO3 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2520_GPIO3_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2520_GPIO3_TIMER_PERIPH_HANDLE
 * corresponding to GPIO3.
 *
 * This macro is defined only if GPIO3 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO3_TIMER_CCIS
 */
#define BSP430_RF_CC2520_GPIO3_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2520 GPIO3.
 *
 * This macro is defined only if GPIO3 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO3_TIMER_CCIDX
 */
#define BSP430_RF_CC2520_GPIO3_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO4_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO4_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO4_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO4 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO4_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_GPIO4_PORT_PERIPH_HANDLE for GPIO4.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_GPIO4_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO4.
 *
 * This macro is defined only if GPIO4 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2520_GPIO4_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2520_GPIO4_TIMER_PERIPH_HANDLE
 * corresponding to GPIO4.
 *
 * This macro is defined only if GPIO4 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO4_TIMER_CCIS
 */
#define BSP430_RF_CC2520_GPIO4_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2520 GPIO4.
 *
 * This macro is defined only if GPIO4 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO4_TIMER_CCIDX
 */
#define BSP430_RF_CC2520_GPIO4_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO5_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO5_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_GPIO5_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GPIO5 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_GPIO5_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_GPIO5_PORT_PERIPH_HANDLE for GPIO5.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_GPIO5_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GPIO5.
 *
 * This macro is defined only if GPIO5 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2520_GPIO5_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2520_GPIO5_TIMER_PERIPH_HANDLE
 * corresponding to GPIO5.
 *
 * This macro is defined only if GPIO5 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO5_TIMER_CCIS
 */
#define BSP430_RF_CC2520_GPIO5_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2520 GPIO5.
 *
 * This macro is defined only if GPIO5 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2520_GPIO5_TIMER_CCIDX
 */
#define BSP430_RF_CC2520_GPIO5_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_RESETn_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_RESETn_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_RESETn_PORT_PERIPH_HANDLE for RESETn.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_RESETn_PORT_BIT include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2520_SO_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2520_SO_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2520_SO_PORT_PERIPH_HANDLE for SO.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2520_SO_PORT_BIT include <bsp430/platform.h>

/* -- End material from genrf */

#endif /* BSP430_DOXYGEN */

#endif /* BSP430_RF_RFEM_H */
