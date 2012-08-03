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
 * @brief Genericized UCSI (USCI_A/USCI_B) for 2xx/4xx devices.
 *
 * If you're interested in the USCI_A or USCI_B from 5xx/6xx devices,
 * you want <bsp430/periph/usci5.h>.
 *
 * If you're interested in the eUSCI_A from FRAM 5xx devices, you want
 * <bsp430/periph/euscia.h>.
 *
 * If you're interested in the eUSCI_B from FRAM 5xx devices, wait.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/* !BSP430! periph=usci */
/* !BSP430! instance=USCI_A0,USCI_A1,USCI_B0,USCI_B1 */

#ifndef BSP430_PERIPH_USCI_H
#define BSP430_PERIPH_USCI_H

#include <bsp430/periph.h>
#if ! defined(__MSP430_HAS_USCI__)
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_USCI__ */

/** Register map for USCI_A/USCI_B peripheral on a MSP430 2xx/4xx MCU. */
typedef struct sBSP430hplUSCI {
  unsigned char ctl0;	/**< UCtxCTL0 */ /* 0x00 */
  unsigned char ctl1;	/**< UCtxCTL1 */ /* 0x01 */
  unsigned char br0;	/**< UCtxBR0 */ /* 0x02 */
  unsigned char br1;	/**< UCtxBR1 */ /* 0x03 */
  unsigned char mctl;	/**< UCAxMCTL (UART) */ /* 0x04 */
  unsigned char stat;	/**< UCtxSTAT */ /* 0x05 */
  unsigned char rxbuf; /**< UCtxRXBUF */ /* 0x06 */
  unsigned char txbuf; /**< UCtxTXBUF */ /* 0x07 */
} sBSP430hplUSCI;

/** @cond DOXYGEN_INTERNAL */
#define BSP430_PERIPH_USCI_A0_BASEADDRESS_ 0x0060
#define BSP430_PERIPH_USCI_A1_BASEADDRESS_ 0x00d0
#define BSP430_PERIPH_USCI_B0_BASEADDRESS_ 0x0068
#define BSP430_PERIPH_USCI_B1_BASEADDRESS_ 0x00d8
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Structure holding hardware abstraction layer state for
 * USCI_A/USCI_B on 2xx/4xx devices.
 *
 * This structure is internal state, for access by applications only
 * when overriding BSP430 HAL capabilities. */
typedef struct sBSP430halUSCI {
  /** Flags indicating various things: primarily, whether anybody is
   * using the device. */
  unsigned int flags;

  /** Pointer to the peripheral register structure. */
  volatile sBSP430hplUSCI * const usci;

  /** Pointer to the interrupt enable register for the peripheral */
  volatile unsigned char * const iep;

  /** Pointer to the interrupt flag register for the peripheral */
  volatile unsigned char * const ifgp;

  /** Bit within *iep and *ifgp used to denote an RX interrupt */
  unsigned char const rx_bit;

  /** Bit within *iep and *ifgp used to denote a TX interrupt */
  unsigned char const tx_bit;

  /** The callback chain to invoke when a byte is received */
  const struct sBSP430halISRCallbackVoid * rx_callback;

  /** The callback chain to invoke when space is available in the
   * transmission buffer */
  const struct sBSP430halISRCallbackVoid * tx_callback;

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
} sBSP430halUSCI;

/** The USCI internal state is private to the implementation. */
typedef struct sBSP430halUSCI * hBSP430halUSCI;

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** @def configBSP430_HAL_USCI_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_A0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI_A0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI_A0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI_A0
#define configBSP430_HAL_USCI_A0 0
#endif /* configBSP430_HAL_USCI_A0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI_A0 - 0
/* You don't need to know about this */
extern sBSP430halUSCI xBSP430hal_USCI_A0_;
#endif /* configBSP430_HAL_USCI_A0 */
/** @endcond */

/** BSP430 HAL handle for USCI_A0.
 *
 * The handle may be used only if #configBSP430_HAL_USCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_A0 - 0)
#define BSP430_HAL_USCI_A0 (&xBSP430hal_USCI_A0_)
#endif /* configBSP430_HAL_USCI_A0 */

/** @def configBSP430_HAL_USCI_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_A1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI_A1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI_A1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI_A1
#define configBSP430_HAL_USCI_A1 0
#endif /* configBSP430_HAL_USCI_A1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI_A1 - 0
/* You don't need to know about this */
extern sBSP430halUSCI xBSP430hal_USCI_A1_;
#endif /* configBSP430_HAL_USCI_A1 */
/** @endcond */

/** BSP430 HAL handle for USCI_A1.
 *
 * The handle may be used only if #configBSP430_HAL_USCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_A1 - 0)
#define BSP430_HAL_USCI_A1 (&xBSP430hal_USCI_A1_)
#endif /* configBSP430_HAL_USCI_A1 */

