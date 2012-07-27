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
 * Platform-specific include for <a href="http://www.ti.com/tool/msp-exp430fr5739">EXP430FR5739</a>
 *
 * This board is also known as the "Fraunchpad" 
 *
 * The following platform-specific features are supported: <ul>
 *
 * <li> #vBSP430platformSpinForJumper_ni The jumper for this platform
 * is P4.0, located on the right of the board at the top.  Place the
 * jumper between P4.0 and GND.
 *
 * <li> #BSP430_PERIPH_EXPOSED_CLOCKS Clocks share LED lines, and can
 * be measured at the test points below the LEDs.  Proceeding from the
 * rightmost LED: TP10 provides SMCLK, TP11 provides MCLK, and TP12
 * provides ACLK.
 *
 * </ul>
 */
#ifndef BSP430_PLATFORM_EXP430FR5739_H
#define BSP430_PLATFORM_EXP430FR5739_H

#include <bsp430/common/platform.h>

#endif /* BSP430_PLATFORM_EXP430FR5739_H */
