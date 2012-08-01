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
/* !BSP430! instance=PORT1,PORT2,PORT3,PORT4,PORT5,PORT6,PORT7,PORT8,PORT9,PORT10,PORT11 */

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
  xBSP430periphPORT_16_u in;	/**< PxIN */ /* 0x00 */
  xBSP430periphPORT_16_u out;	/**< PxOUT */ /* 0x02 */
  xBSP430periphPORT_16_u dir;	/**< PxDIR (set for output) */ /* 0x04 */
  xBSP430periphPORT_16_u ren;	/**< PxREN (set to enable) */ /* 0x06 */
  xBSP430periphPORT_16_u ds;	/**< PxDS (drive select, some devices) */ /* 0x08 */
  union {
    xBSP430periphPORT_16_u sel; /**< PxSEL (non-FR5xx devices) */ /* 0x0A */
    xBSP430periphPORT_16_u sel0; /**< PxSEL0 (FR5xx devices) */ /* 0x0A */
  };
  xBSP430periphPORT_16_u sel1; /**< PxSEL1 (secondary/tertiary function, FR5xx devices only) */ /* 0x0C */
  unsigned int _reserved_x0E;
  xBSP430periphPORT_16_u selc; /**< PxSELC (support atomic transition to tertiary function, FR5xx devices only) */ /* 0x10 */
  unsigned int _reserved_x12;
  unsigned int _reserved_x14;
  unsigned int _reserved_x16;
  xBSP430periphPORT_16_u ies;	 /**< PxIES */ /* 0x18 */
  xBSP430periphPORT_16_u ie;	 /**< PxIE */ /* 0x1A */
  xBSP430periphPORT_16_u ifg;	 /**< PxIFG */ /* 0x1C */
} xBSP430periphPORT_5XX_16;

/** Layout for 5xx family ports, 8-bit access
 *
 * This structure includes the necessary padding to maintain alignment
 * when referencing the second 8-bit register in a 16-bit bank.
 */
typedef struct xBSP430periphPORT_5XX_8 {
  unsigned char in;	/**< PxIN */ /* 0x00 */
  unsigned char _reserved_x01;
  unsigned char out;	/**< PxOUT */ /* 0x02 */
  unsigned char _reserved_x03;
  unsigned char dir;	/**< PxDIR (set for output) */ /* 0x04 */
  unsigned char _reserved_x05;
  unsigned char ren;	/**< PxREN (set to enable) */ /* 0x06 */
  unsigned char _reserved_x07;
  unsigned char ds; /**< PxDS (drive select, some devices) */ /* 0x08 */
  unsigned char _reserved_x09;
  union {
    unsigned char sel; /**< PxSEL (non-FR5xx devices) */ /* 0x0A */
    unsigned char sel0; /**< PxSEL0 (FR5xx devices) */ /* 0x0A */
  };
  unsigned char _reserved_x0B;
  unsigned char sel1; /**< PxSEL1 (secondary/tertiary function, FR5xx devices only) */ /* 0x0C */
  unsigned char _reserved_x0D;
  unsigned char _reserved_x0E;
  unsigned char _reserved_x0F;
  unsigned char selc; /**< PxSELC (support atomic transition to tertiary function, FR5xx devices only) *//* 0x10 */
  unsigned char _reserved_x11;
  unsigned char _reserved_x12;
  unsigned char _reserved_x13;
  unsigned char _reserved_x14;
  unsigned char _reserved_x15;
  unsigned char _reserved_x16;
  unsigned char _reserved_x17;
  unsigned char ies;	 /**< PxIES */ /* 0x18 */
  unsigned char _reserved_x19;
  unsigned char ie;	 /**< PxIE */ /* 0x1A */
  unsigned char _reserved_x1B;
  unsigned char ifg;	 /**< PxIFG */ /* 0x1C */
} xBSP430periphPORT_5XX_8;

