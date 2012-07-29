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
 * platform source files.  If a recognized @c BSP430_PLATFORM_foo
 * macro is defined to a true value, the corresponding
 * platform-specific header will be included for you.
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
 * @li Attempt to enable the platform crystal with iBSP430clockConfigureXT1_ni()
 * @li Configure ACLK to use the crystal (if available and stable)
 * @li Configure the MCLK frequency based on
 * #BSP430_CLOCK_NOMINAL_MCLK_HZ if that is defined to a non-zero
 * value.  (If it is defined to zero or a negative value, the power-up
 * MCLK/FLL/DCO configuration is left unchanged.)
 * @li Configure the SMCLK to source from the same origin as MCLK and
 * divide based on #BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT if that
 * is defined to a non-negative value.  (If it is a negative value,
 * the power-up SMCLK source and divider are left unchanged.)
 * @li Start the system clock (if #configBSP430_UPTIME)
 */
void vBSP430platformSetup_ni (void);

/** Configure the pins associated with a given peripheral.
 *
 * @note All platforms should support #BSP430_PERIPH_XT1.
 *
 * Implementers should consult the MCU data sheet to determine the
 * appropriate port selection register configuration.  Note that in
 * many cases it is not necessary to configure the port direction
 * (e.g. P1DIR) for peripheral functions (e.g. UCA0RXD and UCA0TXD).
 * In some cases it is not necessary to configure XOUT if XIN is
 * configured.
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
 * The platform-specific header should document whether this function
 * is supported, and if it is what jumper configuration is recognized.
 *
 * @note vBSP430ledInitialize_ni() must have been invoked before this
 * function is called. */
void vBSP430platformSpinForJumper_ni (void);

/* !BSP430! instance=exp430f5438,exp430fr5739,exp430fg4618,exp430g2 */
/* !BSP430! insert=platform_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_decl] */
/** @def BSP430_PLATFORM_EXP430F5438
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-$(instance)s">MSP-EXP430F5438</a> platform.
 * This causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430f5438.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430F5438 will be
 * defined for you.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430F5438
#define BSP430_PLATFORM_EXP430F5438 0
#endif /* BSP430_PLATFORM_EXP430F5438 */

#if defined(BSP430_DOXYGEN) || (BSP430_PLATFORM_EXP430F5438 - 0)
#include <bsp430/platform/exp430f5438.h>
#endif /* BSP430_PLATFORM_EXP430F5438 */

/** @def BSP430_PLATFORM_EXP430FR5739
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-$(instance)s">MSP-EXP430FR5739</a> platform.
 * This causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430fr5739.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430FR5739 will be
 * defined for you.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430FR5739
#define BSP430_PLATFORM_EXP430FR5739 0
#endif /* BSP430_PLATFORM_EXP430FR5739 */

#if defined(BSP430_DOXYGEN) || (BSP430_PLATFORM_EXP430FR5739 - 0)
#include <bsp430/platform/exp430fr5739.h>
#endif /* BSP430_PLATFORM_EXP430FR5739 */

/** @def BSP430_PLATFORM_EXP430FG4618
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-$(instance)s">MSP-EXP430FG4618</a> platform.
 * This causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430fg4618.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430FG4618 will be
 * defined for you.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430FG4618
#define BSP430_PLATFORM_EXP430FG4618 0
#endif /* BSP430_PLATFORM_EXP430FG4618 */

#if defined(BSP430_DOXYGEN) || (BSP430_PLATFORM_EXP430FG4618 - 0)
#include <bsp430/platform/exp430fg4618.h>
#endif /* BSP430_PLATFORM_EXP430FG4618 */

/** @def BSP430_PLATFORM_EXP430G2
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-$(instance)s">MSP-EXP430G2</a> platform.
 * This causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430g2.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430G2 will be
 * defined for you.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430G2
#define BSP430_PLATFORM_EXP430G2 0
#endif /* BSP430_PLATFORM_EXP430G2 */

#if defined(BSP430_DOXYGEN) || (BSP430_PLATFORM_EXP430G2 - 0)
#include <bsp430/platform/exp430g2.h>
#endif /* BSP430_PLATFORM_EXP430G2 */

/* END AUTOMATICALLY GENERATED CODE [platform_decl] */
/* !BSP430! end=platform_decl */

#endif /* BSP430_PLATFORM_H */
