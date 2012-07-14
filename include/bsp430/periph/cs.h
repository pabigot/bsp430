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
 * This module supports the Clock System (CS) peripheral present in
 * FR5xx-family devices.
 *
 * The module supports only a few factory-trimmed frequencies: 5.33,
 * 6.67, and 8 MHz on low-speed devices.  On high-speed--capable
 * devices the speed may also be set to 16, 20 and 24 MHz.
 *
 * Other refinements in this module:
 *
 * @li #ulBSP430clockMCLK_Hz assumes that DCOCLK is the selected
 * source for MCLK, and returns the selected trimmed DCOCLK frequency.
 * 
 * @li #ulBSP430clockSMCLK_Hz assumes DCOCLK is the selected source
 * for SMCLK, and returns the selected trimmed DCOCLK frequency
 * shifted right by #configBSP430_CLOCK_SMCLK_DIVIDING_SHIFT.
 *
 * @li #usBSP430clockACLK_Hz assumes returns 32768 if XT1CLK is the
 * selected source for ACLK and OFIFG is clear, and returns 10000 (the
 * nominal VLOCLK frequency) otherwise.  Be aware that the actual
 * VLOCLK frequency may be different by 10-20%.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_CS_H
#define BSP430_PERIPH_CS_H

#include <bsp430/clock.h>

/** @def configBSP430_CS_XT1_DELAY_CYCLES
 *
 * Define this to the number of MCLK cycles to delay, after clearing
 * oscillator faults, before checking for oscillator stability.  This
 * must be a compile-time constant integer compatible with
 * <tt>unsigned long</tt>.  See also the @a usLoops parameter to
 * #xBSP430csACLKSourceXT1.  To wait up to one second for the crystal
 * to stabilize, use:
 *
 * @code
 * // Set up pins for XT1 function
 * if (! xBSP430csACLKSourceXT1(1, configCPU_CLOCK_HZ / configBSP430_CS_XT1_DELAY_CYCLES)) {
 *    // XT1 not available, return pins to GPIO function
 * }
 * @endcode
 *
 * Be aware that it may take several hundred milliseconds to stabilize
 * the crystal; 500ms has been observed to be too short.
 */
#ifndef configBSP430_CS_XT1_DELAY_CYCLES
#define configBSP430_CS_XT1_DELAY_CYCLES 10000
#endif /* configBSP430_CS_XT1_DELAY_CYCLES */

/** Call this to configure MCLK and SMCLK via CS peripheral.
 *
 * @param ulFrequency_Hz The target frequency for DCOCLKDIV=MCLK.  The
 * actual frequency will be the closest available factory-configured
 * frequency.  This may be higher or lower than the requested
 * frequency.
 *
 * @return an estimate of the actual running frequency.
 */
unsigned long ulBSP430csConfigureMCLK (unsigned long ulFrequency_Hz);

/** Call this to configure ACLK to use XT1.
 *
 * If @a xUseXT1 is @c pdTRUE, check whether the crystal is present and
 * stabilized.  If so, configure ACLK to source from XT1 and return
 * pdTRUE.  Otherwise configure ACLK to source from VLOCLK and return
 * pdFALSE.
 *
 * Stabilization is checked by clearing the XT1 oscillator faults,
 * waiting #configBSP430_CS_XT1_DELAY_CYCLES MCLK cycles, and checking
 * for a fault.  This is repeated, limited by @a usLoops, until
 * stabilization is detected.  Prior to invoking this function the
 * crystal's port pins must be configured to their peripheral function
 * as specified in the MCU data sheet.
 *
 * @param xUseXT1 Pass a nonzero value if the ACLK should be sourced
 * from XT1.  Pass zero if it should be sourced from VLOCLK.
 *
 * @param usLoops The number of times the stabilization check should
 * be repeated.  If stabilization has not been achieved after this
 * many loops, assume the crystal is absent and configure for VLOCLK.
 * Pass a value of zero to loop until stabilization is detected
 * regardless of how long that might take.
 *
 * @return Nonzero iff ACLK is sourced from a stable XT1.
 */
portBASE_TYPE xBSP430csACLKSourceXT1 (portBASE_TYPE xUseXT1,
									  unsigned short usLoops);

#endif /* BSP430_PERIPH_CS_H */
