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
 * Timer functionality exists on most MSP430 MCUs.  This module
 * supports both Timer_A and Timer_B, but does not provide any
 * distinction between them.  At this time, Timer_D is not supported
 * by BSP430.
 *
 * Conventional peripheral handles are #BSP430_PERIPH_TA0 and
 * #BSP430_PERIPH_TB0.  The handles are available only when the
 * corresponding @hpl is requested.  The number of capture/compare
 * blocks supported by the MCU is not an identifying characteristic of
 * the peripheral instance.
 *
 * @note The original nomenclature for MSP430 timers in MSP430 has
 * proven to be inconsistent as the number of instances grew.  While
 * the identifier @c TA3 in early MCUs specified the presence of a
 * Timer_A instance with 3 CCs, this led to the need to denote a
 * second Timer_A instance with 5 CCs as T1A5 within the MCU-specific
 * headers.  The data sheets, however, refer to that same instance as
 * TA1, with a dot-separated CC number where necessary (e.g., TA1.4
 * refers to the fifth CC on the second Timer_A instance, and is
 * controlled via TA1CCTL4).
 *
 * @note In BSP430 timer instance names uniformly begin with instance
 * zero, even on MCUs where the documented peripheral name does not
 * include an instance.  For example, peripheral called @c TA2 on the
 * MSP430G2231 is simply the first Timer_A instance and has 2 CCs.  It
 * is identified here as @c TA0, with the number of CCs left implicit.
 *
 * @section h_periph_timer_opt Module Configuration Options
 *
 * @li #configBSP430_HPL_TA0 to enable the HPL handle declarations
 *
 * @li #configBSP430_HAL_TA0 to enable the HAL infrastructure
 *
 * @li #configBSP430_HAL_TA0_ISR to enable the HAL ISR for
 * overflow and capture/compare blocks 1 and higher
 *
 * @li #configBSP430_HAL_TA0_CC0_ISR to enable the HAL ISR for
 * capture/compare block 0
 *
 * @li #configBSP430_TIMER_CCACLK to request a platform-specific clock
 * with certain capture/compare capabilities be identified
 *
 * Substitute other instance names (e.g., @b TB1) as necessary.
 *
 * @section h_periph_timer_hpl Hardware Presentation Layer
 *
 * Though Timer_A and Timer_B have distinct capabilities, their
 * underlying register maps are compatible and #sBSP430hplTIMER is
 * used for both.  The structure includes space for all eight
 * potential capture/compare registers, but only the ones supported by
 * the device should be accessed.
 *
 * @section h_periph_timer_hal Hardware Adaptation Layer
 *
 * The timer @hal uses the sBSP430halTIMER structure.
 *
 * Enabling #configBSP430_HAL_TA0 also enables
 * #configBSP430_HAL_TA0_ISR unless previously disabled.  When this
 * primary ISR is enabled an interrupt service routine is provided
 * supporting interrupts related to timer overflow and capture/compare
 * events in CC blocks 1 and higher.  The corresponding interrupts
 * must be enabled by the application.  On an overflow interrupt, the
 * sBSP430halTIMER.overflow_count is incremented and the
 * sBSP430halTIMER.overflow_callback callback chain is invoked.  On a
 * capture/compare interrupt, the corresponding chain from
 * sBSP430halTIMER.cc_callback is invoked.  (Note that the index in
 * this array is the CC number.)
 *
 * The secondary ISR controlled by #configBSP430_HAL_TA0_CC0_ISR is
 * left disabled unless explicitly enabled.  If enabled, an interrupt
 * service routine is provided supporting capture/compare interrupts
 * related to CC0.  The first chain in the sBSP430halTIMER.cc_callback
 * array is invoked by this ISR.  The interrupt enable bit is
 * controlled by the application.
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

/** @def BSP430_MODULE_TIMER_A
 *
 * Defined on inclusion of <bsp430/periph/timer.h>.  The value
 * evaluates to true if the target MCU supports the Timer_A Module,
 * and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_TIMER_A (defined(__MSP430_HAS_TA2__)      \
                               || defined(__MSP430_HAS_TA3__)   \
                               || defined(__MSP430_HAS_TA5__)   \
                               || defined(__MSP430_HAS_T0A3__)  \
                               || defined(__MSP430_HAS_T0A5__))

/** @def BSP430_MODULE_TIMER_B
 *
 * Defined on inclusion of <bsp430/periph/timer.h>.  The value
 * evaluates to true if the target MCU supports the Timer_B Module,
 * and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_TIMER_B (defined(__MSP430_HAS_TB3__)      \
                               || defined(__MSP430_HAS_TB7__)   \
                               || defined(__MSP430_HAS_T0B3__)  \
                               || defined(__MSP430_HAS_T0B7__))


/** @def BSP430_MODULE_TIMER
 *
 * Defined on inclusion of <bsp430/periph/timer.h>.  The value
 * evaluates to true if the target MCU supports the Timer_A or
 * Timer_B, and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_TIMER (BSP430_MODULE_TIMER_A || BSP430_MODULE_TIMER_B)

/* Surprisingly, not every MCU has a Timer_A.  Some have only a Basic
 * Timer (BT). */
