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
 * @li #ulBSP430clockACLK_Hz assumes returns 32768 if XT1CLK is the
 * selected source for ACLK and OFIFG is clear, and returns 10000 (the
 * nominal VLOCLK frequency) otherwise.  Be aware that the actual
 * VLOCLK frequency may be different by 10-20%.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_CLOCKS_CS_H
#define BSP430_CLOCKS_CS_H

#include "FreeRTOS.h"
#include <bsp430/common/clocks.h>

/** @def configBSP430_FR5XX_CS_ENABLE_HIGH_SPEED
 *
 * Define this to a true value to indicate that the MCU being used is
 * expected to be capable of setting the CSCTL1.DCORSEL bit to select
 * higher trimmed speeds.  This extends the set of reachable
 * frequencies to include 16 MHz, 20 MHz, and 24 MHz. */
#ifndef configBSP430_FR5XX_CS_ENABLE_HIGH_SPEED
#define configBSP430_FR5XX_CS_ENABLE_HIGH_SPEED 0
#endif /* configBSP430_FR5XX_CS_ENABLE_HIGH_SPEED */

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
 * If @a yesp is @c pdTRUE, check whether the crystal is present and
 * stabilized.  If so, configure ACLK to source from XT1 and return
 * pdTRUE.  Otherwise configure ACLK to source from VLOCLK and return
 * pdFALSE.
 *
 * Stabilization is checked by clearing the XT1 oscillator faults,
 * waiting 50k MCLK cycles, and checking for a fault.  This is
 * repeated, limited by @a usLoops, until stabilization is detected.
 * Prior to invoking this function the crystal's port pins must be
 * configured to their peripheral function as specified in the MCU
 * data sheet.
 *
 * @param yesp pdTRUE if the ACLK should be sourced from XT1; pdFALSE
 * if it should be sourced from VLOCLK.
 *
 * @param usLoops The number of times the stabilization check should
 * be repeated.  If stabilization has not been achieved after this
 * many loops, assume the crystal is absent and configure for VLOCLK.
 * Pass a value of zero to loop until stabilization is detected
 * regardless of how long that might take.
 *
 * @param pdTRUE iff ACLK is sourced from a stable XT1.
 */
portBASE_TYPE xBSP430csACLKSourceXT1 (portBASE_TYPE xUseXT1,
									  unsigned short usLoops);

#endif /* BSP430_CLOCKS_CS_H */
