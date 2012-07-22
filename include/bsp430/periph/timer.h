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
 * @brief Hardware presentation/abstraction for generic timers (T\#A/T\#B)
 *
 * A hardware presentation layer is defined that provides a structure
 * definition associated with an MCU-specific address to allow
 * manipulation of standard timer registers.  This abstraction
 * supports timers by type (Timer_A or Timer_B) and instance number
 * within the MCU.
 *
 * The number of capture/compare registers supported by an instance is
 * not an identifying characteristic.  The HPL peripheral register
 * structure provides access to these as both named indexed elements.
 *
 * Instance names uniformly begin with instance zero, even on MCUs
 * where the documented peripheral name does not include an instance.
 * For example, peripheral @c TA2 on the MSP430G2231 is identified as
 * @c T0A, and has two capture/compare registers.
 *
 * This module specifically does not support Timer_D, which has an
 * incompatible peripheral register map at the hardware presentation
 * layer.
 *
 * @author Peter A. Bigot <bigotp@acm.org> @homepage
 * http://github.com/pabigot/freertos-mspgcc @date 2012 @copyright <a
 * href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_TIMER_H
#define BSP430_PERIPH_TIMER_H

/* !BSP430! periph=timer */
/* !BSP430! instance=T0A,T1A,T2A,T3A,T0B,T1B,T2B */

#include <bsp430/periph.h>
#if ! (defined(__MSP430_HAS_TA2__)						\
	   || defined(__MSP430_HAS_TA3__)					\
	   || defined(__MSP430_HAS_TA5__)					\
	   || defined(__MSP430_HAS_T0A3__)					\
	   || defined(__MSP430_HAS_T0A5__)					\
	   )												\
#warning Peripheral not supported by configured MCU
#endif /* has Timer_A */

/** Layout for Timer_A and Timer_B peripherals.
 */
typedef struct xBSP430periphTIMER {
	unsigned int ctl;			/**< Control register (TA0CTL) */
	union {
		unsigned int cctl[7];		/**< Capture/Compare Control registers (indexed) */
		struct {
			unsigned int cctl0;	/**< Tx#CCTL0 */
			unsigned int cctl1;	/**< Tx#CCTL1 */
			unsigned int cctl2;	/**< Tx#CCTL2 */
			unsigned int cctl3;	/**< Tx#CCTL3 */
			unsigned int cctl4;	/**< Tx#CCTL4 */
			unsigned int cctl5; /**< TB#CCTL5 */
			unsigned int cctl6;	/**< TB#CCTL6 */
		};
	};
	unsigned int r;				/**< Raw timer counter (TA0R) */
	union {
		unsigned int ccr[7];	/**< Capture/Compare registers (indexed) */
		struct {
			unsigned int ccr0;	/**< Tx#CCR0 */
			unsigned int ccr1;	/**< Tx#CCR1 */
			unsigned int ccr2;	/**< Tx#CCR2 */
			unsigned int ccr3;	/**< Tx#CCR3 */
			unsigned int ccr4;	/**< Tx#CCR4 */
			unsigned int ccr5;	/**< TB#CCR5 */
			unsigned int ccr6;	/**< TB#CCR6 */
		};
	};		
} xBSP430periphTIMER;

/** @def configBSP430_TIMER_SHARE_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * shared TIMER peripheral HAL interrupt vector.  If this is defined, a
 * single ISR implementation is used, with the individual ISRs
 * invoking it with the instance state and interrupt cause as
 * parameters.
 *
 * This must be available in order to use the HAL interrupt vector for
 * specific device instances such as #configBSP430_PERIPH_T0A_ISR. */
#ifndef configBSP430_TIMER_SHARE_ISR
#define configBSP430_TIMER_SHARE_ISR 1
#endif /* configBSP430_TIMER_SHARE_ISR */

/** Get the peripheral register pointer for a specific timer.
 *
 * @param xHandle The handle identifier, such as #BSP430_PERIPH_T0A.
 *
 * @return A typed pointer that can be used to manipulate the timer.
 * A null pointer is returned if the handle does not correspond to a
 * timer which has been enabled (e.g., with #configBSP430_PERIPH_T0A).
 */
volatile xBSP430periphTIMER * xBSP430periphLookupTIMER (xBSP430periphHandle xHandle);

