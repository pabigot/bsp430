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
 * @brief Hardware presentation/abstraction for eUSCI.
 *
 * The Enhanced Universal Serial Communication Interface is available
 * in the 5xx/6xx/FR5xx family, primarily on FRAM devices.  See
 * bsp430/periph/usci.h and bsp430/periph/usci5.h for related
 * peripherals in other families.
 *
 * Conventional peripheral handles are #BSP430_PERIPH_EUSCI_A0,
 * #BSP430_PERIPH_EUSCI_B0, and others dependent on availability.
 *
 * @section h_periph_eusci_opt Module Configuration Options
 *
 * @li #configBSP430_HPL_EUSCI_A0 to enable the HPL handle declarations
 *
 * @li #configBSP430_HAL_EUSCI_A0 to enable the HAL infrastructure
 *
 * @li #configBSP430_HAL_EUSCI_A0_ISR to control inclusion of the HAL
 * ISR for the peripheral instance
 *
 * Substitute other instance names (e.g., @b B0) as necessary.
 *
 * @section h_periph_eusci_hpl Hardware Presentation Layer
 *
 * Like previous USCI modules, eUSCI supports an @b A variant with
 * UART and SPI capabilities, and a @b B variant with SPI and I2C
 * capabilities.  Unlike previous modules the register maps for the
 * two variants are not compatible; in particular introduction of
 * certain I2C capabilities causes some SPI registers to be placed at
 * different offsets in the @b A and @b B variables.
 *
 * This module defines #sBSP430hplEUSCIA as the HPL structure for the
 * @b A variant, and #sBSP430hplEUSCIB as the HPL structure for the @b
 * B variant.
 *
 * @section h_periph_eusci_hal Hardware Adaptation Layer
 *
 * The eUSCI hardware adaptation layer uses the #sBSP430halSERIAL
 * structure from the @link bsp430/serial.h generic serial adaptation
 * layer@endlink.  It can also be accessed directly using the
 * functions defined in this module.  Both @b A and @b B variants are
 * supported by the same @HAL structure.
 *
 * Enabling the HAL layer for an eUSCI instance enables the
 * corresponding interrupt structure by default.  eUSCI does not
 * support a secondary ISR.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_EUSCI_H
#define BSP430_PERIPH_EUSCI_H

#include <bsp430/periph.h>
#include <bsp430/serial_.h>

/** Defined on inclusion of <bsp430/periph/eusci.h>.  The value
 * evaluates to true if the target MCU supports the Enhanced Universal
 * Serial Communcations Interface peripheral, and false if it does
 * not.
 *
 * @cppflag
 */
#define BSP430_MODULE_EUSCI (defined(__MSP430_HAS_EUSCI_A0__)           \
                             || defined(__MSP430_HAS_EUSCI_B0__))

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_EUSCI - 0)

/** Maximum baud rate acceptable for UART mode.
 *
 * Not to say you can actually get this to work, but the API will reject
 * configuration of a UART rate above this. */
#ifndef BSP430_EUSCI_UART_MAX_BAUD
#define BSP430_EUSCI_UART_MAX_BAUD 1000000UL
#endif /* BSP430_EUSCI_UART_MAX_BAUD */

/** Register map for eUSCI_A peripheral hardware presentation layer. */
typedef struct sBSP430hplEUSCIA {
  union {                       /* 0x00 */
    unsigned int ctlw0;         /**< UCAxCTLW0 */
    struct {
      unsigned char ctl1;       /**< UCAxCTL1 */
      unsigned char ctl0;       /**< UCAxCTL0 */
    };
  };
  unsigned int ctlw1;           /**< UCAxCTLW1 */ /* 0x02 */
  unsigned int _unused_0x04;
  union {                       /* 0x06 */
    unsigned int brw;           /**< UCAxBRW */
    struct {
      unsigned char br0;        /**< UCAxBR0 */
      unsigned char br1;        /**< UCAxBR1 */
    };
  };
  unsigned int mctlw;           /**< UCAxMCTLW (UART) */ /* 0x08 */
  unsigned int statw;           /**< UCAxSTATW */ /* 0x0A */
  unsigned int rxbuf;           /**< UCAxRXBUF */ /* 0x0C */
  unsigned int txbuf;           /**< UCAxTXBUF */ /* 0x0E */
  unsigned int abctl;           /**< UCAxABCTL (UART) */ /* 0x10 */
  union {                       /* 0x12 */
    unsigned int irctl;         /**< UCAxIRCTL (UART) */
    struct {
      unsigned char irtctl;     /**< UCAxIRTCTL (UART) */ /* 0x12 */
      unsigned char irrctl;     /**< UCAxIRRCTL (UART) */ /* 0x13 */
    };
  };
  unsigned int _unused_0x14;
  unsigned int _unused_0x16;
  unsigned int _unused_0x18;
  unsigned int ie;              /**< UCAxIE */ /* 0x1A */
  unsigned int ifg;             /**< UCAxIFG */ /* 0x1C */
  unsigned int iv;              /**< UCAxIV */ /* 0x1E */
} sBSP430hplEUSCIA;

