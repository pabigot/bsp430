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
 * Genericized UCSI_A/USCI_B on 5xx/6xx devices
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_USCI_H
#define BSP430_PERIPH_USCI_H

#include <bsp430/periph.h>
#if ! defined(__MSP430_HAS_USCI_A0__)
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_USCI_A0__ */

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#if defined(__MSP430_HAS_EUSCI_A0__)
#error Wrong header: use <bsp430/periph/eusci_a.h>
#endif /* EUSCI */

/** Register map for USCI_A/USCI_B peripheral on a MSP430 5xx/6xx MCU. */
typedef struct xBSP430Periph_USCI {
	/** @cond Doxygen_Suppress */
	union {						/* 0x00 */
		unsigned int ctlw0;
		struct {
			unsigned char ctl1;
			unsigned char ctl0;
		};
	};
	unsigned int _unused_0x02;
	unsigned int _unused_0x04;
	union {						/* 0x06 */
		unsigned int brw;
		struct {
			unsigned char br0;
			unsigned char br1;
		};
	};
	union {						/* 0x08 */
		struct { /* USCI_A */
			unsigned char mctl;
			unsigned char _reserved_0x09;
		};
		unsigned int _reserved_0x08; /* USCI_B */
	};
	unsigned char stat;			/* 0x0A */
	unsigned char _reserved_0x0B;
	unsigned char rxbuf;		/* 0x0C */
	unsigned char _reserved_0x0D;
	unsigned char txbuf;		/* 0x0E */
	unsigned char _reserved_0x0F;
	union {						/* 0x10 */
		struct {				/* USCI_A */
			unsigned char abctl;
			unsigned char _reserved_0x11;
		};
		unsigned int i2coa;		/* USCI_B */
	};
	union {						/* 0x12 */
		unsigned int irctl;		/* USCI_A */
		struct {
			unsigned char irtctl;
			unsigned char irrctl;
		};
		unsigned int i2csa;		/* USCI_B */
	};
	unsigned int _unused_0x14;
	unsigned int _unused_0x16;
	unsigned int _unused_0x18;
	unsigned int _unused_0x1a;
	union {						/* 0x1C */
		unsigned int ictl;
		struct {
			unsigned char ie;
			unsigned char ifg;
		};
	};
	unsigned int iv;			/* 0x1E */
	/** @endcond */
} xBSP430Periph_USCI;

/** Structure holding USCI-related information */
struct xBSP430USCI {
	/** Flags indicating various things: primarily, whether anybody is
	 * using the device. */
	unsigned int flags;
	
	/** Pointer to the peripheral register structure. */
	volatile xBSP430Periph_USCI * const usci;

	/** Queue used to collect input via interrupt.  If null,
	 * interrupts are not used for reception. */
	xQueueHandle rx_queue;

	/** Queue used to transmit output via interrupt.  If null,
	 * interrupts are not used for transmission. */
	xQueueHandle tx_queue;

	/** Total number of received octets */
	unsigned long num_rx;

	/** Total number of transmitted octets */
	unsigned long num_tx;
};

/* BEGIN EMBED 5xx_periph_decl */
/* AUTOMATICALLY GENERATED CODE---DO NOT MODIFY */

/** @def configBSP430_PERIPH_USCI_A0
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_A0 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_A0__) */
#ifndef configBSP430_PERIPH_USCI_A0
#define configBSP430_PERIPH_USCI_A0 0
#endif /* configBSP430_PERIPH_USCI_A0 */

/** Handle for the raw USCI_A0 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USCI_A0 is defined to a true
 * value. */
#define xBSP430Periph_USCI_A0 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_A0__))

/** @def configBSP430_PERIPH_USCI_A1
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_A1 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_A1__) */
#ifndef configBSP430_PERIPH_USCI_A1
#define configBSP430_PERIPH_USCI_A1 0
#endif /* configBSP430_PERIPH_USCI_A1 */

/** Handle for the raw USCI_A1 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USCI_A1 is defined to a true
 * value. */
#define xBSP430Periph_USCI_A1 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_A1__))

/** @def configBSP430_PERIPH_USCI_A2
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_A2 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_A2__) */
#ifndef configBSP430_PERIPH_USCI_A2
#define configBSP430_PERIPH_USCI_A2 0
#endif /* configBSP430_PERIPH_USCI_A2 */

/** Handle for the raw USCI_A2 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USCI_A2 is defined to a true
 * value. */
#define xBSP430Periph_USCI_A2 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_A2__))

/** @def configBSP430_PERIPH_USCI_A3
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_A3 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_A3__) */
#ifndef configBSP430_PERIPH_USCI_A3
#define configBSP430_PERIPH_USCI_A3 0
#endif /* configBSP430_PERIPH_USCI_A3 */

/** Handle for the raw USCI_A3 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USCI_A3 is defined to a true
 * value. */
