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
 * @brief Hardware presentation/abstraction for USCI_A/USCI_B on 5xx/6xx devices.
 *
 * This version of the Universal Serial Communication Interface is
 * available in the 5xx/6xx family.  See bsp430/periph/usci.h and
 * bsp430/periph/eusci.h for related peripherals in other families.
 *
 * Conventional peripheral handles are #BSP430_PERIPH_USCI5_A0,
 * #BSP430_PERIPH_USCI5_B0, and others dependent on availability.
 *
 * @section h_periph_usci5_opt Module Configuration Options
 *
 * @li #configBSP430_HPL_USCI5_A0 to enable the HPL handle declarations
 *
 * @li #configBSP430_HAL_USCI5_A0 to enable the HAL infrastructure
 *
 * @li #configBSP430_HAL_USCI5_A0_ISR to control inclusion of the HAL
 * ISR for the peripheral instance
 *
 * Substitute other instance names (e.g., @b B0) as necessary.
 *
 * @section h_periph_usci5_hpl Hardware Presentation Layer
 *
 * USCI supports an @b A variant with UART and SPI capabilities, and a
 * @b B variant with SPI and I2C capabilities.  The register maps for
 * the two variants are compatible, and #sBSP430hplUSCI5 is used for
 * both variants.  This structure is not compatible with the @link
 * bsp430/periph/usci.h previous generation USCI module@endlink.

 * @section h_periph_usci5_hal Hardware Adaptation Layer
 *
 * The USCI5 hardware adaptation layer uses the #sBSP430halSERIAL
 * structure from the @link bsp430/serial.h generic serial adaptation
 * layer@endlink.  It can also be accessed directly using the
 * functions defined in this module.  Both @b A and @b B variants are
 * supported by the same @HAL structure.
 *
 * Enabling the HAL layer for a USCI5 instance enables the
 * corresponding interrupt structure by default.  USCI5 does not
 * support a secondary ISR.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/* !BSP430! periph=usci5 */
/* !BSP430! instance=USCI5_A0,USCI5_A1,USCI5_A2,USCI5_A3,USCI5_B0,USCI5_B1,USCI5_B2,USCI5_B3 */

#ifndef BSP430_PERIPH_USCI5_H
#define BSP430_PERIPH_USCI5_H

#include <bsp430/periph.h>
#include <bsp430/serial_.h>

/** @def BSP430_MODULE_USCI5
 *
 * Defined on inclusion of <bsp430/periph/usci5.h>.  The value
 * evaluates to true if the target MCU supports the Universal Serial
 * Communications Interface (5xx/6xx version), and false if it does
 * not.
 *
 * @cppflag
 */
#define BSP430_MODULE_USCI5 (defined(__MSP430_HAS_USCI_A0__)     \
                            || defined(__MSP430_HAS_USCI_B0__)) \
 
#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_USCI5 - 0)

/** Maximum baud rate acceptable for UART mode.
 *
 * Not to say you can actually get this to work, but the API will reject
 * configuration of a UART rate above this. */
#ifndef BSP430_USCI5_UART_MAX_BAUD
#define BSP430_USCI5_UART_MAX_BAUD 1000000UL
#endif /* BSP430_USCI5_UART_MAX_BAUD */

