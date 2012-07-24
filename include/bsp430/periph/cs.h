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
 * @brief Hardware presentation/abstraction for Clock System (CS).
 *
 * This module supports the Clock System (CS) peripheral present in
 * FR5xx-family devices.
 *
 * The peripheral supports only a few factory-trimmed frequencies:
 * 5.33, 6.67, and 8 MHz on low-speed devices.  On high-speed--capable
 * devices the speed may also be set to 16, 20 and 24 MHz.
 *
 * Other refinements in this module:
 *
 * @li #ulBSP430clockMCLK_Hz assumes that DCOCLK is the selected
 * source for MCLK, and returns the selected trimmed DCOCLK frequency.
 * 
 * @li #ulBSP430clockSMCLK_Hz assumes DCOCLK is the selected source
 * for SMCLK, and returns the selected trimmed DCOCLK frequency
 * shifted right by #BSP430_CLOCK_SMCLK_DIVIDING_SHIFT.
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
#include <bsp430/periph.h>

#if ! (defined(__MSP430_HAS_CS__) || defined(__MSP430_HAS_CS_A__))
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_CS__ */

#undef BSP430_CLOCK_LFXT1_IS_FAULTED
/** Check whether the LFXT1 crystal has a fault condition.
 *
 * This definition overrides the generic definition to test the
 * crystal-specific flags. */
#define BSP430_CLOCK_LFXT1_IS_FAULTED() (CSCTL5 & XT1OFFG)

#undef BSP430_CLOCK_LFXT1_CLEAR_FAULT
/** Clear the fault associated with LFXT1.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag. */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do {	\
		CSCTL5 &= ~XT1OFFG;						\
		SFRIFG1 &= ~OFIFG;						\
	} while (0)

/** Call this to configure MCLK and SMCLK via CS peripheral.
 *
 * @param ulFrequency_Hz The target frequency for DCOCLKDIV=MCLK.  The
 * actual frequency will be the closest available factory-configured
 * frequency.  This may be higher or lower than the requested
 * frequency.
 *
 * @return an estimate of the actual running frequency.
 *
 * @note This function has no effect on ACLK configuration.
 */
unsigned long ulBSP430csConfigureMCLK (unsigned long ulFrequency_Hz);

/** Call this to configure ACLK via the CS peripheral.
 *
 * Prior to invoking this, use #iBSP430clockConfigureXT1 to check for
 * crystal stability, if ACLK is to be sourced from XT1.
 * 
 * @param sela The constant to assign to the SELA field of CSCTL2.
 * Standard values are @c SELA__XT1CLK and @c SELA_VLOCLK.
 *
 * @return Zero if the configuration was successful; -1 if the value
 * for @a sela was not valid.
 */
int iBSP430csConfigureACLK (unsigned int sela);

#endif /* BSP430_PERIPH_CS_H */