/** @cond DOXYGEN_INTERNAL */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
#define BSP430_PERIPH_PORTIE_ xBSP430periphPORT_5XX_8
#define BSP430_PERIPH_PORT_ xBSP430periphPORT_5XX_8
#define BSP430_PERIPH_PORTW_ xBSP430periphPORT_5XX_16
#else /* MSP430XV2 */
#define BSP430_PERIPH_PORTIE_ xBSP430periphPORT_IE_8
#define BSP430_PERIPH_PORT_ xBSP430periphPORT_8
#define BSP430_PERIPH_PORTW_ xBSP430periphPORT_16
#endif /* MSP430XV2 */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Structure used to access ports with interrupt capability.
 *
 * The underlying structure selected depends on the MCU family. */
typedef BSP430_PERIPH_PORTIE_ xBSP430periphPORTIE;

/** Structure used to access ports without interrupt capability.
 *
 * The underlying structure selected depends on the MCU family. */
typedef BSP430_PERIPH_PORT_ xBSP430periphPORT;

/** Structure used to access 16-bit ports.
 *
 * The underlying structure selected depends on the MCU family. */
typedef BSP430_PERIPH_PORTW_ xBSP430periphPORTW;

/** Get the peripheral register pointer for an interrupt-enabled port.
 *
 * @note All ports on 5xx-family devices are interrupt-enabled.
 *
 * @param xHandle The handle identifier, such as #BSP430_PERIPH_PORT1.
 *
 * @return A typed pointer that can be used to manipulate the port.  A
 * null pointer is returned if the handle does not correspond to a
 * timer which has been enabled (e.g., with
 * #configBSP430_PERIPH_PORT1), or if the specified port does not
 * support interrupts (see #xBSP430periphLookupPORT)
 */
volatile xBSP430periphPORTIE * xBSP430periphLookupPORTIE (xBSP430periphHandle xHandle);

/** Get the peripheral register pointer for a non-interrupt-enabled port.
 *
 * @param xHandle The handle identifier, such as #BSP430_PERIPH_PORT1.
 *
 * @return A typed pointer that can be used to manipulate the port.  A
 * null pointer is returned if the handle does not correspond to a
 * timer which has been enabled (e.g., with
 * #configBSP430_PERIPH_PORT1), or if the specified port supports
 * interrupts (see #xBSP430periphLookupPORTIE)
 */
volatile xBSP430periphPORT * xBSP430periphLookupPORT (xBSP430periphHandle xHandle);

/** @cond DOXYGEN_INTERNAL */

#if defined(__MSP430_HAS_MSP430XV2_CPU__)
/* All 5xx ports are resistor enabled */
#define BSP430_PERIPH_PORTA_BASEADDRESS_ __MSP430_BASEADDRESS_PORTA_R__
#define BSP430_PERIPH_PORTB_BASEADDRESS_ __MSP430_BASEADDRESS_PORTB_R__
#define BSP430_PERIPH_PORTC_BASEADDRESS_ __MSP430_BASEADDRESS_PORTC_R__
#define BSP430_PERIPH_PORTD_BASEADDRESS_ __MSP430_BASEADDRESS_PORTD_R__
#define BSP430_PERIPH_PORTE_BASEADDRESS_ __MSP430_BASEADDRESS_PORTE_R__
#define BSP430_PERIPH_PORTF_BASEADDRESS_ __MSP430_BASEADDRESS_PORTF_R__

#define BSP430_PERIPH_PORT1_BASEADDRESS_ __MSP430_BASEADDRESS_PORT1_R__
#define BSP430_PERIPH_PORT2_BASEADDRESS_ __MSP430_BASEADDRESS_PORT2_R__
#define BSP430_PERIPH_PORT3_BASEADDRESS_ __MSP430_BASEADDRESS_PORT3_R__
#define BSP430_PERIPH_PORT4_BASEADDRESS_ __MSP430_BASEADDRESS_PORT4_R__
#define BSP430_PERIPH_PORT5_BASEADDRESS_ __MSP430_BASEADDRESS_PORT5_R__
#define BSP430_PERIPH_PORT6_BASEADDRESS_ __MSP430_BASEADDRESS_PORT6_R__
#define BSP430_PERIPH_PORT7_BASEADDRESS_ __MSP430_BASEADDRESS_PORT7_R__
#define BSP430_PERIPH_PORT8_BASEADDRESS_ __MSP430_BASEADDRESS_PORT8_R__
#define BSP430_PERIPH_PORT9_BASEADDRESS_ __MSP430_BASEADDRESS_PORT9_R__
#define BSP430_PERIPH_PORT10_BASEADDRESS_ __MSP430_BASEADDRESS_PORT10_R__
#define BSP430_PERIPH_PORT11_BASEADDRESS_ __MSP430_BASEADDRESS_PORT11_R__