#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_TIMER - 0)

/** @def configBSP430_TIMER_CCACLK
 *
 * Define to true to indicate that the application or infrastructure
 * would like to use a timer that supports using ACLK as a
 * capture/compare input signal.  The timer is identified by
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE, and that macro is defined only
 * if such a timer is available on the platform.
 *
 * This timer is used by selecting an alternative clock source, then
 * using the ACLK signal to capture the number of pulses on that
 * source over a known period.  For example, this is used to measure
 * the relative speed of SMCLK and ACLK when manually trimming the FLL
 * in iBSP430ucsTrimDCOCLKDIV_ni().
 *
 * @note Where possible, a timer other than TA0 should be used, as TA0
 * is the default for the uptime facility and other things that use an
 * ACLK-sourced counter.  If this cannot be satisfied, the timer
 * should still be made available but #BSP430_TIMER_CCACLK_IS_TA0
 * should be defined to a true value.
 *
 * @cppflag
 * @affects #BSP430_TIMER_CCACLK
 * @defaulted */
#ifndef configBSP430_TIMER_CCACLK
#define configBSP430_TIMER_CCACLK 0
#endif /* configBSP430_TIMER_CCACLK */

/** @def configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE
 *
 * Define to a true value to use the default (platform-specific)
 * CCACLK timer.  This is true by default if
 * #configBSP430_TIMER_CCACLK is true.  If it remains true, the HPL
 * resources corresponding to #BSP430_TIMER_CCACLK_PERIPH_HANDLE and
 * #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE will also be enabled
 * by default.
 *
 * If you want to override the default, define this to a false value
 * and provide definitions for:
 * <ul>
 * <li>#BSP430_TIMER_CCACLK_PERIPH_HANDLE
 * <li>#BSP430_TIMER_CCACLK_IS_TA0
 * <li>#BSP430_TIMER_CCACLK_CC_INDEX
 * <li>#BSP430_TIMER_CCACLK_CCIS
 * <li>#BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
 * <li>#BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT1
 * </ul>
 *
 * Also be sure you enable the HAL or HPL resources for the timer and
 * the port.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE
#define configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE (configBSP430_TIMER_CCACLK - 0)
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE */

/** @def configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL
 *
 * Define to a true value to automatically request the HAL support for
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE when the latter is defaulted.
 * When false, only the HPL support for the peripheral is enabled.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL
#define configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL 0
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL */

/** @def configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL
 *
 * Define to a true value to automatically request the HAL support for
 * #BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE when the latter is
 * defaulted.  When false, only the HPL support for the peripheral is
 * enabled.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL
#define configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL 0
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL */

/** @def BSP430_TIMER_CCACLK
 *
 * Defined to a true value if ACLK-triggered timer captures are
 * supported using the platform-specific values of:
 *
 * <ul>
 * <li>#BSP430_TIMER_CCACLK_PERIPH_HANDLE
 * <li>#BSP430_TIMER_CCACLK_IS_TA0
 * <li>#BSP430_TIMER_CCACLK_CC_INDEX
 * <li>#BSP430_TIMER_CCACLK_CCIS
 * </ul>
 *
 * The values above should not be referenced if this macro is
 * undefined or has a false value.
 *
 * @note It is recommended that application code use of this feature
 * be done in a non-interruptible context that is run-to-completion,
 * to be sure that no other user is active.  The configuration of the
 * timer as a whole should be considered volatile.  Proper use
 * requires initializing the timer before use and stopping it when
 * finished.
 *
 * @warning If ACLK is configured to source from XT1CLK and the
 * crystal is faulted resulting in an internal fallback to VLOCLK, the
 * capture/compare input remains the LFXT1 signal and no events will
 * occur.  This can be demonstrated on the EXP430G2 LaunchPad.
 *
 * @dependency #configBSP430_TIMER_CCACLK
 * @platformdefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_IS_TA0
 *
 * Defined to a true value by default or external configuration when
 * the timer identified by #BSP430_TIMER_CCACLK_PERIPH_HANDLE is
 * #BSP430_PERIPH_TA0.
 *
 * Applications making use of #BSP430_TIMER_CCACLK_PERIPH_HANDLE in a
 * context where TA0 might be used for other purposes (such as a
 * persistent system uptime) should check this value to determine
 * whether it is safe to reconfigure the timer.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @platformdefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_IS_TA0 include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_PERIPH_HANDLE
 *
 * The peripheral handle for a timer that is capable of using ACLK as
 * a capture/compare input.  Preferably this would not be TA0.  The
 * intended use of this timer is to measure pulses of some fast clock
 * (SMCLK or an external clock) against ACLK.
 *
 * See #configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE if you intend
 * to override the platform default.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @platformdefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_CC_INDEX
 *
 * The capture/compare block index within
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE that can use ACLK as an input
 * signal.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @platformdefault  */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CC_INDEX include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_CCIS
 *
 * The capture/compare block input signal bits to be selected in the
 * control word for CC block #BSP430_TIMER_CCACLK_CC_INDEX of
 * #BSP430_TIMER_CCACLK_PERIPH_HANDLE to use ACLK as an input signal.
 *
 * @dependency #BSP430_TIMER_CCACLK
 * @platformdefault  */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CCIS include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE
 *
 * The BSP430 port peripheral on which the external clock source for
 * BSP430_TIMER_CCACLK_PERIPH_HANDLE can be found.
 *
 * @note If #configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE is true,
 * the corresponding HAL interface will default to enabled. */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_TIMER_CCACLK_CLK_PORT_BIT
 *
 * The pin on BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE to which the
 * external clock source for BSP430_TIMER_CCACLK_PERIPH_HANDLE can be
 * provided. */
