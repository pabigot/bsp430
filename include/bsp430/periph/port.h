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
 * @brief Genericized digital I/O port (PORTX/PORTX_R) for MSP430 MCUs
 *
 * A hardware presentation layer is defined that provides a structure
 * definition associated with a port-specific address, to allow
 * manipulation of standard port registers like @c PxDIR and @c PxSEL
 * through a single pointer.  This would be useful when an external
 * component such as a DS18B20 may be placed on one of several ports
 * depending on platform.  Presentation layer structures are defined
 * for each 8-bit port, and for the 16-bit port interfaces on MCUs
 * that support them.
 *
 * A hardware abstraction layer is defined that allows registering
 * callbacks to be invoked when port interrupts occur.  This is
 * necessary when a library provides support for an external component
 * that signals events through an interrupt, but is not permitted to
 * define the interrupt handler for the entire port.  The abstraction
 * layer is supported only for the 8-bit port interface.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

/* !BSP430! periph=port */

#ifndef BSP430_PERIPH_PORT_H
#define BSP430_PERIPH_PORT_H

#include <bsp430/periph.h>
#if ! (defined(__MSP430_HAS_PORT1__) || defined(__MSP430_HAS_PORT1_R__))
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_PORT1__ */

/* Analysis of port capabilities:
 *
 * 1xx: P1/P2 are uniform contiguous with interrupt capability.  P3-P6
 * are uniform contiguous without interrupt capability.  There is no
 * REN capability.
 *
 * 2xx: P1/P2 are uniform contiguous inclusive of REN but exclusive of
 * SEL2, which is contiguous at a distinct base address, and have
 * interrupt capability.  P3-P6 are uniform 8-bit and P7-P8 are
 * uniform 16-bit contiguous, exclusive of REN and SEL2.  PA=(P7,P8)
 * in the headers, but is not named as such in the user's guide.
 * 
 * 3xx: P1/P2 are uniform contiguous with interrupt capability.  P3-P4
 * are uniform contiguous without interrupt capability.  There is no
 * REN capability.
 * 
 * 4xx: P1/P2 are uniform contiguous including REN capability, and
 * have interrupt capability.  P3-P6 are uniform 8-bit and P7-P10 are
 * uniform 16-bit contiguous; these are exclusive of REN, which is
 * contiguous at a distinct base address.  PA=(P7,P8), PB=(P9,P10).
 *
 * 5xx: All ports uniform 16-bit contiguous.  PA=(P1,P2), PB=(P3,P4),
 * and so forth.  All registers have space reserved for interrupt
 * support.
 *
 * In the above, "8-bit" means that the register fields are
 * immediately adjacent with byte addresses.  "16-bit" means that
 * register pairs are interleaved within function; e.g. P1IN would be
 * at 0x00 while P2IN is at 0x01 and P1OUT at 0x02.
 *
 * 16-bit ports were introduced in MSP430X and are available on
 * several chips in the 2xx and 4xx family.  In 2xx/4xx MCUs, PORTA
 * overlays P7/P8; in 5xx MCUs it overlays P1/P2.
 *
 * In pre-5xx families, registers supporting interrupts have a
 * different layout than registers that do not support interrupts.
 * However, the layouts (exclusive of SEL2 and REN) are compatible
 * within those types.
 *
 * Thus there are several distinct layouts that need to be supported:
 *
 * + [PORT_IE_8] Pre-5xx interrupt-capable, 8-bit interface
 * + [PORT_8] Pre-5xx non-interrupt, 8-bit interface
 * + [PORT_16] Pre-5xx non-interrupt, 16-bit interface
 * + [PORT_5XX] 5xx registers, 8-bit interface
 * + [PORT_5XX_16] 5xx registers, 16-bit interface
 *
 * 5xx chips are identified by MSP430XV2_CPU.  For these, a uniform
 * HPL based on the 16-bit alphabetic register names is presented.
 */

/** Layout for pre-5xx--family ports supporting interrupts.
 *
 * Access to SEL2 capability for these ports is not available in the
 * hardware-presentation layer. */
typedef struct xBSP430periphPORT_IE_8 {
	unsigned char in;			/* 0x00 */
	unsigned char out;			/* 0x01 */
	unsigned char dir;			/* 0x02 */
	unsigned char ifg;			/* 0x03 */
	unsigned char ies;			/* 0x04 */
	unsigned char ie;			/* 0x05 */
	unsigned char sel;			/* 0x06 */
	unsigned char ren;			/* 0x07 */
} xBSP430periphPORT_IE_8;

