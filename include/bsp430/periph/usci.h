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
 * @brief Hardware presentation/abstraction for USCI_A/USCI_B on 2xx/4xx devices.
 *
 * This version of the Universial Serial Communications Interface is
 * available in the 2xx and 4xx families.  See bsp430/periph/usci5.h
 * and bsp430/periph/eusci.h for related peripherals in other
 * families.
 *
 * Conventional peripheral handles are #BSP430_PERIPH_USCI_A0,
 * #BSP430_PERIPH_USCI_B0, and others dependent on availability.
 *
 * @section h_periph_usci_opt Module Configuration Options
 *
 * @li #configBSP430_HPL_USCI_A0 to enable the HPL handle declarations
 *
 * @li #configBSP430_HAL_USCI_A0 to enable the HAL infrastructure
 *
 * @li #configBSP430_HAL_USCI_AB0RX_ISR to control inclusion of the HAL
 * ISR for interrupt-driven reception operations on
 * #BSP430_PERIPH_USCI_A0 or #BSP430_PERIPH_USCI_B0
 *
 * @li #configBSP430_HAL_USCI_AB0TX_ISR to control inclusion of the HAL
 * ISR for interrupt-driven transmission operations on
 * #BSP430_PERIPH_USCI_A0 or #BSP430_PERIPH_USCI_B0
 *
 * Substitute other instance names (e.g., @b B0 or @b AB1) as
 * necessary.
 *
 * @section h_periph_usci_hpl Hardware Presentation Layer
 *
 * USCI supports an @b A variant with UART and SPI capabilities, and a
 * @b B variant with SPI and I2C capabilities.  The register maps for
 * the two variants are compatible, and #sBSP430hplUSCI is used for
 * both variants.  This structure is not compatible with the @link
 * bsp430/periph/usci5.h next generation USCI module@endlink.
 *
 * @section h_periph_usci_hal Hardware Adaptation Layer
 *
 * The USCI @HAL uses the #sBSP430halSERIAL structure from the @link
 * bsp430/serial.h generic serial adaptation layer@endlink.  It can
 * also be accessed directly using the functions defined in this
 * module.  Both @b A and @b B variants are supported by the same @HAL
 * structure.
 *
 * Enabling the HAL layer for a USCI instance enables both the
 * transmission and reception interrupt service routines by default.
 * Although transmission and receiption interrupts are distinct and
 * shared by both the @b A and @b B variants of a specific instance,
 * this is obscured by the generic serial adaptation layer, with
 * distinct callbacks for each event on each instance.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/* !BSP430! periph=usci */
/* !BSP430! instance=USCI_A0,USCI_A1,USCI_B0,USCI_B1 */

#ifndef BSP430_PERIPH_USCI_H
#define BSP430_PERIPH_USCI_H

#include <bsp430/periph.h>
#include <bsp430/serial_.h>

/** @def BSP430_MODULE_USCI
 *
 * Defined on inclusion of <bsp430/periph/usci.h>.  The value
 * evaluates to true if the target MCU supports the Universal Serial
 * Communications Interface (2xx/4xx version), and false if it does
 * not.
 *
 * @cppflag
 */
#define BSP430_MODULE_USCI defined(__MSP430_HAS_USCI__)

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_USCI - 0)

/** Maximum baud rate acceptable for UART mode.
 *
 * Not to say you can actually get this to work, but the API will reject
 * configuration of a UART rate above this. */
#ifndef BSP430_USCI_UART_MAX_BAUD
#define BSP430_USCI_UART_MAX_BAUD 1000000UL
#endif /* BSP430_USCI_UART_MAX_BAUD */

/** Register map for USCI_A/USCI_B peripheral on a MSP430 2xx/4xx MCU. */
typedef struct sBSP430hplUSCI {
  unsigned char ctl0;               /**< UCtxCTL0 */ /* 0x00 */
  unsigned char ctl1;               /**< UCtxCTL1 */ /* 0x01 */
  unsigned char br0;                /**< UCtxBR0 */ /* 0x02 */
  unsigned char br1;                /**< UCtxBR1 */ /* 0x03 */
  unsigned char mctl;               /**< UCAxMCTL (UART) */ /* 0x04 */
  unsigned char stat;               /**< UCtxSTAT */ /* 0x05 */
  unsigned char rxbuf;              /**< UCtxRXBUF */ /* 0x06 */
  unsigned char txbuf;              /**< UCtxTXBUF */ /* 0x07 */
} sBSP430hplUSCI;