#if defined(BSP430_DOXYGEN)
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** Count number of timer transitions over a span of ACLK ticks
 *
 * This function uses the timer capture/compare infrastructure to
 * count clock cycles.  The source for a timer is configured
 * externally (for example, to SMCLK, or to an external source).  The
 * CCACLK capture/control block is configured based on @a
 * capture_mode.  The code synchronizes with a capture, then loops for
 * @a period captures.  The CCACLK timer counter is recorded at the
 * start of the first and end of the last cycle.  The capture/control
 * block control is reset, and the difference between first and last
 * counters is returned.
 *
 * The expectation is that the events triggered by the input selection
 * are slow relative to the MCLK and timer sources.  If the timer is
 * slower than the input, the use of the #SCS flag in the
 * implementation will result in the function returning @a count.
 *
 * @note The function does not modify the timer-level configuration;
 * the timer source must be assigned and the timer started prior to
 * invoking this function.
 *
 * @param periph the peripheral identifier for the timer on which the
 * capture is to be made.
 *
 * @param ccidx the capture/compare block index to use.  No validity
 * checks on this are made; you must be sure the index is in range for
 * the provided timer.
 *
 * @param capture_mode the edge detection capture specification.  The
 * appropriate value is given by constants in the <msp430.h> header,
 * such as #CM_1.  The passed value is masked to eliminate extraneous
 * bits; if the resulting value does not identify a capture, the
 * function returns immediately with a value of -1.
 *
 * @param ccis the capture/compare input selection.  The appropriate
 * value is a constant from the <msp430.h> header, such as #CCIS_1.
 * Consult the MCU-specific datasheet to determine the functions of
 * the input alternatives for each timer/CC-block pair.
 *
 * @param count the number of capture events over which the delta is
 * measured.
 *
 * @return -1 if @a capture_mode is not valid or the timer is
 * unrecognized or stopped.  Otherwise the delta in the counter of the
 * timer over @a count captures. */
unsigned int uiBSP430timerCaptureDelta_ni (tBSP430periphHandle periph,
    int ccidx,
    unsigned int capture_mode,
    unsigned int ccis,
    unsigned int count);

/** Layout for Timer_A and Timer_B peripherals.
 */
typedef struct sBSP430hplTIMER {
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
} sBSP430hplTIMER;

/** @cond DOXYGEN_INTERNAL
 *
 * What we're doing here is providing the base addresses for the
 * peripheral register maps in a manner that ignores the number of
 * CCs present in an instance. */
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
/* Find the relevant tests with the following command over the
 * upstream include files in the msp430mcu package::

  for i in TA T0A T1A T2A T3A TB T0B T1B T2B T3B ; do
     grep -h BASEADDRESS_${i} * \
       | sort \
       | uniq ;
  done

 * Use the alternative with the most CCs as the default.  If the MCU
 * doesn't support it but a user references the handle, the name will
 * be unresolved at compile time, which is a fair error message.
 */
#if defined(__MSP430_HAS_T0A3__)
#define BSP430_PERIPH_TA0_BASEADDRESS_ __MSP430_BASEADDRESS_T0A3__
#else /* TA0 */
#define BSP430_PERIPH_TA0_BASEADDRESS_ __MSP430_BASEADDRESS_T0A5__
#endif /* TA0 */

#if defined(__MSP430_HAS_T1A2__)
#define BSP430_PERIPH_TA1_BASEADDRESS_ __MSP430_BASEADDRESS_T1A2__
#else /* TA1 */
#define BSP430_PERIPH_TA1_BASEADDRESS_ __MSP430_BASEADDRESS_T1A3__
#endif /* TA1 */

#if defined(__MSP430_HAS_T2A2__)
#define BSP430_PERIPH_TA2_BASEADDRESS_ __MSP430_BASEADDRESS_T2A2__
#else /* TA2 */
#define BSP430_PERIPH_TA2_BASEADDRESS_ __MSP430_BASEADDRESS_T2A3__
#endif /* TA2 */

#if defined(__MSP430_HAS_T3A2__)
#define BSP430_PERIPH_TA3_BASEADDRESS_ __MSP430_BASEADDRESS_T3A2__
#else /* TA3 */
#define BSP430_PERIPH_TA3_BASEADDRESS_ __MSP430_BASEADDRESS_T3A3__
#endif /* TA3 */

#if defined(__MSP430_HAS_T0B3__)
#define BSP430_PERIPH_TB0_BASEADDRESS_ __MSP430_BASEADDRESS_T0B3__
#else /* TB0 */
#define BSP430_PERIPH_TB0_BASEADDRESS_ __MSP430_BASEADDRESS_T0B7__
#endif /* TB0 */