#define BSP430_PERIPH_PORTJ_BASEADDRESS_ __MSP430_BASEADDRESS_PORTJ_R__

#else /* 5xx */
/* Port addresses are fixed for all pre-5xx families. */
#define BSP430_PERIPH_PORT1_BASEADDRESS_ 0x0020
#define BSP430_PERIPH_PORT2_BASEADDRESS_ 0x0028
#define BSP430_PERIPH_PORT3_BASEADDRESS_ 0x0018
#define BSP430_PERIPH_PORT4_BASEADDRESS_ 0x000C
#define BSP430_PERIPH_PORT5_BASEADDRESS_ 0x0030
#define BSP430_PERIPH_PORT6_BASEADDRESS_ 0x0034

#define BSP430_PERIPH_PORTA_BASEADDRESS_ 0x0038
#define BSP430_PERIPH_PORT7_BASEADDRESS_ 0x0038
#define BSP430_PERIPH_PORT8_BASEADDRESS_ 0x0039

#define BSP430_PERIPH_PORTB_BASEADDRESS_ 0x0008
#define BSP430_PERIPH_PORT9_BASEADDRESS_ 0x0008
#define BSP430_PERIPH_PORT10_BASEADDRESS_ 0x0009

#endif /* 5xx */

/** @endcond */ /* DOXYGEN_INTERNAL */

/* Forward declaration to hardware abstraction layer ports */
struct xBSP430portState;

/** The PORT internal state is protected. */
typedef struct xBSP430portState * xBSP430portHandle;

#if 1 // defined(__MSP430_HAS_MSP430XV2_CPU__)

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** @def configBSP430_HAL_PORT1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT1 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT1 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT1
#define configBSP430_HAL_PORT1 0
#endif /* configBSP430_HAL_PORT1 */

/** BSP430 HAL handle for PORT1.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT1
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT1 - 0)
extern xBSP430portHandle const xBSP430port_PORT1;
#endif /* configBSP430_HAL_PORT1 */

/** @def configBSP430_HAL_PORT2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT2 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT2 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT2
#define configBSP430_HAL_PORT2 0
#endif /* configBSP430_HAL_PORT2 */

/** BSP430 HAL handle for PORT2.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT2
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT2 - 0)
extern xBSP430portHandle const xBSP430port_PORT2;
#endif /* configBSP430_HAL_PORT2 */

/** @def configBSP430_HAL_PORT3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT3 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT3 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT3
#define configBSP430_HAL_PORT3 0
#endif /* configBSP430_HAL_PORT3 */

/** BSP430 HAL handle for PORT3.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT3
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT3 - 0)
extern xBSP430portHandle const xBSP430port_PORT3;
#endif /* configBSP430_HAL_PORT3 */

/** @def configBSP430_HAL_PORT4
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT4 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT4 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT4
#define configBSP430_HAL_PORT4 0
#endif /* configBSP430_HAL_PORT4 */

/** BSP430 HAL handle for PORT4.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT4
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT4 - 0)
extern xBSP430portHandle const xBSP430port_PORT4;
#endif /* configBSP430_HAL_PORT4 */

/** @def configBSP430_HAL_PORT5
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT5 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT5 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT5
#define configBSP430_HAL_PORT5 0
#endif /* configBSP430_HAL_PORT5 */

/** BSP430 HAL handle for PORT5.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT5
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT5 - 0)
extern xBSP430portHandle const xBSP430port_PORT5;
#endif /* configBSP430_HAL_PORT5 */

/** @def configBSP430_HAL_PORT6
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT6 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT6 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT6
#define configBSP430_HAL_PORT6 0
#endif /* configBSP430_HAL_PORT6 */

/** BSP430 HAL handle for PORT6.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT6
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT6 - 0)
extern xBSP430portHandle const xBSP430port_PORT6;
#endif /* configBSP430_HAL_PORT6 */