/** Register map for eUSCI_B peripheral hardware presentation layer. */
typedef struct sBSP430hplEUSCIB {
  union {                       /* 0x00 */
    unsigned int ctlw0;         /**< UCBxCTLW0 */
    struct {
      unsigned char ctl1;       /**< UCBxCTL1 */
      unsigned char ctl0;       /**< UCBxCTL0 */
    };
  };
  unsigned int ctlw1;           /**< UCBxCTLW1 */ /* 0x02 */
  unsigned int _unused_0x04;
  union {                       /* 0x06 */
    unsigned int brw;           /**< UCBxBRW */
    struct {
      unsigned char br0;        /**< UCBxBR0 */
      unsigned char br1;        /**< UCBxBR1 */
    };
  };
  union {
    unsigned int statw;         /**< UCBxSTATW */ /* 0x08 */
    struct {
      unsigned char stat;       /**< UCBxSTAT */
      unsigned char bcnt;       /**< UCBxBCNT */
    };
  };
  unsigned int tbcnt;           /**< UCBxTBCNT */ /* 0x0A */
  unsigned int rxbuf;           /**< UCAxRXBUF */ /* 0x0C */
  unsigned int txbuf;           /**< UCAxTXBUF */ /* 0x0E */
  unsigned int _unused_0x10;
  unsigned int _unused_0x12;
  unsigned int i2coa0;          /**< UCBxI2COA0 */ /* 0x14 */
  unsigned int i2coa1;          /**< UCBxI2COA1 */ /* 0x16 */
  unsigned int i2coa2;          /**< UCBxI2COA2 */ /* 0x18 */
  unsigned int i2coa3;          /**< UCBxI2COA3 */ /* 0x1A */
  unsigned int addrx;           /**< UCBxADDRX */ /* 0x1C */
  unsigned int addmask;         /**< UCBxADDMASK */ /* 0x1E */
  unsigned int i2csa;           /**< UCBxI2CSA */ /* 0x20 */
  unsigned int _unused_0x22;
  unsigned int _unused_0x24;
  unsigned int _unused_0x26;
  unsigned int _unused_0x28;
  unsigned int ie;              /**< UCAxIE */ /* 0x2A */
  unsigned int ifg;             /**< UCAxIFG */ /* 0x2C */
  unsigned int iv;              /**< UCAxIV */ /* 0x2E */
} sBSP430hplEUSCIB;

/** @cond DOXYGEN_INTERNAL */
#define BSP430_PERIPH_EUSCI_A0_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_A0__
#define BSP430_PERIPH_EUSCI_A1_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_A1__
#define BSP430_PERIPH_EUSCI_A2_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_A2__
#define BSP430_PERIPH_EUSCI_A3_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_A3__
#define BSP430_PERIPH_EUSCI_B0_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_B0__
#define BSP430_PERIPH_EUSCI_B1_BASEADDRESS_ __MSP430_BASEADDRESS_EUSCI_B1__
/** @endcond */ /* DOXYGEN_INTERNAL */