#define BSP430_PERIPH_TB1_BASEADDRESS_ __MSP430_BASEADDRESS_T1B3__
#define BSP430_PERIPH_TB2_BASEADDRESS_ __MSP430_BASEADDRESS_T2B3__

#else /* MSP430XV2 */

/* Non-5xx MCU headers don't provide peripheral base addresses.
 * Fortunately, all the variants are at the same address in these
 * families (in 5xx there are a couple chips with different addresses
 * for TA0). */
#define BSP430_PERIPH_TA0_BASEADDRESS_ 0x0160
/* In some chips (e.g., msp430g2553) there are two Timer_As, and the
 * second is located at the same address as Timer_B in other chips in
 * the family.  There are no MCUs of this type that have a Timer_B. */
#define BSP430_PERIPH_TA1_BASEADDRESS_ 0x0180
#define BSP430_PERIPH_TB0_BASEADDRESS_ 0x0180
#endif /* MSP430XV2 */
/** @endcond */ /* DOXYGEN_INTERNAL */

/** Get the HPL handle for a specific timer.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return A typed pointer that can be used to manipulate the timer.
 * A null pointer is returned if the handle does not correspond to a
 * timer for which the HPL interface been enabled (e.g., with
 * #configBSP430_HPL_TA0).
 */
volatile sBSP430hplTIMER * xBSP430hplLookupTIMER (tBSP430periphHandle periph);

/** Field value for variant stored in sBSP430halTIMER.hal_state.cflags
 * when HPL reference is to an #sBSP430hplTIMER. */
#define BSP430_TIMER_HAL_HPL_VARIANT_TIMER 1

/** Structure holding hardware abstraction layer state for Timer_A and
 * Timer_B. */
typedef struct sBSP430halTIMER {
  /** Common header used to extract the correct HPL pointer type from
   * the hpl union. */
  sBSP430hplHALStatePrefix hal_state;

  /** The underlying timer peripheral register structure */
  volatile sBSP430hplTIMER * const hpl;

  /** The number of times the timer has wrapped.
   *
   * The value is maintained only if the corresponding HAL ISR is
   * enabled for the timer and the overflow interrupt is explicitly
   * enabled by the application.
   *
   * @note This field is not marked volatile because doing so costs
   * several extra instructions due to it being a multi-word value.
   * It should be read and written only when interrupts are
   * disabled. */
  unsigned long overflow_count;

  /** The callback chain to invoke when an overflow interrupt is
   * received. */
  const struct sBSP430halISRCallbackVoid * overflow_callback;

  /** The callback chain to invoke when a CCx interrupt is received.
   *
   * The chains are independent for each capture/compare block, but
   * the block index is passed into the chain so that a common handler
   * can be invoked if desired.  The chain for CC0 is accessed only if
   * the corresponding ISR is enabled (e.g.,
   * #configBSP430_HAL_TA0_CC0_ISR) */
  const struct sBSP430halISRCallbackIndexed * * const cc_callback;
} sBSP430halTIMER;

/** The timer internal state is protected. */
typedef struct sBSP430halTIMER * hBSP430halTIMER;

/** Get the HAL handle for a specific timer.
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return the HAL handle for the timer.  A null pointer is returned
 * if the handle does not correspond to a timer for which the HAL
 * interface has been enabled (e.g., with #configBSP430_HAL_TA0).
 */
hBSP430halTIMER hBSP430timerLookup (tBSP430periphHandle periph);

/** Get a human-readable identifier for the timer
 *
 * @param periph The handle identifier, such as #BSP430_PERIPH_TA0.
 *
 * @return The name of the timer, e.g. "TA0".  If the peripheral is
 * not recognized as a timer, a null pointer is returned.
 */
const char * xBSP430timerName (tBSP430periphHandle periph);

/** Provide the frequency of the timer source, if that can be determined.
 *
 * @param timer The timer for which the source frequency is desired
 *
 * @return 0 if the timer is stopped; -1 if the frequency cannot be
 * determined, a positive value for a known source. */
unsigned long ulBSP430timerFrequency_Hz_ni (hBSP430halTIMER timer);

/** Read the timer counter assuming interrupts are disabled.
 *
 * @param timer The timer for which the count is desired.
 *
 * @param overflowp An optional pointer in which the high word of the
 * overflow counter is stored, supporting a 48-bit counter.
 *
 * @return A 32-bit unsigned count of the number of clock ticks
 * observed since the timer was last reset. */
unsigned long ulBSP430timerCounter_ni (hBSP430halTIMER timer,
                                       unsigned int * overflowp);

/** Read timer counter regardless of interrupt enable state.
 *
 * This wraps #ulBSP430timerCounter_ni with code that saves the
 * interrupt state, reads the timer, then restores the entry interrupt
 * state prior to returning the timer counts.
 */
static unsigned long
__inline__
ulBSP430timerCounter (hBSP430halTIMER timer,
                      unsigned int * overflowp)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  unsigned long rv;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rv = ulBSP430timerCounter_ni(timer, overflowp);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

