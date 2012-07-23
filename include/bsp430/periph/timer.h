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
 * @brief Hardware presentation/abstraction for generic timers (Timer_A/Timer_B)
 *
 * A hardware presentation layer is defined that provides a structure
 * definition associated with an MCU-specific address to allow
 * manipulation of standard timer registers.  This abstraction
 * supports timers by type (Timer_A or Timer_B) and instance number
 * within the MCU.
 *
 * The number of capture/compare registers supported by an instance is
 * not an identifying characteristic.  The HPL peripheral register
 * structure provides access to these as both named and indexed
 * elements.
 *
 * The original nomenclature for MSP430 timers in MSP430 has proven to
 * be inconsistent as the number of instances grew.  While the
 * identifier @c TA3 in early MCUs specified the presence of a Timer_A
 * instance with 3 CCRs, this led to the need to denote a second
 * Timer_A instance with 5 CCRs as T1A5 within the MCU-specific
 * headers.  The data sheets, however, refer to that same instance as
 * TA1, with a dot-separated CCR number where necessary (e.g., TA1.4
 * refers to the fifth CCR on the second Timer_A instance, and is
 * controlled via TA1CCTL4).
 *
 * For this interface, instance names uniformly begin with instance
 * zero, even on MCUs where the documented peripheral name does not
 * include an instance.  For example, peripheral called @c TA2 on the
 * MSP430G2231 is simply the first Timer_A instance and has 2 CCRs.
 * It is identified here as @c TA0, with the number of CCRs left
 * implicit.
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
/* !BSP430! instance=TA0,TA1,TA2,TA3,TB0,TB1,TB2 */

#include <bsp430/periph.h>
/* Surprisingly, not every MCU has a Timer_A.  Some have only a Basic
 * Timer (BT). */
#if ! (defined(__MSP430_HAS_TA2__)						\
	   || defined(__MSP430_HAS_TA3__)					\
	   || defined(__MSP430_HAS_TA5__)					\
	   || defined(__MSP430_HAS_T0A3__)					\
	   || defined(__MSP430_HAS_T0A5__)					\
	   )
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

/** Get the peripheral register pointer for a specific timer.
 *
 * @param xHandle The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return A typed pointer that can be used to manipulate the timer.
 * A null pointer is returned if the handle does not correspond to a
 * timer which has been enabled (e.g., with #configBSP430_PERIPH_TA0).
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

  for i in TA0 TA1 TA2 TA3 TB0 TB1 TB2 TB3 ; do
     grep -h BASEADDRESS_${i} * \
       | sort \
       | uniq ;
  done

 * Use the alternative with the most CCRs as the default.  If the MCU
 * doesn't support it but a user references the handle, the name will
 * be unresolved at compile time, which is a fair error message.
 */
#if defined(__MSP430_HAS_T0A3__)
#define _BSP430_PERIPH_TA0_BASEADDRESS __MSP430_BASEADDRESS_T0A3__
#else /* TA0 */
#define _BSP430_PERIPH_TA0_BASEADDRESS __MSP430_BASEADDRESS_T0A5__
#endif /* TA0 */

#if defined(__MSP430_HAS_T1A2__)
#define _BSP430_PERIPH_TA1_BASEADDRESS __MSP430_BASEADDRESS_T1A2__
#else /* TA1 */
#define _BSP430_PERIPH_TA1_BASEADDRESS __MSP430_BASEADDRESS_T1A3__
#endif /* TA1 */

#if defined(__MSP430_HAS_T2A2__)
#define _BSP430_PERIPH_TA2_BASEADDRESS __MSP430_BASEADDRESS_T2A2__
#else /* TA2 */
#define _BSP430_PERIPH_TA2_BASEADDRESS __MSP430_BASEADDRESS_T2A3__
#endif /* TA2 */

#if defined(__MSP430_HAS_T3A2__)
#define _BSP430_PERIPH_TA3_BASEADDRESS __MSP430_BASEADDRESS_T3A2__
#else /* TA3 */
#define _BSP430_PERIPH_TA3_BASEADDRESS __MSP430_BASEADDRESS_T3A3__
#endif /* TA3 */

#if defined(__MSP430_HAS_T0B3__)
#define _BSP430_PERIPH_TB0_BASEADDRESS __MSP430_BASEADDRESS_T0B3__
#else /* TB0 */
#define _BSP430_PERIPH_TB0_BASEADDRESS __MSP430_BASEADDRESS_T0B7__
#endif /* TB0 */

#define _BSP430_PERIPH_TB1_BASEADDRESS __MSP430_BASEADDRESS_T1B3__
#define _BSP430_PERIPH_TB2_BASEADDRESS __MSP430_BASEADDRESS_T2B3__

#else /* MSP430XV2 */

/* Non-5xx MCU headers don't provide peripheral base addresses.
 * Fortunately, all the variants are at the same address in these
 * families (in 5xx there are a couple chips with different addresses
 * for TA0). */
