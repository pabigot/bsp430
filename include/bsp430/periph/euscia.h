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
 * @brief Hardware presentation/abstraction for eUSCI_A.
 *
 * Genericized eUCSI_A on 5xx/6xx devices
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/* !BSP430! periph=euscia */
/* !BSP430! instance=EUSCI_A0,EUSCI_A1,EUSCI_A2 */

#ifndef BSP430_PERIPH_EUSCIA_H
#define BSP430_PERIPH_EUSCIA_H

#include <bsp430/periph.h>

#if ! defined(__MSP430_HAS_EUSCI_A0__)
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_EUSCI_A0__ */

/** Register map for eUSCI_A peripheral hardware presentation layer. */
typedef struct xBSP430periphEUSCIA {
  union {						/* 0x00 */
    unsigned int ctlw0;		/**< UCAxCTLW0 */
    struct {
      unsigned char ctl1; /**< UCAxCTL1 */
      unsigned char ctl0; /**< UCAxCTL0 */
    };
  };
  unsigned int ctlw1;			/**< UCAxCTLW1 */ /* 0x02 */
  unsigned int _unused_0x04;
  union {						/* 0x06 */
    unsigned int brw;		/**< UCAxBRW */
    struct {
      unsigned char br0;	/**< UCAxBR0 */
      unsigned char br1;	/**< UCAxBR1 */
    };
  };
  unsigned int mctlw;			/**< UCAxMCTLW (UART) */ /* 0x08 */
  unsigned int statw;			/**< UCAxSTATW */ /* 0x0A */
  unsigned int rxbuf;			/**< UCAxRXBUF */ /* 0x0C */
  unsigned int txbuf;			/**< UCAxTXBUF */ /* 0x0E */
  unsigned int abctl;			/**< UCAxABCTL (UART) */ /* 0x10 */
  union {						/* 0x12 */
    unsigned int irctl;		/**< UCAxIRCTL (UART) */
    struct {
      unsigned char irtctl; /**< UCAxIRTCTL (UART) */ /* 0x12 */
      unsigned char irrctl; /**< UCAxIRRCTL (UART) */ /* 0x13 */
    };
  };
  unsigned int _unused_0x14;
  unsigned int _unused_0x16;
  unsigned int _unused_0x18;
  unsigned int ie;			/**< UCAxIE */ /* 0x1A */
  unsigned int ifg;			/**< UCAxIFG */ /* 0x1C */
  unsigned int iv;			/**< UCAxIV */ /* 0x1E */
} xBSP430periphEUSCIA;

/** @cond DOXYGEN_INTERNAL */
#define _BSP430_PERIPH_EUSCI_A0_BASEADDRESS __MSP430_BASEADDRESS_EUSCI_A0__
#define _BSP430_PERIPH_EUSCI_A1_BASEADDRESS __MSP430_BASEADDRESS_EUSCI_A1__
#define _BSP430_PERIPH_EUSCI_A2_BASEADDRESS __MSP430_BASEADDRESS_EUSCI_A2__
#define _BSP430_PERIPH_EUSCI_A3_BASEADDRESS __MSP430_BASEADDRESS_EUSCI_A3__
#define _BSP430_PERIPH_EUSCI_B0_BASEADDRESS __MSP430_BASEADDRESS_EUSCI_B0__
#define _BSP430_PERIPH_EUSCI_B1_BASEADDRESS __MSP430_BASEADDRESS_EUSCI_B1__
#define _BSP430_PERIPH_EUSCI_B2_BASEADDRESS __MSP430_BASEADDRESS_EUSCI_B2__
#define _BSP430_PERIPH_EUSCI_B3_BASEADDRESS __MSP430_BASEADDRESS_EUSCI_B3__
/** @endcond */ /* DOXYGEN_INTERNAL */

#if configBSP430_RTOS_FREERTOS - 0
#include "FreeRTOS.h"
#include "queue.h"
#endif /* configBSP430_RTOS_FREERTOS */

/* Forward declaration to hardware abstraction layer state for eUSCI_A. */
struct xBSP430eusciaState;

/** The USCI internal state is private to the implementation. */
typedef struct xBSP430eusciaState * xBSP430eusciaHandle;

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** @def configBSP430_HAL_EUSCI_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A0 peripheral HAL interface.  This defines a global
 * object xBSP430eusciaHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_EUSCI_A0 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A0
#define configBSP430_HAL_EUSCI_A0 0
#endif /* configBSP430_HAL_EUSCI_A0 */

/** BSP430 HAL handle for EUSCI_A0.
 *
 * The handle may be used only if #configBSP430_PERIPH_EUSCI_A0
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A0 - 0)
extern xBSP430eusciaHandle const xBSP430euscia_EUSCI_A0;
#endif /* configBSP430_HAL_EUSCI_A0 */