/** Read the timer counter.
 *
 * @param timer The timer for which the count is desired.
 *
 * @param overflowp An optional pointer in which the high word of the
 * overflow counter is stored, supporting a 48-bit counter.
 *
 * @return A 32-bit unsigned count of the number of clock ticks
 * observed since the timer was last reset. */
unsigned long ulBSP430timerCounter (hBSP430halTIMER timer,
                                    unsigned int * overflowp);

/** Reset the timer counter.
 *
 * This clears both the overflow count and the timer internal counter.
 * It does not start or stop the timer. */
void vBSP430timerResetCounter_ni (hBSP430halTIMER timer);

/* !BSP430! insert=hal_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_decl] */
/** @def configBSP430_HAL_TA0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TA0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TA0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TA0
#define configBSP430_HAL_TA0 0
#endif /* configBSP430_HAL_TA0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TA0 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TA0_;
#endif /* configBSP430_HAL_TA0 */
/** @endcond */

/** BSP430 HAL handle for TA0.
 *
 * The handle may be used only if #configBSP430_HAL_TA0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA0 - 0)
#define BSP430_HAL_TA0 (&xBSP430hal_TA0_)
#endif /* configBSP430_HAL_TA0 */

/** @def configBSP430_HAL_TA1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TA1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TA1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TA1
#define configBSP430_HAL_TA1 0
#endif /* configBSP430_HAL_TA1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TA1 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TA1_;
#endif /* configBSP430_HAL_TA1 */
/** @endcond */

/** BSP430 HAL handle for TA1.
 *
 * The handle may be used only if #configBSP430_HAL_TA1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA1 - 0)
#define BSP430_HAL_TA1 (&xBSP430hal_TA1_)
#endif /* configBSP430_HAL_TA1 */

/** @def configBSP430_HAL_TA2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA2 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TA2 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TA2 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TA2
#define configBSP430_HAL_TA2 0
#endif /* configBSP430_HAL_TA2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TA2 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TA2_;
#endif /* configBSP430_HAL_TA2 */
/** @endcond */

/** BSP430 HAL handle for TA2.
 *
 * The handle may be used only if #configBSP430_HAL_TA2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA2 - 0)
#define BSP430_HAL_TA2 (&xBSP430hal_TA2_)
#endif /* configBSP430_HAL_TA2 */

/** @def configBSP430_HAL_TA3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA3 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TA3 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TA3 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TA3
#define configBSP430_HAL_TA3 0
#endif /* configBSP430_HAL_TA3 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TA3 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TA3_;
#endif /* configBSP430_HAL_TA3 */
/** @endcond */

/** BSP430 HAL handle for TA3.
 *
 * The handle may be used only if #configBSP430_HAL_TA3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TA3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TA3 - 0)
#define BSP430_HAL_TA3 (&xBSP430hal_TA3_)
#endif /* configBSP430_HAL_TA3 */

/** @def configBSP430_HAL_TB0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB0 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TB0 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TB0 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TB0
#define configBSP430_HAL_TB0 0
#endif /* configBSP430_HAL_TB0 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TB0 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TB0_;
#endif /* configBSP430_HAL_TB0 */
/** @endcond */

/** BSP430 HAL handle for TB0.
 *
 * The handle may be used only if #configBSP430_HAL_TB0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TB0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TB0 - 0)
#define BSP430_HAL_TB0 (&xBSP430hal_TB0_)
#endif /* configBSP430_HAL_TB0 */

/** @def configBSP430_HAL_TB1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB1 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TB1 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TB1 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TB1
#define configBSP430_HAL_TB1 0
#endif /* configBSP430_HAL_TB1 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TB1 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TB1_;
#endif /* configBSP430_HAL_TB1 */
/** @endcond */

/** BSP430 HAL handle for TB1.
 *
 * The handle may be used only if #configBSP430_HAL_TB1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TB1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TB1 - 0)
#define BSP430_HAL_TB1 (&xBSP430hal_TB1_)
#endif /* configBSP430_HAL_TB1 */

/** @def configBSP430_HAL_TB2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB2 peripheral HAL interface.  This defines a global
 * object supporting enhanced functionality for the peripheral, and a
 * macro BSP430_HAL_TB2 that is a reference to that object.
 *
 * @note Enabling this defaults #configBSP430_HPL_TB2 to
 * true, since the HAL infrastructure requires the underlying HPL
 * infrastructure.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HAL_TB2
#define configBSP430_HAL_TB2 0
#endif /* configBSP430_HAL_TB2 */

/** @cond DOXYGEN_EXCLUDE */
#if configBSP430_HAL_TB2 - 0
/* You don't need to know about this */
extern sBSP430halTIMER xBSP430hal_TB2_;
#endif /* configBSP430_HAL_TB2 */
/** @endcond */

/** BSP430 HAL handle for TB2.
 *
 * The handle may be used only if #configBSP430_HAL_TB2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HAL_TB2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HAL_TB2 - 0)
#define BSP430_HAL_TB2 (&xBSP430hal_TB2_)
#endif /* configBSP430_HAL_TB2 */

/* END AUTOMATICALLY GENERATED CODE [hal_decl] */
/* !BSP430! end=hal_decl */

