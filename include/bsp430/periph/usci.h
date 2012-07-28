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
typedef struct xBSP430periphUSCI {
  unsigned char ctl0;	/**< UCtxCTL0 */ /* 0x00 */
  unsigned char ctl1;	/**< UCtxCTL1 */ /* 0x01 */
  unsigned char br0;	/**< UCtxBR0 */ /* 0x02 */
  unsigned char br1;	/**< UCtxBR1 */ /* 0x03 */
  unsigned char mctl;	/**< UCAxMCTL (UART) */ /* 0x04 */
  unsigned char stat;	/**< UCtxSTAT */ /* 0x05 */
  unsigned char rxbuf; /**< UCtxRXBUF */ /* 0x06 */
  unsigned char txbuf; /**< UCtxTXBUF */ /* 0x07 */
} xBSP430periphUSCI;

/** @cond DOXYGEN_INTERNAL */
#define _BSP430_PERIPH_USCI_A0_BASEADDRESS 0x0060
#define _BSP430_PERIPH_USCI_A1_BASEADDRESS 0x00d0
#define _BSP430_PERIPH_USCI_B0_BASEADDRESS 0x0068
#define _BSP430_PERIPH_USCI_B1_BASEADDRESS 0x00d8
/** @endcond */ /* DOXYGEN_INTERNAL */

#include "FreeRTOS.h"
#include "queue.h"

/* Forward declaration to hardware abstraction layer state for USCI_A/USCI_B. */
struct xBSP430usciState;

/** The USCI internal state is private to the implementation. */
typedef struct xBSP430usciState * xBSP430usciHandle;

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** @def configBSP430_HAL_USCI_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_A0 peripheral HAL interface.  This defines a global
 * object xBSP430usciHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_USCI_A0 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI_A0
#define configBSP430_HAL_USCI_A0 0
#endif /* configBSP430_HAL_USCI_A0 */

/** BSP430 HAL handle for USCI_A0.
 *
 * The handle may be used only if #configBSP430_PERIPH_USCI_A0
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_A0 - 0)
extern xBSP430usciHandle const xBSP430usci_USCI_A0;
#endif /* configBSP430_HAL_USCI_A0 */

/** @def configBSP430_HAL_USCI_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_A1 peripheral HAL interface.  This defines a global
 * object xBSP430usciHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_USCI_A1 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI_A1
#define configBSP430_HAL_USCI_A1 0
#endif /* configBSP430_HAL_USCI_A1 */

/** BSP430 HAL handle for USCI_A1.
 *
 * The handle may be used only if #configBSP430_PERIPH_USCI_A1
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_A1 - 0)
extern xBSP430usciHandle const xBSP430usci_USCI_A1;
#endif /* configBSP430_HAL_USCI_A1 */

/** @def configBSP430_HAL_USCI_B0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_B0 peripheral HAL interface.  This defines a global
 * object xBSP430usciHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_USCI_B0 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI_B0
#define configBSP430_HAL_USCI_B0 0
#endif /* configBSP430_HAL_USCI_B0 */

/** BSP430 HAL handle for USCI_B0.
 *
 * The handle may be used only if #configBSP430_PERIPH_USCI_B0
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_B0 - 0)
extern xBSP430usciHandle const xBSP430usci_USCI_B0;
#endif /* configBSP430_HAL_USCI_B0 */

/** @def configBSP430_HAL_USCI_B1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_B1 peripheral HAL interface.  This defines a global
 * object xBSP430usciHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_USCI_B1 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_USCI_B1
#define configBSP430_HAL_USCI_B1 0
#endif /* configBSP430_HAL_USCI_B1 */

/** BSP430 HAL handle for USCI_B1.
 *
 * The handle may be used only if #configBSP430_PERIPH_USCI_B1
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_USCI_B1 - 0)
extern xBSP430usciHandle const xBSP430usci_USCI_B1;
#endif /* configBSP430_HAL_USCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! insert=hpl_ba_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_ba_decl] */
/** @def configBSP430_PERIPH_USCI_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_A0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI_A0 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_USCI_A0
#define configBSP430_PERIPH_USCI_A0 (configBSP430_HAL_USCI_A0 - 0)
#endif /* configBSP430_PERIPH_USCI_A0 */

