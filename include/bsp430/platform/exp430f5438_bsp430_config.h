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
 * @brief Platform-specific BSP430 configuration directives for <a href="http://www.ti.com/tool/msp-exp430f5438">MSP-EXP430F5438</a>
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_EXP430F5438_BSP430_CONFIG_H
#define BSP430_PLATFORM_EXP430F5438_BSP430_CONFIG_H

#ifndef BSP430_UCS_TRIMFLL_TIMER_PERIPH_HANDLE
/** Unless overridden, this platform uses TB0 to trim the FLL
 * @defaulted */
#define BSP430_UCS_TRIMFLL_TIMER_PERIPH_HANDLE BSP430_PERIPH_TB0
/** TB0.6 CCI0B sources SMCLK
 * @defaulted */
#define BSP430_UCS_TRIMFLL_TIMER_ACLK_CC_INDEX 6
/** TB0.6 CCI0B sources SMCLK
 * @defaulted */
#define BSP430_UCS_TRIMFLL_TIMER_ACLK_CCIS CCIS_1
/** Require TB0 for FLL trim
 * @defaulted */
#define configBSP430_PERIPH_TB0 1
#endif /* BSP430_UCS_TRIMFLL_TIMER_PERIPH_HANDLE */

#endif /* BSP430_PLATFORM_EXP430F5438_BSP430_CONFIG_H */