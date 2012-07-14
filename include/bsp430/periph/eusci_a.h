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
 * Genericized eUCSI_A on 5xx/6xx devices
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_EUSCI_A_H
#define BSP430_PERIPH_EUSCI_A_H

#include <bsp430/periph.h>

/** Register map for eUSCI_A peripheral. */
typedef struct xBSP430Periph_eUSCI_A {
	/** @cond Doxygen_Suppress */
	union {						/* 0x00 */
		unsigned int ctlw0;
		struct {
			unsigned char ctl1;
			unsigned char ctl0;
		};
	};
	unsigned int ctlw1;			/* 0x02 */
	unsigned int _unused_0x04;
	union {						/* 0x06 */
		unsigned int brw;
		struct {
			unsigned char br0;
			unsigned char br1;
		};
	};
	unsigned int mctlw;			/* 0x08 */
	unsigned int statw;			/* 0x0A */
	unsigned int rxbuf;			/* 0x0C */
	unsigned int txbuf;			/* 0x0E */
	unsigned int abctl;			/* 0x10 */
	union {						/* 0x12 */
		unsigned int irctl;
		struct {
			unsigned char irtctl; /* 0x12 */
			unsigned char irrctl; /* 0x13 */
		};
	};
	unsigned int _unused_0x14;
	unsigned int _unused_0x16;
	unsigned int _unused_0x18;
	unsigned int ie;			/* 0x1A */
	unsigned int ifg;			/* 0x1C */
	unsigned int iv;			/* 0x1E */
	/** @endcond */
} xBSP430Periph_eUSCI_A;

/** Register map for eUSCI_B peripheral. */
typedef struct xBSP430Periph_eUSCI_B {
	/** @cond Doxygen_Suppress */
	union {						/* 0x00 */
		unsigned int ctlw0;
		struct {
			unsigned char ctl1;
			unsigned char ctl0;
		};
	};
	unsigned int ctlw1;			/* 0x02 */
	unsigned int _unused_0x04;
	union {						/* 0x06 */
		unsigned int brw;
		struct {
			unsigned char br0;
			unsigned char br1;
		};
	};
	union {						/* 0x08 */
		unsigned int statw;
		struct {				/* I2C only */
			unsigned char stat;
			unsigned char bcnt;
		};
	};
	unsigned int tbcnt;			/* 0x0A */
	unsigned int rxbuf;			/* 0x0C */
	unsigned int txbuf;			/* 0x0E */
	unsigned int _unused_0x10;
	unsigned int _unused_0x12;
	unsigned int i2coa0;		/* 0x14 */
	unsigned int i2coa1;		/* 0x16 */
	unsigned int i2coa2;		/* 0x18 */
	unsigned int i2coa3;		/* 0x1A */
	unsigned int addrx;			/* 0x1C */
	unsigned int addmask;		/* 0x1E */
	unsigned int i2csa;			/* 0x20 */
	unsigned int _unused_0x22;
	unsigned int _unused_0x24;
	unsigned int _unused_0x26;
	unsigned int _unused_0x28;
	unsigned int ie;			/* 0x2A */
	unsigned int ifg;			/* 0x2C */
	unsigned int iv;			/* 0x2E */
	/** @endcond */
} xBSP430Periph_eUSCI_B;

/* BEGIN EMBED 5xx_periph_decl */
/* AUTOMATICALLY GENERATED CODE---DO NOT MODIFY */

/** Handle for the raw USCI_B3 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_B3 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B3 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_USCI_B3__))

/** @def configBSP430_PERIPH_USE_EUSCI_A0
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * EUSCI_A0 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_EUSCI_A0__) */
#ifndef configBSP430_PERIPH_USE_EUSCI_A0
#define configBSP430_PERIPH_USE_EUSCI_A0 0
#endif /* configBSP430_PERIPH_USE_EUSCI_A0 */

/** Handle for the raw EUSCI_A0 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_EUSCI_A0 is defined to a true
 * value. */
#define xBSP430Periph_EUSCI_A0 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_EUSCI_A0__))

