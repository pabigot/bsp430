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
 * @brief Platform-specific include for <a href="http://www.ti.com/tool/msp-exp430g2">MSP-EXP430G2 ("LaunchPad")</a>
 *
 * This board is also known as the "LaunchPad"
 *
 * The following platform-specific features are supported: <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni The jumper pair for this
 * platform is P1.4 and P1.5, located in the left header.  Place the
 * jumper across these pins.
 *
 * </ul>
 */

#ifndef BSP430_PLATFORM_EXP430G2_H
#define BSP430_PLATFORM_EXP430G2_H

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_EXP430G2 1

/** Where clocks are found on this platform */
#define BSP430_PERIPH_EXPOSED_CLOCKS_HELP "SMCLK on P1.4; ACLK on P1.0 (red LED)"

#if defined(BSP430_DOXYGEN)                                     \
  || ((configBSP430_PLATFORM_TIMER_CCACLK - 0)                  \
      && ! defined(BSP430_BSP430_PLATFORM_TIMER_CCACLK))
#if defined(__MSP430G2553__)
/** Defined only if #configBSP430_PLATFORM_TIMER_CCACLK. @defaulted */
#define BSP430_PLATFORM_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TA0
/** Defined only if #configBSP430_PLATFORM_TIMER_CCACLK and using default #BSP430_PLATFORM_TIMER_CCACLK_PERIPH_HANDLE */
#define BSP430_PLATFORM_TIMER_CCACLK_CC_INDEX 0
/** Defined only if #configBSP430_PLATFORM_TIMER_CCACLK and using default #BSP430_PLATFORM_TIMER_CCACLK_PERIPH_HANDLE */
#define BSP430_PLATFORM_TIMER_CCACLK_CCIS CCIS_1
#endif /* MCU */
#endif /* configBSP430_PLATFORM_TIMER_CCACLK && need default */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_EXP430G2_H */