/* !BSP430! periph=serial instance=EUSCI_A0,EUSCI_A1,EUSCI_A2,EUSCI_A3,EUSCI_B0,EUSCI_B1 insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** Control inclusion of the @HAL interface to #BSP430_PERIPH_EUSCI_A0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_EUSCI_A0.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_EUSCI_A0).
 *
 * @cppflag
 * @affects #configBSP430_HPL_EUSCI_A0 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A0
#define configBSP430_HAL_EUSCI_A0 0
#endif /* configBSP430_HAL_EUSCI_A0 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_EUSCI_A0 - 0)
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_A0_;
#endif /* configBSP430_HAL_EUSCI_A0 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_EUSCI_A0.
 *
 * This pointer may be used only if #configBSP430_HAL_EUSCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A0 - 0)
#define BSP430_HAL_EUSCI_A0 (&xBSP430hal_EUSCI_A0_)
#endif /* configBSP430_HAL_EUSCI_A0 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_EUSCI_A1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_EUSCI_A1.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_EUSCI_A1).
 *
 * @cppflag
 * @affects #configBSP430_HPL_EUSCI_A1 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A1
#define configBSP430_HAL_EUSCI_A1 0
#endif /* configBSP430_HAL_EUSCI_A1 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_EUSCI_A1 - 0)
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_A1_;
#endif /* configBSP430_HAL_EUSCI_A1 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_EUSCI_A1.
 *
 * This pointer may be used only if #configBSP430_HAL_EUSCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A1 - 0)
#define BSP430_HAL_EUSCI_A1 (&xBSP430hal_EUSCI_A1_)
#endif /* configBSP430_HAL_EUSCI_A1 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_EUSCI_A2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_EUSCI_A2.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_EUSCI_A2).
 *
 * @cppflag
 * @affects #configBSP430_HPL_EUSCI_A2 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A2
#define configBSP430_HAL_EUSCI_A2 0
#endif /* configBSP430_HAL_EUSCI_A2 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_EUSCI_A2 - 0)
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_A2_;
#endif /* configBSP430_HAL_EUSCI_A2 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_EUSCI_A2.
 *
 * This pointer may be used only if #configBSP430_HAL_EUSCI_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A2 - 0)
#define BSP430_HAL_EUSCI_A2 (&xBSP430hal_EUSCI_A2_)
#endif /* configBSP430_HAL_EUSCI_A2 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_EUSCI_A3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_EUSCI_A3.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_EUSCI_A3).
 *
 * @cppflag
 * @affects #configBSP430_HPL_EUSCI_A3 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A3
#define configBSP430_HAL_EUSCI_A3 0
#endif /* configBSP430_HAL_EUSCI_A3 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_EUSCI_A3 - 0)
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_A3_;
#endif /* configBSP430_HAL_EUSCI_A3 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_EUSCI_A3.
 *
 * This pointer may be used only if #configBSP430_HAL_EUSCI_A3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A3 - 0)
#define BSP430_HAL_EUSCI_A3 (&xBSP430hal_EUSCI_A3_)
#endif /* configBSP430_HAL_EUSCI_A3 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_EUSCI_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_EUSCI_B0.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_EUSCI_B0).
 *
 * @cppflag
 * @affects #configBSP430_HPL_EUSCI_B0 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_B0
#define configBSP430_HAL_EUSCI_B0 0
#endif /* configBSP430_HAL_EUSCI_B0 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_EUSCI_B0 - 0)
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_B0_;
#endif /* configBSP430_HAL_EUSCI_B0 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_EUSCI_B0.
 *
 * This pointer may be used only if #configBSP430_HAL_EUSCI_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_B0 - 0)
#define BSP430_HAL_EUSCI_B0 (&xBSP430hal_EUSCI_B0_)
#endif /* configBSP430_HAL_EUSCI_B0 */

/** Control inclusion of the @HAL interface to #BSP430_PERIPH_EUSCI_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430halSERIAL structure reference is
 * available as #BSP430_HAL_EUSCI_B1.
 *
 * It may also be obtained using
 * #hBSP430serialLookup(#BSP430_PERIPH_EUSCI_B1).
 *
 * @cppflag
 * @affects #configBSP430_HPL_EUSCI_B1 is default-enabled
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_B1
#define configBSP430_HAL_EUSCI_B1 0
#endif /* configBSP430_HAL_EUSCI_B1 */

/** @cond DOXYGEN_EXCLUDE */
#if (configBSP430_HAL_EUSCI_B1 - 0)
/* You do not need to know about this */
extern sBSP430halSERIAL xBSP430hal_EUSCI_B1_;
#endif /* configBSP430_HAL_EUSCI_B1 */
/** @endcond */

/** sBSP430halSERIAL HAL handle for #BSP430_PERIPH_EUSCI_B1.
 *
 * This pointer may be used only if #configBSP430_HAL_EUSCI_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_EUSCI_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_B1 - 0)
#define BSP430_HAL_EUSCI_B1 (&xBSP430hal_EUSCI_B1_)
#endif /* configBSP430_HAL_EUSCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! periph=euscia instance=EUSCI_A0,EUSCI_A1,EUSCI_A2,EUSCI_A3 insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** Control inclusion of the @HPL interface to #BSP430_PERIPH_EUSCI_A0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplEUSCIA structure reference is
 * available as #BSP430_HPL_EUSCI_A0.
 *
 * It may also be obtained using
 * #xBSP430hplLookupEUSCIA(#BSP430_PERIPH_EUSCI_A0).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_A0
#define configBSP430_HPL_EUSCI_A0 (configBSP430_HAL_EUSCI_A0 - 0)
#endif /* configBSP430_HPL_EUSCI_A0 */