/** Layout for pre-5xx--family 8-bit ports not supporting interrupts.
 *
 * Access to SEL2 and REN capability for these ports is not available
 * in the hardware-presentation layer. */
typedef struct xBSP430periphPORT_8 {
	unsigned char in;			/* 0x00 */
	unsigned char out;			/* 0x01 */
	unsigned char dir;			/* 0x02 */
	unsigned char sel;			/* 0x03 */
} xBSP430periphPORT_8;

/** Helper for accessing 8-bit registers within 16-bit registers */
typedef union xBSP430periphPORT_16_u {
	unsigned int w;				/**< Word access (e.g., PAIN) */
	unsigned char b[2];			/**< Byte access, indexed */
	struct {
		unsigned char l;		/**< Low byte access for odd members of pair (e.g., P1IN) */
		unsigned char h;		/**< High byte access for even members of pair (e.g., P2IN) */
	};
} xBSP430periphPORT_16_u;

/** Layout for 2xx/4xx family 16-bit ports not supporting interrupts.
 *
 * Access to SEL2 and REN capability for these ports is not available
 * in the hardware-presentation layer. */
typedef struct xBSP430periphPORT_16 {
	xBSP430periphPORT_16_u in;	/* 0x00 */
	xBSP430periphPORT_16_u out;	/* 0x02 */
	xBSP430periphPORT_16_u dir;	/* 0x04 */
	xBSP430periphPORT_16_u sel;	/* 0x08 */
} xBSP430periphPORT_16;

/** Layout for 5xx family ports, 16-bit access
 */
typedef struct xBSP430periphPORT_5XX_16 {
	xBSP430periphPORT_16_u in;	/* 0x00 */
	xBSP430periphPORT_16_u out;	/* 0x02 */
	xBSP430periphPORT_16_u dir;	/* 0x04 */
	xBSP430periphPORT_16_u ren;	/* 0x06 */
	unsigned int _reserved_x08;
	xBSP430periphPORT_16_u sel0; /* 0x0A */
	xBSP430periphPORT_16_u sel1; /* 0x0C */
	unsigned int _reserved_x0E;
	xBSP430periphPORT_16_u selc; /* 0x10 */
	unsigned int _reserved_x12;
	unsigned int _reserved_x14;
	unsigned int _reserved_x16;
	xBSP430periphPORT_16_u ies;	 /* 0x18 */
	xBSP430periphPORT_16_u ie;	 /* 0x1A */
	xBSP430periphPORT_16_u ifg;	 /* 0x1C */
} xBSP430periphPORT_5XX_16;

/** Layout for 5xx family ports, 8-bit access
 *
 * This structure includes the necessary padding to maintain alignment
 * when referencing the second 8-bit register in a 16-bit bank.
 */
typedef struct xBSP430periphPORT_5XX_8 {
	unsigned char in;	/* 0x00 */
	unsigned char _reserved_x01;
	unsigned char out;	/* 0x02 */
	unsigned char _reserved_x03;
	unsigned char dir;	/* 0x04 */
	unsigned char _reserved_x05;
	unsigned char ren;	/* 0x06 */
	unsigned char _reserved_x07;
	unsigned char _reserved_x08;
	unsigned char _reserved_x09;
	unsigned char sel0; /* 0x0A */
	unsigned char _reserved_x0B;
	unsigned char sel1; /* 0x0C */
	unsigned char _reserved_x0D;
	unsigned char _reserved_x0E;
	unsigned char _reserved_x0F;
	unsigned char selc; /* 0x10 */
	unsigned char _reserved_x11;
	unsigned char _reserved_x12;
	unsigned char _reserved_x13;
	unsigned char _reserved_x14;
	unsigned char _reserved_x15;
	unsigned char _reserved_x16;
	unsigned char _reserved_x17;
	unsigned char ies;	 /* 0x18 */
	unsigned char _reserved_x19;
	unsigned char ie;	 /* 0x1A */
	unsigned char _reserved_x1B;
	unsigned char ifg;	 /* 0x1C */
} xBSP430periphPORT_5XX_8;