/** @def configBSP430_HAL_EUSCI_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A1 peripheral HAL interface.  This defines a global
 * object xBSP430eusciaHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_EUSCI_A1 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A1
#define configBSP430_HAL_EUSCI_A1 0
#endif /* configBSP430_HAL_EUSCI_A1 */

/** BSP430 HAL handle for EUSCI_A1.
 *
 * The handle may be used only if #configBSP430_PERIPH_EUSCI_A1
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A1 - 0)
extern xBSP430eusciaHandle const xBSP430euscia_EUSCI_A1;
#endif /* configBSP430_HAL_EUSCI_A1 */

/** @def configBSP430_HAL_EUSCI_A2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A2 peripheral HAL interface.  This defines a global
 * object xBSP430eusciaHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_EUSCI_A2 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A2
#define configBSP430_HAL_EUSCI_A2 0
#endif /* configBSP430_HAL_EUSCI_A2 */

/** BSP430 HAL handle for EUSCI_A2.
 *
 * The handle may be used only if #configBSP430_PERIPH_EUSCI_A2
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_EUSCI_A2 - 0)
extern xBSP430eusciaHandle const xBSP430euscia_EUSCI_A2;
#endif /* configBSP430_HAL_EUSCI_A2 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! insert=hpl_ba_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_ba_decl] */
/** @def configBSP430_PERIPH_EUSCI_A0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_EUSCI_A0 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_EUSCI_A0
#define configBSP430_PERIPH_EUSCI_A0 (configBSP430_HAL_EUSCI_A0 - 0)
#endif /* configBSP430_PERIPH_EUSCI_A0 */

#if (configBSP430_HAL_EUSCI_A0 - 0) && ! (configBSP430_PERIPH_EUSCI_A0 - 0)
#warning configBSP430_HAL_EUSCI_A0 requested without configBSP430_PERIPH_EUSCI_A0
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A0 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_EUSCI_A0
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_EUSCI_A0 - 0)
#define BSP430_PERIPH_EUSCI_A0 ((xBSP430periphHandle)(_BSP430_PERIPH_EUSCI_A0_BASEADDRESS))
#endif /* configBSP430_PERIPH_EUSCI_A0 */

/** Pointer to the peripheral register map for EUSCI_A0.
 *
 * The pointer may be used only if #configBSP430_PERIPH_EUSCI_A0
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_EUSCI_A0 - 0)
static volatile xBSP430periphEUSCIA * const xBSP430periph_EUSCI_A0 = (volatile xBSP430periphEUSCIA *)_BSP430_PERIPH_EUSCI_A0_BASEADDRESS;
#endif /* configBSP430_PERIPH_EUSCI_A0 */

/** @def configBSP430_PERIPH_EUSCI_A1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_EUSCI_A1 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_EUSCI_A1
#define configBSP430_PERIPH_EUSCI_A1 (configBSP430_HAL_EUSCI_A1 - 0)
#endif /* configBSP430_PERIPH_EUSCI_A1 */

#if (configBSP430_HAL_EUSCI_A1 - 0) && ! (configBSP430_PERIPH_EUSCI_A1 - 0)
#warning configBSP430_HAL_EUSCI_A1 requested without configBSP430_PERIPH_EUSCI_A1
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A1 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_EUSCI_A1
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_EUSCI_A1 - 0)
#define BSP430_PERIPH_EUSCI_A1 ((xBSP430periphHandle)(_BSP430_PERIPH_EUSCI_A1_BASEADDRESS))
#endif /* configBSP430_PERIPH_EUSCI_A1 */

/** Pointer to the peripheral register map for EUSCI_A1.
 *
 * The pointer may be used only if #configBSP430_PERIPH_EUSCI_A1
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_EUSCI_A1 - 0)
static volatile xBSP430periphEUSCIA * const xBSP430periph_EUSCI_A1 = (volatile xBSP430periphEUSCIA *)_BSP430_PERIPH_EUSCI_A1_BASEADDRESS;
#endif /* configBSP430_PERIPH_EUSCI_A1 */

/** @def configBSP430_PERIPH_EUSCI_A2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c EUSCI_A2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_EUSCI_A2 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_EUSCI_A2
#define configBSP430_PERIPH_EUSCI_A2 (configBSP430_HAL_EUSCI_A2 - 0)
#endif /* configBSP430_PERIPH_EUSCI_A2 */

#if (configBSP430_HAL_EUSCI_A2 - 0) && ! (configBSP430_PERIPH_EUSCI_A2 - 0)
#warning configBSP430_HAL_EUSCI_A2 requested without configBSP430_PERIPH_EUSCI_A2
#endif /* HAL and not HPL */