#define xBSP430Periph_USCI_A3 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_A3__))

/** @def configBSP430_PERIPH_USCI_B0
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_B0 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_B0__) */
#ifndef configBSP430_PERIPH_USCI_B0
#define configBSP430_PERIPH_USCI_B0 0
#endif /* configBSP430_PERIPH_USCI_B0 */

/** Handle for the raw USCI_B0 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USCI_B0 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B0 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_B0__))

/** @def configBSP430_PERIPH_USCI_B1
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_B1 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_B1__) */
#ifndef configBSP430_PERIPH_USCI_B1
#define configBSP430_PERIPH_USCI_B1 0
#endif /* configBSP430_PERIPH_USCI_B1 */

/** Handle for the raw USCI_B1 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USCI_B1 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B1 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_B1__))

/** @def configBSP430_PERIPH_USCI_B2
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_B2 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_B2__) */
#ifndef configBSP430_PERIPH_USCI_B2
#define configBSP430_PERIPH_USCI_B2 0
#endif /* configBSP430_PERIPH_USCI_B2 */

/** Handle for the raw USCI_B2 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USCI_B2 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B2 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_B2__))

/** @def configBSP430_PERIPH_USCI_B3
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_B3 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_B3__) */
#ifndef configBSP430_PERIPH_USCI_B3
#define configBSP430_PERIPH_USCI_B3 0
#endif /* configBSP430_PERIPH_USCI_B3 */

/** Handle for the raw USCI_B3 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USCI_B3 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B3 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_B3__))

/* END EMBED 5xx_periph_decl: AUTOMATICALLY GENERATED CODE */

/** The USCI internal state is private to the implementation. */
typedef struct xBSP430USCI * xBSP430USCIHandle;

/** Request and configure a USCI device in UART mode.
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
 * @param rx_queue A references to a queue to be used for receiving.
 * A non-null value enables interrupt-driven reception, and data
 * should be read from the queue by the application.
 *
 * @param tx_queue A references to a queue to be used for
 * transmitting.  A non-null value enables interrupt-driven
 * transmission, and the application should add data to the queue for
 * transmission.
 *
 * @return @a xUSCI if the allocation and configuration is successful,
 * and a null handle if something went wrong. */
xBSP430USCIHandle xBSP430USCIOpenUART (xBSP430periphHandle xPeriph,
									   unsigned int control_word,
									   unsigned long baud,
									   xQueueHandle rx_queue,
									   xQueueHandle tx_queue);

xBSP430USCIHandle xBSP430USCIOpenSPI (xBSP430periphHandle xPeriph,
									  unsigned int control_word,
									  unsigned int prescaler,
									  xQueueHandle rx_queue,
									  xQueueHandle tx_queue);

/** Release a USCI device.
 *
 * This places the device into reset mode, reset the peripheral pins
 * to port function, and releases any resources allocated when the
 * device was opened.
 *
 * @param xUSCI The device to be closed.
 *
 * @return 0 if the close occurred without error. */
int iBSP430USCIClose (xBSP430USCIHandle xUSCI);

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
void vBSP430USCIWakeupTransmit (xBSP430USCIHandle xUSCI);

/** Analog to fputc */
int iBSP430USCIputc (int c, xBSP430USCIHandle xUSCI);

/** Analog to fputs */
int iBSP430USCIputs (const char* str, xBSP430USCIHandle xUSCI);

/* BEGIN EMBED usci_decl */
/* AUTOMATICALLY GENERATED CODE---DO NOT MODIFY */

/** Handle for USCI_A0.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USCI_A0 is defined to a true value. */
extern xBSP430USCIHandle const xBSP430USCI_A0;

/** Handle for USCI_A1.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USCI_A1 is defined to a true value. */
extern xBSP430USCIHandle const xBSP430USCI_A1;

/** Handle for USCI_A2.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USCI_A2 is defined to a true value. */
extern xBSP430USCIHandle const xBSP430USCI_A2;

/** Handle for USCI_A3.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USCI_A3 is defined to a true value. */
extern xBSP430USCIHandle const xBSP430USCI_A3;

/** Handle for USCI_B0.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USCI_B0 is defined to a true value. */
extern xBSP430USCIHandle const xBSP430USCI_B0;

/** Handle for USCI_B1.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USCI_B1 is defined to a true value. */
extern xBSP430USCIHandle const xBSP430USCI_B1;

/** Handle for USCI_B2.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USCI_B2 is defined to a true value. */
extern xBSP430USCIHandle const xBSP430USCI_B2;

/** Handle for USCI_B3.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USCI_B3 is defined to a true value. */
extern xBSP430USCIHandle const xBSP430USCI_B3;

/* END EMBED usci_decl: AUTOMATICALLY GENERATED CODE */

#endif /* BSP430_PERIPH_USCI_H */