/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define _BSP430_PERIPH_PORTIE xBSP430periphPORT_5XX_8
#define _BSP430_PERIPH_PORT xBSP430periphPORT_5XX_8
#define _BSP430_PERIPH_PORTW xBSP430periphPORT_5XX_16
#else /* MSP430XV2 */
#define _BSP430_PERIPH_PORTIE xBSP430periphPORT_IE_8
#define _BSP430_PERIPH_PORT xBSP430periphPORT_8
#define _BSP430_PERIPH_PORTW xBSP430periphPORT_16
#endif /* MSP430XV2 */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Structure used to access ports with interrupt capability.
 *
 * The underlying structure selected depends on the MCU family. */
typedef _BSP430_PERIPH_PORTIE xBSP430periphPORTIE;

/** Structure used to access ports without interrupt capability.
 *
 * The underlying structure selected depends on the MCU family. */
typedef _BSP430_PERIPH_PORT xBSP430periphPORT;

/** Structure used to access 16-bit ports.
 *
 * The underlying structure selected depends on the MCU family. */
typedef _BSP430_PERIPH_PORTW xBSP430periphPORTW;

/** Callback flag indicating ISR should yield.
 *
 * Since the #xBSP430portInterruptCallback does not execute in the
 * stack frame of the ISR top half, this bit is to be added to the
 * return value of a callback if the callback has detected that the
 * interrupt should yield before returning. */
#define BSP430_PORT_ISR_YIELD 0x1000

#if 1 // defined(__MSP430_HAS_MSP430XV2_CPU__)
/* 5xx-family port management */

/* Forward declaration to hardware abstraction layer for 5xx-family ports */
struct xBSP430port5xxState;

/** The PORT internal state is protected. */
typedef struct xBSP430port5xxState * xBSP430portHandle;

/** Prototype for callbacks from shared port interrupt service routines.
 *
 * @note Because these callbacks do not execute in the stack frame of
 * the ISR itself, you cannot use the standard @c
 * __bic_status_register_on_exit() intrinsic to affect the status
 * register flags upon return.  Instead, you must provide the bits
 * that you want cleared as the return value of the callback.
 *
 * @param port A reference to the HAL port structure associated with the port.
 *
 * @param bit The bit for which the interrupt was received; values
 * range from 0 through 7.
 *
 * @return Bits to clear in the status register before returning from
 * the ISR.  For example, if the routine wants to leave low-power mode
 * without affecting the interrupt enable bit, return @c LPM4_bits.
 * Other bits are also relevant, see #BSP430_PORT_ISR_YIELD. */
typedef int (* xBSP430portInterruptCallback) (xBSP430portHandle port, int bit);

struct xBSP430port5xxState {
	unsigned int flags;
	volatile xBSP430periphPORT_5XX_8 * const port;
	xBSP430portInterruptCallback isr[8];
};

/** @def configBSP430_PORT_SHARE_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * shared PORT peripheral HAL interrupt vector.  If this is defined, a
 * single ISR implementation is used, with the individual ISRs
 * invoking it with the instance state and interrupt cause as
 * parameters.
 *
 * This must be available in order to use the HAL interrupt vector for
 * specific device instances such as
 * #configBSP430_PERIPH_PORT1_ISR. */
#ifndef configBSP430_PORT_SHARE_ISR
#define configBSP430_PORT_SHARE_ISR 1
#endif /* configBSP430_PORT_SHARE_ISR */

/* !BSP430! portexpand=True instance=A,B,C,D,E,F insert=hpl_port_5xx */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_port_5xx] */

/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_PORTA_R__)
#define _BSP430_PERIPH_PORT1_BASEADDRESS __MSP430_BASEADDRESS_PORTA_R__
#define _BSP430_PERIPH_PORT2_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTA_R__)
#else /* PORTA_R */
#define _BSP430_PERIPH_PORT1_BASEADDRESS __MSP430_BASEADDRESS_PORTA__
#define _BSP430_PERIPH_PORT2_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTA__)
#endif /* PORTA_R */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Handle for the raw PORT1 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT1
 * is defined to a true value. */
#define BSP430_PERIPH_PORT1 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT1_BASEADDRESS))

/** Handle for the raw PORT2 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT2
 * is defined to a true value. */
#define BSP430_PERIPH_PORT2 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT2_BASEADDRESS))

/** Handle for the raw PORTA device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORTA
 * is defined to a true value. */
#define BSP430_PERIPH_PORTA ((xBSP430periphHandle)(1 + _BSP430_PERIPH_PORT1_BASEADDRESS))