#if (configBSP430_HAL_USCI_A0 - 0) && ! (configBSP430_PERIPH_USCI_A0 - 0)
#warning configBSP430_HAL_USCI_A0 requested without configBSP430_PERIPH_USCI_A0
#endif /* HAL and not HPL */

/** Handle for the raw USCI_A0 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_USCI_A0
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_USCI_A0 - 0)
#define BSP430_PERIPH_USCI_A0 ((xBSP430periphHandle)(_BSP430_PERIPH_USCI_A0_BASEADDRESS))
#endif /* configBSP430_PERIPH_USCI_A0 */

/** Pointer to the peripheral register map for USCI_A0.
 *
 * The pointer may be used only if #configBSP430_PERIPH_USCI_A0
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_USCI_A0 - 0)
static volatile xBSP430periphUSCI * const xBSP430periph_USCI_A0 = (volatile xBSP430periphUSCI *)_BSP430_PERIPH_USCI_A0_BASEADDRESS;
#endif /* configBSP430_PERIPH_USCI_A0 */

/** @def configBSP430_PERIPH_USCI_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_A1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI_A1 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_USCI_A1
#define configBSP430_PERIPH_USCI_A1 (configBSP430_HAL_USCI_A1 - 0)
#endif /* configBSP430_PERIPH_USCI_A1 */

#if (configBSP430_HAL_USCI_A1 - 0) && ! (configBSP430_PERIPH_USCI_A1 - 0)
#warning configBSP430_HAL_USCI_A1 requested without configBSP430_PERIPH_USCI_A1
#endif /* HAL and not HPL */

/** Handle for the raw USCI_A1 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_USCI_A1
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_USCI_A1 - 0)
#define BSP430_PERIPH_USCI_A1 ((xBSP430periphHandle)(_BSP430_PERIPH_USCI_A1_BASEADDRESS))
#endif /* configBSP430_PERIPH_USCI_A1 */

/** Pointer to the peripheral register map for USCI_A1.
 *
 * The pointer may be used only if #configBSP430_PERIPH_USCI_A1
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_USCI_A1 - 0)
static volatile xBSP430periphUSCI * const xBSP430periph_USCI_A1 = (volatile xBSP430periphUSCI *)_BSP430_PERIPH_USCI_A1_BASEADDRESS;
#endif /* configBSP430_PERIPH_USCI_A1 */

/** @def configBSP430_PERIPH_USCI_B0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_B0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI_B0 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_USCI_B0
#define configBSP430_PERIPH_USCI_B0 (configBSP430_HAL_USCI_B0 - 0)
#endif /* configBSP430_PERIPH_USCI_B0 */

#if (configBSP430_HAL_USCI_B0 - 0) && ! (configBSP430_PERIPH_USCI_B0 - 0)
#warning configBSP430_HAL_USCI_B0 requested without configBSP430_PERIPH_USCI_B0
#endif /* HAL and not HPL */

/** Handle for the raw USCI_B0 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_USCI_B0
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_USCI_B0 - 0)
#define BSP430_PERIPH_USCI_B0 ((xBSP430periphHandle)(_BSP430_PERIPH_USCI_B0_BASEADDRESS))
#endif /* configBSP430_PERIPH_USCI_B0 */

/** Pointer to the peripheral register map for USCI_B0.
 *
 * The pointer may be used only if #configBSP430_PERIPH_USCI_B0
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_USCI_B0 - 0)
static volatile xBSP430periphUSCI * const xBSP430periph_USCI_B0 = (volatile xBSP430periphUSCI *)_BSP430_PERIPH_USCI_B0_BASEADDRESS;
#endif /* configBSP430_PERIPH_USCI_B0 */

/** @def configBSP430_PERIPH_USCI_B1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c USCI_B1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_USCI_B1 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_USCI_B1
#define configBSP430_PERIPH_USCI_B1 (configBSP430_HAL_USCI_B1 - 0)
#endif /* configBSP430_PERIPH_USCI_B1 */

#if (configBSP430_HAL_USCI_B1 - 0) && ! (configBSP430_PERIPH_USCI_B1 - 0)
#warning configBSP430_HAL_USCI_B1 requested without configBSP430_PERIPH_USCI_B1
#endif /* HAL and not HPL */

/** Handle for the raw USCI_B1 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_USCI_B1
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_USCI_B1 - 0)
#define BSP430_PERIPH_USCI_B1 ((xBSP430periphHandle)(_BSP430_PERIPH_USCI_B1_BASEADDRESS))
#endif /* configBSP430_PERIPH_USCI_B1 */