/** @def configBSP430_HAL_PORT7
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT7 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT7 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT7
#define configBSP430_HAL_PORT7 0
#endif /* configBSP430_HAL_PORT7 */

/** BSP430 HAL handle for PORT7.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT7
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT7 - 0)
extern xBSP430portHandle const xBSP430port_PORT7;
#endif /* configBSP430_HAL_PORT7 */

/** @def configBSP430_HAL_PORT8
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT8 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT8 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT8
#define configBSP430_HAL_PORT8 0
#endif /* configBSP430_HAL_PORT8 */

/** BSP430 HAL handle for PORT8.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT8
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT8 - 0)
extern xBSP430portHandle const xBSP430port_PORT8;
#endif /* configBSP430_HAL_PORT8 */

/** @def configBSP430_HAL_PORT9
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT9 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT9 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT9
#define configBSP430_HAL_PORT9 0
#endif /* configBSP430_HAL_PORT9 */

/** BSP430 HAL handle for PORT9.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT9
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT9 - 0)
extern xBSP430portHandle const xBSP430port_PORT9;
#endif /* configBSP430_HAL_PORT9 */

/** @def configBSP430_HAL_PORT10
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT10 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT10 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT10
#define configBSP430_HAL_PORT10 0
#endif /* configBSP430_HAL_PORT10 */

/** BSP430 HAL handle for PORT10.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT10
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT10 - 0)
extern xBSP430portHandle const xBSP430port_PORT10;
#endif /* configBSP430_HAL_PORT10 */

/** @def configBSP430_HAL_PORT11
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT11 peripheral HAL interface.  This defines a global
 * object xBSP430portHandle supporting enhanced functionality
 * for the peripheral.
 *
 * @note Enabling this defaults #configBSP430_PERIPH_PORT11 to true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT11
#define configBSP430_HAL_PORT11 0
#endif /* configBSP430_HAL_PORT11 */

/** BSP430 HAL handle for PORT11.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT11
 * is defined to a true value. */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_PORT11 - 0)
extern xBSP430portHandle const xBSP430port_PORT11;
#endif /* configBSP430_HAL_PORT11 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! insert=hpl_ba_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_ba_decl] */
/** @def configBSP430_PERIPH_PORT1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT1 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT1
#define configBSP430_PERIPH_PORT1 (configBSP430_HAL_PORT1 - 0)
#endif /* configBSP430_PERIPH_PORT1 */

#if (configBSP430_HAL_PORT1 - 0) && ! (configBSP430_PERIPH_PORT1 - 0)
#warning configBSP430_HAL_PORT1 requested without configBSP430_PERIPH_PORT1
#endif /* HAL and not HPL */

/** Handle for the raw PORT1 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT1
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT1 - 0)
#define BSP430_PERIPH_PORT1 ((xBSP430periphHandle)(BSP430_PERIPH_PORT1_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT1 */

/** Pointer to the peripheral register map for PORT1.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT1
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT1 - 0)
/** Typed pointer to HPL structure for PORT1 suitable for use in const initializers */
#define BSP430_HPL_PORT1 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT1)
static volatile xBSP430periphPORT * const xBSP430periph_PORT1 = BSP430_HPL_PORT1;
#endif /* configBSP430_PERIPH_PORT1 */

/** @def configBSP430_PERIPH_PORT2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT2 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT2
#define configBSP430_PERIPH_PORT2 (configBSP430_HAL_PORT2 - 0)
#endif /* configBSP430_PERIPH_PORT2 */

#if (configBSP430_HAL_PORT2 - 0) && ! (configBSP430_PERIPH_PORT2 - 0)
#warning configBSP430_HAL_PORT2 requested without configBSP430_PERIPH_PORT2
#endif /* HAL and not HPL */

/** Handle for the raw PORT2 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT2
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT2 - 0)
#define BSP430_PERIPH_PORT2 ((xBSP430periphHandle)(BSP430_PERIPH_PORT2_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT2 */

