/*
  
Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the software nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

/** Unified Clock System
 *
 * This module supports the Unified Clock System (UCS) and UCS_RF
 * peripherals, which are present in 5xx/6xx-family devices.
 *
 * UCS_RF differs from UCS by adding support for XT2.  This difference
 * is currently ignored.
 *
 * Based on experimentation, the following is assumed or enforced for
 * all supported clock configurations:
 * 
 * - SELREF is XT1CLK running at 32768 Hz.  The configuration routine
 *   expects the XIN/XOUT pins to have been configured for their
 *   peripheral function prior to being invoked, but will do the XT1
 *   enabling and fault checking for you.
 *
 * - FLLD is consistently set to /2, which is the PUC value and which
 *   is adequate to support speeds up to 32 MiHz with a 32768 Hz
 *   REFCLK.
 *
 * - FLLREFDIV is consistently set to /1, which is the PUC value.
 *
 * - MCLK and SMCLK are set to DCOCLKDIV, and ACLK to XT1CLK.
 *
 * - Due to UCS10 and UCS7, normal practice on UCS-based MCUs is to
 *   leave the FLL disabled, and adjust it periodically when the
 *   clocks are otherwise unused.  Thus SCG0 is expected to be set at
 *   all times except when trimming.
 */

#ifndef _BSP430_UCS_H_
#define _BSP430_UCS_H_

#include "FreeRTOS.h"

/** Call this to initially configure the UCS peripheral.
 *
 * @param ulFrequency_Hz The target frequency for DCOCLKDIV=MCLK=SMCLK.
 *
 * @param sRSEL The DCO frequency range selection.  The appropriate
 * value is frequency-dependent, and specified in the "DCO Frequency"
 * table in the device-specific data sheet.  The same target frequency
 * may be reachable with different RSELs but with different
 * accuracies.  If a negative value is given, the function may be able
 * to select a default.
 * 
 * @return an estimate of the actual running frequency. */

unsigned long ulBSP430ucsConfigure ( unsigned long ulFrequency_Hz,
									 short sRSEL );

/** Call this periodically to trim the FLL.
 *
 * The function uses TB0 to determine the speed of SMCLK, and if it is
 * "too far" from the value specified in the last call to
 * ulBSP430ucsConfigure() enables the FLL for a short period to see if
 * accuracy can be improved.
 *
 * @warning MCLK, SMCLK, and any clocks derived from them are unstable
 * while this routine is being run, so UART, SPI, and other
 * peripherals may need to be shut down first.
 * 
 * @note This function must be invoked with interrupts disabled.
 *
 * @return an estimate of the actual running frequency.
 */
unsigned long ulBSP430ucsTrimFLL ();

/** Return the last calculated MCLK frequency.
 *
 * This may be off if the FLL has drifted and not been trimmed
 * recently.
 *
 * @return an estimate of the MCLK frequency, in Hz */
unsigned long ulBSP430ucsMCLK_Hz ();

/** Return the last calculated SMCLK frequency.
 *
 * This may be off if the FLL has drifted and not been trimmed
 * recently.
 *
 * @return an estimate of the SMCLK frequency, in Hz */
unsigned long ulBSP430ucsSMCLK_Hz ();

#endif /* _BSP430_UCS_H_ */