/** @cond DOXYGEN_INTERNAL */
#define BSP430_PERIPH_USCI_A0_BASEADDRESS_ 0x0060
#define BSP430_PERIPH_USCI_A1_BASEADDRESS_ 0x00d0
#define BSP430_PERIPH_USCI_B0_BASEADDRESS_ 0x0068
#define BSP430_PERIPH_USCI_B1_BASEADDRESS_ 0x00d8
/** @endcond */ /* DOXYGEN_INTERNAL */

/* !BSP430! periph=serial insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI_A0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI_A0.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI_A0).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI_A0 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_A0
#define configBSP430_HAL_USCI_A0 0
#endif /* configBSP430_HAL_USCI_A0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI_A0 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI_A0_;
#endif /* configBSP430_HAL_USCI_A0 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI_A0.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_A0 - 0)
#define BSP430_HAL_USCI_A0 (&xBSP430hal_USCI_A0_)
#endif /* configBSP430_HAL_USCI_A0 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI_A1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI_A1.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI_A1).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI_A1 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_A1
#define configBSP430_HAL_USCI_A1 0
#endif /* configBSP430_HAL_USCI_A1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI_A1 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI_A1_;
#endif /* configBSP430_HAL_USCI_A1 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI_A1.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_A1 - 0)
#define BSP430_HAL_USCI_A1 (&xBSP430hal_USCI_A1_)
#endif /* configBSP430_HAL_USCI_A1 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI_B0.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI_B0).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI_B0 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_B0
#define configBSP430_HAL_USCI_B0 0
#endif /* configBSP430_HAL_USCI_B0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI_B0 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI_B0_;
#endif /* configBSP430_HAL_USCI_B0 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI_B0.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_B0 - 0)
#define BSP430_HAL_USCI_B0 (&xBSP430hal_USCI_B0_)
#endif /* configBSP430_HAL_USCI_B0 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI_B1.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI_B1).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI_B1 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_B1
#define configBSP430_HAL_USCI_B1 0
#endif /* configBSP430_HAL_USCI_B1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI_B1 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI_B1_;
#endif /* configBSP430_HAL_USCI_B1 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI_B1.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_B1 - 0)
#define BSP430_HAL_USCI_B1 (&xBSP430hal_USCI_B1_)
#endif /* configBSP430_HAL_USCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */
/* !BSP430! periph=usci */

/* !BSP430! insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI_A0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI structure reference is
 * available as #BSP430_HPL_USCI_A0.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI(#BSP430_PERIPH_USCI_A0).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI_A0
#define configBSP430_HPL_USCI_A0 (configBSP430_HAL_USCI_A0 - 0)
#endif /* configBSP430_HPL_USCI_A0 */

#if (configBSP430_HAL_USCI_A0 - 0) && ! (configBSP430_HPL_USCI_A0 - 0)
#warning configBSP430_HAL_USCI_A0 requested without configBSP430_HPL_USCI_A0
#endif /* HAL and not HPL */

/** Handle for the raw USCI_A0 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_A0 - 0)
#define BSP430_PERIPH_USCI_A0 ((tBSP430periphHandle)(BSP430_PERIPH_USCI_A0_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI_A0 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI_A1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI structure reference is
 * available as #BSP430_HPL_USCI_A1.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI(#BSP430_PERIPH_USCI_A1).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI_A1
#define configBSP430_HPL_USCI_A1 (configBSP430_HAL_USCI_A1 - 0)
#endif /* configBSP430_HPL_USCI_A1 */

#if (configBSP430_HAL_USCI_A1 - 0) && ! (configBSP430_HPL_USCI_A1 - 0)
#warning configBSP430_HAL_USCI_A1 requested without configBSP430_HPL_USCI_A1
#endif /* HAL and not HPL */

/** Handle for the raw USCI_A1 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_A1 - 0)
#define BSP430_PERIPH_USCI_A1 ((tBSP430periphHandle)(BSP430_PERIPH_USCI_A1_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI_A1 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI structure reference is
 * available as #BSP430_HPL_USCI_B0.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI(#BSP430_PERIPH_USCI_B0).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI_B0
#define configBSP430_HPL_USCI_B0 (configBSP430_HAL_USCI_B0 - 0)
#endif /* configBSP430_HPL_USCI_B0 */

#if (configBSP430_HAL_USCI_B0 - 0) && ! (configBSP430_HPL_USCI_B0 - 0)
#warning configBSP430_HAL_USCI_B0 requested without configBSP430_HPL_USCI_B0
#endif /* HAL and not HPL */

