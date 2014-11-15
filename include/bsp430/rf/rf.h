/* Copyright 2013-2014, Peter A. Bigot
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
 * @brief RF Support
 *
 * Many MSP430 based systems may include RF components which are
 * accessed through <bsp430/rf/rfem.h> or through hard-wired radios.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_RF_RF_H
#define BSP430_RF_RF_H

#include <bsp430/core.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Define to a true value to enable any timer infrastructure
 * associated with RF component GPIOs.  A timer that will support this
 * must be identified; see #BSP430_RF_TIMER_PERIPH_CPPID.
 *
 * @cppflag
 * @affects #BSP430_RF_TIMER_PERIPH_HANDLE
 * @defaulted
 */
#ifndef configBSP430_RF_TIMER
#define configBSP430_RF_TIMER 0
#endif /* configBSP430_RF_TIMER */

/** Define to the preprocessor-compatible identifier for a timer that
 * may be used to capture or generate signals on RF module GPIO pins.
 * This timer should run at a speed capable of resolving individual
 * symbols of the RF stream.  It may only be active at times when the
 * RF module is enabled.
 *
 * The define must appear in the @ref bsp430_config subsystem so that
 * functional resource requests are correctly propagated to the
 * underlying resource instances.
 *
 * @note This interface assumes there is one timer that is shared
 * among all GPIOs that have capture/compare capability.  This can be
 * validated by checking it against specific GPIO timer peripherals,
 * such as #BSP430_RF_CC2520_GPIO5_TIMER_PERIPH_CPPID.
 *
 * @defaulted
 * @platformvalue
 * @affects #BSP430_RF_TIMER_PERIPH_HANDLE */
#if defined(BSP430_DOXYGEN)
#define BSP430_RF_TIMER_PERIPH_CPPID include "bsp430_config.h"
#endif /* BSP430_DOXYGEN */

/** Defined to the peripheral identifier for a timer that can be used
 * to capture or generate signals on RF module GPIO pins.  This
 * derives directly from #BSP430_RF_TIMER_PERIPH_CPPID, but is a timer
 * peripheral handle suitable for use in code.
 *
 * The corresponding HAL and primary ISR features are automatically
 * enabled for this peripheral.  CC0 ISR is default included only if
 * there is a GPIO associated with CC0 on this timer.
 *
 * @dependency #BSP430_RF_TIMER_PERIPH_CPPID, #configBSP430_RF_TIMER */
#if defined(BSP430_DOXYGEN)
#define BSP430_RF_TIMER_PERIPH_HANDLE platform or application specific
#elif ! (configBSP430_RF_TIMER - 0)
/* leave undefined if not requested */
/* !BSP430! instance=@timers functional=rf_timer subst=functional insert=periph_sethandle */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_sethandle] */

#elif BSP430_RF_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA0
#define BSP430_RF_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA0

#elif BSP430_RF_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA1
#define BSP430_RF_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA1

#elif BSP430_RF_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA2
#define BSP430_RF_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA2

#elif BSP430_RF_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TA3
#define BSP430_RF_TIMER_PERIPH_HANDLE BSP430_PERIPH_TA3

#elif BSP430_RF_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB0
#define BSP430_RF_TIMER_PERIPH_HANDLE BSP430_PERIPH_TB0

#elif BSP430_RF_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB1
#define BSP430_RF_TIMER_PERIPH_HANDLE BSP430_PERIPH_TB1

#elif BSP430_RF_TIMER_PERIPH_CPPID == BSP430_PERIPH_CPPID_TB2
#define BSP430_RF_TIMER_PERIPH_HANDLE BSP430_PERIPH_TB2
/* END AUTOMATICALLY GENERATED CODE [periph_sethandle] */
/* !BSP430! end=periph_sethandle */
#endif /* BSP430_RF_TIMER_PERIPH_CPPID */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP430_RF_RF_H */