/** @cond DOXYGEN_INTERNAL
 *
 * What we're doing here is providing the base addresses for the
 * peripheral register maps in a manner that ignores the number of
 * CCRs present in an instance. */ 
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
/* Find the relevant tests with the following command over the
 * upstream include files in the msp430mcu package::

  for i in T0A T1A T2A T3A T0B T1B T2B T3B ; do
     grep -h BASEADDRESS_${i} * \
       | sort \
       | uniq ;
  done

 * Use the alternative with the most CCRs as the default.  If the MCU
 * doesn't support it but a user references the handle, the name will
 * be unresolved at compile time, which is a fair error message.
 */
#if defined(__MSP430_HAS_T0A3__)
#define _BSP430_PERIPH_T0A_BASEADDRESS __MSP430_BASEADDRESS_T0A3__
#else /* T0A */
#define _BSP430_PERIPH_T0A_BASEADDRESS __MSP430_BASEADDRESS_T0A5__
#endif /* T0A */

#if defined(__MSP430_HAS_T1A2__)
#define _BSP430_PERIPH_T1A_BASEADDRESS __MSP430_BASEADDRESS_T1A2__
#else /* T1A */
#define _BSP430_PERIPH_T1A_BASEADDRESS __MSP430_BASEADDRESS_T1A3__
#endif /* T1A */

#if defined(__MSP430_HAS_T2A2__)
#define _BSP430_PERIPH_T2A_BASEADDRESS __MSP430_BASEADDRESS_T2A2__
#else /* T2A */
#define _BSP430_PERIPH_T2A_BASEADDRESS __MSP430_BASEADDRESS_T2A3__
#endif /* T2A */

#if defined(__MSP430_HAS_T3A2__)
#define _BSP430_PERIPH_T3A_BASEADDRESS __MSP430_BASEADDRESS_T3A2__
#else /* T3A */
#define _BSP430_PERIPH_T3A_BASEADDRESS __MSP430_BASEADDRESS_T3A3__
#endif /* T3A */

#if defined(__MSP430_HAS_T0B3__)
#define _BSP430_PERIPH_T0B_BASEADDRESS __MSP430_BASEADDRESS_T0B3__
#else /* T0B */
#define _BSP430_PERIPH_T0B_BASEADDRESS __MSP430_BASEADDRESS_T0B7__
#endif /* T0B */

#define _BSP430_PERIPH_T1B_BASEADDRESS __MSP430_BASEADDRESS_T1B3__
#define _BSP430_PERIPH_T2B_BASEADDRESS __MSP430_BASEADDRESS_T2B3__

#else /* MSP430XV2 */

/* Non-5xx MCU headers don't provide peripheral base addresses.
 * Fortunately, all the variants are at the same address in these
 * families (in 5xx there are a couple chips with different addresses
 * for T0A). */
#define _BSP430_PERIPH_T0A_BASEADDRESS 0x0160
/* In some chips (e.g., msp430g2553) there are two Timer_As, and the
 * second is located at the same address as Timer_B in other chips in
 * the family.  There are no MCUs of this type that have a Timer_B. */
#define _BSP430_PERIPH_T1A_BASEADDRESS 0x0180
#define _BSP430_PERIPH_T0B_BASEADDRESS 0x0180
#endif /* MSP430XV2 */
/** @endcond */ /* DOXYGEN_INTERNAL */

/* !BSP430! insert=hpl_timer */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_timer] */
/** @def configBSP430_PERIPH_T0A
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c T0A peripheral HPL or HAL interface.  If the MCU
 * does not support this timer, a compile-time error will probably
 * be produced. */
#ifndef configBSP430_PERIPH_T0A
#define configBSP430_PERIPH_T0A 0
#endif /* configBSP430_PERIPH_T0A */

/** @def configBSP430_PERIPH_T0A_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c T0A.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_TIMER_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_T0A_ISR
#define configBSP430_PERIPH_T0A_ISR 1
#endif /* configBSP430_PERIPH_T0A_ISR */

/** Handle for the raw T0A device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_T0A is defined to a true
 * value. */
#define BSP430_PERIPH_T0A ((xBSP430periphHandle)(__MSP430_BASEADDRESS_T0A__))

/** @def configBSP430_PERIPH_T1A
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c T1A peripheral HPL or HAL interface.  If the MCU
 * does not support this timer, a compile-time error will probably
 * be produced. */
#ifndef configBSP430_PERIPH_T1A
#define configBSP430_PERIPH_T1A 0
#endif /* configBSP430_PERIPH_T1A */

/** @def configBSP430_PERIPH_T1A_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c T1A.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_TIMER_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_T1A_ISR
#define configBSP430_PERIPH_T1A_ISR 1
#endif /* configBSP430_PERIPH_T1A_ISR */

