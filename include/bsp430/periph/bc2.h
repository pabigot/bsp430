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
 * @brief Hardware presentation/abstraction for Basic Clock Module+ (BC2).
 * 
 * This module supports the BC2 ("Basic Clock Module+") peripheral,
 * which is present in 2xx-family devices.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @date 2012
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_CLOCKS_BC2_H
#define BSP430_CLOCKS_BC2_H

#include <bsp430/clock.h>
#include <bsp430/periph.h>

#if ! defined(__MSP430_HAS_BC2__)
#warning Peripheral not supported by configured MCU
#endif /* __MSP430_HAS_BC2__ */

#undef BSP430_CLOCK_LFXT1_IS_FAULTED
/** Check whether the LFXT1 crystal has a fault condition.
 *
 * This definition overrides the generic definition to test the
 * crystal-specific flags. */
#define BSP430_CLOCK_LFXT1_IS_FAULTED() (BCSCTL3 & LFXT1OF)

#undef BSP430_CLOCK_LFXT1_CLEAR_FAULT
/** Clear the fault associated with LFXT1.
 *
 * This definition overrides the generic definition to clear the
 * crystal-specific flags as well as the system flag. */
#define BSP430_CLOCK_LFXT1_CLEAR_FAULT() do {					\
		/* User's guide says BCSCTL3.LFXT1OF is read-only. */	\
		IFG1 &= ~OFIFG;											\
	} while (0)

/** Call this to reconfigure the BC2 peripheral.
 *
 * The relevant BC2 registers are configured as requested.  See the
 * 2xx Family User's Guide for details.
 *
 * @param ucDCOCTL This configures the frequency and modulator
 * selection.
 *
 * @param ucBCSCTL1 This configures the external crystal modes, ACLK
 * divider, and range selection.
 * 
 * @param ucBCSCTL2 This sets clock dividers.
 *
 * @param ucBCSCTL3 This configures external crystals.  If bit LFXT1S1
 * is set, the configuration uses VLOCLK and presence/stabilization of
 * the crystal is not performed.  If bit LFXT1S1 is clear, the routine
 * will attempt to configure the crystal; if this fails, it will fall
 * back to VLOCLK.  The code invokes
 * #iBSP430platformConfigurePeripheralPins_ni as necessary.
 *
 * @return pdTrue if LFXT1 is enabled and has stabilized within the
 * default wait time, and pdFalse in all other cases. */
unsigned char
ucBSP430bc2Configure (unsigned char ucDCOCTL,
					  unsigned char ucBCSCTL1,
					  unsigned char ucBCSCTL2,
					  unsigned char ucBCSCTL3);

#endif /* BSP430_CLOCKS_BC2_H */