#if (configBSP430_HAL_EUSCI_A0 - 0) && ! (configBSP430_HPL_EUSCI_A0 - 0)
#warning configBSP430_HAL_EUSCI_A0 requested without configBSP430_HPL_EUSCI_A0
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A0 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A0 - 0)
#define BSP430_PERIPH_EUSCI_A0 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_A0_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_A0 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_EUSCI_A1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplEUSCIA structure reference is
 * available as #BSP430_HPL_EUSCI_A1.
 *
 * It may also be obtained using
 * #xBSP430hplLookupEUSCIA(#BSP430_PERIPH_EUSCI_A1).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_A1
#define configBSP430_HPL_EUSCI_A1 (configBSP430_HAL_EUSCI_A1 - 0)
#endif /* configBSP430_HPL_EUSCI_A1 */

#if (configBSP430_HAL_EUSCI_A1 - 0) && ! (configBSP430_HPL_EUSCI_A1 - 0)
#warning configBSP430_HAL_EUSCI_A1 requested without configBSP430_HPL_EUSCI_A1
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A1 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A1 - 0)
#define BSP430_PERIPH_EUSCI_A1 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_A1_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_A1 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_EUSCI_A2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplEUSCIA structure reference is
 * available as #BSP430_HPL_EUSCI_A2.
 *
 * It may also be obtained using
 * #xBSP430hplLookupEUSCIA(#BSP430_PERIPH_EUSCI_A2).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_A2
#define configBSP430_HPL_EUSCI_A2 (configBSP430_HAL_EUSCI_A2 - 0)
#endif /* configBSP430_HPL_EUSCI_A2 */

#if (configBSP430_HAL_EUSCI_A2 - 0) && ! (configBSP430_HPL_EUSCI_A2 - 0)
#warning configBSP430_HAL_EUSCI_A2 requested without configBSP430_HPL_EUSCI_A2
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A2 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A2 - 0)
#define BSP430_PERIPH_EUSCI_A2 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_A2_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_A2 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_EUSCI_A3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplEUSCIA structure reference is
 * available as #BSP430_HPL_EUSCI_A3.
 *
 * It may also be obtained using
 * #xBSP430hplLookupEUSCIA(#BSP430_PERIPH_EUSCI_A3).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_A3
#define configBSP430_HPL_EUSCI_A3 (configBSP430_HAL_EUSCI_A3 - 0)
#endif /* configBSP430_HPL_EUSCI_A3 */

#if (configBSP430_HAL_EUSCI_A3 - 0) && ! (configBSP430_HPL_EUSCI_A3 - 0)
#warning configBSP430_HAL_EUSCI_A3 requested without configBSP430_HPL_EUSCI_A3
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A3 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_A3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A3 - 0)
#define BSP430_PERIPH_EUSCI_A3 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_A3_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_A3 */

/* END AUTOMATICALLY GENERATED CODE [periph_decl] */
/* !BSP430! end=periph_decl */

/* !BSP430! insert=hpl_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_decl] */
/** sBSP430hplEUSCIA HPL pointer for #BSP430_PERIPH_EUSCI_A0.
 *
 * This pointer to a volatile structure overlaying the EUSCI_A0
 * peripheral register map may be used only if
 * #configBSP430_HPL_EUSCI_A0 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A0 - 0)
#define BSP430_HPL_EUSCI_A0 ((volatile sBSP430hplEUSCIA *)BSP430_PERIPH_EUSCI_A0)
#endif /* configBSP430_HPL_EUSCI_A0 */

/** sBSP430hplEUSCIA HPL pointer for #BSP430_PERIPH_EUSCI_A1.
 *
 * This pointer to a volatile structure overlaying the EUSCI_A1
 * peripheral register map may be used only if
 * #configBSP430_HPL_EUSCI_A1 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A1 - 0)
#define BSP430_HPL_EUSCI_A1 ((volatile sBSP430hplEUSCIA *)BSP430_PERIPH_EUSCI_A1)
#endif /* configBSP430_HPL_EUSCI_A1 */