#define _BSP430_PERIPH_TA0_BASEADDRESS 0x0160
/* In some chips (e.g., msp430g2553) there are two Timer_As, and the
 * second is located at the same address as Timer_B in other chips in
 * the family.  There are no MCUs of this type that have a Timer_B. */
#define _BSP430_PERIPH_TA1_BASEADDRESS 0x0180
#define _BSP430_PERIPH_TB0_BASEADDRESS 0x0180
#endif /* MSP430XV2 */
/** @endcond */ /* DOXYGEN_INTERNAL */

/* Forward declaration.  Structure declaration provided in
 * <bsp430/timer_.h>. */
struct xBSP430timerState;

/** The timer internal state is protected. */
typedef struct xBSP430timerState * xBSP430timerHandle;

/* !BSP430! insert=hpl_ba_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_ba_decl] */
/** @def configBSP430_PERIPH_TA0
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c TA0 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_TA0__) */
#ifndef configBSP430_PERIPH_TA0
#define configBSP430_PERIPH_TA0 0
#endif /* configBSP430_PERIPH_TA0 */

/** Handle for the raw TA0 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_TA0
 * is defined to a true value. */
#define BSP430_PERIPH_TA0 ((xBSP430periphHandle)(_BSP430_PERIPH_TA0_BASEADDRESS))

/** Pointer to the peripheral register map for TA0.
 *
 * The pointer may be used only if #configBSP430_PERIPH_TA0
 * is defined to a true value. */
extern volatile xBSP430periphTIMER * const xBSP430periph_TA0;

/** @def configBSP430_PERIPH_TA1
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c TA1 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_TA1__) */
#ifndef configBSP430_PERIPH_TA1
#define configBSP430_PERIPH_TA1 0
#endif /* configBSP430_PERIPH_TA1 */

/** Handle for the raw TA1 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_TA1
 * is defined to a true value. */
#define BSP430_PERIPH_TA1 ((xBSP430periphHandle)(_BSP430_PERIPH_TA1_BASEADDRESS))

/** Pointer to the peripheral register map for TA1.
 *
 * The pointer may be used only if #configBSP430_PERIPH_TA1
 * is defined to a true value. */
extern volatile xBSP430periphTIMER * const xBSP430periph_TA1;

/** @def configBSP430_PERIPH_TA2
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c TA2 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_TA2__) */
#ifndef configBSP430_PERIPH_TA2
#define configBSP430_PERIPH_TA2 0
#endif /* configBSP430_PERIPH_TA2 */

/** Handle for the raw TA2 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_TA2
 * is defined to a true value. */
#define BSP430_PERIPH_TA2 ((xBSP430periphHandle)(_BSP430_PERIPH_TA2_BASEADDRESS))

/** Pointer to the peripheral register map for TA2.
 *
 * The pointer may be used only if #configBSP430_PERIPH_TA2
 * is defined to a true value. */
extern volatile xBSP430periphTIMER * const xBSP430periph_TA2;

/** @def configBSP430_PERIPH_TA3
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c TA3 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_TA3__) */
#ifndef configBSP430_PERIPH_TA3
#define configBSP430_PERIPH_TA3 0
#endif /* configBSP430_PERIPH_TA3 */

/** Handle for the raw TA3 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_TA3
 * is defined to a true value. */
#define BSP430_PERIPH_TA3 ((xBSP430periphHandle)(_BSP430_PERIPH_TA3_BASEADDRESS))

/** Pointer to the peripheral register map for TA3.
 *
 * The pointer may be used only if #configBSP430_PERIPH_TA3
 * is defined to a true value. */
extern volatile xBSP430periphTIMER * const xBSP430periph_TA3;

/** @def configBSP430_PERIPH_TB0
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c TB0 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_TB0__) */
#ifndef configBSP430_PERIPH_TB0
#define configBSP430_PERIPH_TB0 0
#endif /* configBSP430_PERIPH_TB0 */

/** Handle for the raw TB0 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_TB0
 * is defined to a true value. */
#define BSP430_PERIPH_TB0 ((xBSP430periphHandle)(_BSP430_PERIPH_TB0_BASEADDRESS))

/** Pointer to the peripheral register map for TB0.
 *
 * The pointer may be used only if #configBSP430_PERIPH_TB0
 * is defined to a true value. */
extern volatile xBSP430periphTIMER * const xBSP430periph_TB0;

/** @def configBSP430_PERIPH_TB1
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c TB1 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_TB1__) */
#ifndef configBSP430_PERIPH_TB1
#define configBSP430_PERIPH_TB1 0
#endif /* configBSP430_PERIPH_TB1 */

/** Handle for the raw TB1 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_TB1
 * is defined to a true value. */
#define BSP430_PERIPH_TB1 ((xBSP430periphHandle)(_BSP430_PERIPH_TB1_BASEADDRESS))

/** Pointer to the peripheral register map for TB1.
 *
 * The pointer may be used only if #configBSP430_PERIPH_TB1
 * is defined to a true value. */