/** @def configBSP430_HAL_USCI_B0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_B0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI_B0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI_B0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI_B0
#define configBSP430_HAL_USCI_B0 0
#endif /* configBSP430_HAL_USCI_B0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI_B0 - 0
/* You don't need to know about this */
extern sBSP430halUSCI xBSP430hal_USCI_B0_;
#endif /* configBSP430_HAL_USCI_B0 */
/** @endcond */

/** BSP430 HAL handle for USCI_B0.
 *
 * The handle may be used only if #configBSP430_HAL_USCI_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_B0 - 0)
#define BSP430_HAL_USCI_B0 (&xBSP430hal_USCI_B0_)
#endif /* configBSP430_HAL_USCI_B0 */

/** @def configBSP430_HAL_USCI_B1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_B1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_USCI_B1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_USCI_B1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI_B1
#define configBSP430_HAL_USCI_B1 0
#endif /* configBSP430_HAL_USCI_B1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_USCI_B1 - 0
/* You don't need to know about this */
extern sBSP430halUSCI xBSP430hal_USCI_B1_;
#endif /* configBSP430_HAL_USCI_B1 */
/** @endcond */

/** BSP430 HAL handle for USCI_B1.
 *
 * The handle may be used only if #configBSP430_HAL_USCI_B1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_USCI_B1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_B1 - 0)
#define BSP430_HAL_USCI_B1 (&xBSP430hal_USCI_B1_)
#endif /* configBSP430_HAL_USCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** @def configBSP430_HPL_USCI_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_A0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI_A0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
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

/** @def configBSP430_HPL_USCI_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_A1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI_A1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
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

/** @def configBSP430_HPL_USCI_B0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_B0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI_B0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
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

/** @def configBSP430_HPL_USCI_B1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_B1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI_B1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
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
/** HPL pointer for USCI_A0.
 *
 * Typed pointer to a volatile structure overlaying the USCI_A0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI_A0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_A0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_A0 - 0)
#define BSP430_HPL_USCI_A0 ((volatile sBSP430hplUSCI *)BSP430_PERIPH_USCI_A0)
#endif /* configBSP430_HPL_USCI_A0 */

/** HPL pointer for USCI_A1.
 *
 * Typed pointer to a volatile structure overlaying the USCI_A1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI_A1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_A1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_A1 - 0)
#define BSP430_HPL_USCI_A1 ((volatile sBSP430hplUSCI *)BSP430_PERIPH_USCI_A1)
#endif /* configBSP430_HPL_USCI_A1 */

/** HPL pointer for USCI_B0.
 *
 * Typed pointer to a volatile structure overlaying the USCI_B0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI_B0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_USCI_B0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_USCI_B0 - 0)
#define BSP430_HPL_USCI_B0 ((volatile sBSP430hplUSCI *)BSP430_PERIPH_USCI_B0)
#endif /* configBSP430_HPL_USCI_B0 */

/** HPL pointer for USCI_B1.
 *
 * Typed pointer to a volatile structure overlaying the USCI_B1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_USCI_B1
 * is defined to a true value.
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
/** @def configBSP430_HAL_USCIAB0RX_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI_A0 or @c
 * USCI_B0, but want to define your own interrupt service
 * routine for receive operations for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI_A0 or
 * #configBSP430_HAL_USCI_B0 defaults this to true, so you
 * only need to explicitly set it if you do not want to use the
 * standard ISR provided by BSP430.
 *
 * @note Enabling this requires that at least one of
 * #configBSP430_HAL_USCI_A0 and
 * #configBSP430_HAL_USCI_B0 also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCIAB0RX_ISR
#define configBSP430_HAL_USCIAB0RX_ISR ((configBSP430_HAL_USCI_A0 - 0) | (configBSP430_HAL_USCI_B0 - 0))
#endif /* configBSP430_HAL_USCIAB0RX_ISR */

#if ((configBSP430_HAL_0_ISR - 0) \
     && ! ((configBSP430_HAL_USCI_A0RX - 0) | (configBSP430_HAL_USCI_B0 - 0)))
#warning configBSP430_HAL_USCIAB0RX_ISR requested without configBSP430_HAL_USCI_A0 or configBSP430_HAL_USCI_B0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCIAB0TX_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI_A0 or @c
 * USCI_B0, but want to define your own interrupt service
 * routine for transmit operations for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI_A0 or
 * #configBSP430_HAL_USCI_B0 defaults this to true, so you
 * only need to explicitly set it if you do not want to use the
 * standard ISR provided by BSP430.
 *
 * @note Enabling this requires that at least one of
 * #configBSP430_HAL_USCI_A0 and
 * #configBSP430_HAL_USCI_B0 also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCIAB0TX_ISR
