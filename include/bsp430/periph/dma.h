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
 * @brief Hardware presentation/abstraction for DMA (DMA/DMAX)
 *
 * The Direct Memory Access (DMA) module is present on certain MCUs in
 * all MSP430 families.  The functionality is essentially the same
 * across the line, but the register layout is different on CPUX-based
 * devices where addresses require 20 bits.
 *
 * @section h_periph_dma_opt Module Configuration Options
 *
 * None supported.
 *
 * @section h_periph_dma_hpl Hardware Presentation Layer
 *
 * There is only one instance of DMA on any MCU, but because that
 * instance can support multiple channels the #sBSP430hplDMA structure
 * is defined to simplify access to the #sBSP430hplDMAchannel
 * registers for a channel index.
 *
 * @section h_periph_dma_hal Hardware Adaptation Layer
 *
 * The #sBSP430halDMA structure exists primarily to enable sharing the
 * DMA interrupt infrastructure among independently maintained
 * modules.
 *
 * There are no value-added functions for this peripheral.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_DMA_H
#define BSP430_PERIPH_DMA_H

#include <bsp430/periph.h>

/** Defined to true if DMAX peripheral is supported.
 *
 * Defined on inclusion of <bsp430/periph/dma.h> based on MCU-specific
 * flags.  The value evaluates to true if the target MCU supports the
 * DMAX peripheral, and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_DMAX (defined(__MSP430_HAS_DMAX_3__)       \
                            || defined(__MSP430_HAS_DMAX_6__))

/** Defined to true if any DMA peripheral is supported.
 *
 * Defined on inclusion of <bsp430/periph/dma.h> based on MCU-specific
 * flags.  The value evaluates to true if the target MCU supports the
 * DMA or DMAX peripheral, and false if it does not.
 *
 * @cppflag
 */

#define BSP430_MODULE_DMA (defined(__MSP430_HAS_DMA_1__)        \
                           || defined(__MSP430_HAS_DMA_6__)     \
                           || (BSP430_MODULE_DMAX - 0))

/* Only provide declarations if module is supported */
#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_DMA - 0)

/* !BSP430! periph=dma instance=dma */

/** Field value for sBSP430halDMA.hal_state.cflags when using a DMA peripheral.
 *
 * This is informational only and can be used to distinguish the MCU
 * from those with the later DMAX peripheral. */
#define BSP430_DMA_HAL_HPL_VARIANT_DMA 1

/** Field value for sBSP430halDMA.hal_state.cflags when using a DMAX peripheral.
 *
 * This is informational only and can be used to distinguish the MCU
 * from those with the earlier DMA peripheral. */
#define BSP430_DMA_HAL_HPL_VARIANT_DMAX 2

/** Number of DMA channels supported.
 *
 * This is an MCU-specific value. */
#if defined(__MSP430_HAS_DMA_1__)
#define BSP430_DMA_NUM_CHANNELS 1
#elif defined(BSP430_DOXYGEN) || defined(__MSP430_HAS_DMA_3__) || defined(__MSP430_HAS_DMAX_3__)
#define BSP430_DMA_NUM_CHANNELS 3
#elif defined(__MSP430_HAS_DMAX_6__)
#define BSP430_DMA_NUM_CHANNELS 6
#endif /* MCU DMA */

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_DMAX - 0)

/** Peripheral map for DMAX channel register sets.
 *
 * These are only used on CPUX MCUs, and support 20-bit addresses. */
typedef struct sBSP430hplDMAchannel {
  unsigned int ctl;             /**< Channel control : 0x00 */
  unsigned long sa;             /**< Source address: 0x02 */
  unsigned long da;             /**< Destination address: 0x06 */
  unsigned int sz;              /**< Size of transfer: 0x0a */
  unsigned int _reserved_x0c;
  unsigned int _reserved_x0e;
} sBSP430hplDMAchannel;

#if (BSP430_CORE_FAMILY_IS_5XX - 0)
/** Peripheral register map for 5xx and later family MCUs */
typedef struct sBSP430hplDMA {
  unsigned int ctl0;            /**< Control for channels 0 and 1 : 0x00 */
  unsigned int ctl1;            /**< Control for channels 2 and 3 : 0x02 */
  unsigned int ctl2;            /**< Control for channels 4 and 5 : 0x04 */
  unsigned int ctl3;            /**< Control for channels 6 and 7 : 0x06*/
  unsigned int ctl4;            /**< General peripheral configuration : 0x08 */
  unsigned int _reserved_x0a;
  unsigned int _reserved_x0c;
  unsigned int iv;              /**< Interrupt vector word : 0x0e */
  sBSP430hplDMAchannel ch[BSP430_DMA_NUM_CHANNELS];
} sBSP430hplDMA;
#else /* BSP430_CORE_FAMILY_IS_5XX */
/** Peripheral register map for 2xx/4xx MCUs with DMAX capability. */
typedef struct sBSP430hplDMA {
  unsigned int ctl0;            /**< Control for channels 0 and 1 : 0x00 */
  unsigned int ctl1;            /**< Control for channels 2 and 3 : 0x02 */
  unsigned int iv;              /**< Interrupt vector word : 0x04 */
  unsigned int _reserved_skip[(0x1D0 - 0x128) / sizeof(unsigned int)];
  sBSP430hplDMAchannel ch[BSP430_DMA_NUM_CHANNELS]; /**< HPL for each supported channel */
} sBSP430hplDMA;
#endif /* BSP430_CORE_FAMILY_IS_5XX */

#else /* BSP430_MODULE_DMAX */