/** Handle for the raw EUSCI_A2 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_EUSCI_A2
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_EUSCI_A2 - 0)
#define BSP430_PERIPH_EUSCI_A2 ((xBSP430periphHandle)(_BSP430_PERIPH_EUSCI_A2_BASEADDRESS))
#endif /* configBSP430_PERIPH_EUSCI_A2 */

/** Pointer to the peripheral register map for EUSCI_A2.
 *
 * The pointer may be used only if #configBSP430_PERIPH_EUSCI_A2
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_EUSCI_A2 - 0)
static volatile xBSP430periphEUSCIA * const xBSP430periph_EUSCI_A2 = (volatile xBSP430periphEUSCIA *)_BSP430_PERIPH_EUSCI_A2_BASEADDRESS;
#endif /* configBSP430_PERIPH_EUSCI_A2 */

/* END AUTOMATICALLY GENERATED CODE [hpl_ba_decl] */
/* !BSP430! end=hpl_ba_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** @def configBSP430_HAL_EUSCI_A0_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c EUSCI_A0, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_EUSCI_A0 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_EUSCI_A0
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A0_ISR
#define configBSP430_HAL_EUSCI_A0_ISR (configBSP430_HAL_EUSCI_A0 - 0)
#endif /* configBSP430_HAL_EUSCI_A0_ISR */

#if (configBSP430_HAL_EUSCI_A0_ISR - 0) && ! (configBSP430_HAL_EUSCI_A0 - 0)
#warning configBSP430_HAL_EUSCI_A0_ISR requested without configBSP430_HAL_EUSCI_A0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_EUSCI_A1_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c EUSCI_A1, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_EUSCI_A1 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_EUSCI_A1
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A1_ISR
#define configBSP430_HAL_EUSCI_A1_ISR (configBSP430_HAL_EUSCI_A1 - 0)
#endif /* configBSP430_HAL_EUSCI_A1_ISR */

#if (configBSP430_HAL_EUSCI_A1_ISR - 0) && ! (configBSP430_HAL_EUSCI_A1 - 0)
#warning configBSP430_HAL_EUSCI_A1_ISR requested without configBSP430_HAL_EUSCI_A1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_EUSCI_A2_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c EUSCI_A2, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_EUSCI_A2 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_EUSCI_A2
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_EUSCI_A2_ISR
#define configBSP430_HAL_EUSCI_A2_ISR (configBSP430_HAL_EUSCI_A2 - 0)
#endif /* configBSP430_HAL_EUSCI_A2_ISR */

#if (configBSP430_HAL_EUSCI_A2_ISR - 0) && ! (configBSP430_HAL_EUSCI_A2 - 0)
#warning configBSP430_HAL_EUSCI_A2_ISR requested without configBSP430_HAL_EUSCI_A2
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

/** Request and configure a USCI device in UART mode.
 *
 * If queues had been associated with this device using
 * #iBSP430eusciaConfigureQueues(), behavior with respect to
 * interrupts is as if those queues were associated during this call.
 *
 * @param xPeriph The raw device identifier for the USCI device that
 * is being requested. E.g., @c xBSP430Periph_EUSCI_A0.
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
xBSP430eusciaHandle xBSP430eusciaOpenUART (xBSP430periphHandle xPeriph,
    unsigned int control_word,
    unsigned long baud);

xBSP430eusciaHandle xBSP430eusciaOpenSPI (xBSP430periphHandle xPeriph,
    unsigned int control_word,
    unsigned int prescaler);

#if configBSP430_RTOS_FREERTOS - 0
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
int iBSP430eusciaConfigureQueues (xBSP430eusciaHandle xUSCI,
                                  xQueueHandle rx_queue,
                                  xQueueHandle tx_queue);
#endif /* configBSP430_RTOS_FREERTOS */

/** Release a USCI device.
 *
 * This places the device into reset mode and resets the peripheral
 * pins to port function.  It does not release or disassociate any
 * queues that were provided through #iBSP430usciConfigureQueues.
 *
 * @param xUSCI The device to be closed.
 *
 * @return 0 if the close occurred without error. */
int iBSP430eusciaClose (xBSP430eusciaHandle xUSCI);

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
 * @param xUSCI An EUSCI_A device which must have a transmit queue.
 */
void vBSP430eusciaWakeupTransmit (xBSP430eusciaHandle xUSCI);

/** Analog to fputc */
int iBSP430eusciaPutc (int c, xBSP430eusciaHandle xUSCI);

/** Analog to fputs */
int iBSP430eusciaPuts (const char* str, xBSP430eusciaHandle xUSCI);

#endif /* BSP430_PERIPH_EUSCIA_H */