/** Register map for USCI_A/USCI_B peripheral on a MSP430 5xx/6xx MCU. */
typedef struct sBSP430hplUSCI5 {
  union {                       /* 0x00 */
    unsigned int ctlw0;         /**< UCtxCTLW0 */
    struct {
      unsigned char ctl1;       /**< UCtxCTL1 */
      unsigned char ctl0;       /**< UCtxCTL0 */
    };
  };
  unsigned int _unused_0x02;
  unsigned int _unused_0x04;
  union {                       /* 0x06 */
    unsigned int brw;           /**< UCtxBRW */
    struct {
      unsigned char br0;        /**< UCtxBR0 */
      unsigned char br1;        /**< UCtxBR1 */
    };
  };
  union {                       /* 0x08 */
    struct { /* USCI_A */
      unsigned char mctl;       /**< UCAxMCTL (UART) */
      unsigned char _reserved_0x09;
    };
    unsigned int _reserved_0x08; /* USCI_B */
  };
  unsigned char stat;           /**< UCtxSTAT */ /* 0x0A */
  unsigned char _reserved_0x0B;
  unsigned char rxbuf;          /**< UCtxRXBUF */ /* 0x0C */
  unsigned char _reserved_0x0D;
  unsigned char txbuf;          /**< UCtxTXBUF */ /* 0x0E */
  unsigned char _reserved_0x0F;
  union {                       /* 0x10 */
    struct {                    /* USCI_A */
      unsigned char abctl;      /**< UCAxABCTL (UART) */
      unsigned char _reserved_0x11;
    };
    unsigned int i2coa;         /**< UCBxI2COA (I2C) */ /* USCI_B */
  };
  union {                       /* 0x12 */
    unsigned int irctl;         /**< UCAxIRCTL (UART)*/
    struct {
      unsigned char irtctl;     /**< UCAxIRTCTL (UART) */
      unsigned char irrctl;     /**< UCAxIRRCTL (UART) */
    };
    unsigned int i2csa;         /**< UCBxI2CSA (I2C) */
  };
  unsigned int _unused_0x14;
  unsigned int _unused_0x16;
  unsigned int _unused_0x18;
  unsigned int _unused_0x1a;
  union {                       /* 0x1C */
    unsigned int ictl;          /**< UCtxICTL */
    struct {
      unsigned char ie;         /**< UCtxIE */
      unsigned char ifg;        /**< UCtxIFG */
    };
  };
  unsigned int iv;              /**< UCtxIV */ /* 0x1E */
} sBSP430hplUSCI5;

/** @cond DOXYGEN_INTERNAL */
#define BSP430_PERIPH_USCI5_A0_BASEADDRESS_ __MSP430_BASEADDRESS_USCI_A0__
#define BSP430_PERIPH_USCI5_A1_BASEADDRESS_ __MSP430_BASEADDRESS_USCI_A1__
#define BSP430_PERIPH_USCI5_A2_BASEADDRESS_ __MSP430_BASEADDRESS_USCI_A2__
#define BSP430_PERIPH_USCI5_A3_BASEADDRESS_ __MSP430_BASEADDRESS_USCI_A3__
#define BSP430_PERIPH_USCI5_B0_BASEADDRESS_ __MSP430_BASEADDRESS_USCI_B0__
#define BSP430_PERIPH_USCI5_B1_BASEADDRESS_ __MSP430_BASEADDRESS_USCI_B1__
#define BSP430_PERIPH_USCI5_B2_BASEADDRESS_ __MSP430_BASEADDRESS_USCI_B2__
#define BSP430_PERIPH_USCI5_B3_BASEADDRESS_ __MSP430_BASEADDRESS_USCI_B3__
/** @endcond */ /* DOXYGEN_INTERNAL */