/** Handle for the raw USCI_B0 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_B0 - 0)
#define BSP430_PERIPH_USCI_B0 ((tBSP430periphHandle)(BSP430_PERIPH_USCI_B0_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI_B0 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI structure reference is
 * available as #BSP430_HPL_USCI_B1.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI(#BSP430_PERIPH_USCI_B1).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI_B1
#define configBSP430_HPL_USCI_B1 (configBSP430_HAL_USCI_B1 - 0)
#endif /* configBSP430_HPL_USCI_B1 */

#if (configBSP430_HAL_USCI_B1 - 0) && ! (configBSP430_HPL_USCI_B1 - 0)
#warning configBSP430_HAL_USCI_B1 requested without configBSP430_HPL_USCI_B1
#endif /* HAL and not HPL */

/** Handle for the raw USCI_B1 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_B1 - 0)
#define BSP430_PERIPH_USCI_B1 ((tBSP430periphHandle)(BSP430_PERIPH_USCI_B1_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [periph_decl] */
/* !BSP430! end=periph_decl */

/* !BSP430! insert=hpl_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_decl] */
/** sBSP430hplUSCI HPL pointer for #BSP430_PERIPH_USCI_A0.
 *
 * This pointer to a volatile structure overlaying the USCI_A0
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI_A0 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_A0 - 0)
#define BSP430_HPL_USCI_A0 ((volatile sBSP430hplUSCI *)BSP430_PERIPH_USCI_A0)
#endif /* configBSP430_HPL_USCI_A0 */

/** sBSP430hplUSCI HPL pointer for #BSP430_PERIPH_USCI_A1.
 *
 * This pointer to a volatile structure overlaying the USCI_A1
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI_A1 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_A1 - 0)
#define BSP430_HPL_USCI_A1 ((volatile sBSP430hplUSCI *)BSP430_PERIPH_USCI_A1)
#endif /* configBSP430_HPL_USCI_A1 */

/** sBSP430hplUSCI HPL pointer for #BSP430_PERIPH_USCI_B0.
 *
 * This pointer to a volatile structure overlaying the USCI_B0
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI_B0 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_B0 - 0)
#define BSP430_HPL_USCI_B0 ((volatile sBSP430hplUSCI *)BSP430_PERIPH_USCI_B0)
#endif /* configBSP430_HPL_USCI_B0 */

/** sBSP430hplUSCI HPL pointer for #BSP430_PERIPH_USCI_B1.
 *
 * This pointer to a volatile structure overlaying the USCI_B1
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI_B1 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_B1 - 0)
#define BSP430_HPL_USCI_B1 ((volatile sBSP430hplUSCI *)BSP430_PERIPH_USCI_B1)
#endif /* configBSP430_HPL_USCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! instance=0,1 */
/* !BSP430! insert=hal_usci_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_usci_isr_decl] */
/** Control inclusion of the @HAL receive interrupt handler for #BSP430_PERIPH_USCI_A0 and #BSP430_PERIPH_USCI_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when either or
 * both #configBSP430_HAL_USCI_A0 or
 * #configBSP430_HAL_USCI_B0 are set, but it may be
 * explicitly disabled if you intend to provide your own
 * implementation or will not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI_A0 or #BSP430_HAL_USCI_B0.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_AB0RX_ISR
#define configBSP430_HAL_USCI_AB0RX_ISR ((configBSP430_HAL_USCI_A0 - 0) || (configBSP430_HAL_USCI_B0 - 0))
#endif /* configBSP430_HAL_USCI_AB0RX_ISR */

#if ((configBSP430_HAL_0_ISR - 0) \
     && ! ((configBSP430_HAL_USCI_A0RX - 0) | (configBSP430_HAL_USCI_B0 - 0)))
#warning configBSP430_HAL_USCI_AB0RX_ISR requested without configBSP430_HAL_USCI_A0 or configBSP430_HAL_USCI_B0
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the @HAL transmit interrupt handler for #BSP430_PERIPH_USCI_A0 and #BSP430_PERIPH_USCI_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when either or
 * both #configBSP430_HAL_USCI_A0 or
 * #configBSP430_HAL_USCI_B0 are set, but it may be
 * explicitly disabled if you intend to provide your own
 * implementation or will not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI_A0 or #BSP430_HAL_USCI_B0.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_AB0TX_ISR
#define configBSP430_HAL_USCI_AB0TX_ISR ((configBSP430_HAL_USCI_A0 - 0) | (configBSP430_HAL_USCI_B0 - 0))
#endif /* configBSP430_HAL_USCI_AB0TX_ISR */