/** Pointer to the peripheral register map for PORT2.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT2
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT2 - 0)
/** Typed pointer to HPL structure for PORT2 suitable for use in const initializers */
#define BSP430_HPL_PORT2 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT2)
static volatile xBSP430periphPORT * const xBSP430periph_PORT2 = BSP430_HPL_PORT2;
#endif /* configBSP430_PERIPH_PORT2 */

/** @def configBSP430_PERIPH_PORT3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT3 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT3 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT3
#define configBSP430_PERIPH_PORT3 (configBSP430_HAL_PORT3 - 0)
#endif /* configBSP430_PERIPH_PORT3 */

#if (configBSP430_HAL_PORT3 - 0) && ! (configBSP430_PERIPH_PORT3 - 0)
#warning configBSP430_HAL_PORT3 requested without configBSP430_PERIPH_PORT3
#endif /* HAL and not HPL */

/** Handle for the raw PORT3 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT3
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT3 - 0)
#define BSP430_PERIPH_PORT3 ((xBSP430periphHandle)(BSP430_PERIPH_PORT3_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT3 */

/** Pointer to the peripheral register map for PORT3.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT3
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT3 - 0)
/** Typed pointer to HPL structure for PORT3 suitable for use in const initializers */
#define BSP430_HPL_PORT3 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT3)
static volatile xBSP430periphPORT * const xBSP430periph_PORT3 = BSP430_HPL_PORT3;
#endif /* configBSP430_PERIPH_PORT3 */

/** @def configBSP430_PERIPH_PORT4
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT4 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT4 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT4
#define configBSP430_PERIPH_PORT4 (configBSP430_HAL_PORT4 - 0)
#endif /* configBSP430_PERIPH_PORT4 */

#if (configBSP430_HAL_PORT4 - 0) && ! (configBSP430_PERIPH_PORT4 - 0)
#warning configBSP430_HAL_PORT4 requested without configBSP430_PERIPH_PORT4
#endif /* HAL and not HPL */

/** Handle for the raw PORT4 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT4
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT4 - 0)
#define BSP430_PERIPH_PORT4 ((xBSP430periphHandle)(BSP430_PERIPH_PORT4_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT4 */

/** Pointer to the peripheral register map for PORT4.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT4
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT4 - 0)
/** Typed pointer to HPL structure for PORT4 suitable for use in const initializers */
#define BSP430_HPL_PORT4 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT4)
static volatile xBSP430periphPORT * const xBSP430periph_PORT4 = BSP430_HPL_PORT4;
#endif /* configBSP430_PERIPH_PORT4 */

/** @def configBSP430_PERIPH_PORT5
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT5 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT5 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT5
#define configBSP430_PERIPH_PORT5 (configBSP430_HAL_PORT5 - 0)
#endif /* configBSP430_PERIPH_PORT5 */

#if (configBSP430_HAL_PORT5 - 0) && ! (configBSP430_PERIPH_PORT5 - 0)
#warning configBSP430_HAL_PORT5 requested without configBSP430_PERIPH_PORT5
#endif /* HAL and not HPL */

/** Handle for the raw PORT5 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT5
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT5 - 0)
#define BSP430_PERIPH_PORT5 ((xBSP430periphHandle)(BSP430_PERIPH_PORT5_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT5 */

/** Pointer to the peripheral register map for PORT5.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT5
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT5 - 0)
/** Typed pointer to HPL structure for PORT5 suitable for use in const initializers */
#define BSP430_HPL_PORT5 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT5)
static volatile xBSP430periphPORT * const xBSP430periph_PORT5 = BSP430_HPL_PORT5;
#endif /* configBSP430_PERIPH_PORT5 */

/** @def configBSP430_PERIPH_PORT6
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT6 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT6 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT6
#define configBSP430_PERIPH_PORT6 (configBSP430_HAL_PORT6 - 0)
#endif /* configBSP430_PERIPH_PORT6 */

#if (configBSP430_HAL_PORT6 - 0) && ! (configBSP430_PERIPH_PORT6 - 0)
#warning configBSP430_HAL_PORT6 requested without configBSP430_PERIPH_PORT6
#endif /* HAL and not HPL */

/** Handle for the raw PORT6 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT6
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT6 - 0)
#define BSP430_PERIPH_PORT6 ((xBSP430periphHandle)(BSP430_PERIPH_PORT6_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT6 */

