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
 * Peripheral identifiers and hardware interfaces for 5xx/6xx MCUs.
 *
 * This file, or class-specific include files, declares handles to
 * peripherals which in turn are used in application code.  Use of a
 * given peripheral must be indicated by defining the corresponding @c
 * configBSP430_PERIPH_USE_xx in the application @c FreeRTOSConfig.h
 * file.
 *
 * Structures are defined for each class of peripheral to simplify
 * access by converting the periphal address to a pointer to such a
 * structure.  These structures intentionally uses the GCC/ISO C11
 * extensions for unnamed struct/union fields.  Access to the
 * peripheral area through such pointers must be done with care, as
 * some structure fields are not valid for some variants of the
 * peripheral.
 *
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_5XX_PERIPH_H
#define BSP430_5XX_PERIPH_H

#include <bsp430/common/periph.h>

/* Register map for USCI_A/USCI_B peripheral on a MSP430 5xx/6xx MCU. */
typedef struct xBSP430Periph_USCI {
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
} xBSP430Periph_USCI;

/* Register map for eUSCI_A peripheral on a MSP430 5xx/6xx MCU. */
typedef struct xBSP430Periph_eUSCI_A {
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
} xBSP430Periph_eUSCI_A;

/* Register map for eUSCI_B peripheral on a MSP430 5xx/6xx MCU. */
typedef struct xBSP430Periph_eUSCI_B {
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
} xBSP430Periph_eUSCI_B;

#if 0

/* Structure for a single DMA channel */
typedef struct bsp430_DMAX_channel {
	unsigned int ctl;			/* 0x00 */
	unsigned long int __attribute__((__a20__)) sa; /* 0x02 */
	unsigned long int __attribute__((__a20__)) da; /* 0x06 */
	unsigned int sz;							   /* 0x0a */
	unsigned int _reserved_0x0c;
	unsigned int _reserved_0x0e;
} bsp430_DMAX_channel;

/* Structure for the DMAX peripheral.  The number of channels is
 * device-specific. */
typedef struct bsp430_DMAX {
	unsigned int ctl0;
	unsigned int ctl1;
	unsigned int ctl2;
	unsigned int ctl3;
	unsigned int ctl4;
	unsigned int iv;
	bsp430_DMAX_channel channel[1];
} bsp430_DMAX;

#endif

/* BEGIN EMBED 5xx_periph_decl */
/* AUTOMATICALLY GENERATED CODE---DO NOT MODIFY */

/** @def configBSP430_PERIPH_USE_USCI_A0
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_A0 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_A0__) */
#ifndef configBSP430_PERIPH_USE_USCI_A0
#define configBSP430_PERIPH_USE_USCI_A0 0
#endif /* configBSP430_PERIPH_USE_USCI_A0 */

/** Handle for the raw USCI_A0 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_A0 is defined to a true
 * value. */
#define xBSP430Periph_USCI_A0 ((xBSP430Periph)(__MSP430_BASEADDRESS_USCI_A0__))

/** @def configBSP430_PERIPH_USE_USCI_A1
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_A1 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_A1__) */
#ifndef configBSP430_PERIPH_USE_USCI_A1
#define configBSP430_PERIPH_USE_USCI_A1 0
#endif /* configBSP430_PERIPH_USE_USCI_A1 */

/** Handle for the raw USCI_A1 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_A1 is defined to a true
 * value. */
#define xBSP430Periph_USCI_A1 ((xBSP430Periph)(__MSP430_BASEADDRESS_USCI_A1__))

/** @def configBSP430_PERIPH_USE_USCI_A2
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_A2 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_A2__) */
#ifndef configBSP430_PERIPH_USE_USCI_A2
#define configBSP430_PERIPH_USE_USCI_A2 0
#endif /* configBSP430_PERIPH_USE_USCI_A2 */

/** Handle for the raw USCI_A2 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_A2 is defined to a true
 * value. */
#define xBSP430Periph_USCI_A2 ((xBSP430Periph)(__MSP430_BASEADDRESS_USCI_A2__))

/** @def configBSP430_PERIPH_USE_USCI_A3
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_A3 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_A3__) */
#ifndef configBSP430_PERIPH_USE_USCI_A3
#define configBSP430_PERIPH_USE_USCI_A3 0
#endif /* configBSP430_PERIPH_USE_USCI_A3 */

/** Handle for the raw USCI_A3 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_A3 is defined to a true
 * value. */
#define xBSP430Periph_USCI_A3 ((xBSP430Periph)(__MSP430_BASEADDRESS_USCI_A3__))

/** @def configBSP430_PERIPH_USE_USCI_B0
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_B0 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_B0__) */
#ifndef configBSP430_PERIPH_USE_USCI_B0
#define configBSP430_PERIPH_USE_USCI_B0 0
#endif /* configBSP430_PERIPH_USE_USCI_B0 */

/** Handle for the raw USCI_B0 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_B0 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B0 ((xBSP430Periph)(__MSP430_BASEADDRESS_USCI_B0__))

/** @def configBSP430_PERIPH_USE_USCI_B1
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_B1 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_B1__) */
#ifndef configBSP430_PERIPH_USE_USCI_B1
#define configBSP430_PERIPH_USE_USCI_B1 0
#endif /* configBSP430_PERIPH_USE_USCI_B1 */

/** Handle for the raw USCI_B1 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_B1 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B1 ((xBSP430Periph)(__MSP430_BASEADDRESS_USCI_B1__))

/** @def configBSP430_PERIPH_USE_USCI_B2
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_B2 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_B2__) */
#ifndef configBSP430_PERIPH_USE_USCI_B2
#define configBSP430_PERIPH_USE_USCI_B2 0
#endif /* configBSP430_PERIPH_USE_USCI_B2 */

/** Handle for the raw USCI_B2 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_B2 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B2 ((xBSP430Periph)(__MSP430_BASEADDRESS_USCI_B2__))

/** @def configBSP430_PERIPH_USE_USCI_B3
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * USCI_B3 peripheral.  Only do this if the MCU you are using actually
 * has this device (check __MSP430_HAS_USCI_B3__) */
#ifndef configBSP430_PERIPH_USE_USCI_B3
#define configBSP430_PERIPH_USE_USCI_B3 0
#endif /* configBSP430_PERIPH_USE_USCI_B3 */

/** Handle for the raw USCI_B3 device.  The handle may be referenced 
 * only if #configBSP430_PERIPH_USE_USCI_B3 is defined to a true
 * value. */
#define xBSP430Periph_USCI_B3 ((xBSP430Periph)(__MSP430_BASEADDRESS_USCI_B3__))

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
#define xBSP430Periph_EUSCI_A0 ((xBSP430Periph)(__MSP430_BASEADDRESS_EUSCI_A0__))

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
#define xBSP430Periph_EUSCI_A1 ((xBSP430Periph)(__MSP430_BASEADDRESS_EUSCI_A1__))

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
#define xBSP430Periph_EUSCI_A2 ((xBSP430Periph)(__MSP430_BASEADDRESS_EUSCI_A2__))

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
#define xBSP430Periph_EUSCI_B0 ((xBSP430Periph)(__MSP430_BASEADDRESS_EUSCI_B0__))

/* END EMBED 5xx_periph_decl: AUTOMATICALLY GENERATED CODE */

#endif /* BSP430_5XX_PERIPH_H */