/** @def configBSP430_PERIPH_USE_EUSCI_A1
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * EUSCI_A1 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_EUSCI_A1__) */
#ifndef configBSP430_PERIPH_USE_EUSCI_A1
#define configBSP430_PERIPH_USE_EUSCI_A1 0
#endif /* configBSP430_PERIPH_USE_EUSCI_A1 */

/** Handle for the raw EUSCI_A1 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_EUSCI_A1 is defined to a true
 * value. */
#define xBSP430Periph_EUSCI_A1 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_EUSCI_A1__))

/** @def configBSP430_PERIPH_USE_EUSCI_A2
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * EUSCI_A2 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_EUSCI_A2__) */
#ifndef configBSP430_PERIPH_USE_EUSCI_A2
#define configBSP430_PERIPH_USE_EUSCI_A2 0
#endif /* configBSP430_PERIPH_USE_EUSCI_A2 */

/** Handle for the raw EUSCI_A2 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_EUSCI_A2 is defined to a true
 * value. */
#define xBSP430Periph_EUSCI_A2 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_EUSCI_A2__))

/** @def configBSP430_PERIPH_USE_EUSCI_B0
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * EUSCI_B0 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_EUSCI_B0__) */
#ifndef configBSP430_PERIPH_USE_EUSCI_B0
#define configBSP430_PERIPH_USE_EUSCI_B0 0
#endif /* configBSP430_PERIPH_USE_EUSCI_B0 */

/** Handle for the raw EUSCI_B0 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_EUSCI_B0 is defined to a true
 * value. */
#define xBSP430Periph_EUSCI_B0 ((xBSP430periphHandle)(__MSP430_BASEADDRESS_EUSCI_B0__))

/* END EMBED 5xx_periph_decl: AUTOMATICALLY GENERATED CODE */

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/** Structure holding USCI-related information */
struct xBSP430eUSCIA {
	/** Flags indicating various things: primarily, whether anybody is
	 * using the device. */
	unsigned int flags;
	
	/** Pointer to the peripheral register structure. */
	volatile xBSP430Periph_eUSCI_A * const usci;

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

/** The USCI internal state is private to the implementation. */
typedef struct xBSP430eUSCIA * xBSP430eUSCIAHandle;

/** Request and configure a USCI device in UART mode.
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
xBSP430eUSCIAHandle xBSP430eUSCIAOpenUART (xBSP430periphHandle xPeriph,
										   unsigned int control_word,
										   unsigned long baud,
										   xQueueHandle rx_queue,
										   xQueueHandle tx_queue);

xBSP430eUSCIAHandle xBSP430eUSCIAOpenSPI (xBSP430periphHandle xPeriph,
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
int iBSP430eUSCIAClose (xBSP430eUSCIAHandle xUSCI);

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
void vBSP430eUSCIAWakeupTransmit (xBSP430eUSCIAHandle xUSCI);

/** Analog to fputc */
int iBSP430eUSCIAputc (int c, xBSP430eUSCIAHandle xUSCI);

/** Analog to fputs */
int iBSP430eUSCIAputs (const char* str, xBSP430eUSCIAHandle xUSCI);

/* BEGIN EMBED eusci_a_decl */
/* AUTOMATICALLY GENERATED CODE---DO NOT MODIFY */

/** Handle for EUSCI_A0.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USE_EUSCI_A0 is defined to a true value. */
extern xBSP430eUSCIAHandle const xBSP430EUSCI_A0;

/** Handle for EUSCI_A1.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USE_EUSCI_A1 is defined to a true value. */
extern xBSP430eUSCIAHandle const xBSP430EUSCI_A1;

/** Handle for EUSCI_A2.  The handle may be referenced only if 
 * #configBSP430_PERIPH_USE_EUSCI_A2 is defined to a true value. */
extern xBSP430eUSCIAHandle const xBSP430EUSCI_A2;

/* END EMBED eusci_a_decl: AUTOMATICALLY GENERATED CODE */

#endif /* BSP430_PERIPH_EUSCI_A_H */