/* !BSP430! periph=serial insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI5_A0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI5_A0.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI5_A0).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI5_A0 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A0
#define configBSP430_HAL_USCI5_A0 0
#endif /* configBSP430_HAL_USCI5_A0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_A0 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_A0_;
#endif /* configBSP430_HAL_USCI5_A0 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI5_A0.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI5_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A0 - 0)
#define BSP430_HAL_USCI5_A0 (&xBSP430hal_USCI5_A0_)
#endif /* configBSP430_HAL_USCI5_A0 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI5_A1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI5_A1.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI5_A1).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI5_A1 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A1
#define configBSP430_HAL_USCI5_A1 0
#endif /* configBSP430_HAL_USCI5_A1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_A1 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_A1_;
#endif /* configBSP430_HAL_USCI5_A1 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI5_A1.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI5_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A1 - 0)
#define BSP430_HAL_USCI5_A1 (&xBSP430hal_USCI5_A1_)
#endif /* configBSP430_HAL_USCI5_A1 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI5_A2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI5_A2.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI5_A2).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI5_A2 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A2
#define configBSP430_HAL_USCI5_A2 0
#endif /* configBSP430_HAL_USCI5_A2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_A2 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_A2_;
#endif /* configBSP430_HAL_USCI5_A2 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI5_A2.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI5_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A2 - 0)
#define BSP430_HAL_USCI5_A2 (&xBSP430hal_USCI5_A2_)
#endif /* configBSP430_HAL_USCI5_A2 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI5_A3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI5_A3.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI5_A3).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI5_A3 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A3
#define configBSP430_HAL_USCI5_A3 0
#endif /* configBSP430_HAL_USCI5_A3 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_A3 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_A3_;
#endif /* configBSP430_HAL_USCI5_A3 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI5_A3.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI5_A3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A3 - 0)
#define BSP430_HAL_USCI5_A3 (&xBSP430hal_USCI5_A3_)
#endif /* configBSP430_HAL_USCI5_A3 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI5_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI5_B0.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI5_B0).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI5_B0 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B0
#define configBSP430_HAL_USCI5_B0 0
#endif /* configBSP430_HAL_USCI5_B0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_B0 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_B0_;
#endif /* configBSP430_HAL_USCI5_B0 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI5_B0.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI5_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B0 - 0)
#define BSP430_HAL_USCI5_B0 (&xBSP430hal_USCI5_B0_)
#endif /* configBSP430_HAL_USCI5_B0 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI5_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI5_B1.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI5_B1).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI5_B1 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B1
#define configBSP430_HAL_USCI5_B1 0
#endif /* configBSP430_HAL_USCI5_B1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_B1 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_B1_;
#endif /* configBSP430_HAL_USCI5_B1 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI5_B1.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI5_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B1 - 0)
#define BSP430_HAL_USCI5_B1 (&xBSP430hal_USCI5_B1_)
#endif /* configBSP430_HAL_USCI5_B1 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI5_B2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI5_B2.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI5_B2).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI5_B2 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B2
#define configBSP430_HAL_USCI5_B2 0
#endif /* configBSP430_HAL_USCI5_B2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_B2 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_B2_;
#endif /* configBSP430_HAL_USCI5_B2 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI5_B2.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI5_B2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_B2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B2 - 0)
#define BSP430_HAL_USCI5_B2 (&xBSP430hal_USCI5_B2_)
#endif /* configBSP430_HAL_USCI5_B2 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_USCI5_B3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_USCI5_B3.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_USCI5_B3).
 *
 * @cppflag
 * @affects #configBSP430_HPL_USCI5_B3 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B3
#define configBSP430_HAL_USCI5_B3 0
#endif /* configBSP430_HAL_USCI5_B3 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_B3 - 0
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_B3_;
#endif /* configBSP430_HAL_USCI5_B3 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_USCI5_B3.
 *
 * This pointer may be used only if #configBSP430_HAL_USCI5_B3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_B3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B3 - 0)
#define BSP430_HAL_USCI5_B3 (&xBSP430hal_USCI5_B3_)
#endif /* configBSP430_HAL_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */
/* !BSP430! periph=usci5 */


/* !BSP430! insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI5_A0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI5 structure reference is
 * available as #BSP430_HPL_USCI5_A0.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI5(#BSP430_PERIPH_USCI5_A0).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI5_A0
#define configBSP430_HPL_USCI5_A0 (configBSP430_HAL_USCI5_A0 - 0)
#endif /* configBSP430_HPL_USCI5_A0 */

#if (configBSP430_HAL_USCI5_A0 - 0) && ! (configBSP430_HPL_USCI5_A0 - 0)
#warning configBSP430_HAL_USCI5_A0 requested without configBSP430_HPL_USCI5_A0
#endif /* HAL and not HPL */

/** Handle for the raw USCI5_A0 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI5_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A0 - 0)
#define BSP430_PERIPH_USCI5_A0 ((tBSP430periphHandle)(BSP430_PERIPH_USCI5_A0_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI5_A0 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI5_A1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI5 structure reference is
 * available as #BSP430_HPL_USCI5_A1.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI5(#BSP430_PERIPH_USCI5_A1).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI5_A1
#define configBSP430_HPL_USCI5_A1 (configBSP430_HAL_USCI5_A1 - 0)
#endif /* configBSP430_HPL_USCI5_A1 */

#if (configBSP430_HAL_USCI5_A1 - 0) && ! (configBSP430_HPL_USCI5_A1 - 0)
#warning configBSP430_HAL_USCI5_A1 requested without configBSP430_HPL_USCI5_A1
#endif /* HAL and not HPL */

