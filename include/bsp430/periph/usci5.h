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
#if ! defined(__MSP430_HAS_USCI_A0__)
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_USCI_A0__ */

/** Register map for USCI_A/USCI_B peripheral on a MSP430 5xx/6xx MCU. */
typedef struct sBSP430periphUSCI5 {
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
} sBSP430periphUSCI5;

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

/** Structure holding hardware abstraction layer state for eUSCI_A.
 *
 * This structure is internal state, for access by applications only
 * when overriding BSP430 HAL capabilities. */
typedef struct sBSP430usci5State {
  /** Flags indicating various things: primarily, whether anybody is
   * using the device. */
  unsigned int flags;

  /** Pointer to the peripheral register structure. */
  volatile sBSP430periphUSCI5 * const usci5;

  /** The callback chain to invoke when a byte is received */
  const struct sBSP430periphISRCallbackVoid * rx_callback;

  /** The callback chain to invoke when space is available in the
   * transmission buffer */
  const struct sBSP430periphISRCallbackVoid * tx_callback;

  /** Location to store a single incoming character when #rx_queue
   * is undefined. */
  uint8_t rx_byte;

  /** Location to store a single outgoing character when #tx_queue
   * is undefined.  This is probably not very useful. */
  uint8_t tx_byte;

  /** Total number of received octets */
  unsigned long num_rx;

  /** Total number of transmitted octets */
  unsigned long num_tx;
} sBSP430usci5State;

/** The USCI5 internal state is private to the implementation. */
typedef struct sBSP430usci5State * tBSP430usci5Handle;

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** @def configBSP430_HAL_USCI5_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A0 peripheral HAL interface.  This defines a global
 * object tBSP430usci5Handle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_A0 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A0
#define configBSP430_HAL_USCI5_A0 0
#endif /* configBSP430_HAL_USCI5_A0 */

/** BSP430 HAL handle for USCI5_A0.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_A0
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A0 - 0)
extern tBSP430usci5Handle const hBSP430usci5_USCI5_A0;
#endif /* configBSP430_HAL_USCI5_A0 */

/** @def configBSP430_HAL_USCI5_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A1 peripheral HAL interface.  This defines a global
 * object tBSP430usci5Handle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_A1 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A1
#define configBSP430_HAL_USCI5_A1 0
#endif /* configBSP430_HAL_USCI5_A1 */

/** BSP430 HAL handle for USCI5_A1.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_A1
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A1 - 0)
extern tBSP430usci5Handle const hBSP430usci5_USCI5_A1;
#endif /* configBSP430_HAL_USCI5_A1 */

/** @def configBSP430_HAL_USCI5_A2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A2 peripheral HAL interface.  This defines a global
 * object tBSP430usci5Handle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_A2 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A2
#define configBSP430_HAL_USCI5_A2 0
#endif /* configBSP430_HAL_USCI5_A2 */

/** BSP430 HAL handle for USCI5_A2.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_A2
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A2 - 0)
extern tBSP430usci5Handle const hBSP430usci5_USCI5_A2;
#endif /* configBSP430_HAL_USCI5_A2 */

/** @def configBSP430_HAL_USCI5_A3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A3 peripheral HAL interface.  This defines a global
 * object tBSP430usci5Handle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_A3 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_A3
#define configBSP430_HAL_USCI5_A3 0
#endif /* configBSP430_HAL_USCI5_A3 */

/** BSP430 HAL handle for USCI5_A3.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_A3
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_A3 - 0)
extern tBSP430usci5Handle const hBSP430usci5_USCI5_A3;
#endif /* configBSP430_HAL_USCI5_A3 */

/** @def configBSP430_HAL_USCI5_B0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B0 peripheral HAL interface.  This defines a global
 * object tBSP430usci5Handle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_B0 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B0
#define configBSP430_HAL_USCI5_B0 0
#endif /* configBSP430_HAL_USCI5_B0 */

/** BSP430 HAL handle for USCI5_B0.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_B0
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B0 - 0)
extern tBSP430usci5Handle const hBSP430usci5_USCI5_B0;
#endif /* configBSP430_HAL_USCI5_B0 */

