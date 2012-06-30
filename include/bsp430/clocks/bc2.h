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

/* This module supports the BC2 ("Basic Clock Module+") peripheral,
 * which is present in 2xx-family devices. */

#ifndef _BSP430_BC2_H_
#define _BSP430_BC2_H_

#include "FreeRTOS.h"

/** Call this to reconfigure the BC2 peripheral.
 *
 * The relevant BC2 registers are configured as requested.  See the
 * 2xx Family User's Guide for details.
 *
 * @param ucDCOCTL.  This configures the frequency and modulator
 * selection.
 *
 * @param ucBCSCTL1.  This configures the external crystal modes, ACLK
 * divider, and range selection.
 * 
 * @param ucBCSCTL2.  This sets clock dividers.
 *
 * @param ucBCSCTL3.  This configures external crystals.  If bit
 * LFXT1S1 is set, the configuration uses VLOCLK and
 * presence/stabilization of the crystal is not performed.  If bit
 * LFXT1S1 is clear, ucBSP430bc2Configure() will attempt to
 * configure the crystal; if this fails, it will fall back to
 * VLOCLK.
 *
 * @note XIN and XOUT must be externally configured to their
 * peripheral function prior to invoking this if the crystal is to be
 * used.  As this is MCU-specific, the code is not part of this
 * function.  If crystal stability is not achieved, the caller should
 * deconfigure these pins.
 *
 * @return pdTrue if the oscillator has stabilized within the
 * default wait time, and pdFalse if not. */
unsigned char
ucBSP430bc2Configure (unsigned char ucDCOCTL,
					  unsigned char ucBCSCTL1,
					  unsigned char ucBCSCTL2,
					  unsigned char ucBCSCTL3);

#endif /* _BSP430_BC2_H_ */
