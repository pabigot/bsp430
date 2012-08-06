/* Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>
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
 * @brief Genericized UCSI (USCI_A/USCI_B) for 5xx/6xx devices.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/* !BSP430! periph=usci5 */
/* !BSP430! instance=USCI5_A0,USCI5_A1,USCI5_A2,USCI5_A3,USCI5_B0,USCI5_B1,USCI5_B2,USCI5_B3 */

#ifndef BSP430_PERIPH_USCI5_H
#define BSP430_PERIPH_USCI5_H

#include <bsp430/periph.h>
#include <bsp430/serial.h>

#if ! defined(__MSP430_HAS_USCI_A0__)
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_USCI_A0__ */

/** Maximum baud rate acceptable for UART mode.
 *
 * Not to say you can actually get this to work, but the API will reject
 * configuration of a UART rate above this. */
#ifndef BSP430_USCI5_UART_MAX_BAUD
#define BSP430_USCI5_UART_MAX_BAUD 1000000UL
#endif /* BSP430_USCI5_UART_MAX_BAUD */

/** Register map for USCI_A/USCI_B peripheral on a MSP430 5xx/6xx MCU. */
typedef struct sBSP430hplUSCI5 {
  union {						/* 0x00 */
    unsigned int ctlw0;		/**< UCtxCTLW0 */
    struct {
      unsigned char ctl1;	/**< UCtxCTL1 */
      unsigned char ctl0;	/**< UCtxCTL0 */
    };
  };
  unsigned int _unused_0x02;
  unsigned int _unused_0x04;
  union {						/* 0x06 */
    unsigned int brw;		/**< UCtxBRW */
    struct {
      unsigned char br0;	/**< UCtxBR0 */
      unsigned char br1;	/**< UCtxBR1 */
    };
  };
  union {						/* 0x08 */
    struct { /* USCI_A */
      unsigned char mctl;	/**< UCAxMCTL (UART) */
      unsigned char _reserved_0x09;
    };
    unsigned int _reserved_0x08; /* USCI_B */
  };
  unsigned char stat;			/**< UCtxSTAT */ /* 0x0A */
  unsigned char _reserved_0x0B;
  unsigned char rxbuf;		/**< UCtxRXBUF */ /* 0x0C */
  unsigned char _reserved_0x0D;
  unsigned char txbuf;		/**< UCtxTXBUF */ /* 0x0E */
  unsigned char _reserved_0x0F;
  union {						/* 0x10 */
    struct {				/* USCI_A */
      unsigned char abctl; /**< UCAxABCTL (UART) */
      unsigned char _reserved_0x11;
    };
    unsigned int i2coa;		/**< UCBxI2COA (I2C) */ /* USCI_B */
  };
  union {						/* 0x12 */
    unsigned int irctl;		/**< UCAxIRCTL (UART)*/
    struct {
      unsigned char irtctl; /**< UCAxIRTCTL (UART) */
      unsigned char irrctl; /**< UCAxIRRCTL (UART) */
    };
    unsigned int i2csa;		/**< UCBxI2CSA (I2C) */
  };
  unsigned int _unused_0x14;
  unsigned int _unused_0x16;
  unsigned int _unused_0x18;
  unsigned int _unused_0x1a;
  union {						/* 0x1C */
    unsigned int ictl;		/**< UCtxICTL */
    struct {
      unsigned char ie;	/**< UCtxIE */
      unsigned char ifg;	/**< UCtxIFG */
    };
  };
  unsigned int iv;			/**< UCtxIV */ /* 0x1E */
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
/** @def configBSP430_HAL_USCI5_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI5_A0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_A0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A0
#define configBSP430_HAL_USCI5_A0 0
#endif /* configBSP430_HAL_USCI5_A0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_A0 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_A0_;
#endif /* configBSP430_HAL_USCI5_A0 */
/** @endcond */

/** BSP430 HAL handle for USCI5_A0.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A0 - 0)
#define BSP430_HAL_USCI5_A0 (&xBSP430hal_USCI5_A0_)
#endif /* configBSP430_HAL_USCI5_A0 */

/** @def configBSP430_HAL_USCI5_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI5_A1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_A1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A1
#define configBSP430_HAL_USCI5_A1 0
#endif /* configBSP430_HAL_USCI5_A1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_A1 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_A1_;
#endif /* configBSP430_HAL_USCI5_A1 */
/** @endcond */

/** BSP430 HAL handle for USCI5_A1.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A1 - 0)
#define BSP430_HAL_USCI5_A1 (&xBSP430hal_USCI5_A1_)
#endif /* configBSP430_HAL_USCI5_A1 */

/** @def configBSP430_HAL_USCI5_A2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A2 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI5_A2 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_A2 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A2
#define configBSP430_HAL_USCI5_A2 0
#endif /* configBSP430_HAL_USCI5_A2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_A2 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_A2_;
#endif /* configBSP430_HAL_USCI5_A2 */
/** @endcond */

/** BSP430 HAL handle for USCI5_A2.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A2 - 0)
#define BSP430_HAL_USCI5_A2 (&xBSP430hal_USCI5_A2_)
#endif /* configBSP430_HAL_USCI5_A2 */

/** @def configBSP430_HAL_USCI5_A3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A3 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI5_A3 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_A3 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A3
#define configBSP430_HAL_USCI5_A3 0
#endif /* configBSP430_HAL_USCI5_A3 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_A3 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_A3_;
#endif /* configBSP430_HAL_USCI5_A3 */
/** @endcond */

/** BSP430 HAL handle for USCI5_A3.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_A3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A3 - 0)
#define BSP430_HAL_USCI5_A3 (&xBSP430hal_USCI5_A3_)
#endif /* configBSP430_HAL_USCI5_A3 */

/** @def configBSP430_HAL_USCI5_B0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI5_B0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_B0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B0
#define configBSP430_HAL_USCI5_B0 0
#endif /* configBSP430_HAL_USCI5_B0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_B0 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_B0_;
#endif /* configBSP430_HAL_USCI5_B0 */
/** @endcond */

/** BSP430 HAL handle for USCI5_B0.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B0 - 0)
#define BSP430_HAL_USCI5_B0 (&xBSP430hal_USCI5_B0_)
#endif /* configBSP430_HAL_USCI5_B0 */

/** @def configBSP430_HAL_USCI5_B1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI5_B1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_B1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B1
#define configBSP430_HAL_USCI5_B1 0
#endif /* configBSP430_HAL_USCI5_B1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_B1 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_B1_;
#endif /* configBSP430_HAL_USCI5_B1 */
/** @endcond */

/** BSP430 HAL handle for USCI5_B1.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B1 - 0)
#define BSP430_HAL_USCI5_B1 (&xBSP430hal_USCI5_B1_)
#endif /* configBSP430_HAL_USCI5_B1 */

/** @def configBSP430_HAL_USCI5_B2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B2 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI5_B2 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_B2 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B2
#define configBSP430_HAL_USCI5_B2 0
#endif /* configBSP430_HAL_USCI5_B2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_B2 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_B2_;
#endif /* configBSP430_HAL_USCI5_B2 */
/** @endcond */

/** BSP430 HAL handle for USCI5_B2.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_B2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI5_B2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B2 - 0)
#define BSP430_HAL_USCI5_B2 (&xBSP430hal_USCI5_B2_)
#endif /* configBSP430_HAL_USCI5_B2 */

/** @def configBSP430_HAL_USCI5_B3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B3 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI5_B3 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_B3 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B3
#define configBSP430_HAL_USCI5_B3 0
#endif /* configBSP430_HAL_USCI5_B3 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI5_B3 - 0
/* You don't need to know about this */
extern sBSP430halSERIAL xBSP430hal_USCI5_B3_;
#endif /* configBSP430_HAL_USCI5_B3 */
/** @endcond */

/** BSP430 HAL handle for USCI5_B3.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_B3
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
/** @def configBSP430_HPL_USCI5_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_A0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_USCI5_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_A1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_USCI5_A2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_A2 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_USCI5_A3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A3 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_A3 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_USCI5_B0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_B0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_USCI5_B1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_B1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_USCI5_B2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_B2 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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

/** @def configBSP430_HPL_USCI5_B3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B3 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_B3 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
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
/** HPL pointer for USCI5_A0.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_A0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A0 - 0)
#define BSP430_HPL_USCI5_A0 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_A0)
#endif /* configBSP430_HPL_USCI5_A0 */

/** HPL pointer for USCI5_A1.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_A1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A1 - 0)
#define BSP430_HPL_USCI5_A1 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_A1)
#endif /* configBSP430_HPL_USCI5_A1 */

/** HPL pointer for USCI5_A2.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_A2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_A2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A2 - 0)
#define BSP430_HPL_USCI5_A2 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_A2)
#endif /* configBSP430_HPL_USCI5_A2 */

/** HPL pointer for USCI5_A3.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_A3
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_A3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A3 - 0)
#define BSP430_HPL_USCI5_A3 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_A3)
#endif /* configBSP430_HPL_USCI5_A3 */

/** HPL pointer for USCI5_B0.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_B0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B0 - 0)
#define BSP430_HPL_USCI5_B0 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_B0)
#endif /* configBSP430_HPL_USCI5_B0 */

/** HPL pointer for USCI5_B1.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_B1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B1 - 0)
#define BSP430_HPL_USCI5_B1 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_B1)
#endif /* configBSP430_HPL_USCI5_B1 */

/** HPL pointer for USCI5_B2.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_B2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_B2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B2 - 0)
#define BSP430_HPL_USCI5_B2 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_B2)
#endif /* configBSP430_HPL_USCI5_B2 */

/** HPL pointer for USCI5_B3.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_B3
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_B3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI5_B3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B3 - 0)
#define BSP430_HPL_USCI5_B3 ((volatile sBSP430hplUSCI5 *)BSP430_PERIPH_USCI5_B3)
#endif /* configBSP430_HPL_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** @def configBSP430_HAL_USCI5_A0_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI5_A0, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI5_A0 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_USCI5_A0
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A0_ISR
#define configBSP430_HAL_USCI5_A0_ISR (configBSP430_HAL_USCI5_A0 - 0)
#endif /* configBSP430_HAL_USCI5_A0_ISR */

#if (configBSP430_HAL_USCI5_A0_ISR - 0) && ! (configBSP430_HAL_USCI5_A0 - 0)
#warning configBSP430_HAL_USCI5_A0_ISR requested without configBSP430_HAL_USCI5_A0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCI5_A1_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI5_A1, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI5_A1 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_USCI5_A1
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A1_ISR
#define configBSP430_HAL_USCI5_A1_ISR (configBSP430_HAL_USCI5_A1 - 0)
#endif /* configBSP430_HAL_USCI5_A1_ISR */

#if (configBSP430_HAL_USCI5_A1_ISR - 0) && ! (configBSP430_HAL_USCI5_A1 - 0)
#warning configBSP430_HAL_USCI5_A1_ISR requested without configBSP430_HAL_USCI5_A1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCI5_A2_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI5_A2, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI5_A2 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_USCI5_A2
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A2_ISR
#define configBSP430_HAL_USCI5_A2_ISR (configBSP430_HAL_USCI5_A2 - 0)
#endif /* configBSP430_HAL_USCI5_A2_ISR */

#if (configBSP430_HAL_USCI5_A2_ISR - 0) && ! (configBSP430_HAL_USCI5_A2 - 0)
#warning configBSP430_HAL_USCI5_A2_ISR requested without configBSP430_HAL_USCI5_A2
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCI5_A3_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI5_A3, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI5_A3 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_USCI5_A3
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A3_ISR
#define configBSP430_HAL_USCI5_A3_ISR (configBSP430_HAL_USCI5_A3 - 0)
#endif /* configBSP430_HAL_USCI5_A3_ISR */

#if (configBSP430_HAL_USCI5_A3_ISR - 0) && ! (configBSP430_HAL_USCI5_A3 - 0)
#warning configBSP430_HAL_USCI5_A3_ISR requested without configBSP430_HAL_USCI5_A3
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCI5_B0_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI5_B0, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI5_B0 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_USCI5_B0
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B0_ISR
#define configBSP430_HAL_USCI5_B0_ISR (configBSP430_HAL_USCI5_B0 - 0)
#endif /* configBSP430_HAL_USCI5_B0_ISR */

#if (configBSP430_HAL_USCI5_B0_ISR - 0) && ! (configBSP430_HAL_USCI5_B0 - 0)
#warning configBSP430_HAL_USCI5_B0_ISR requested without configBSP430_HAL_USCI5_B0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCI5_B1_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI5_B1, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI5_B1 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_USCI5_B1
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B1_ISR
#define configBSP430_HAL_USCI5_B1_ISR (configBSP430_HAL_USCI5_B1 - 0)
#endif /* configBSP430_HAL_USCI5_B1_ISR */

#if (configBSP430_HAL_USCI5_B1_ISR - 0) && ! (configBSP430_HAL_USCI5_B1 - 0)
#warning configBSP430_HAL_USCI5_B1_ISR requested without configBSP430_HAL_USCI5_B1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCI5_B2_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI5_B2, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI5_B2 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_USCI5_B2
 * also be true.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B2_ISR
#define configBSP430_HAL_USCI5_B2_ISR (configBSP430_HAL_USCI5_B2 - 0)
#endif /* configBSP430_HAL_USCI5_B2_ISR */

#if (configBSP430_HAL_USCI5_B2_ISR - 0) && ! (configBSP430_HAL_USCI5_B2 - 0)
#warning configBSP430_HAL_USCI5_B2_ISR requested without configBSP430_HAL_USCI5_B2
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCI5_B3_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI5_B3, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI5_B3 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_USCI5_B3
 * also be true.
 *
 * @cppflag
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

/** USCI5-specific implementation of iBSP430serialConfigureCallbacks() */
int iBSP430usci5ConfigureCallbacks (hBSP430halSERIAL device,
                                    const struct sBSP430halISRCallbackVoid * rx_callback,
                                    const struct sBSP430halISRCallbackVoid * tx_callback);

/** USCI5-specific implementation of iBSP430serialClose() */
int iBSP430usci5Close (hBSP430halSERIAL xUSCI);

/** USCI5-specific implementation of vBSP430serialWakeupTransmit_ni() */
void vBSP430usci5WakeupTransmit_ni (hBSP430halSERIAL device);

/** USCI5-specific implementation of vBSP430serialFlush_ni() */
void vBSP430usci5Flush_ni (hBSP430halSERIAL device);

/** USCI5-specific implementation of iBSP430serialUARTrxByte_ni() */
int iBSP430usci5UARTrxByte_ni (hBSP430halSERIAL device);

/** USCI5-specific implementation of iBSP430serialUARTtxByte_ni() */
int iBSP430usci5UARTtxByte_ni (hBSP430halSERIAL device, uint8_t c);

/** USCI5-specific implementation of iBSP430serialUARTtxData_ni() */
int iBSP430usci5UARTtxData_ni (hBSP430halSERIAL device, const uint8_t * data, size_t len);

/** USCI5-specific implementation of iBSP430serialUARTtxASCIIZ_ni() */
int iBSP430usci5UARTtxASCIIZ_ni (hBSP430halSERIAL device, const char * str);

/** USCI5-specific implementation of iBSP430serialSPITxRx_ni() */
int iBSP430usci5SPITxRx_ni (hBSP430halSERIAL hal,
                                               const uint8_t * tx_data,
                                               size_t tx_len,
                                               size_t rx_len,
                                               uint8_t * rx_data);

/** Get the HPL handle for a specific USCI5 instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI5_A0.
 *
 * @return A typed pointer that can be used to manipulate the
 * peripheral.  A null pointer is returned if the handle does not
 * correspond to a timer for which the HPL interface been enabled
 * (e.g., with #configBSP430_HPL_USCI5_A0).
 */
volatile sBSP430hplUSCI5 * xBSP430hplLookupUSCI5 (tBSP430periphHandle periph);

/** Get the HAL handle for a specific USCI5 instance.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI5_A0.
 *
 * @return the HAL handle for the peripheral.  A null pointer is
 * returned if the handle does not correspond to a timer for which the
 * HAL interface has been enabled (e.g., with #configBSP430_HAL_USCI5_A0).
 */
hBSP430halSERIAL xBSP430usciLookup5 (tBSP430periphHandle periph);

/** Get a human-readable identifier for the USCI5 peripheral
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_USCI5_A0.
 *
 * @return The short name of the port, e.g. "USCI5_A0".  If the peripheral
 * is not recognized as a USCI5 device, a null pointer is returned. */
const char * xBSP430usci5Name (tBSP430periphHandle periph);

#endif /* BSP430_PERIPH_USCI5_H */