/** @def configBSP430_HAL_USCI5_B1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B1 peripheral HAL interface.  This defines a global
 * object tBSP430usci5Handle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_B1 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B1
#define configBSP430_HAL_USCI5_B1 0
#endif /* configBSP430_HAL_USCI5_B1 */

/** BSP430 HAL handle for USCI5_B1.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_B1
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B1 - 0)
extern tBSP430usci5Handle const hBSP430usci5_USCI5_B1;
#endif /* configBSP430_HAL_USCI5_B1 */

/** @def configBSP430_HAL_USCI5_B2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B2 peripheral HAL interface.  This defines a global
 * object tBSP430usci5Handle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_B2 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B2
#define configBSP430_HAL_USCI5_B2 0
#endif /* configBSP430_HAL_USCI5_B2 */

/** BSP430 HAL handle for USCI5_B2.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_B2
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B2 - 0)
extern tBSP430usci5Handle const hBSP430usci5_USCI5_B2;
#endif /* configBSP430_HAL_USCI5_B2 */

/** @def configBSP430_HAL_USCI5_B3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_B3 peripheral HAL interface.  This defines a global
 * object tBSP430usci5Handle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI5_B3 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B3
#define configBSP430_HAL_USCI5_B3 0
#endif /* configBSP430_HAL_USCI5_B3 */

/** BSP430 HAL handle for USCI5_B3.
 *
 * The handle may be used only if #configBSP430_HAL_USCI5_B3
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI5_B3 - 0)
extern tBSP430usci5Handle const hBSP430usci5_USCI5_B3;
#endif /* configBSP430_HAL_USCI5_B3 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** @def configBSP430_HPL_USCI5_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI5_A0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI5_A0 defaults this to
 * true, so you only need to explicitly request this if you want the HPL
 * interface without the HAL interface.
 *
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
 * @defaulted
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
 * true, so you only need to explicitly request this if you want the HPL
 * interface without the HAL interface.
 *
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
 * @defaulted
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
 * true, so you only need to explicitly request this if you want the HPL
 * interface without the HAL interface.
 *
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
 * @defaulted
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
 * true, so you only need to explicitly request this if you want the HPL
 * interface without the HAL interface.
 *
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
 * @defaulted
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
 * true, so you only need to explicitly request this if you want the HPL
 * interface without the HAL interface.
 *
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
 * @defaulted
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
 * true, so you only need to explicitly request this if you want the HPL
 * interface without the HAL interface.
 *
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
 * @defaulted
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
 * true, so you only need to explicitly request this if you want the HPL
 * interface without the HAL interface.
 *
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
 * @defaulted
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
 * true, so you only need to explicitly request this if you want the HPL
 * interface without the HAL interface.
 *
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
 * @defaulted
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
 * @defaulted
 * @dependency #configBSP430_HPL_USCI5_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A0 - 0)
#define BSP430_HPL_USCI5_A0 ((volatile sBSP430periphUSCI5 *)BSP430_PERIPH_USCI5_A0)
#endif /* configBSP430_HPL_USCI5_A0 */

/** HPL pointer for USCI5_A1.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_A1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_A1
 * is defined to a true value.
 *
 * @defaulted
 * @dependency #configBSP430_HPL_USCI5_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A1 - 0)
#define BSP430_HPL_USCI5_A1 ((volatile sBSP430periphUSCI5 *)BSP430_PERIPH_USCI5_A1)
#endif /* configBSP430_HPL_USCI5_A1 */

/** HPL pointer for USCI5_A2.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_A2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_A2
 * is defined to a true value.
 *
 * @defaulted
 * @dependency #configBSP430_HPL_USCI5_A2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A2 - 0)
#define BSP430_HPL_USCI5_A2 ((volatile sBSP430periphUSCI5 *)BSP430_PERIPH_USCI5_A2)
#endif /* configBSP430_HPL_USCI5_A2 */

/** HPL pointer for USCI5_A3.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_A3
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_A3
 * is defined to a true value.
 *
 * @defaulted
 * @dependency #configBSP430_HPL_USCI5_A3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_A3 - 0)
#define BSP430_HPL_USCI5_A3 ((volatile sBSP430periphUSCI5 *)BSP430_PERIPH_USCI5_A3)
#endif /* configBSP430_HPL_USCI5_A3 */