#if ((configBSP430_HAL_0_ISR - 0) \
     && ! ((configBSP430_HAL_USCI_A0TX - 0) | (configBSP430_HAL_USCI_B0 - 0)))
#warning configBSP430_HAL_USCI_AB0TX_ISR requested without configBSP430_HAL_USCI_A0 or configBSP430_HAL_USCI_B0
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the @HAL receive interrupt handler for #BSP430_PERIPH_USCI_A1 and #BSP430_PERIPH_USCI_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when either or
 * both #configBSP430_HAL_USCI_A1 or
 * #configBSP430_HAL_USCI_B1 are set, but it may be
 * explicitly disabled if you intend to provide your own
 * implementation or will not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI_A1 or #BSP430_HAL_USCI_B1.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_AB1RX_ISR
#define configBSP430_HAL_USCI_AB1RX_ISR ((configBSP430_HAL_USCI_A1 - 0) || (configBSP430_HAL_USCI_B1 - 0))
#endif /* configBSP430_HAL_USCI_AB1RX_ISR */

#if ((configBSP430_HAL_1_ISR - 0) \
     && ! ((configBSP430_HAL_USCI_A1RX - 0) | (configBSP430_HAL_USCI_B1 - 0)))
#warning configBSP430_HAL_USCI_AB1RX_ISR requested without configBSP430_HAL_USCI_A1 or configBSP430_HAL_USCI_B1
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the @HAL transmit interrupt handler for #BSP430_PERIPH_USCI_A1 and #BSP430_PERIPH_USCI_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when either or
 * both #configBSP430_HAL_USCI_A1 or
 * #configBSP430_HAL_USCI_B1 are set, but it may be
 * explicitly disabled if you intend to provide your own
 * implementation or will not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI_A1 or #BSP430_HAL_USCI_B1.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI_AB1TX_ISR
#define configBSP430_HAL_USCI_AB1TX_ISR ((configBSP430_HAL_USCI_A1 - 0) | (configBSP430_HAL_USCI_B1 - 0))
#endif /* configBSP430_HAL_USCI_AB1TX_ISR */

#if ((configBSP430_HAL_1_ISR - 0) \
     && ! ((configBSP430_HAL_USCI_A1TX - 0) | (configBSP430_HAL_USCI_B1 - 0)))
#warning configBSP430_HAL_USCI_AB1TX_ISR requested without configBSP430_HAL_USCI_A1 or configBSP430_HAL_USCI_B1
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_usci_isr_decl] */
/* !BSP430! end=hal_usci_isr_decl */
/* !BSP430! instance=USCI_A0,USCI_A1,USCI_B0,USCI_B1 */

/** USCI-specific implementation of hBSP430serialOpenUART() */
hBSP430halSERIAL hBSP430usciOpenUART (hBSP430halSERIAL hal,
                                      unsigned char ctl0_byte,
                                      unsigned char ctl1_byte,
                                      unsigned long baud);

/** USCI-specific implementation of hBSP430serialOpenSPI() */
hBSP430halSERIAL hBSP430usciOpenSPI (hBSP430halSERIAL hal,
                                     unsigned char ctl0_byte,
                                     unsigned char ctl1_byte,
                                     unsigned int prescaler);

/** USCI-specific implementation of hBSP430serialOpenI2C() */
hBSP430halSERIAL hBSP430usciOpenI2C (hBSP430halSERIAL hal,
                                     unsigned char ctl0_byte,
                                     unsigned char ctl1_byte,
                                     unsigned int prescaler);

/** USCI-specific implementation of iBSP430serialSetHold_ni() */
int iBSP430usciSetHold_ni (hBSP430halSERIAL hal, int holdp);

/** USCI-specific implementation of iBSP430serialClose() */
int iBSP430usciClose (hBSP430halSERIAL xUSCI);

/** USCI-specific implementation of vBSP430serialWakeupTransmit_ni() */
void vBSP430usciWakeupTransmit_ni (hBSP430halSERIAL device);

/** USCI-specific implementation of vBSP430serialFlush_ni() */
void vBSP430usciFlush_ni (hBSP430halSERIAL device);

/** USCI-specific implementation of iBSP430uartRxByte_ni() */
int iBSP430usciUARTrxByte_ni (hBSP430halSERIAL device);

/** USCI-specific implementation of iBSP430uartTxByte_ni() */
int iBSP430usciUARTtxByte_ni (hBSP430halSERIAL device, uint8_t c);