/** Pointer to the peripheral register map for PORT6.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT6
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT6 - 0)
/** Typed pointer to HPL structure for PORT6 suitable for use in const initializers */
#define BSP430_HPL_PORT6 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT6)
static volatile xBSP430periphPORT * const xBSP430periph_PORT6 = BSP430_HPL_PORT6;
#endif /* configBSP430_PERIPH_PORT6 */

/** @def configBSP430_PERIPH_PORT7
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT7 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT7 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT7
#define configBSP430_PERIPH_PORT7 (configBSP430_HAL_PORT7 - 0)
#endif /* configBSP430_PERIPH_PORT7 */

#if (configBSP430_HAL_PORT7 - 0) && ! (configBSP430_PERIPH_PORT7 - 0)
#warning configBSP430_HAL_PORT7 requested without configBSP430_PERIPH_PORT7
#endif /* HAL and not HPL */

/** Handle for the raw PORT7 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT7
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT7 - 0)
#define BSP430_PERIPH_PORT7 ((xBSP430periphHandle)(BSP430_PERIPH_PORT7_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT7 */

/** Pointer to the peripheral register map for PORT7.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT7
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT7 - 0)
/** Typed pointer to HPL structure for PORT7 suitable for use in const initializers */
#define BSP430_HPL_PORT7 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT7)
static volatile xBSP430periphPORT * const xBSP430periph_PORT7 = BSP430_HPL_PORT7;
#endif /* configBSP430_PERIPH_PORT7 */

/** @def configBSP430_PERIPH_PORT8
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT8 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT8 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT8
#define configBSP430_PERIPH_PORT8 (configBSP430_HAL_PORT8 - 0)
#endif /* configBSP430_PERIPH_PORT8 */

#if (configBSP430_HAL_PORT8 - 0) && ! (configBSP430_PERIPH_PORT8 - 0)
#warning configBSP430_HAL_PORT8 requested without configBSP430_PERIPH_PORT8
#endif /* HAL and not HPL */

/** Handle for the raw PORT8 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT8
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT8 - 0)
#define BSP430_PERIPH_PORT8 ((xBSP430periphHandle)(BSP430_PERIPH_PORT8_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT8 */

/** Pointer to the peripheral register map for PORT8.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT8
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT8 - 0)
/** Typed pointer to HPL structure for PORT8 suitable for use in const initializers */
#define BSP430_HPL_PORT8 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT8)
static volatile xBSP430periphPORT * const xBSP430periph_PORT8 = BSP430_HPL_PORT8;
#endif /* configBSP430_PERIPH_PORT8 */

/** @def configBSP430_PERIPH_PORT9
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT9 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT9 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT9
#define configBSP430_PERIPH_PORT9 (configBSP430_HAL_PORT9 - 0)
#endif /* configBSP430_PERIPH_PORT9 */

#if (configBSP430_HAL_PORT9 - 0) && ! (configBSP430_PERIPH_PORT9 - 0)
#warning configBSP430_HAL_PORT9 requested without configBSP430_PERIPH_PORT9
#endif /* HAL and not HPL */

/** Handle for the raw PORT9 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT9
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT9 - 0)
#define BSP430_PERIPH_PORT9 ((xBSP430periphHandle)(BSP430_PERIPH_PORT9_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT9 */

/** Pointer to the peripheral register map for PORT9.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT9
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT9 - 0)
/** Typed pointer to HPL structure for PORT9 suitable for use in const initializers */
#define BSP430_HPL_PORT9 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT9)
static volatile xBSP430periphPORT * const xBSP430periph_PORT9 = BSP430_HPL_PORT9;
#endif /* configBSP430_PERIPH_PORT9 */

/** @def configBSP430_PERIPH_PORT10
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT10 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT10 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT10
#define configBSP430_PERIPH_PORT10 (configBSP430_HAL_PORT10 - 0)
#endif /* configBSP430_PERIPH_PORT10 */

#if (configBSP430_HAL_PORT10 - 0) && ! (configBSP430_PERIPH_PORT10 - 0)
#warning configBSP430_HAL_PORT10 requested without configBSP430_PERIPH_PORT10
#endif /* HAL and not HPL */