/* !BSP430! insert=periph_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_decl] */
/** @def configBSP430_HPL_TA0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TA0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_TA0
#define configBSP430_HPL_TA0 (configBSP430_HAL_TA0 - 0)
#endif /* configBSP430_HPL_TA0 */

#if (configBSP430_HAL_TA0 - 0) && ! (configBSP430_HPL_TA0 - 0)
#warning configBSP430_HAL_TA0 requested without configBSP430_HPL_TA0
#endif /* HAL and not HPL */

/** Handle for the raw TA0 device.
 *
 * The handle may be used only if #configBSP430_HPL_TA0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA0 - 0)
#define BSP430_PERIPH_TA0 ((tBSP430periphHandle)(BSP430_PERIPH_TA0_BASEADDRESS_))
#endif /* configBSP430_HPL_TA0 */

/** @def configBSP430_HPL_TA1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TA1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_TA1
#define configBSP430_HPL_TA1 (configBSP430_HAL_TA1 - 0)
#endif /* configBSP430_HPL_TA1 */

#if (configBSP430_HAL_TA1 - 0) && ! (configBSP430_HPL_TA1 - 0)
#warning configBSP430_HAL_TA1 requested without configBSP430_HPL_TA1
#endif /* HAL and not HPL */

/** Handle for the raw TA1 device.
 *
 * The handle may be used only if #configBSP430_HPL_TA1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA1 - 0)
#define BSP430_PERIPH_TA1 ((tBSP430periphHandle)(BSP430_PERIPH_TA1_BASEADDRESS_))
#endif /* configBSP430_HPL_TA1 */

/** @def configBSP430_HPL_TA2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TA2 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_TA2
#define configBSP430_HPL_TA2 (configBSP430_HAL_TA2 - 0)
#endif /* configBSP430_HPL_TA2 */

#if (configBSP430_HAL_TA2 - 0) && ! (configBSP430_HPL_TA2 - 0)
#warning configBSP430_HAL_TA2 requested without configBSP430_HPL_TA2
#endif /* HAL and not HPL */

/** Handle for the raw TA2 device.
 *
 * The handle may be used only if #configBSP430_HPL_TA2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA2 - 0)
#define BSP430_PERIPH_TA2 ((tBSP430periphHandle)(BSP430_PERIPH_TA2_BASEADDRESS_))
#endif /* configBSP430_HPL_TA2 */

/** @def configBSP430_HPL_TA3
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TA3 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TA3 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_TA3
#define configBSP430_HPL_TA3 (configBSP430_HAL_TA3 - 0)
#endif /* configBSP430_HPL_TA3 */

#if (configBSP430_HAL_TA3 - 0) && ! (configBSP430_HPL_TA3 - 0)
#warning configBSP430_HAL_TA3 requested without configBSP430_HPL_TA3
#endif /* HAL and not HPL */

/** Handle for the raw TA3 device.
 *
 * The handle may be used only if #configBSP430_HPL_TA3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA3 - 0)
#define BSP430_PERIPH_TA3 ((tBSP430periphHandle)(BSP430_PERIPH_TA3_BASEADDRESS_))
#endif /* configBSP430_HPL_TA3 */

/** @def configBSP430_HPL_TB0
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB0 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TB0 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_TB0
#define configBSP430_HPL_TB0 (configBSP430_HAL_TB0 - 0)
#endif /* configBSP430_HPL_TB0 */

#if (configBSP430_HAL_TB0 - 0) && ! (configBSP430_HPL_TB0 - 0)
#warning configBSP430_HAL_TB0 requested without configBSP430_HPL_TB0
#endif /* HAL and not HPL */

/** Handle for the raw TB0 device.
 *
 * The handle may be used only if #configBSP430_HPL_TB0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB0 - 0)
#define BSP430_PERIPH_TB0 ((tBSP430periphHandle)(BSP430_PERIPH_TB0_BASEADDRESS_))
#endif /* configBSP430_HPL_TB0 */

/** @def configBSP430_HPL_TB1
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB1 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TB1 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_TB1
#define configBSP430_HPL_TB1 (configBSP430_HAL_TB1 - 0)
#endif /* configBSP430_HPL_TB1 */

#if (configBSP430_HAL_TB1 - 0) && ! (configBSP430_HPL_TB1 - 0)
#warning configBSP430_HAL_TB1 requested without configBSP430_HPL_TB1
#endif /* HAL and not HPL */

/** Handle for the raw TB1 device.
 *
 * The handle may be used only if #configBSP430_HPL_TB1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB1 - 0)
#define BSP430_PERIPH_TB1 ((tBSP430periphHandle)(BSP430_PERIPH_TB1_BASEADDRESS_))
#endif /* configBSP430_HPL_TB1 */

/** @def configBSP430_HPL_TB2
 *
 * Define to a true value in @c bsp430_config.h to enable use of the
 * @c TB2 peripheral HPL interface.  Only do this if the MCU
 * supports this device.
 *
 * @note Enabling #configBSP430_HAL_TB2 defaults this to
 * true, so you only need to explicitly request this if you want the
 * HPL interface without the HAL interface.
 *
 * @cppflag
 * @defaulted */