/** Handle for the raw USCI5_A1 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI5_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A1 - 0)
#define BSP430_PERIPH_USCI5_A1 ((tBSP430periphHandle)(BSP430_PERIPH_USCI5_A1_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI5_A1 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI5_A2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI5 structure reference is
 * available as #BSP430_HPL_USCI5_A2.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI5(#BSP430_PERIPH_USCI5_A2).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI5_A2
#define configBSP430_HPL_USCI5_A2 (configBSP430_HAL_USCI5_A2 - 0)
#endif /* configBSP430_HPL_USCI5_A2 */

#if (configBSP430_HAL_USCI5_A2 - 0) && ! (configBSP430_HPL_USCI5_A2 - 0)
#warning configBSP430_HAL_USCI5_A2 requested without configBSP430_HPL_USCI5_A2
#endif /* HAL and not HPL */

/** Handle for the raw USCI5_A2 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI5_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A2 - 0)
#define BSP430_PERIPH_USCI5_A2 ((tBSP430periphHandle)(BSP430_PERIPH_USCI5_A2_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI5_A2 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI5_A3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI5 structure reference is
 * available as #BSP430_HPL_USCI5_A3.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI5(#BSP430_PERIPH_USCI5_A3).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI5_A3
#define configBSP430_HPL_USCI5_A3 (configBSP430_HAL_USCI5_A3 - 0)
#endif /* configBSP430_HPL_USCI5_A3 */

#if (configBSP430_HAL_USCI5_A3 - 0) && ! (configBSP430_HPL_USCI5_A3 - 0)
#warning configBSP430_HAL_USCI5_A3 requested without configBSP430_HPL_USCI5_A3
#endif /* HAL and not HPL */

/** Handle for the raw USCI5_A3 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI5_A3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A3 - 0)
#define BSP430_PERIPH_USCI5_A3 ((tBSP430periphHandle)(BSP430_PERIPH_USCI5_A3_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI5_A3 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI5_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI5 structure reference is
 * available as #BSP430_HPL_USCI5_B0.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI5(#BSP430_PERIPH_USCI5_B0).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI5_B0
#define configBSP430_HPL_USCI5_B0 (configBSP430_HAL_USCI5_B0 - 0)
#endif /* configBSP430_HPL_USCI5_B0 */

#if (configBSP430_HAL_USCI5_B0 - 0) && ! (configBSP430_HPL_USCI5_B0 - 0)
#warning configBSP430_HAL_USCI5_B0 requested without configBSP430_HPL_USCI5_B0
#endif /* HAL and not HPL */

/** Handle for the raw USCI5_B0 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI5_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B0 - 0)
#define BSP430_PERIPH_USCI5_B0 ((tBSP430periphHandle)(BSP430_PERIPH_USCI5_B0_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI5_B0 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI5_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI5 structure reference is
 * available as #BSP430_HPL_USCI5_B1.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI5(#BSP430_PERIPH_USCI5_B1).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI5_B1
#define configBSP430_HPL_USCI5_B1 (configBSP430_HAL_USCI5_B1 - 0)
#endif /* configBSP430_HPL_USCI5_B1 */

#if (configBSP430_HAL_USCI5_B1 - 0) && ! (configBSP430_HPL_USCI5_B1 - 0)
#warning configBSP430_HAL_USCI5_B1 requested without configBSP430_HPL_USCI5_B1
#endif /* HAL and not HPL */

/** Handle for the raw USCI5_B1 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI5_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B1 - 0)
#define BSP430_PERIPH_USCI5_B1 ((tBSP430periphHandle)(BSP430_PERIPH_USCI5_B1_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI5_B1 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI5_B2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI5 structure reference is
 * available as #BSP430_HPL_USCI5_B2.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI5(#BSP430_PERIPH_USCI5_B2).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI5_B2
#define configBSP430_HPL_USCI5_B2 (configBSP430_HAL_USCI5_B2 - 0)
#endif /* configBSP430_HPL_USCI5_B2 */

#if (configBSP430_HAL_USCI5_B2 - 0) && ! (configBSP430_HPL_USCI5_B2 - 0)
#warning configBSP430_HAL_USCI5_B2 requested without configBSP430_HPL_USCI5_B2
#endif /* HAL and not HPL */