/** sBSP430hplEUSCIA HPL pointer for #BSP430_PERIPH_EUSCI_A2.
 *
 * This pointer to a volatile structure overlaying the EUSCI_A2
 * peripheral register map may be used only if
 * #configBSP430_HPL_EUSCI_A2 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A2 - 0)
#define BSP430_HPL_EUSCI_A2 ((volatile sBSP430hplEUSCIA *)BSP430_PERIPH_EUSCI_A2)
#endif /* configBSP430_HPL_EUSCI_A2 */

/** sBSP430hplEUSCIA HPL pointer for #BSP430_PERIPH_EUSCI_A3.
 *
 * This pointer to a volatile structure overlaying the EUSCI_A3
 * peripheral register map may be used only if
 * #configBSP430_HPL_EUSCI_A3 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_A3 - 0)
#define BSP430_HPL_EUSCI_A3 ((volatile sBSP430hplEUSCIA *)BSP430_PERIPH_EUSCI_A3)
#endif /* configBSP430_HPL_EUSCI_A3 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_EUSCI_A0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_EUSCI_A0 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_EUSCI_A0.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A0_ISR
#define configBSP430_HAL_EUSCI_A0_ISR (configBSP430_HAL_EUSCI_A0 - 0)
#endif /* configBSP430_HAL_EUSCI_A0_ISR */

#if (configBSP430_HAL_EUSCI_A0_ISR - 0) && ! (configBSP430_HAL_EUSCI_A0 - 0)
#warning configBSP430_HAL_EUSCI_A0_ISR requested without configBSP430_HAL_EUSCI_A0
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_EUSCI_A1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_EUSCI_A1 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_EUSCI_A1.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A1_ISR
#define configBSP430_HAL_EUSCI_A1_ISR (configBSP430_HAL_EUSCI_A1 - 0)
#endif /* configBSP430_HAL_EUSCI_A1_ISR */

#if (configBSP430_HAL_EUSCI_A1_ISR - 0) && ! (configBSP430_HAL_EUSCI_A1 - 0)
#warning configBSP430_HAL_EUSCI_A1_ISR requested without configBSP430_HAL_EUSCI_A1
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_EUSCI_A2
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_EUSCI_A2 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_EUSCI_A2.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A2_ISR
#define configBSP430_HAL_EUSCI_A2_ISR (configBSP430_HAL_EUSCI_A2 - 0)
#endif /* configBSP430_HAL_EUSCI_A2_ISR */

#if (configBSP430_HAL_EUSCI_A2_ISR - 0) && ! (configBSP430_HAL_EUSCI_A2 - 0)
#warning configBSP430_HAL_EUSCI_A2_ISR requested without configBSP430_HAL_EUSCI_A2
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_EUSCI_A3
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_EUSCI_A3 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_EUSCI_A3.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A3_ISR
#define configBSP430_HAL_EUSCI_A3_ISR (configBSP430_HAL_EUSCI_A3 - 0)
#endif /* configBSP430_HAL_EUSCI_A3_ISR */

#if (configBSP430_HAL_EUSCI_A3_ISR - 0) && ! (configBSP430_HAL_EUSCI_A3 - 0)
#warning configBSP430_HAL_EUSCI_A3_ISR requested without configBSP430_HAL_EUSCI_A3
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

/* !BSP430! periph=euscib instance=EUSCI_B0,EUSCI_B1 insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** Control inclusion of the @HPL interface to #BSP430_PERIPH_EUSCI_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplEUSCIB structure reference is
 * available as #BSP430_HPL_EUSCI_B0.
 *
 * It may also be obtained using
 * #xBSP430hplLookupEUSCIB(#BSP430_PERIPH_EUSCI_B0).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_B0
#define configBSP430_HPL_EUSCI_B0 (configBSP430_HAL_EUSCI_B0 - 0)
#endif /* configBSP430_HPL_EUSCI_B0 */

#if (configBSP430_HAL_EUSCI_B0 - 0) && ! (configBSP430_HPL_EUSCI_B0 - 0)
#warning configBSP430_HAL_EUSCI_B0 requested without configBSP430_HPL_EUSCI_B0
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_B0 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_B0 - 0)
#define BSP430_PERIPH_EUSCI_B0 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_B0_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_B0 */