#ifndef configBSP430_HPL_TB2
#define configBSP430_HPL_TB2 (configBSP430_HAL_TB2 - 0)
#endif /* configBSP430_HPL_TB2 */

#if (configBSP430_HAL_TB2 - 0) && ! (configBSP430_HPL_TB2 - 0)
#warning configBSP430_HAL_TB2 requested without configBSP430_HPL_TB2
#endif /* HAL and not HPL */

/** Handle for the raw TB2 device.
 *
 * The handle may be used only if #configBSP430_HPL_TB2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB2 - 0)
#define BSP430_PERIPH_TB2 ((tBSP430periphHandle)(BSP430_PERIPH_TB2_BASEADDRESS_))
#endif /* configBSP430_HPL_TB2 */

/* END AUTOMATICALLY GENERATED CODE [periph_decl] */
/* !BSP430! end=periph_decl */

/* !BSP430! insert=hpl_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hpl_decl] */
/** HPL pointer for TA0.
 *
 * Typed pointer to a volatile structure overlaying the TA0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TA0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA0 - 0)
#define BSP430_HPL_TA0 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA0)
#endif /* configBSP430_HPL_TA0 */

/** HPL pointer for TA1.
 *
 * Typed pointer to a volatile structure overlaying the TA1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TA1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA1 - 0)
#define BSP430_HPL_TA1 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA1)
#endif /* configBSP430_HPL_TA1 */

/** HPL pointer for TA2.
 *
 * Typed pointer to a volatile structure overlaying the TA2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TA2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA2 - 0)
#define BSP430_HPL_TA2 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA2)
#endif /* configBSP430_HPL_TA2 */

/** HPL pointer for TA3.
 *
 * Typed pointer to a volatile structure overlaying the TA3
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TA3
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TA3 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TA3 - 0)
#define BSP430_HPL_TA3 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TA3)
#endif /* configBSP430_HPL_TA3 */

/** HPL pointer for TB0.
 *
 * Typed pointer to a volatile structure overlaying the TB0
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TB0
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB0 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB0 - 0)
#define BSP430_HPL_TB0 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB0)
#endif /* configBSP430_HPL_TB0 */

/** HPL pointer for TB1.
 *
 * Typed pointer to a volatile structure overlaying the TB1
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TB1
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB1 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB1 - 0)
#define BSP430_HPL_TB1 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB1)
#endif /* configBSP430_HPL_TB1 */

/** HPL pointer for TB2.
 *
 * Typed pointer to a volatile structure overlaying the TB2
 * peripheral register map.
 *
 * The pointer may be used only if #configBSP430_HPL_TB2
 * is defined to a true value.
 *
 * @dependency #configBSP430_HPL_TB2 */
#if defined(BSP430_DOXYGEN) || (configBSP430_HPL_TB2 - 0)
#define BSP430_HPL_TB2 ((volatile sBSP430hplTIMER *)BSP430_PERIPH_TB2)
#endif /* configBSP430_HPL_TB2 */

/* END AUTOMATICALLY GENERATED CODE [hpl_decl] */
/* !BSP430! end=hpl_decl */

/* !BSP430! insert=hal_timer_isr_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_timer_isr_decl] */
/** @def configBSP430_HAL_TA0_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA0, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TA0_ISR, enabling #configBSP430_HAL_TA0
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TA0 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA0_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA0_CC0_ISR - 0) && ! (configBSP430_HAL_TA0 - 0)
#warning configBSP430_HAL_TA0_CC0_ISR requested without configBSP430_HAL_TA0
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TA0_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TA0 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA0 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TA0 */
#ifndef configBSP430_HAL_TA0_ISR
#define configBSP430_HAL_TA0_ISR (configBSP430_HAL_TA0 - 0)
#endif /* configBSP430_HAL_TA0_ISR */

#if (configBSP430_HAL_TA0_ISR - 0) && ! (configBSP430_HAL_TA0 - 0)
#warning configBSP430_HAL_TA0_ISR requested without configBSP430_HAL_TA0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TA1_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA1, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TA1_ISR, enabling #configBSP430_HAL_TA1
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TA1 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA1_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA1_CC0_ISR - 0) && ! (configBSP430_HAL_TA1 - 0)
#warning configBSP430_HAL_TA1_CC0_ISR requested without configBSP430_HAL_TA1
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TA1_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TA1 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA1 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TA1 */
#ifndef configBSP430_HAL_TA1_ISR
#define configBSP430_HAL_TA1_ISR (configBSP430_HAL_TA1 - 0)
#endif /* configBSP430_HAL_TA1_ISR */

#if (configBSP430_HAL_TA1_ISR - 0) && ! (configBSP430_HAL_TA1 - 0)
#warning configBSP430_HAL_TA1_ISR requested without configBSP430_HAL_TA1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TA2_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA2, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TA2_ISR, enabling #configBSP430_HAL_TA2
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TA2 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA2_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA2_CC0_ISR - 0) && ! (configBSP430_HAL_TA2 - 0)
#warning configBSP430_HAL_TA2_CC0_ISR requested without configBSP430_HAL_TA2
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TA2_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TA2 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA2 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TA2 */
#ifndef configBSP430_HAL_TA2_ISR
#define configBSP430_HAL_TA2_ISR (configBSP430_HAL_TA2 - 0)
#endif /* configBSP430_HAL_TA2_ISR */