/** Handle for the raw T1A device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_T1A is defined to a true
 * value. */
#define BSP430_PERIPH_T1A ((xBSP430periphHandle)(__MSP430_BASEADDRESS_T1A__))

/** @def configBSP430_PERIPH_T2A
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c T2A peripheral HPL or HAL interface.  If the MCU
 * does not support this timer, a compile-time error will probably
 * be produced. */
#ifndef configBSP430_PERIPH_T2A
#define configBSP430_PERIPH_T2A 0
#endif /* configBSP430_PERIPH_T2A */

/** @def configBSP430_PERIPH_T2A_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c T2A.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_TIMER_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_T2A_ISR
#define configBSP430_PERIPH_T2A_ISR 1
#endif /* configBSP430_PERIPH_T2A_ISR */

/** Handle for the raw T2A device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_T2A is defined to a true
 * value. */
#define BSP430_PERIPH_T2A ((xBSP430periphHandle)(__MSP430_BASEADDRESS_T2A__))

/** @def configBSP430_PERIPH_T3A
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c T3A peripheral HPL or HAL interface.  If the MCU
 * does not support this timer, a compile-time error will probably
 * be produced. */
#ifndef configBSP430_PERIPH_T3A
#define configBSP430_PERIPH_T3A 0
#endif /* configBSP430_PERIPH_T3A */

/** @def configBSP430_PERIPH_T3A_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c T3A.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_TIMER_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_T3A_ISR
#define configBSP430_PERIPH_T3A_ISR 1
#endif /* configBSP430_PERIPH_T3A_ISR */

/** Handle for the raw T3A device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_T3A is defined to a true
 * value. */
#define BSP430_PERIPH_T3A ((xBSP430periphHandle)(__MSP430_BASEADDRESS_T3A__))

/** @def configBSP430_PERIPH_T0B
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c T0B peripheral HPL or HAL interface.  If the MCU
 * does not support this timer, a compile-time error will probably
 * be produced. */
#ifndef configBSP430_PERIPH_T0B
#define configBSP430_PERIPH_T0B 0
#endif /* configBSP430_PERIPH_T0B */

/** @def configBSP430_PERIPH_T0B_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c T0B.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_TIMER_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_T0B_ISR
#define configBSP430_PERIPH_T0B_ISR 1
#endif /* configBSP430_PERIPH_T0B_ISR */

/** Handle for the raw T0B device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_T0B is defined to a true
 * value. */
#define BSP430_PERIPH_T0B ((xBSP430periphHandle)(__MSP430_BASEADDRESS_T0B__))

/** @def configBSP430_PERIPH_T1B
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c T1B peripheral HPL or HAL interface.  If the MCU
 * does not support this timer, a compile-time error will probably
 * be produced. */
#ifndef configBSP430_PERIPH_T1B
#define configBSP430_PERIPH_T1B 0
#endif /* configBSP430_PERIPH_T1B */

/** @def configBSP430_PERIPH_T1B_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c T1B.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_TIMER_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_T1B_ISR
#define configBSP430_PERIPH_T1B_ISR 1
#endif /* configBSP430_PERIPH_T1B_ISR */

/** Handle for the raw T1B device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_T1B is defined to a true
 * value. */
#define BSP430_PERIPH_T1B ((xBSP430periphHandle)(__MSP430_BASEADDRESS_T1B__))

/** @def configBSP430_PERIPH_T2B
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c T2B peripheral HPL or HAL interface.  If the MCU
 * does not support this timer, a compile-time error will probably
 * be produced. */
#ifndef configBSP430_PERIPH_T2B
#define configBSP430_PERIPH_T2B 0
#endif /* configBSP430_PERIPH_T2B */

/** @def configBSP430_PERIPH_T2B_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c T2B.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself.
 *
 * @c #configBSP430_TIMER_SHARE_ISR must be enabled for this to be
 * enabled. */
#ifndef configBSP430_PERIPH_T2B_ISR
#define configBSP430_PERIPH_T2B_ISR 1
#endif /* configBSP430_PERIPH_T2B_ISR */

/** Handle for the raw T2B device.
 *
 * The handle may be referenced only if
 * #configBSP430_PERIPH_T2B is defined to a true
 * value. */
#define BSP430_PERIPH_T2B ((xBSP430periphHandle)(__MSP430_BASEADDRESS_T2B__))

/* END AUTOMATICALLY GENERATED CODE [hpl_timer] */
/* !BSP430! end=hpl_timer */

#endif /* BSP430_PERIPH_TIMER_H */
