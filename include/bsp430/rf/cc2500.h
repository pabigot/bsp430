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
 * @brief BSP430 HPL macros for signals corresponding to the CC2500 radio interface
 *
 * The <a href="http://www.ti.com/product/cc2500">CC2500</a> is a
 * 2.4GHz RF transceiver designed for low-power wireless applications
 * in the 2.4GHz ISM band.  This header provides declarations enabling
 * BSP430 applications to access its resources through
 * platform-specific hardware presentation adaptation.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RF_CC2500_H
#define BSP430_RF_CC2500_H

#include <bsp430/core.h>

/** @def configBSP430_RF_CC2500EMK
 *
 * Define to a true value to indicate intent to use a <a
 * href="http://www.ti.com/tool/cc2500emk">CC2500 RF Evaluation
 * Module</a> accessed through the RFEM header.
 *
 * A true value requests that <bsp430/platform.h> map
 * platform-specific RFEM resources as described in <bsp430/rf/rfem.h>
 * into the standard names described in the <bsp430/rf/cc2500.h>
 * header.
 *
 * Platforms that have a CC2500 radio hard-wired may instead request
 * this functionality through #configBSP430_PLATFORM_RF.
 *
 * @cppflag
 * @affects #BSP430_RF_CC2500
 * @defaulted
 */
#ifndef configBSP430_RF_CC2500EMK
#define configBSP430_RF_CC2500EMK 0
#endif /* configBSP430_RF_CC2500EMK */

#if defined(BSP430_DOXYGEN)
/** Defined to a true value if a CC2500 radio is expected to be
 * available to the application.
 *
 * This may be enabled by #configBSP430_RF_CC2500EMK or
 * #configBSP430_PLATFORM_RF among other means. */
#define BSP430_RF_CC2500 include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

#if defined(BSP430_DOXYGEN) || (BSP430_RF_CC2500 - 0)
/** SPI initialization constant required for CC2500 radio.
 *
 * This value should be provided as the argument for @a ctl0_byte to
 * hBSP430serialOpenSPI() when configuring SPI for a CC2500 radio.
 */
#define BSP430_RF_CC2500_SPI_CTL0_BYTE BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPH | UCMSB | UCMST)
#endif /* BSP430_RF_CC2500 */

#if defined(BSP430_DOXYGEN)

/* -- Begin material from genrf */

/** BSP430 peripheral handle for port on which CSn is placed.
 *
 * The CSn signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2500_CSn_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GDO0 is placed.
 *
 * The GDO0 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2500_GDO0_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GDO1 is placed.
 *
 * The GDO1 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2500_GDO1_PORT_PERIPH_HANDLE include <bsp430/platform.h>

/** BSP430 peripheral handle for port on which GDO2 is placed.
 *
 * The GDO2 signal is ***FILL IN HERE***
 */
#define BSP430_RF_CC2500_GDO2_PORT_PERIPH_HANDLE include <bsp430/platform.h>
/* *** END OF CUSTOMIZED DECLARATIONS */


/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2500_CSn_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2500_CSn_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2500_CSn_PORT_PERIPH_HANDLE for CSn.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2500_CSn_PORT_BIT include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2500_GDO0_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2500_GDO0_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2500_GDO0_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GDO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2500_GDO0_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2500_GDO0_PORT_PERIPH_HANDLE for GDO0.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2500_GDO0_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GDO0.
 *
 * This macro is defined only if GDO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2500_GDO0_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2500_GDO0_TIMER_PERIPH_HANDLE
 * corresponding to GDO0.
 *
 * This macro is defined only if GDO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2500_GDO0_TIMER_CCIS
 */
#define BSP430_RF_CC2500_GDO0_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2500 GDO0.
 *
 * This macro is defined only if GDO0 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2500_GDO0_TIMER_CCIDX
 */
#define BSP430_RF_CC2500_GDO0_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2500_GDO1_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2500_GDO1_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2500_GDO1_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GDO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2500_GDO1_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2500_GDO1_PORT_PERIPH_HANDLE for GDO1.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2500_GDO1_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GDO1.
 *
 * This macro is defined only if GDO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2500_GDO1_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2500_GDO1_TIMER_PERIPH_HANDLE
 * corresponding to GDO1.
 *
 * This macro is defined only if GDO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2500_GDO1_TIMER_CCIS
 */
#define BSP430_RF_CC2500_GDO1_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2500 GDO1.
 *
 * This macro is defined only if GDO1 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2500_GDO1_TIMER_CCIDX
 */
#define BSP430_RF_CC2500_GDO1_TIMER_CCIS include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2500_GDO2_PORT_PERIPH_HANDLE.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2500_GDO2_PORT_PERIPH_CPPID include <bsp430/platform.h>

/** Preprocessor-compatible identifier corresponding to #BSP430_RF_CC2500_GDO2_TIMER_PERIPH_HANDLE.
 *
 * This macro is defined only if GDO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_PERIPH_CPPID_NONE
 */
#define BSP430_RF_CC2500_GDO2_TIMER_PERIPH_CPPID include <bsp430/platform.h>

/** Port bit on #BSP430_RF_CC2500_GDO2_PORT_PERIPH_HANDLE for GDO2.
 *
 * This is the bit mask, not a bit position.
 */
#define BSP430_RF_CC2500_GDO2_PORT_BIT include <bsp430/platform.h>

/** BSP430 peripheral handle for timer connected to GDO2.
 *
 * This macro is defined only if GDO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 */
#define BSP430_RF_CC2500_GDO2_TIMER_PERIPH_HANDLE include <bsp430/platform.h>

/** The capture/compare index on #BSP430_RF_CC2500_GDO2_TIMER_PERIPH_HANDLE
 * corresponding to GDO2.
 *
 * This macro is defined only if GDO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2500_GDO2_TIMER_CCIS
 */
#define BSP430_RF_CC2500_GDO2_TIMER_CCIDX include <bsp430/platform.h>

/** The capture/compare input selector necessary to capture any signal
 * changes associated with CC2500 GDO2.
 *
 * This macro is defined only if GDO2 is connected to a port that
 * supports capture of a timer counter based on signal changes.
 *
 * @see #BSP430_RF_CC2500_GDO2_TIMER_CCIDX
 */
#define BSP430_RF_CC2500_GDO2_TIMER_CCIS include <bsp430/platform.h>

/* -- End material from genrf */

#endif /* BSP430_DOXYGEN */

#endif /* BSP430_RF_RFEM_H */