/** Handle for the raw PORT10 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT10
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT10 - 0)
#define BSP430_PERIPH_PORT10 ((xBSP430periphHandle)(BSP430_PERIPH_PORT10_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT10 */

/** Pointer to the peripheral register map for PORT10.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT10
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT10 - 0)
/** Typed pointer to HPL structure for PORT10 suitable for use in const initializers */
#define BSP430_HPL_PORT10 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT10)
static volatile xBSP430periphPORT * const xBSP430periph_PORT10 = BSP430_HPL_PORT10;
#endif /* configBSP430_PERIPH_PORT10 */

/** @def configBSP430_PERIPH_PORT11
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c PORT11 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_PORT11 defaults this to
 * true, so you only need to explicitly request if you want the HPL
 * interface without the HAL interface.
 *
 * @defaulted */
#ifndef configBSP430_PERIPH_PORT11
#define configBSP430_PERIPH_PORT11 (configBSP430_HAL_PORT11 - 0)
#endif /* configBSP430_PERIPH_PORT11 */

#if (configBSP430_HAL_PORT11 - 0) && ! (configBSP430_PERIPH_PORT11 - 0)
#warning configBSP430_HAL_PORT11 requested without configBSP430_PERIPH_PORT11
#endif /* HAL and not HPL */

/** Handle for the raw PORT11 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_PORT11
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT11 - 0)
#define BSP430_PERIPH_PORT11 ((xBSP430periphHandle)(BSP430_PERIPH_PORT11_BASEADDRESS_))
#endif /* configBSP430_PERIPH_PORT11 */

/** Pointer to the peripheral register map for PORT11.
 *
 * The pointer may be used only if #configBSP430_PERIPH_PORT11
 * is defined to a true value.
 *
 * @defaulted */
#if defined(BSP430_DOXYGEN) || (configBSP430_PERIPH_PORT11 - 0)
/** Typed pointer to HPL structure for PORT11 suitable for use in const initializers */
#define BSP430_HPL_PORT11 ((volatile xBSP430periphPORT *)BSP430_PERIPH_PORT11)
static volatile xBSP430periphPORT * const xBSP430periph_PORT11 = BSP430_HPL_PORT11;
#endif /* configBSP430_PERIPH_PORT11 */

/* END AUTOMATICALLY GENERATED CODE [hpl_ba_decl] */
/* !BSP430! end=hpl_ba_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** @def configBSP430_HAL_PORT1_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT1, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT1 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT1
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT1_ISR
#define configBSP430_HAL_PORT1_ISR (configBSP430_HAL_PORT1 - 0)
#endif /* configBSP430_HAL_PORT1_ISR */

#if (configBSP430_HAL_PORT1_ISR - 0) && ! (configBSP430_HAL_PORT1 - 0)
#warning configBSP430_HAL_PORT1_ISR requested without configBSP430_HAL_PORT1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT2_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT2, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT2 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT2
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT2_ISR
#define configBSP430_HAL_PORT2_ISR (configBSP430_HAL_PORT2 - 0)
#endif /* configBSP430_HAL_PORT2_ISR */

#if (configBSP430_HAL_PORT2_ISR - 0) && ! (configBSP430_HAL_PORT2 - 0)
#warning configBSP430_HAL_PORT2_ISR requested without configBSP430_HAL_PORT2
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT3_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT3, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT3 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT3
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT3_ISR
#define configBSP430_HAL_PORT3_ISR (configBSP430_HAL_PORT3 - 0)
#endif /* configBSP430_HAL_PORT3_ISR */

#if (configBSP430_HAL_PORT3_ISR - 0) && ! (configBSP430_HAL_PORT3 - 0)
#warning configBSP430_HAL_PORT3_ISR requested without configBSP430_HAL_PORT3
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT4_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT4, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT4 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT4
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT4_ISR
#define configBSP430_HAL_PORT4_ISR (configBSP430_HAL_PORT4 - 0)
#endif /* configBSP430_HAL_PORT4_ISR */