/** Control inclusion of the @HPL interface to #BSP430_PERIPH_EUSCI_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interface be included, and 0 to request it be excluded.  By default
 * the interface is excluded.
 *
 * When enabled, the sBSP430hplEUSCIB structure reference is
 * available as #BSP430_HPL_EUSCI_B1.
 *
 * It may also be obtained using
 * #xBSP430hplLookupEUSCIB(#BSP430_PERIPH_EUSCI_B1).
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HPL_EUSCI_B1
#define configBSP430_HPL_EUSCI_B1 (configBSP430_HAL_EUSCI_B1 - 0)
#endif /* configBSP430_HPL_EUSCI_B1 */

#if (configBSP430_HAL_EUSCI_B1 - 0) && ! (configBSP430_HPL_EUSCI_B1 - 0)
#warning configBSP430_HAL_EUSCI_B1 requested without configBSP430_HPL_EUSCI_B1
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_B1 device.
 *
 * The handle may be used only if #configBSP430_HPL_EUSCI_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_B1 - 0)
#define BSP430_PERIPH_EUSCI_B1 ((tBSP430periphHandle)(BSP430_PERIPH_EUSCI_B1_BASEADDRESS_))
#endif /* configBSP430_HPL_EUSCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [periph_decl] */
/* !BSP430! end=periph_decl */

/* !BSP430! insert=hpl_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_decl] */
/** sBSP430hplEUSCIB HPL pointer for #BSP430_PERIPH_EUSCI_B0.
 *
 * This pointer to a volatile structure overlaying the EUSCI_B0
 * peripheral register map may be used only if
 * #configBSP430_HPL_EUSCI_B0 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_B0 - 0)
#define BSP430_HPL_EUSCI_B0 ((volatile sBSP430hplEUSCIB *)BSP430_PERIPH_EUSCI_B0)
#endif /* configBSP430_HPL_EUSCI_B0 */

/** sBSP430hplEUSCIB HPL pointer for #BSP430_PERIPH_EUSCI_B1.
 *
 * This pointer to a volatile structure overlaying the EUSCI_B1
 * peripheral register map may be used only if
 * #configBSP430_HPL_EUSCI_B1 is defined to a true value.
 *
 * @dependency #configBSP430_HPL_EUSCI_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_EUSCI_B1 - 0)
#define BSP430_HPL_EUSCI_B1 ((volatile sBSP430hplEUSCIB *)BSP430_PERIPH_EUSCI_B1)
#endif /* configBSP430_HPL_EUSCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_EUSCI_B0
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_EUSCI_B0 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_EUSCI_B0.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_B0_ISR
#define configBSP430_HAL_EUSCI_B0_ISR (configBSP430_HAL_EUSCI_B0 - 0)
#endif /* configBSP430_HAL_EUSCI_B0_ISR */

#if (configBSP430_HAL_EUSCI_B0_ISR - 0) && ! (configBSP430_HAL_EUSCI_B0 - 0)
#warning configBSP430_HAL_EUSCI_B0_ISR requested without configBSP430_HAL_EUSCI_B0
#endif /* HAL_ISR and not HAL */

/** Control inclusion of the primary @HAL interrupt handler for #BSP430_PERIPH_EUSCI_B1
 *
 * This must be defined to 1 in @c bsp430_config.h to request the
 * interrupt handler be included, and 0 to request it be excluded.
 * Use of the interrupt handler requires that the corresponding @HAL
 * be requested.  By default the interface is included when
 * #configBSP430_HAL_EUSCI_B1 is set, but it may be explicitly
 * disabled if you intend to provide your own implementation or will
 * not be using the interrupt features.
 *
 * Interact with the handler using the @ref callbacks via #BSP430_HAL_EUSCI_B1.
 *
 * @cppflag
 * @ingroup grp_config_core
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_B1_ISR
#define configBSP430_HAL_EUSCI_B1_ISR (configBSP430_HAL_EUSCI_B1 - 0)
#endif /* configBSP430_HAL_EUSCI_B1_ISR */

#if (configBSP430_HAL_EUSCI_B1_ISR - 0) && ! (configBSP430_HAL_EUSCI_B1 - 0)
#warning configBSP430_HAL_EUSCI_B1_ISR requested without configBSP430_HAL_EUSCI_B1
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

/** eUSCI-specific implementation of hBSP430serialOpenUART() */
hBSP430halSERIAL hBSP430eusciOpenUART (hBSP430halSERIAL hal,
                                       unsigned char ctl0_byte,
                                       unsigned char ctl1_byte,
                                       unsigned long baud);

/** eUSCI-specific implementation of hBSP430serialOpenSPI() */
hBSP430halSERIAL hBSP430eusciOpenSPI (hBSP430halSERIAL hal,
                                      unsigned char ctl0_byte,
                                      unsigned char ctl1_byte,
                                      unsigned int prescaler);