/** @def configBSP430_PERIPH_PORT1
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT1 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT1__ or
 * @c __MSP430_HAS_PORT1_R__) */
#ifndef configBSP430_PERIPH_PORT1
#define configBSP430_PERIPH_PORT1 0
#endif /* configBSP430_PERIPH_PORT1 */

/** @def configBSP430_PERIPH_PORT1_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT1.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT1_ISR
#define configBSP430_PERIPH_PORT1_ISR 0
#endif /* configBSP430_PERIPH_PORT1_ISR */

/** @def configBSP430_PERIPH_PORT2
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT2 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT2__ or
 * @c __MSP430_HAS_PORT2_R__) */
#ifndef configBSP430_PERIPH_PORT2
#define configBSP430_PERIPH_PORT2 0
#endif /* configBSP430_PERIPH_PORT2 */

/** @def configBSP430_PERIPH_PORT2_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT2.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT2_ISR
#define configBSP430_PERIPH_PORT2_ISR 0
#endif /* configBSP430_PERIPH_PORT2_ISR */


/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_PORTB_R__)
#define _BSP430_PERIPH_PORT3_BASEADDRESS __MSP430_BASEADDRESS_PORTB_R__
#define _BSP430_PERIPH_PORT4_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTB_R__)
#else /* PORTB_R */
#define _BSP430_PERIPH_PORT3_BASEADDRESS __MSP430_BASEADDRESS_PORTB__
#define _BSP430_PERIPH_PORT4_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTB__)
#endif /* PORTB_R */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Handle for the raw PORT3 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT3
 * is defined to a true value. */
#define BSP430_PERIPH_PORT3 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT3_BASEADDRESS))

/** Handle for the raw PORT4 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT4
 * is defined to a true value. */
#define BSP430_PERIPH_PORT4 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT4_BASEADDRESS))

/** Handle for the raw PORTB device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORTB
 * is defined to a true value. */
#define BSP430_PERIPH_PORTB ((xBSP430periphHandle)(1 + _BSP430_PERIPH_PORT3_BASEADDRESS))

/** @def configBSP430_PERIPH_PORT3
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT3 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT3__ or
 * @c __MSP430_HAS_PORT3_R__) */
#ifndef configBSP430_PERIPH_PORT3
#define configBSP430_PERIPH_PORT3 0
#endif /* configBSP430_PERIPH_PORT3 */

/** @def configBSP430_PERIPH_PORT3_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT3.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT3_ISR
#define configBSP430_PERIPH_PORT3_ISR 0
#endif /* configBSP430_PERIPH_PORT3_ISR */

/** @def configBSP430_PERIPH_PORT4
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT4 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT4__ or
 * @c __MSP430_HAS_PORT4_R__) */
#ifndef configBSP430_PERIPH_PORT4
#define configBSP430_PERIPH_PORT4 0
#endif /* configBSP430_PERIPH_PORT4 */

/** @def configBSP430_PERIPH_PORT4_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT4.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT4_ISR
#define configBSP430_PERIPH_PORT4_ISR 0
#endif /* configBSP430_PERIPH_PORT4_ISR */


/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_PORTC_R__)
#define _BSP430_PERIPH_PORT5_BASEADDRESS __MSP430_BASEADDRESS_PORTC_R__
#define _BSP430_PERIPH_PORT6_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTC_R__)
#else /* PORTC_R */
#define _BSP430_PERIPH_PORT5_BASEADDRESS __MSP430_BASEADDRESS_PORTC__
#define _BSP430_PERIPH_PORT6_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTC__)
#endif /* PORTC_R */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Handle for the raw PORT5 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT5
 * is defined to a true value. */
#define BSP430_PERIPH_PORT5 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT5_BASEADDRESS))

/** Handle for the raw PORT6 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT6
 * is defined to a true value. */
#define BSP430_PERIPH_PORT6 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT6_BASEADDRESS))

/** Handle for the raw PORTC device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORTC
 * is defined to a true value. */
#define BSP430_PERIPH_PORTC ((xBSP430periphHandle)(1 + _BSP430_PERIPH_PORT5_BASEADDRESS))

/** @def configBSP430_PERIPH_PORT5
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT5 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT5__ or
 * @c __MSP430_HAS_PORT5_R__) */
#ifndef configBSP430_PERIPH_PORT5
#define configBSP430_PERIPH_PORT5 0
#endif /* configBSP430_PERIPH_PORT5 */