#define configBSP430_HAL_USCIAB0TX_ISR ((configBSP430_HAL_USCI_A0 - 0) | (configBSP430_HAL_USCI_B0 - 0))
#endif /* configBSP430_HAL_USCIAB0TX_ISR */

#if ((configBSP430_HAL_0_ISR - 0) \
     && ! ((configBSP430_HAL_USCI_A0TX - 0) | (configBSP430_HAL_USCI_B0 - 0)))
#warning configBSP430_HAL_USCIAB0TX_ISR requested without configBSP430_HAL_USCI_A0 or configBSP430_HAL_USCI_B0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCIAB1RX_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI_A1 or @c
 * USCI_B1, but want to define your own interrupt service
 * routine for receive operations for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI_A1 or
 * #configBSP430_HAL_USCI_B1 defaults this to true, so you
 * only need to explicitly set it if you do not want to use the
 * standard ISR provided by BSP430.
 *
 * @note Enabling this requires that at least one of
 * #configBSP430_HAL_USCI_A1 and
 * #configBSP430_HAL_USCI_B1 also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCIAB1RX_ISR
#define configBSP430_HAL_USCIAB1RX_ISR ((configBSP430_HAL_USCI_A1 - 0) | (configBSP430_HAL_USCI_B1 - 0))
#endif /* configBSP430_HAL_USCIAB1RX_ISR */

#if ((configBSP430_HAL_1_ISR - 0) \
     && ! ((configBSP430_HAL_USCI_A1RX - 0) | (configBSP430_HAL_USCI_B1 - 0)))
#warning configBSP430_HAL_USCIAB1RX_ISR requested without configBSP430_HAL_USCI_A1 or configBSP430_HAL_USCI_B1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_USCIAB1TX_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c USCI_A1 or @c
 * USCI_B1, but want to define your own interrupt service
 * routine for transmit operations for the peripheral.
 *
 * Enabling #configBSP430_HAL_USCI_A1 or
 * #configBSP430_HAL_USCI_B1 defaults this to true, so you
 * only need to explicitly set it if you do not want to use the
 * standard ISR provided by BSP430.
 *
 * @note Enabling this requires that at least one of
 * #configBSP430_HAL_USCI_A1 and
 * #configBSP430_HAL_USCI_B1 also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCIAB1TX_ISR
#define configBSP430_HAL_USCIAB1TX_ISR ((configBSP430_HAL_USCI_A1 - 0) | (configBSP430_HAL_USCI_B1 - 0))
#endif /* configBSP430_HAL_USCIAB1TX_ISR */

#if ((configBSP430_HAL_1_ISR - 0) \
     && ! ((configBSP430_HAL_USCI_A1TX - 0) | (configBSP430_HAL_USCI_B1 - 0)))
#warning configBSP430_HAL_USCIAB1TX_ISR requested without configBSP430_HAL_USCI_A1 or configBSP430_HAL_USCI_B1
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_usci_isr_decl] */
/* !BSP430! end=hal_usci_isr_decl */
/* !BSP430! instance=USCI_A0,USCI_A1,USCI_B0,USCI_B1 */

/** USCI-specific implementation of xBSP430serialOpenUART() */
hBSP430halUSCI xBSP430usciOpenUART (tBSP430periphHandle xPeriph,
                                    unsigned int control_word,
                                    unsigned long baud);

/** USCI-specific implementation of xBSP430serialOpenSPI() */
hBSP430halUSCI xBSP430usciOpenSPI (tBSP430periphHandle xPeriph,
                                   unsigned int control_word,
                                   unsigned int prescaler);

/** USCI-specific implementation of xBSP430serialConfigureCallbacks() */
int iBSP430usciConfigureCallbacks (hBSP430halUSCI device,
                                   const struct sBSP430halISRCallbackVoid * rx_callback,
                                   const struct sBSP430halISRCallbackVoid * tx_callback);

/** USCI-specific implementation of xBSP430serialClose() */
int iBSP430usciClose (hBSP430halUSCI xUSCI);

/** USCI-specific implementation of xBSP430serialWakeupTransmit_ni() */
void vBSP430usciWakeupTransmit_ni (hBSP430halUSCI device);

/** USCI-specific implementation of xBSP430serialFlush_ni() */
void vBSP430usciFlush_ni (hBSP430halUSCI device);

/** USCI-specific implementation of xBSP430serialTransmitByte_ni() */
int iBSP430usciTransmitByte_ni (hBSP430halUSCI device, int c);

/** USCI-specific implementation of xBSP430serialTransmitData_ni() */
int iBSP430usciTransmitData_ni (hBSP430halUSCI device, const uint8_t * data, size_t len);

/** USCI-specific implementation of xBSP430serialPutASCIIZ_ni() */
int iBSP430usciTransmitASCIIZ_ni (hBSP430halUSCI device, const char * str);

#endif /* BSP430_PERIPH_USCI_H */