/** eUSCI-specific implementation of hBSP430serialOpenI2C() */
hBSP430halSERIAL hBSP430eusciOpenI2C (hBSP430halSERIAL hal,
                                      unsigned char ctl0_byte,
                                      unsigned char ctl1_byte,
                                      unsigned int prescaler);

/** eUSCI-specific implementation of iBSP430serialSetReset_rh() */
int iBSP430eusciSetReset_rh (hBSP430halSERIAL hal, int resetp);

/** eUSCI-specific implementation of iBSP430serialSetHold_rh() */
int iBSP430eusciSetHold_rh (hBSP430halSERIAL hal, int holdp);

/** eUSCI-specific implementation of iBSP430serialClose() */
int iBSP430eusciClose (hBSP430halSERIAL hal);

/** eUSCI-specific implementation of vBSP430serialWakeupTransmit_rh() */
void vBSP430eusciWakeupTransmit_rh (hBSP430halSERIAL device);

/** eUSCI-specific implementation of vBSP430serialFlush_ni() */
void vBSP430eusciFlush_ni (hBSP430halSERIAL device);

/** eUSCI-specific implementation of iBSP430uartRxByte_rh() */
int iBSP430eusciUARTrxByte_rh (hBSP430halSERIAL device);

/** eUSCI-specific implementation of iBSP430uartTxByte_rh() */
int iBSP430eusciUARTtxByte_rh (hBSP430halSERIAL device, uint8_t c);

/** eUSCI-specific implementation of iBSP430uartTxData_rh() */
int iBSP430eusciUARTtxData_rh (hBSP430halSERIAL device, const uint8_t * data, size_t len);

/** eUSCI-specific implementation of iBSP430uartTxASCIIZ_rh() */
int iBSP430eusciUARTtxASCIIZ_rh (hBSP430halSERIAL device, const char * str);

/** eUSCI-specific implementation of iBSP430spiTxRx_rh() */
int iBSP430eusciSPITxRx_rh (hBSP430halSERIAL hal,
                            const uint8_t * tx_data,
                            size_t tx_len,
                            size_t rx_len,
                            uint8_t * rx_data);


/** eUSCI-specific implementation of iBSP430i2cSetAddresses_rh() */
int iBSP430eusciI2CsetAddresses_rh (hBSP430halSERIAL hal,
                                    int own_address,
                                    int slave_address);

/** eUSCI-specific implementation of iBSP430i2cRxData_rh() */
int iBSP430eusciI2CrxData_rh (hBSP430halSERIAL hal,
                              uint8_t * rx_data,
                              size_t rx_len);

/** eUSCI-specific implementation of iBSP430i2cTxData_rh() */
int iBSP430eusciI2CtxData_rh (hBSP430halSERIAL hal,
                              const uint8_t * tx_data,
                              size_t tx_len);

/** Get the HPL handle for a specific EUSCIA instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_EUSCI_A0.
 *
 * @return A typed pointer that can be used to manipulate the
 * peripheral.  A null pointer is returned if the handle does not
 * correspond to a timer for which the HPL interface been enabled
 * (e.g., with #configBSP430_HPL_EUSCI_A0).
 */
static BSP430_CORE_INLINE
volatile sBSP430hplEUSCIA *
xBSP430hplLookupEUSCIA (tBSP430periphHandle periph)
{
  /* !BSP430! periph=eusci instance=EUSCI_A0,EUSCI_A1,EUSCI_A2,EUSCI_A2 */
  /* !BSP430! insert=periph_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_demux] */
#if (configBSP430_HPL_EUSCI_A0 - 0)
  if (BSP430_PERIPH_EUSCI_A0 == periph) {
    return BSP430_HPL_EUSCI_A0;
  }
#endif /* configBSP430_HPL_EUSCI_A0 */

#if (configBSP430_HPL_EUSCI_A1 - 0)
  if (BSP430_PERIPH_EUSCI_A1 == periph) {
    return BSP430_HPL_EUSCI_A1;
  }
#endif /* configBSP430_HPL_EUSCI_A1 */

#if (configBSP430_HPL_EUSCI_A2 - 0)
  if (BSP430_PERIPH_EUSCI_A2 == periph) {
    return BSP430_HPL_EUSCI_A2;
  }
#endif /* configBSP430_HPL_EUSCI_A2 */

#if (configBSP430_HPL_EUSCI_A2 - 0)
  if (BSP430_PERIPH_EUSCI_A2 == periph) {
    return BSP430_HPL_EUSCI_A2;
  }
#endif /* configBSP430_HPL_EUSCI_A2 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_demux] */
  /* !BSP430! end=periph_hpl_demux */
  return NULL;
}