/** @def configBSP430_PERIPH_PORT5_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT5.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT5_ISR
#define configBSP430_PERIPH_PORT5_ISR 0
#endif /* configBSP430_PERIPH_PORT5_ISR */

/** @def configBSP430_PERIPH_PORT6
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT6 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT6__ or
 * @c __MSP430_HAS_PORT6_R__) */
#ifndef configBSP430_PERIPH_PORT6
#define configBSP430_PERIPH_PORT6 0
#endif /* configBSP430_PERIPH_PORT6 */

/** @def configBSP430_PERIPH_PORT6_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT6.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT6_ISR
#define configBSP430_PERIPH_PORT6_ISR 0
#endif /* configBSP430_PERIPH_PORT6_ISR */


/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_PORTD_R__)
#define _BSP430_PERIPH_PORT7_BASEADDRESS __MSP430_BASEADDRESS_PORTD_R__
#define _BSP430_PERIPH_PORT8_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTD_R__)
#else /* PORTD_R */
#define _BSP430_PERIPH_PORT7_BASEADDRESS __MSP430_BASEADDRESS_PORTD__
#define _BSP430_PERIPH_PORT8_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTD__)
#endif /* PORTD_R */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Handle for the raw PORT7 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT7
 * is defined to a true value. */
#define BSP430_PERIPH_PORT7 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT7_BASEADDRESS))

/** Handle for the raw PORT8 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT8
 * is defined to a true value. */
#define BSP430_PERIPH_PORT8 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT8_BASEADDRESS))

/** Handle for the raw PORTD device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORTD
 * is defined to a true value. */
#define BSP430_PERIPH_PORTD ((xBSP430periphHandle)(1 + _BSP430_PERIPH_PORT7_BASEADDRESS))

/** @def configBSP430_PERIPH_PORT7
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT7 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT7__ or
 * @c __MSP430_HAS_PORT7_R__) */
#ifndef configBSP430_PERIPH_PORT7
#define configBSP430_PERIPH_PORT7 0
#endif /* configBSP430_PERIPH_PORT7 */

/** @def configBSP430_PERIPH_PORT7_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT7.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT7_ISR
#define configBSP430_PERIPH_PORT7_ISR 0
#endif /* configBSP430_PERIPH_PORT7_ISR */

/** @def configBSP430_PERIPH_PORT8
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT8 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT8__ or
 * @c __MSP430_HAS_PORT8_R__) */
#ifndef configBSP430_PERIPH_PORT8
#define configBSP430_PERIPH_PORT8 0
#endif /* configBSP430_PERIPH_PORT8 */

/** @def configBSP430_PERIPH_PORT8_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT8.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT8_ISR
#define configBSP430_PERIPH_PORT8_ISR 0
#endif /* configBSP430_PERIPH_PORT8_ISR */


/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_PORTE_R__)
#define _BSP430_PERIPH_PORT9_BASEADDRESS __MSP430_BASEADDRESS_PORTE_R__
#define _BSP430_PERIPH_PORT10_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTE_R__)
#else /* PORTE_R */
#define _BSP430_PERIPH_PORT9_BASEADDRESS __MSP430_BASEADDRESS_PORTE__
#define _BSP430_PERIPH_PORT10_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTE__)
#endif /* PORTE_R */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Handle for the raw PORT9 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT9
 * is defined to a true value. */
#define BSP430_PERIPH_PORT9 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT9_BASEADDRESS))

/** Handle for the raw PORT10 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT10
 * is defined to a true value. */
#define BSP430_PERIPH_PORT10 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT10_BASEADDRESS))

/** Handle for the raw PORTE device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORTE
 * is defined to a true value. */
#define BSP430_PERIPH_PORTE ((xBSP430periphHandle)(1 + _BSP430_PERIPH_PORT9_BASEADDRESS))

/** @def configBSP430_PERIPH_PORT9
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT9 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT9__ or
 * @c __MSP430_HAS_PORT9_R__) */
#ifndef configBSP430_PERIPH_PORT9
#define configBSP430_PERIPH_PORT9 0
#endif /* configBSP430_PERIPH_PORT9 */

/** @def configBSP430_PERIPH_PORT9_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT9.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT9_ISR
#define configBSP430_PERIPH_PORT9_ISR 0
#endif /* configBSP430_PERIPH_PORT9_ISR */

/** @def configBSP430_PERIPH_PORT10
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT10 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT10__ or
 * @c __MSP430_HAS_PORT10_R__) */