/** Pointer to the peripheral register map for USCI_B1.
 *
 * The pointer may be used only if #configBSP430_PERIPH_USCI_B1
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_USCI_B1 - 0)
static volatile xBSP430periphUSCI * const xBSP430periph_USCI_B1 = (volatile xBSP430periphUSCI *)_BSP430_PERIPH_USCI_B1_BASEADDRESS;
#endif /* configBSP430_PERIPH_USCI_B1 */

/* END AUTOMATICALLY GENERATED CODE [hpl_ba_decl] */
/* !BSP430! end=hpl_ba_decl */

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

/** Request and configure a USCI device in UART mode.
 *
 * If queues had been associated with this device using
 * #iBSP430usciConfigureQueues(), behavior with respect to interrupts
 * is as if those queues were associated during this call.
 *
 * @param xPeriph The raw device identifier for the USCI device that
 * is being requested. E.g., @c xBSP430Periph_USCI_A0.
 *
 * @param control_word The configuration to be written to the device's
 * ctlw0 word.  Most bit fields will be assigned from this value, but
 * UCSYNC will be cleared, and UCSSELx will be set based on baud rate.
 *
 * @param baud The desired baud rate.  This will be configured
 * based on the current clock setting, using ACLK if the rate is low
 * enough and SMCLK otherwise.
 *
 * @return @a xUSCI if the allocation and configuration is successful,
 * and a null handle if something went wrong. */
xBSP430usciHandle xBSP430usciOpenUART (xBSP430periphHandle xPeriph,
                                       unsigned int control_word,
                                       unsigned long baud);

xBSP430usciHandle xBSP430usciOpenSPI (xBSP430periphHandle xPeriph,
                                      unsigned int control_word,
                                      unsigned int prescaler);

/** Assign FreeRTOS queues for transmit and receive.
 *
 * The underlying device is held in reset mode while the queue
 * configuration is changed within a critical section (interrupts
 * disabled).  If a receive queue is provided, the UCRXIE bit is set
 * to enable interrupt-driven reception.  Interrupt-driven
 * transmission is managed using #vBSP430usciWakeupTransmit().
 *
 * Note that #iBSP430usciClose() does not disassociate the queues from
 * the device.  This must be done manually either before or after
 * closing the device, by invoking this function with null handles for
 * the queues.
 *
 * @param xUSCI A USCI device.  If (non-null) queues are already
 * associated with this device, an error will be returned and the
 * previous configuration left unchanged.
 *
 * @param rx_queue A references to a queue to be used for receiving.
 * A non-null value enables interrupt-driven reception, and data
 * should be read from the queue by the application.
 *
 * @param tx_queue A references to a queue to be used for
 * transmitting.  A non-null value enables interrupt-driven
 * transmission, and the application should add data to the queue for
 * transmission.
 *
 * @return Zero if the configuration was successful, a negative value
 * if something went wrong.
 */
int iBSP430usciConfigureQueues (xBSP430usciHandle xUSCI,
                                xQueueHandle rx_queue,
                                xQueueHandle tx_queue);

/** Release a USCI device.
 *
 * This places the device into reset mode and resets the peripheral
 * pins to port function.  It does not release or disassociate any
 * queues that were provided through #iBSP430usciConfigureQueues.
 *
 * @param xUSCI The device to be closed.
 *
 * @return 0 if the close occurred without error. */
int iBSP430usciClose (xBSP430usciHandle xUSCI);

/** Wake up the interrupt-driven transmission if necessary.
 *
 * Normally the transmission infrastructure transmits data that is
 * added to the queue.  However, the infrastructure is disabled when
 * the transmit queue is emptied.  When this has happened, it must be
 * told that more data has been added and the infrastructure
 * re-enabled.
 *
 * For efficiency, this should only be called if it is believed that
 * data is present in the transmit queue but that the transmission
 * infrastructure may be idle.
 *
 * @param xUSCI A USCI device which must have a transmit queue.
 */
void vBSP430usciWakeupTransmit (xBSP430usciHandle xUSCI);

/** Analog to fputc */
int iBSP430usciPutChar (int c, xBSP430usciHandle xUSCI);

/** Analog to fputs */
int iBSP430usciPutString (const char* str, xBSP430usciHandle xUSCI);

#endif /* BSP430_PERIPH_USCI_H */
