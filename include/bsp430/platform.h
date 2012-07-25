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
 * @brief Entrypoint for platform-specific capabilities.
 *
 * Mostly prototypes for functions that are implemented in the
 * platform source files.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_H
#define BSP430_PLATFORM_H

#include <msp430.h>
#include <bsp430/periph.h>

/** Basic configuration for the platform.
 *
 * This routine will:
 * @li Disable the watchdog
 * @li Enable the platform crystal, if available
 * @li Configure ACLK to use the crystal (if available)
 * @li Configure the clocks based on #BSP430_CLOCK_NOMINAL_MCLK_HZ
 * @li Start the system clock (if #configBSP430_UPTIME)
 */
void vBSP430platformSetup_ni ();

/** Configure the pins associated with a given peripheral.
 *
 * @note All platforms should support #BSP430_PERIPH_XT1.  Consult the
 * MCU data sheet to determine which the appropriate port selection
 * register configuration to enable XIN and XOUT.
 * 
 * @param device Raw peripheral device for which pins should be
 * configured.
 * 
 * @param enablep TRUE to enable for peripheral use; FALSE to disable
 * (return to general purpose IO)
 *
 * @return 0 if configuration was successful, -1 if the device was not
 * recognized.
 */
int iBSP430platformConfigurePeripheralPins_ni (xBSP430periphHandle device, int enablep);

/** Block until a platform-specific jumper has been removed.
 *
 * Several platforms that use a TIUSB device to provide serial port
 * emulation, including the MSP430 Launchpad and the EXP430FR5739,
 * must not transmit any data while the driver is initializing.  We
 * need a way to hold off an installed application from writing to the
 * serial port until that's happened.  What we do is, if a
 * platform-specific pin is grounded, block until it's released.  When
 * plugging in a device for the first time, place a jumper connecting
 * that pin to ground, wait for the driver to load, then remove the
 * jumper.
 *
 * @note vBSP430ledInit_ni() must have been invoked before this function
 * is called. */
void vBSP430platformSpinForJumper_ni (void);

#endif /* BSP430_PLATFORM_H */
