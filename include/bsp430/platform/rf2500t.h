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
 * @brief Platform-specific include for <a href="http://www.ti.com/tool/ez430-rf2500t">EZ430-RF2500T</a>
 *
 * The following platform-specific features are supported: <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni The "jumper pair" for this
 * platform is the button.  Hold it down.
 *
 * </ul>
 */

#ifndef BSP430_PLATFORM_RF2500T_H
#define BSP430_PLATFORM_RF2500T_H

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_RF2500T 1

/** Where clocks are found on this platform */
#define BSP430_PERIPH_EXPOSED_CLOCKS_HELP "SMCLK on P2.1 (P4); ACLK on P2.0 (P3)"

/** Serial communications is unusable at 9600, but stable at 2400
 * @defaulted */
#ifndef BSP430_CONSOLE_BAUD_RATE
#define BSP430_CONSOLE_BAUD_RATE 2400
#endif /* BSP430_CONSOLE_BAUD_RATE */

#if defined(BSP430_DOXYGEN)                                     \
  || ((configBSP430_PLATFORM_TIMER_CCACLK - 0)                  \
      && ! defined(BSP430_BSP430_PLATFORM_TIMER_CCACLK))
/** Defined only if #configBSP430_PLATFORM_TIMER_CCACLK. @defaulted */
#define BSP430_PLATFORM_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TB0
/** Defined only if #configBSP430_PLATFORM_TIMER_CCACLK and using default #BSP430_PLATFORM_TIMER_CCACLK_PERIPH_HANDLE */
#define BSP430_PLATFORM_TIMER_CCACLK_CC_INDEX 2
/** Defined only if #configBSP430_PLATFORM_TIMER_CCACLK and using default #BSP430_PLATFORM_TIMER_CCACLK_PERIPH_HANDLE */
#define BSP430_PLATFORM_TIMER_CCACLK_CCIS CCIS_1
#endif /* configBSP430_PLATFORM_TIMER_CCACLK && need default */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_RF2500T_H */
