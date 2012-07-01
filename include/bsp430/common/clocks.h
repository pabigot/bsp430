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
 * This module declares common clock-related functions.
 *
 * The actual implementation of the functions are within the
 * peripheral-specific clock modules.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_COMMON_CLOCKS_H
#define BSP430_COMMON_CLOCKS_H

#include "FreeRTOS.h"

/** @def configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT
 *
 * SMCLK is normally configured to divide another clock by shifting it
 * left this many positions.  E.g., if MCLK is 20 MHz, a dividing shift
 * of 2 will produce a clock divisor of 4 and an SMCLK at 5 MHz.
 *
 * @note The value of define reflects the divisor of MCLK.  If the
 * clock source for SMCLK is different than an undivided MCLK, the
 * code that configures the clock may need to modify the shift
 * value. */
#ifndef configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT
#define configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT 0
#endif /* configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT */

/** Return the best available estimate of MCLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #configCPU_CLOCK_HZ, or it may return a value calculated from
 * observations.
 *
 * @return an estimate of the MCLK frequency, in Hz */
unsigned long ulBSP430clockMCLK_Hz ();

/** Return the best available estimate of SMCLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #configCPU_CLOCK_HZ >> #configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT, or
 * it may return a value calculated from observations.
 *
 * @return an estimate of the SMCLK frequency, in Hz */
unsigned long ulBSP430clockSMCLK_Hz ();

/** Return the best available estimate of ACLK frequency.
 *
 * Depending on clock capabilities, this may simply return
 * #portACLK_FREQUENCY_HZ, or it may return a value calculated from
 * observations.
 *
 * @return an estimate of the ACLK frequency, in Hz */
unsigned short usBSP430clockACLK_Hz ();

#endif /* BSP430_COMMON_CLOCKS_H */