/** HPL pointer for USCI5_B0.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_B0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_B0
 * is defined to a true value.
 *
 * @defaulted
 * @dependency #configBSP430_HPL_USCI5_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B0 - 0)
#define BSP430_HPL_USCI5_B0 ((volatile sBSP430periphUSCI5 *)BSP430_PERIPH_USCI5_B0)
#endif /* configBSP430_HPL_USCI5_B0 */

/** HPL pointer for USCI5_B1.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_B1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_B1
 * is defined to a true value.
 *
 * @defaulted
 * @dependency #configBSP430_HPL_USCI5_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B1 - 0)
#define BSP430_HPL_USCI5_B1 ((volatile sBSP430periphUSCI5 *)BSP430_PERIPH_USCI5_B1)
#endif /* configBSP430_HPL_USCI5_B1 */

/** HPL pointer for USCI5_B2.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_B2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_B2
 * is defined to a true value.
 *
 * @defaulted
 * @dependency #configBSP430_HPL_USCI5_B2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B2 - 0)
#define BSP430_HPL_USCI5_B2 ((volatile sBSP430periphUSCI5 *)BSP430_PERIPH_USCI5_B2)
#endif /* configBSP430_HPL_USCI5_B2 */

/** HPL pointer for USCI5_B3.
 *
 * Typed pointer to a volatile structure overlaying the USCI5_B3
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI5_B3
 * is defined to a true value.
 *
 * @defaulted
 * @dependency #configBSP430_HPL_USCI5_B3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI5_B3 - 0)
#define BSP430_HPL_USCI5_B3 ((volatile sBSP430periphUSCI5 *)BSP430_PERIPH_USCI5_B3)
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
 * @defaulted */
#ifndef configBSP430_HAL_USCI5_B3_ISR
#define configBSP430_HAL_USCI5_B3_ISR (configBSP430_HAL_USCI5_B3 - 0)
#endif /* configBSP430_HAL_USCI5_B3_ISR */

#if (configBSP430_HAL_USCI5_B3_ISR - 0) && ! (configBSP430_HAL_USCI5_B3 - 0)
#warning configBSP430_HAL_USCI5_B3_ISR requested without configBSP430_HAL_USCI5_B3
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

/** USCI5-specific implementation of xBSP430serialOpenUART() */
tBSP430usci5Handle xBSP430usci5OpenUART (tBSP430periphHandle xPeriph,
    unsigned int control_word,
    unsigned long baud);

/** USCI5-specific implementation of xBSP430serialOpenSPI() */
tBSP430usci5Handle xBSP430usci5OpenSPI (tBSP430periphHandle xPeriph,
                                        unsigned int control_word,
                                        unsigned int prescaler);

/** USCI5-specific implementation of xBSP430serialConfigureCallbacks() */
int iBSP430usci5ConfigureCallbacks (tBSP430usci5Handle device,
                                    const struct sBSP430periphISRCallbackVoid * rx_callback,
                                    const struct sBSP430periphISRCallbackVoid * tx_callback);

/** USCI5-specific implementation of xBSP430serialClose() */
int iBSP430usci5Close (tBSP430usci5Handle xUSCI);

/** USCI5-specific implementation of xBSP430serialWakeupTransmit_ni() */
void vBSP430usci5WakeupTransmit_ni (tBSP430usci5Handle device);

/** USCI5-specific implementation of xBSP430serialFlush_ni() */
void vBSP430usci5Flush_ni (tBSP430usci5Handle device);

/** USCI5-specific implementation of xBSP430serialTransmitByte_ni() */
int iBSP430usci5TransmitByte_ni (tBSP430usci5Handle device, int c);

/** USCI5-specific implementation of xBSP430serialTransmitData_ni() */
int iBSP430usci5TransmitData_ni (tBSP430usci5Handle device, const uint8_t * data, size_t len);

/** USCI5-specific implementation of xBSP430serialPutASCIIZ_ni() */
int iBSP430usci5TransmitASCIIZ_ni (tBSP430usci5Handle device, const char * str);

#endif /* BSP430_PERIPH_USCI5_H */