#ifndef configBSP430_PERIPH_PORT10
#define configBSP430_PERIPH_PORT10 0
#endif /* configBSP430_PERIPH_PORT10 */

/** @def configBSP430_PERIPH_PORT10_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT10.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT10_ISR
#define configBSP430_PERIPH_PORT10_ISR 0
#endif /* configBSP430_PERIPH_PORT10_ISR */


/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_PORTF_R__)
#define _BSP430_PERIPH_PORT11_BASEADDRESS __MSP430_BASEADDRESS_PORTF_R__
#define _BSP430_PERIPH_PORT12_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTF_R__)
#else /* PORTF_R */
#define _BSP430_PERIPH_PORT11_BASEADDRESS __MSP430_BASEADDRESS_PORTF__
#define _BSP430_PERIPH_PORT12_BASEADDRESS (1+__MSP430_BASEADDRESS_PORTF__)
#endif /* PORTF_R */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Handle for the raw PORT11 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT11
 * is defined to a true value. */
#define BSP430_PERIPH_PORT11 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT11_BASEADDRESS))

/** Handle for the raw PORT12 device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORT12
 * is defined to a true value. */
#define BSP430_PERIPH_PORT12 ((xBSP430periphHandle)(_BSP430_PERIPH_PORT12_BASEADDRESS))

/** Handle for the raw PORTF device.
 *
 * The handle may be referenced only if configBSP430_PERIPH_PORTF
 * is defined to a true value. */
#define BSP430_PERIPH_PORTF ((xBSP430periphHandle)(1 + _BSP430_PERIPH_PORT11_BASEADDRESS))

/** @def configBSP430_PERIPH_PORT11
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT11 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT11__ or
 * @c __MSP430_HAS_PORT11_R__) */
#ifndef configBSP430_PERIPH_PORT11
#define configBSP430_PERIPH_PORT11 0
#endif /* configBSP430_PERIPH_PORT11 */

/** @def configBSP430_PERIPH_PORT11_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT11.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT11_ISR
#define configBSP430_PERIPH_PORT11_ISR 0
#endif /* configBSP430_PERIPH_PORT11_ISR */

/** @def configBSP430_PERIPH_PORT12
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c PORT12 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_PORT12__ or
 * @c __MSP430_HAS_PORT12_R__) */
#ifndef configBSP430_PERIPH_PORT12
#define configBSP430_PERIPH_PORT12 0
#endif /* configBSP430_PERIPH_PORT12 */

/** @def configBSP430_PERIPH_PORT12_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c PORT12.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_PORT_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_PORT12_ISR
#define configBSP430_PERIPH_PORT12_ISR 0
#endif /* configBSP430_PERIPH_PORT12_ISR */

/* END AUTOMATICALLY GENERATED CODE [hpl_port_5xx] */
/* !BSP430! end=hpl_port_5xx */
#else /* MSP430XV2 */
/* 1xx through 4xx port management */
#warning not yet implemented
#endif /* MSP430XV2 */

/* !BSP430! instance=PORT1,PORT2,PORT3,PORT4,PORT5,PORT6,PORT7,PORT8,PORT9,PORT10,PORT11 insert=hal_port */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_port] */
/** FreeRTOS HAL handle for PORT1.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT1 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT1;

/** FreeRTOS HAL handle for PORT2.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT2 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT2;

/** FreeRTOS HAL handle for PORT3.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT3 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT3;

/** FreeRTOS HAL handle for PORT4.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT4 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT4;

/** FreeRTOS HAL handle for PORT5.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT5 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT5;

/** FreeRTOS HAL handle for PORT6.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT6 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT6;

/** FreeRTOS HAL handle for PORT7.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT7 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT7;

/** FreeRTOS HAL handle for PORT8.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT8 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT8;

/** FreeRTOS HAL handle for PORT9.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT9 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT9;

/** FreeRTOS HAL handle for PORT10.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT10 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT10;

/** FreeRTOS HAL handle for PORT11.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_PORT11 is defined to a true
 * value. */

extern xBSP430portHandle const xBSP430port_PORT11;

/* END AUTOMATICALLY GENERATED CODE [hal_port] */
/* !BSP430! end=hal_port */

xBSP430portHandle xBSP430portLookup (xBSP430periphHandle xHandle);

#endif /* BSP430_PERIPH_PORT_H */