/** Handle for the raw USCI5_B2 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI5_B2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B2 - 0)
#define BSP430_PERIPH_USCI5_B2 ((tBSP430periphHandle)(BSP430_PERIPH_USCI5_B2_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI5_B2 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_USCI5_B3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplUSCI5 structure reference is
 * available as #BSP430_HPL_USCI5_B3.
 *
 * It may also be obtained using
 * #xBSP430hplLookupUSCI5(#BSP430_PERIPH_USCI5_B3).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_USCI5_B3
#define configBSP430_HPL_USCI5_B3 (configBSP430_HAL_USCI5_B3 - 0)
#endif /* configBSP430_HPL_USCI5_B3 */

#if (configBSP430_HAL_USCI5_B3 - 0) && ! (configBSP430_HPL_USCI5_B3 - 0)
#warning configBSP430_HAL_USCI5_B3 requested without configBSP430_HPL_USCI5_B3
#endif /* HAL and not HPL */

/** Handle for the raw USCI5_B3 device.
 *
 * The handle may be used only if #configBSP430_HPL_USCI5_B3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B3 - 0)
#define BSP430_PERIPH_USCI5_B3 ((tBSP430periphHandle)(BSP430_PERIPH_USCI5_B3_BASEADDRESS_))
#endif /* configBSP430_HPL_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [periph_decl] */
/* !BSP430! end=periph_decl */

/* !BSP430! insert=hpl_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_decl] */
/** sBSP430hplUSCI5 HPL pointer for #BSP430_PERIPH_USCI5_A0.
 *
 * This pointer to a volatile structure overlaying the USCI5_A0
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI5_A0 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A0 - 0)
#define BSP430_HPL_USCI5_A0 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_A0)
#endif /* configBSP430_HPL_USCI5_A0 */

/** sBSP430hplUSCI5 HPL pointer for #BSP430_PERIPH_USCI5_A1.
 *
 * This pointer to a volatile structure overlaying the USCI5_A1
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI5_A1 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A1 - 0)
#define BSP430_HPL_USCI5_A1 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_A1)
#endif /* configBSP430_HPL_USCI5_A1 */

/** sBSP430hplUSCI5 HPL pointer for #BSP430_PERIPH_USCI5_A2.
 *
 * This pointer to a volatile structure overlaying the USCI5_A2
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI5_A2 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A2 - 0)
#define BSP430_HPL_USCI5_A2 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_A2)
#endif /* configBSP430_HPL_USCI5_A2 */

/** sBSP430hplUSCI5 HPL pointer for #BSP430_PERIPH_USCI5_A3.
 *
 * This pointer to a volatile structure overlaying the USCI5_A3
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI5_A3 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A3 - 0)
#define BSP430_HPL_USCI5_A3 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_A3)
#endif /* configBSP430_HPL_USCI5_A3 */

/** sBSP430hplUSCI5 HPL pointer for #BSP430_PERIPH_USCI5_B0.
 *
 * This pointer to a volatile structure overlaying the USCI5_B0
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI5_B0 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B0 - 0)
#define BSP430_HPL_USCI5_B0 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_B0)
#endif /* configBSP430_HPL_USCI5_B0 */

/** sBSP430hplUSCI5 HPL pointer for #BSP430_PERIPH_USCI5_B1.
 *
 * This pointer to a volatile structure overlaying the USCI5_B1
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI5_B1 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B1 - 0)
#define BSP430_HPL_USCI5_B1 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_B1)
#endif /* configBSP430_HPL_USCI5_B1 */

/** sBSP430hplUSCI5 HPL pointer for #BSP430_PERIPH_USCI5_B2.
 *
 * This pointer to a volatile structure overlaying the USCI5_B2
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI5_B2 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B2 - 0)
#define BSP430_HPL_USCI5_B2 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_B2)
#endif /* configBSP430_HPL_USCI5_B2 */