/** DMA channel registers for 1xx and FG43x MCUs */
typedef struct sBSP430hplDMAchannel {
  unsigned int ctl;             /**< Channel control: 0x00 */
  unsigned int sa;              /**< Source address: 0x02 */
  unsigned int da;              /**< Destination address: 0x04 */
  unsigned int sz;              /**< Size of transfer: 0x06 */
} sBSP430hplDMAchannel;

/** Peripheral register map for 1xx and FG43x MCUs */
typedef struct sBSP430hplDMA {
  unsigned int ctl0;            /**< Trigger select control : 0x00 */
  unsigned int ctl1;            /**< General peripheral configuration: 0x02 */
  unsigned int _reserved_skip[(0x1E0 - 0x126) / sizeof(unsigned int)];
  sBSP430hplDMAchannel ch[DMA_NUM_CHANNELS]; /**< HPL for each supported channel */
} sBSP430hplDMA;

#endif /* BSP430_MODULE_DMAX */

/** @cond DOXYGEN_INTERNAL
 *
 * Standard technique for locating the peripheral registers. */
#if defined(BSP430_DOXYGEN) || defined(__MSP430_HAS_DMA_1__) || defined(__MSP430_HAS_DMA_3__)
#define BSP430_PERIPH_DMA_BASEADDRESS_ 0x0122
#elif defined(__MSP430_HAS_DMAX_3__)
#if defined(__MSP430_BASEADDRESS_DMAX_3__)
#define BSP430_PERIPH_DMA_BASEADDRESS_ __MSP430_BASEADDRESS_DMAX_3__
#else /* __MSP430_BASEADDRESS_DMAX_3__ */
#define BSP430_PERIPH_DMA_BASEADDRESS_ 0x0122
#endif /* __MSP430_BASEADDRESS_DMAX_3__ */
#elif defined(__MSP430_HAS_DMAX_6__)
#define BSP430_PERIPH_DMA_BASEADDRESS_ __MSP430_BASEADDRESS_DMAX_6__
#endif /* MCU DMA */
/** @endcond */

/** Structure holding hardware abstraction layer for the DMA peripheral. */
typedef struct sBSP430halDMA {
  /** Common header.  This really isn't necessary for this peripheral
   * since there is only one variant on any given MCU, but consistency
   * is important and future enhancements might make use of the flags
   * field. */
  sBSP430hplHALStatePrefix hal_state;

  /** The underlying DMA peripheral register structure */
  volatile sBSP430hplDMA * const hpl;

  /** The callback chain to invoke when a channel interrupt is received. */
  const struct sBSP430halISRIndexedChainNode * volatile * const ch_cbchain_ni;
} sBSP430halDMA;

/** Mild obscuration of the HAL internal structure */
typedef struct sBSP430halDMA * hBSP430halDMA;

/* !BSP430! insert=hal_decl with_lookup=0 */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** Control inclusion of the @HAL interface to #BSP430_PERIPH_DMA
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halDMA structure reference is
 * available as #BSP430_HAL_DMA.
 *
 * @cppflag
 * @affects #configBSP430_HPL_DMA is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_DMA
#define configBSP430_HAL_DMA 0
#endif /* configBSP430_HAL_DMA */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_DMA - 0)
/* You do not need to know about this */
extern sBSP430halDMA xBSP430hal_DMA_;
#endif /* configBSP430_HAL_DMA */
/** @endcond */

/** sBSP430halDMA HAL handle for #BSP430_PERIPH_DMA.
 *
 * This pointer may be used only if #configBSP430_HAL_DMA
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_DMA */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_DMA - 0)
#define BSP430_HAL_DMA (&xBSP430hal_DMA_)
#endif /* configBSP430_HAL_DMA */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! insert=periph_decl with_lookup=0 */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** Control inclusion of the @HPL interface to #BSP430_PERIPH_DMA
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplDMA structure reference is
 * available as #BSP430_HPL_DMA.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_DMA
#define configBSP430_HPL_DMA (configBSP430_HAL_DMA - 0)
#endif /* configBSP430_HPL_DMA */

#if (configBSP430_HAL_DMA - 0) && ! (configBSP430_HPL_DMA - 0)
#warning configBSP430_HAL_DMA requested without configBSP430_HPL_DMA
#endif /* HAL and not HPL */

/** Handle for the raw DMA device.
 *
 * The handle may be used only if #configBSP430_HPL_DMA
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_DMA */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_DMA - 0)
#define BSP430_PERIPH_DMA ((tBSP430periphHandle)(BSP430_PERIPH_DMA_BASEADDRESS_))
#endif /* configBSP430_HPL_DMA */

/* END AUTOMATICALLY GENERATED CODE [periph_decl] */
/* !BSP430! end=periph_decl */

/* !BSP430! insert=hpl_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_decl] */
/** sBSP430hplDMA HPL pointer for #BSP430_PERIPH_DMA.
 *
 * This pointer to a volatile structure overlaying the DMA
 * peripheral register map may be used only if
 * #configBSP430_HPL_DMA is defined to a true value.
 *
 * @dependency #configBSP430_HPL_DMA */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_DMA - 0)
#define BSP430_HPL_DMA ((volatile sBSP430hplDMA *)BSP430_PERIPH_DMA)
#endif /* configBSP430_HPL_DMA */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_DMA
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_DMA is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_DMA.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_DMA_ISR
#define configBSP430_HAL_DMA_ISR (configBSP430_HAL_DMA - 0)
#endif /* configBSP430_HAL_DMA_ISR */

#if (configBSP430_HAL_DMA_ISR - 0) && ! (configBSP430_HAL_DMA - 0)
#warning configBSP430_HAL_DMA_ISR requested without configBSP430_HAL_DMA
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

#endif /* BSP430_MODULE_DMA */

#endif /* BSP430_PERIPH_DMA_H */