/** Get the HPL handle for a specific EUSCIB instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_EUSCI_B0.
 *
 * @return A typed pointer that can be used to manipulate the
 * peripheral.  A null pointer is returned if the handle does not
 * correspond to a timer for which the HPL interface been enabled
 * (e.g., with #configBSP430_HPL_EUSCI_B0).
 */
static BSP430_CORE_INLINE
volatile sBSP430hplEUSCIB *
xBSP430hplLookupEUSCIB (tBSP430periphHandle periph)
{
  /* !BSP430! periph=eusci instance=EUSCI_B0,EUSCI_B1 */
  /* !BSP430! insert=periph_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_demux] */
#if (configBSP430_HPL_EUSCI_B0 - 0)
  if (BSP430_PERIPH_EUSCI_B0 == periph) {
    return BSP430_HPL_EUSCI_B0;
  }
#endif /* configBSP430_HPL_EUSCI_B0 */

#if (configBSP430_HPL_EUSCI_B1 - 0)
  if (BSP430_PERIPH_EUSCI_B1 == periph) {
    return BSP430_HPL_EUSCI_B1;
  }
#endif /* configBSP430_HPL_EUSCI_B1 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_demux] */
  /* !BSP430! end=periph_hpl_demux */
  return NULL;
}

/** Get the HAL handle for a specific EUSCI (A or B) instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_EUSCI_A0.
 *
 * @return the HAL handle for the peripheral.  A null pointer is
 * returned if the handle does not correspond to a timer for which the
 * HAL interface has been enabled (e.g., with #configBSP430_HAL_EUSCI_A0).
 */
static BSP430_CORE_INLINE
hBSP430halSERIAL
hBSP430eusciLookup (tBSP430periphHandle periph)
{
  /* !BSP430! periph=eusci instance=EUSCI_A0,EUSCI_A1,EUSCI_A2,EUSCI_A3,EUSCI_B0,EUSCI_B1 */
  /* !BSP430! insert=periph_hal_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hal_demux] */
#if (configBSP430_HAL_EUSCI_A0 - 0)
  if (BSP430_PERIPH_EUSCI_A0 == periph) {
    return BSP430_HAL_EUSCI_A0;
  }
#endif /* configBSP430_HAL_EUSCI_A0 */

#if (configBSP430_HAL_EUSCI_A1 - 0)
  if (BSP430_PERIPH_EUSCI_A1 == periph) {
    return BSP430_HAL_EUSCI_A1;
  }
#endif /* configBSP430_HAL_EUSCI_A1 */

#if (configBSP430_HAL_EUSCI_A2 - 0)
  if (BSP430_PERIPH_EUSCI_A2 == periph) {
    return BSP430_HAL_EUSCI_A2;
  }
#endif /* configBSP430_HAL_EUSCI_A2 */

#if (configBSP430_HAL_EUSCI_A3 - 0)
  if (BSP430_PERIPH_EUSCI_A3 == periph) {
    return BSP430_HAL_EUSCI_A3;
  }
#endif /* configBSP430_HAL_EUSCI_A3 */

#if (configBSP430_HAL_EUSCI_B0 - 0)
  if (BSP430_PERIPH_EUSCI_B0 == periph) {
    return BSP430_HAL_EUSCI_B0;
  }
#endif /* configBSP430_HAL_EUSCI_B0 */

#if (configBSP430_HAL_EUSCI_B1 - 0)
  if (BSP430_PERIPH_EUSCI_B1 == periph) {
    return BSP430_HAL_EUSCI_B1;
  }
#endif /* configBSP430_HAL_EUSCI_B1 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hal_demux] */
  /* !BSP430! end=periph_hal_demux */
  return NULL;
}

/** Get a human-readable identifier for the EUSCI (A or B) peripheral
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_EUSCI_A0.
 *
 * @return The short name of the port, e.g. "EUSCI_A0".  If the peripheral
 * is not recognized as a EUSCI-A device, a null pointer is returned. */
const char * xBSP430eusciName (tBSP430periphHandle periph);

#endif /* BSP430_MODULE_EUSCI */

#endif /* BSP430_PERIPH_EUSCI_H */