/** sBSP430hplUSCI5 HPL pointer for #BSP430_PERIPH_USCI5_B3.
 *
 * This pointer to a volatile structure overlaying the USCI5_B3
 * peripheral register map may be used only if
 * #configBSP430_HPL_USCI5_B3 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B3 - 0)
#define BSP430_HPL_USCI5_B3 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_B3)
#endif /* configBSP430_HPL_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_USCI5_A0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_USCI5_A0 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI5_A0.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A0_ISR
#define configBSP430_HAL_USCI5_A0_ISR (configBSP430_HAL_USCI5_A0 - 0)
#endif /* configBSP430_HAL_USCI5_A0_ISR */

#if (configBSP430_HAL_USCI5_A0_ISR - 0) && ! (configBSP430_HAL_USCI5_A0 - 0)
#warning configBSP430_HAL_USCI5_A0_ISR requested without configBSP430_HAL_USCI5_A0
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_USCI5_A1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_USCI5_A1 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI5_A1.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A1_ISR
#define configBSP430_HAL_USCI5_A1_ISR (configBSP430_HAL_USCI5_A1 - 0)
#endif /* configBSP430_HAL_USCI5_A1_ISR */

#if (configBSP430_HAL_USCI5_A1_ISR - 0) && ! (configBSP430_HAL_USCI5_A1 - 0)
#warning configBSP430_HAL_USCI5_A1_ISR requested without configBSP430_HAL_USCI5_A1
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_USCI5_A2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_USCI5_A2 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI5_A2.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A2_ISR
#define configBSP430_HAL_USCI5_A2_ISR (configBSP430_HAL_USCI5_A2 - 0)
#endif /* configBSP430_HAL_USCI5_A2_ISR */

#if (configBSP430_HAL_USCI5_A2_ISR - 0) && ! (configBSP430_HAL_USCI5_A2 - 0)
#warning configBSP430_HAL_USCI5_A2_ISR requested without configBSP430_HAL_USCI5_A2
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_USCI5_A3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_USCI5_A3 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI5_A3.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A3_ISR
#define configBSP430_HAL_USCI5_A3_ISR (configBSP430_HAL_USCI5_A3 - 0)
#endif /* configBSP430_HAL_USCI5_A3_ISR */

#if (configBSP430_HAL_USCI5_A3_ISR - 0) && ! (configBSP430_HAL_USCI5_A3 - 0)
#warning configBSP430_HAL_USCI5_A3_ISR requested without configBSP430_HAL_USCI5_A3
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_USCI5_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_USCI5_B0 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI5_B0.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B0_ISR
#define configBSP430_HAL_USCI5_B0_ISR (configBSP430_HAL_USCI5_B0 - 0)
#endif /* configBSP430_HAL_USCI5_B0_ISR */

#if (configBSP430_HAL_USCI5_B0_ISR - 0) && ! (configBSP430_HAL_USCI5_B0 - 0)
#warning configBSP430_HAL_USCI5_B0_ISR requested without configBSP430_HAL_USCI5_B0
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_USCI5_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_USCI5_B1 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI5_B1.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B1_ISR
#define configBSP430_HAL_USCI5_B1_ISR (configBSP430_HAL_USCI5_B1 - 0)
#endif /* configBSP430_HAL_USCI5_B1_ISR */

#if (configBSP430_HAL_USCI5_B1_ISR - 0) && ! (configBSP430_HAL_USCI5_B1 - 0)
#warning configBSP430_HAL_USCI5_B1_ISR requested without configBSP430_HAL_USCI5_B1
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_USCI5_B2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_USCI5_B2 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI5_B2.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B2_ISR
#define configBSP430_HAL_USCI5_B2_ISR (configBSP430_HAL_USCI5_B2 - 0)
#endif /* configBSP430_HAL_USCI5_B2_ISR */

#if (configBSP430_HAL_USCI5_B2_ISR - 0) && ! (configBSP430_HAL_USCI5_B2 - 0)
#warning configBSP430_HAL_USCI5_B2_ISR requested without configBSP430_HAL_USCI5_B2
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_USCI5_B3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_USCI5_B3 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_USCI5_B3.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B3_ISR
#define configBSP430_HAL_USCI5_B3_ISR (configBSP430_HAL_USCI5_B3 - 0)
#endif /* configBSP430_HAL_USCI5_B3_ISR */

#if (configBSP430_HAL_USCI5_B3_ISR - 0) && ! (configBSP430_HAL_USCI5_B3 - 0)
#warning configBSP430_HAL_USCI5_B3_ISR requested without configBSP430_HAL_USCI5_B3
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