/** USCI-specific implementation of iBSP430uartTxData_ni() */
int iBSP430usciUARTtxData_ni (hBSP430halSERIAL device, const uint8_t * data, size_t len);

/** USCI-specific implementation of iBSP430uartTxASCIIZ_ni() */
int iBSP430usciUARTtxASCIIZ_ni (hBSP430halSERIAL device, const char * str);

/** USCI-specific implementation of iBSP430spiTxRx_ni() */
int iBSP430usciSPITxRx_ni (hBSP430halSERIAL hal,
                           const uint8_t * tx_data,
                           size_t tx_len,
                           size_t rx_len,
                           uint8_t * rx_data);

/** USCI-specific implementation of iBSP430i2cSetAddresses_ni() */
int iBSP430usciI2CsetAddresses_ni (hBSP430halSERIAL hal,
                                   int own_address,
                                   int slave_address);

/** USCI-specific implementation of iBSP430i2cRxData_ni() */
int iBSP430usciI2CrxData_ni (hBSP430halSERIAL hal,
                             uint8_t * rx_data,
                             size_t rx_len);

/** USCI-specific implementation of iBSP430i2cTxData_ni() */
int iBSP430usciI2CtxData_ni (hBSP430halSERIAL hal,
                             const uint8_t * tx_data,
                             size_t tx_len);

/** Get the HPL handle for a specific USCI instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI_A0.
 *
 * @return A typed pointer that can be used to manipulate the
 * peripheral.  A null pointer is returned if the handle does not
 * correspond to a timer for which the HPL interface been enabled
 * (e.g., with #configBSP430_HPL_USCI_A0).
 */
static BSP430_CORE_INLINE
volatile sBSP430hplUSCI * xBSP430hplLookupUSCI (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_demux] */
#if configBSP430_HPL_USCI_A0 - 0
  if (BSP430_PERIPH_USCI_A0 == periph) {
    return BSP430_HPL_USCI_A0;
  }
#endif /* configBSP430_HPL_USCI_A0 */

#if configBSP430_HPL_USCI_A1 - 0
  if (BSP430_PERIPH_USCI_A1 == periph) {
    return BSP430_HPL_USCI_A1;
  }
#endif /* configBSP430_HPL_USCI_A1 */

#if configBSP430_HPL_USCI_B0 - 0
  if (BSP430_PERIPH_USCI_B0 == periph) {
    return BSP430_HPL_USCI_B0;
  }
#endif /* configBSP430_HPL_USCI_B0 */

#if configBSP430_HPL_USCI_B1 - 0
  if (BSP430_PERIPH_USCI_B1 == periph) {
    return BSP430_HPL_USCI_B1;
  }
#endif /* configBSP430_HPL_USCI_B1 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_demux] */
  /* !BSP430! end=periph_hpl_demux */
  return NULL;
}

/** Get the HAL handle for a specific USCI instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI_A0.
 *
 * @return the HAL handle for the peripheral.  A null pointer is
 * returned if the handle does not correspond to a timer for which the
 * HAL interface has been enabled (e.g., with #configBSP430_HAL_USCI_A0).
 */
static BSP430_CORE_INLINE
hBSP430halSERIAL hBSP430usciLookup (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if configBSP430_HAL_USCI_A0 - 0
  if (BSP430_PERIPH_USCI_A0 == periph) {
    return BSP430_HAL_USCI_A0;
  }
#endif /* configBSP430_HAL_USCI_A0 */

#if configBSP430_HAL_USCI_A1 - 0
  if (BSP430_PERIPH_USCI_A1 == periph) {
    return BSP430_HAL_USCI_A1;
  }
#endif /* configBSP430_HAL_USCI_A1 */

#if configBSP430_HAL_USCI_B0 - 0
  if (BSP430_PERIPH_USCI_B0 == periph) {
    return BSP430_HAL_USCI_B0;
  }
#endif /* configBSP430_HAL_USCI_B0 */

#if configBSP430_HAL_USCI_B1 - 0
  if (BSP430_PERIPH_USCI_B1 == periph) {
    return BSP430_HAL_USCI_B1;
  }
#endif /* configBSP430_HAL_USCI_B1 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}

/** Get a human-readable identifier for the USCI peripheral
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI_A0.
 *
 * @return The short name of the port, e.g. "USCI_A0".  If the peripheral
 * is not recognized as a USCI device, a null pointer is returned. */
const char * xBSP430usciName (tBSP430periphHandle periph);

#endif /* BSP430_MODULE_USCI */

#endif /* BSP430_PERIPH_USCI_H */