extern volatile xBSP430periphTIMER * const xBSP430periph_TB1;

/** @def configBSP430_PERIPH_TB2
 *
 * Define to a true value in @c FreeRTOSConfig.h to enable use of the
 * @c TB2 peripheral HPL or HAL interface.  Only do this if the MCU
 * supports this device (check @c __MSP430_HAS_TB2__) */
#ifndef configBSP430_PERIPH_TB2
#define configBSP430_PERIPH_TB2 0
#endif /* configBSP430_PERIPH_TB2 */

/** Handle for the raw TB2 device.
 *
 * The handle may be used only if #configBSP430_PERIPH_TB2
 * is defined to a true value. */
#define BSP430_PERIPH_TB2 ((xBSP430periphHandle)(_BSP430_PERIPH_TB2_BASEADDRESS))

/** Pointer to the peripheral register map for TB2.
 *
 * The pointer may be used only if #configBSP430_PERIPH_TB2
 * is defined to a true value. */
extern volatile xBSP430periphTIMER * const xBSP430periph_TB2;

/* END AUTOMATICALLY GENERATED CODE [hpl_ba_decl] */
/* !BSP430! end=hpl_ba_decl */

/* !BSP430! insert=hal_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_isr_decl] */
/** @def configBSP430_HAL_TA0_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c TA0.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself. */
#ifndef configBSP430_HAL_TA0_ISR
#define configBSP430_HAL_TA0_ISR 1
#endif /* configBSP430_HAL_TA0_ISR */

/** @def configBSP430_HAL_TA1_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c TA1.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself. */
#ifndef configBSP430_HAL_TA1_ISR
#define configBSP430_HAL_TA1_ISR 1
#endif /* configBSP430_HAL_TA1_ISR */

/** @def configBSP430_HAL_TA2_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c TA2.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself. */
#ifndef configBSP430_HAL_TA2_ISR
#define configBSP430_HAL_TA2_ISR 1
#endif /* configBSP430_HAL_TA2_ISR */

/** @def configBSP430_HAL_TA3_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c TA3.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself. */
#ifndef configBSP430_HAL_TA3_ISR
#define configBSP430_HAL_TA3_ISR 1
#endif /* configBSP430_HAL_TA3_ISR */

/** @def configBSP430_HAL_TB0_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c TB0.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself. */
#ifndef configBSP430_HAL_TB0_ISR
#define configBSP430_HAL_TB0_ISR 1
#endif /* configBSP430_HAL_TB0_ISR */

/** @def configBSP430_HAL_TB1_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c TB1.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself. */
#ifndef configBSP430_HAL_TB1_ISR
#define configBSP430_HAL_TB1_ISR 1
#endif /* configBSP430_HAL_TB1_ISR */

/** @def configBSP430_HAL_TB2_ISR
 *
 * Define to a true value in @c FreeRTOSConfig.h to use the BSP430 HAL
 * interrupt vector for @c TB2.  Define to a false value if you
 * need complete control over how interrupts are handled for the device
 * and will be defining the vector yourself. */
#ifndef configBSP430_HAL_TB2_ISR
#define configBSP430_HAL_TB2_ISR 1
#endif /* configBSP430_HAL_TB2_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_isr_decl] */
/* !BSP430! end=hal_isr_decl */

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** FreeRTOS HAL handle for TA0.
 *
 * The handle may be used only if #configBSP430_PERIPH_TA0
 * is defined to a true value. */
extern xBSP430timerHandle const xBSP430timer_TA0;

/** FreeRTOS HAL handle for TA1.
 *
 * The handle may be used only if #configBSP430_PERIPH_TA1
 * is defined to a true value. */
extern xBSP430timerHandle const xBSP430timer_TA1;

/** FreeRTOS HAL handle for TA2.
 *
 * The handle may be used only if #configBSP430_PERIPH_TA2
 * is defined to a true value. */
extern xBSP430timerHandle const xBSP430timer_TA2;

/** FreeRTOS HAL handle for TA3.
 *
 * The handle may be used only if #configBSP430_PERIPH_TA3
 * is defined to a true value. */
extern xBSP430timerHandle const xBSP430timer_TA3;

/** FreeRTOS HAL handle for TB0.
 *
 * The handle may be used only if #configBSP430_PERIPH_TB0
 * is defined to a true value. */
extern xBSP430timerHandle const xBSP430timer_TB0;

/** FreeRTOS HAL handle for TB1.
 *
 * The handle may be used only if #configBSP430_PERIPH_TB1
 * is defined to a true value. */
extern xBSP430timerHandle const xBSP430timer_TB1;

/** FreeRTOS HAL handle for TB2.
 *
 * The handle may be used only if #configBSP430_PERIPH_TB2
 * is defined to a true value. */
extern xBSP430timerHandle const xBSP430timer_TB2;

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

#endif /* BSP430_PERIPH_TIMER_H */