/** USCI5-specific implementation of hBSP430serialOpenUART() */
hBSP430halSERIAL hBSP430usci5OpenUART (hBSP430halSERIAL hal,
                                       unsigned char ctl0_byte,
                                       unsigned char ctl1_byte,
                                       unsigned long baud);

/** USCI5-specific implementation of hBSP430serialOpenSPI() */
hBSP430halSERIAL hBSP430usci5OpenSPI (hBSP430halSERIAL hal,
                                      unsigned char ctl0_byte,
                                      unsigned char ctl1_byte,
                                      unsigned int prescaler);

/** USCI5-specific implementation of iBSP430serialSetHold_ni() */
int iBSP430usci5SetHold_ni (hBSP430halSERIAL hal, int holdp);

/** USCI5-specific implementation of iBSP430serialClose() */
int iBSP430usci5Close (hBSP430halSERIAL xUSCI);

/** USCI5-specific implementation of vBSP430serialWakeupTransmit_ni() */
void vBSP430usci5WakeupTransmit_ni (hBSP430halSERIAL device);

/** USCI5-specific implementation of vBSP430serialFlush_ni() */
void vBSP430usci5Flush_ni (hBSP430halSERIAL device);

/** USCI5-specific implementation of iBSP430uartRxByte_ni() */
int iBSP430usci5UARTrxByte_ni (hBSP430halSERIAL device);

/** USCI5-specific implementation of iBSP430uartTxByte_ni() */
int iBSP430usci5UARTtxByte_ni (hBSP430halSERIAL device, uint8_t c);

/** USCI5-specific implementation of iBSP430uartTxData_ni() */
int iBSP430usci5UARTtxData_ni (hBSP430halSERIAL device, const uint8_t * data, size_t len);

/** USCI5-specific implementation of iBSP430uartTxASCIIZ_ni() */
int iBSP430usci5UARTtxASCIIZ_ni (hBSP430halSERIAL device, const char * str);

/** USCI5-specific implementation of iBSP430spiTxRx_ni() */
int iBSP430usci5SPITxRx_ni (hBSP430halSERIAL hal,
                            const uint8_t * tx_data,
                            size_t tx_len,
                            size_t rx_len,
                            uint8_t * rx_data);

/** USCI5-specific implementation of iBSP430i2cSetAddresses_ni() */
int iBSP430usci5I2CsetAddresses_ni (hBSP430halSERIAL hal,
                                    int own_address,
                                    int slave_address);

/** USCI5-specific implementation of iBSP430i2cRxData_ni() */
int iBSP430usci5I2CrxData_ni (hBSP430halSERIAL hal,
                              uint8_t * rx_data,
                              size_t rx_len);

/** USCI5-specific implementation of iBSP430i2cTxData_ni() */
int iBSP430usci5I2CtxData_ni (hBSP430halSERIAL hal,
                              const uint8_t * tx_data,
                              size_t tx_len);

/** Get the HPL handle for a specific USCI5 instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI5_A0.
 *
 * @return A typed pointer that can be used to manipulate the
 * peripheral.  A null pointer is returned if the handle does not
 * correspond to a timer for which the HPL interface been enabled
 * (e.g., with #configBSP430_HPL_USCI5_A0).
 */
static BSP430_CORE_INLINE
volatile sBSP430hplUSCI5 * xBSP430hplLookupUSCI5 (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_demux] */
#if configBSP430_HPL_USCI5_A0 - 0
  if (BSP430_PERIPH_USCI5_A0 == periph) {
    return BSP430_HPL_USCI5_A0;
  }
#endif /* configBSP430_HPL_USCI5_A0 */

#if configBSP430_HPL_USCI5_A1 - 0
  if (BSP430_PERIPH_USCI5_A1 == periph) {
    return BSP430_HPL_USCI5_A1;
  }
#endif /* configBSP430_HPL_USCI5_A1 */

#if configBSP430_HPL_USCI5_A2 - 0
  if (BSP430_PERIPH_USCI5_A2 == periph) {
    return BSP430_HPL_USCI5_A2;
  }
#endif /* configBSP430_HPL_USCI5_A2 */