#if (configBSP430_HAL_PORT4_ISR - 0) && ! (configBSP430_HAL_PORT4 - 0)
#warning configBSP430_HAL_PORT4_ISR requested without configBSP430_HAL_PORT4
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT5_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT5, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT5 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT5
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT5_ISR
#define configBSP430_HAL_PORT5_ISR (configBSP430_HAL_PORT5 - 0)
#endif /* configBSP430_HAL_PORT5_ISR */

#if (configBSP430_HAL_PORT5_ISR - 0) && ! (configBSP430_HAL_PORT5 - 0)
#warning configBSP430_HAL_PORT5_ISR requested without configBSP430_HAL_PORT5
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT6_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT6, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT6 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT6
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT6_ISR
#define configBSP430_HAL_PORT6_ISR (configBSP430_HAL_PORT6 - 0)
#endif /* configBSP430_HAL_PORT6_ISR */

#if (configBSP430_HAL_PORT6_ISR - 0) && ! (configBSP430_HAL_PORT6 - 0)
#warning configBSP430_HAL_PORT6_ISR requested without configBSP430_HAL_PORT6
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT7_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT7, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT7 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT7
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT7_ISR
#define configBSP430_HAL_PORT7_ISR (configBSP430_HAL_PORT7 - 0)
#endif /* configBSP430_HAL_PORT7_ISR */

#if (configBSP430_HAL_PORT7_ISR - 0) && ! (configBSP430_HAL_PORT7 - 0)
#warning configBSP430_HAL_PORT7_ISR requested without configBSP430_HAL_PORT7
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT8_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT8, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT8 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT8
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT8_ISR
#define configBSP430_HAL_PORT8_ISR (configBSP430_HAL_PORT8 - 0)
#endif /* configBSP430_HAL_PORT8_ISR */

#if (configBSP430_HAL_PORT8_ISR - 0) && ! (configBSP430_HAL_PORT8 - 0)
#warning configBSP430_HAL_PORT8_ISR requested without configBSP430_HAL_PORT8
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT9_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT9, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT9 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT9
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT9_ISR
#define configBSP430_HAL_PORT9_ISR (configBSP430_HAL_PORT9 - 0)
#endif /* configBSP430_HAL_PORT9_ISR */

#if (configBSP430_HAL_PORT9_ISR - 0) && ! (configBSP430_HAL_PORT9 - 0)
#warning configBSP430_HAL_PORT9_ISR requested without configBSP430_HAL_PORT9
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT10_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT10, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT10 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT10
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT10_ISR
#define configBSP430_HAL_PORT10_ISR (configBSP430_HAL_PORT10 - 0)
#endif /* configBSP430_HAL_PORT10_ISR */

#if (configBSP430_HAL_PORT10_ISR - 0) && ! (configBSP430_HAL_PORT10 - 0)
#warning configBSP430_HAL_PORT10_ISR requested without configBSP430_HAL_PORT10
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_PORT11_ISR
 *
 * Define to a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c PORT11, but want to define your
 * own interrupt service routine for the peripheral.
 *
 * Enabling #configBSP430_HAL_PORT11 defaults this to
 * true, so you only need to explicitly set it if you do not want to
 * use the standard ISR provided by BSP430.
 *
 * @note Enabling this requires that #configBSP430_HAL_PORT11
 * also be true.
 *
 * @defaulted */
#ifndef configBSP430_HAL_PORT11_ISR
#define configBSP430_HAL_PORT11_ISR (configBSP430_HAL_PORT11 - 0)
#endif /* configBSP430_HAL_PORT11_ISR */

#if (configBSP430_HAL_PORT11_ISR - 0) && ! (configBSP430_HAL_PORT11 - 0)
#warning configBSP430_HAL_PORT11_ISR requested without configBSP430_HAL_PORT11
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

#else /* MSP430XV2 */
/* 1xx through 4xx port management */
#warning not yet implemented
#endif /* MSP430XV2 */

/** Translate a port HPL handle to its corresponding HAL handle.
 *
 * @param hpl the hardware presentation layer handle for a port
 *
 * @return the HAL handle for the port, or NULL if either the HPL port
 * is unrecognized or the corresponding HAL port was not enabled. */
xBSP430portHandle xBSP430portLookup (xBSP430periphHandle hpl);

#endif /* BSP430_PERIPH_PORT_H */