#if (configBSP430_HAL_TA2_ISR - 0) && ! (configBSP430_HAL_TA2 - 0)
#warning configBSP430_HAL_TA2_ISR requested without configBSP430_HAL_TA2
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TA3_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA3, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TA3_ISR, enabling #configBSP430_HAL_TA3
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TA3 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TA3_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TA3_CC0_ISR - 0) && ! (configBSP430_HAL_TA3 - 0)
#warning configBSP430_HAL_TA3_CC0_ISR requested without configBSP430_HAL_TA3
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TA3_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TA3 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TA3 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TA3 */
#ifndef configBSP430_HAL_TA3_ISR
#define configBSP430_HAL_TA3_ISR (configBSP430_HAL_TA3 - 0)
#endif /* configBSP430_HAL_TA3_ISR */

#if (configBSP430_HAL_TA3_ISR - 0) && ! (configBSP430_HAL_TA3 - 0)
#warning configBSP430_HAL_TA3_ISR requested without configBSP430_HAL_TA3
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TB0_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB0, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TB0_ISR, enabling #configBSP430_HAL_TB0
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TB0 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB0_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB0_CC0_ISR - 0) && ! (configBSP430_HAL_TB0 - 0)
#warning configBSP430_HAL_TB0_CC0_ISR requested without configBSP430_HAL_TB0
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TB0_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TB0 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB0 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TB0 */
#ifndef configBSP430_HAL_TB0_ISR
#define configBSP430_HAL_TB0_ISR (configBSP430_HAL_TB0 - 0)
#endif /* configBSP430_HAL_TB0_ISR */

#if (configBSP430_HAL_TB0_ISR - 0) && ! (configBSP430_HAL_TB0 - 0)
#warning configBSP430_HAL_TB0_ISR requested without configBSP430_HAL_TB0
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TB1_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB1, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TB1_ISR, enabling #configBSP430_HAL_TB1
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TB1 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB1_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB1_CC0_ISR - 0) && ! (configBSP430_HAL_TB1 - 0)
#warning configBSP430_HAL_TB1_CC0_ISR requested without configBSP430_HAL_TB1
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TB1_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TB1 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB1 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TB1 */
#ifndef configBSP430_HAL_TB1_ISR
#define configBSP430_HAL_TB1_ISR (configBSP430_HAL_TB1 - 0)
#endif /* configBSP430_HAL_TB1_ISR */

#if (configBSP430_HAL_TB1_ISR - 0) && ! (configBSP430_HAL_TB1 - 0)
#warning configBSP430_HAL_TB1_ISR requested without configBSP430_HAL_TB1
#endif /* HAL_ISR and not HAL */

/** @def configBSP430_HAL_TB2_CC0_ISR
 *
 * This is the TIMERx_t0_VECTOR interrupt, handling only CC0.
 *
 * Define to a true value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB2, and want to BSP430 to provide
 * an interrupt service routine that dispatches CC0 events.
 *
 * @note Unlike #configBSP430_HAL_TB2_ISR, enabling #configBSP430_HAL_TB2
 * does not default to enable this feature.
 *
 *
 * @cppflag
 * @nodefault
 * @dependency #configBSP430_HAL_TB2 */
#if defined(BSP430_DOXYGEN)
#define configBSP430_HAL_TB2_CC0_ISR 0
#endif /* BSP430_DOXYGEN */

#if (configBSP430_HAL_TB2_CC0_ISR - 0) && ! (configBSP430_HAL_TB2 - 0)
#warning configBSP430_HAL_TB2_CC0_ISR requested without configBSP430_HAL_TB2
#endif /* HAL_CC0_ISR and not HAL */

/** @def configBSP430_HAL_TB2_ISR
 *
 * This is the TIMERx_t1_VECTOR interrupt, handling overflows and CC1-CC6.
 *
 * This option defaults to true when #configBSP430_HAL_TB2 is
 * enabled.
 *
 * Define it a false value in @c bsp430_config.h if you are using the
 * BSP430 HAL interface for @c TB2 but want to define your
 * own interrupt service routine for the peripheral.
 *
 * @cppflag
 * @defaulted
 * @dependency #configBSP430_HAL_TB2 */
#ifndef configBSP430_HAL_TB2_ISR
#define configBSP430_HAL_TB2_ISR (configBSP430_HAL_TB2 - 0)
#endif /* configBSP430_HAL_TB2_ISR */

#if (configBSP430_HAL_TB2_ISR - 0) && ! (configBSP430_HAL_TB2 - 0)
#warning configBSP430_HAL_TB2_ISR requested without configBSP430_HAL_TB2
#endif /* HAL_ISR and not HAL */

/* END AUTOMATICALLY GENERATED CODE [hal_timer_isr_decl] */
/* !BSP430! end=hal_timer_isr_decl */

#endif /* BSP430_MODULE_TIMER */

#endif /* BSP430_PERIPH_TIMER_H */