#if configBSP430_HPL_USCI5_A3 - 0
  if (BSP430_PERIPH_USCI5_A3 == periph) {
    return BSP430_HPL_USCI5_A3;
  }
#endif /* configBSP430_HPL_USCI5_A3 */

#if configBSP430_HPL_USCI5_B0 - 0
  if (BSP430_PERIPH_USCI5_B0 == periph) {
    return BSP430_HPL_USCI5_B0;
  }
#endif /* configBSP430_HPL_USCI5_B0 */

#if configBSP430_HPL_USCI5_B1 - 0
  if (BSP430_PERIPH_USCI5_B1 == periph) {
    return BSP430_HPL_USCI5_B1;
  }
#endif /* configBSP430_HPL_USCI5_B1 */

#if configBSP430_HPL_USCI5_B2 - 0
  if (BSP430_PERIPH_USCI5_B2 == periph) {
    return BSP430_HPL_USCI5_B2;
  }
#endif /* configBSP430_HPL_USCI5_B2 */

#if configBSP430_HPL_USCI5_B3 - 0
  if (BSP430_PERIPH_USCI5_B3 == periph) {
    return BSP430_HPL_USCI5_B3;
  }
#endif /* configBSP430_HPL_USCI5_B3 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_demux] */
  /* !BSP430! end=periph_hpl_demux */
  return NULL;
}

/** Get the HAL handle for a specific USCI5 instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI5_A0.
 *
 * @return the HAL handle for the peripheral.  A null pointer is
 * returned if the handle does not correspond to a timer for which the
 * HAL interface has been enabled (e.g., with #configBSP430_HAL_USCI5_A0).
 */
static BSP430_CORE_INLINE
hBSP430halSERIAL xBSP430usci5Lookup (tBSP430periphHandle periph)
{
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if configBSP430_HAL_USCI5_A0 - 0
  if (BSP430_PERIPH_USCI5_A0 == periph) {
    return BSP430_HAL_USCI5_A0;
  }
#endif /* configBSP430_HAL_USCI5_A0 */

#if configBSP430_HAL_USCI5_A1 - 0
  if (BSP430_PERIPH_USCI5_A1 == periph) {
    return BSP430_HAL_USCI5_A1;
  }
#endif /* configBSP430_HAL_USCI5_A1 */

#if configBSP430_HAL_USCI5_A2 - 0
  if (BSP430_PERIPH_USCI5_A2 == periph) {
    return BSP430_HAL_USCI5_A2;
  }
#endif /* configBSP430_HAL_USCI5_A2 */

#if configBSP430_HAL_USCI5_A3 - 0
  if (BSP430_PERIPH_USCI5_A3 == periph) {
    return BSP430_HAL_USCI5_A3;
  }
#endif /* configBSP430_HAL_USCI5_A3 */

#if configBSP430_HAL_USCI5_B0 - 0
  if (BSP430_PERIPH_USCI5_B0 == periph) {
    return BSP430_HAL_USCI5_B0;
  }
#endif /* configBSP430_HAL_USCI5_B0 */

#if configBSP430_HAL_USCI5_B1 - 0
  if (BSP430_PERIPH_USCI5_B1 == periph) {
    return BSP430_HAL_USCI5_B1;
  }
#endif /* configBSP430_HAL_USCI5_B1 */

#if configBSP430_HAL_USCI5_B2 - 0
  if (BSP430_PERIPH_USCI5_B2 == periph) {
    return BSP430_HAL_USCI5_B2;
  }
#endif /* configBSP430_HAL_USCI5_B2 */

#if configBSP430_HAL_USCI5_B3 - 0
  if (BSP430_PERIPH_USCI5_B3 == periph) {
    return BSP430_HAL_USCI5_B3;
  }
#endif /* configBSP430_HAL_USCI5_B3 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}

/** Get a human-readable identifier for the USCI5 peripheral
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI5_A0.
 *
 * @return The short name of the port, e.g. "USCI5_A0".  If the peripheral
 * is not recognized as a USCI5 device, a null pointer is returned. */
const char * xBSP430usci5Name (tBSP430periphHandle periph);

#endif /* BSP430_MODULE_USCI5 */

#endif /* BSP430_PERIPH_USCI5_H */
