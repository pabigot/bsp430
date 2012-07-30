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
 * This header defines prototypes for functions that are implemented
 * in the platform-specific implementation file.
 *
 * Further, if a recognized @c BSP430_PLATFORM_foo macro is defined to
 * a true value, the corresponding platform-specific header will be
 * included for you.  Platform-specific headers are included based on
 * the definitions of:
 * <ul>
 * <li>#BSP430_PLATFORM_EXP430G2</a>
 * <li>#BSP430_PLATFORM_EXP430FG4619</a>
 * <li>#BSP430_PLATFORM_EXP430F5438</a>
 * <li>#BSP430_PLATFORM_EXP430F5529</a>
 * <li>#BSP430_PLATFORM_EXP430FR5739</a>
 * <li>#BSP430_PLATFORM_RF2500T</a>
 * </ul>
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
 * @li Crystal configuration (see #BSP430_PLATFORM_BOOT_CONFIGURE_CRYSTAL)
 * @li Clock configuration (see #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS)
 * @li Start the system clock (if #BSP430_UPTIME)
 */
void vBSP430platformInitialize_ni (void);

/** Configure the pins associated with a given peripheral.
 *
 * @note All platforms should support #BSP430_PERIPH_LFXT1.
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

/** @def configBSP430_PLATFORM_SPIN_FOR_JUMPER
 *
 * Define to a true value to indicate that the application or
 * infrastructure suggests use of the
 * vBSP430platformSpinForJumper_ni() function.
 *
 * This value represents an application or system request for the
 * feature; availability of the feature must be tested using
 * #BSP430_PLATFORM_SPIN_FOR_JUMP.
 *
 * @defaulted */
#ifndef configBSP430_PLATFORM_SPIN_FOR_JUMPER
#define configBSP430_PLATFORM_SPIN_FOR_JUMPER 0
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

/** Block until a platform-specific jumper has been removed.
 *
 * Several platforms that use a TIUSB device to provide serial port
 * emulation, including the MSP430 LaunchPad and the EXP430FR5739,
 * must not transmit any data while a driver is initializing,
 * especially on Linux systems.  We need a way to hold off an
 * installed application from writing to the serial port until that's
 * happened.  What we do is, if a platform-specific pin is grounded,
 * block until it's released.  When plugging in a device for the first
 * time, place a jumper connecting that pin to ground, wait for the
 * driver to load, then remove the jumper.
 *
 * The platform-specific header should document whether this function
 * is supported, and if it is what jumper configuration is recognized.
 *
 * @note This function will configure the LEDs and use them regardless
 * of other configuration.
 *
 * @note This function should only be referenced if
 * #BSP430_PLATFORM_SPIN_FOR_JUMPER is true. */
#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_SPIN_FOR_JUMPER - 0)
void vBSP430platformSpinForJumper_ni (void);
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

/* !BSP430! tool=msp subst=tool instance=exp430f5438,exp430fr5739,exp430fg4618,exp430g2 */
/* !BSP430! insert=platform_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_decl] */
/** @def BSP430_PLATFORM_EXP430F5438
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430f5438">MSP-EXP430F5438</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430f5438.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430F5438 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430f5438_bsp430_config.h> for you.
 * You should not include that header directly.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430F5438
#define BSP430_PLATFORM_EXP430F5438 0
#endif /* BSP430_PLATFORM_EXP430F5438 */

#if BSP430_PLATFORM_EXP430F5438 - 0
#include <bsp430/platform/exp430f5438.h>
#endif /* BSP430_PLATFORM_EXP430F5438 */

/** @def BSP430_PLATFORM_EXP430FR5739
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430fr5739">MSP-EXP430FR5739</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430fr5739.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430FR5739 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430fr5739_bsp430_config.h> for you.
 * You should not include that header directly.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430FR5739
#define BSP430_PLATFORM_EXP430FR5739 0
#endif /* BSP430_PLATFORM_EXP430FR5739 */

#if BSP430_PLATFORM_EXP430FR5739 - 0
#include <bsp430/platform/exp430fr5739.h>
#endif /* BSP430_PLATFORM_EXP430FR5739 */

/** @def BSP430_PLATFORM_EXP430FG4618
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430fg4618">MSP-EXP430FG4618</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430fg4618.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430FG4618 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430fg4618_bsp430_config.h> for you.
 * You should not include that header directly.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430FG4618
#define BSP430_PLATFORM_EXP430FG4618 0
#endif /* BSP430_PLATFORM_EXP430FG4618 */

#if BSP430_PLATFORM_EXP430FG4618 - 0
#include <bsp430/platform/exp430fg4618.h>
#endif /* BSP430_PLATFORM_EXP430FG4618 */

/** @def BSP430_PLATFORM_EXP430G2
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430g2">MSP-EXP430G2</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430g2.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430G2 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430g2_bsp430_config.h> for you.
 * You should not include that header directly.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430G2
#define BSP430_PLATFORM_EXP430G2 0
#endif /* BSP430_PLATFORM_EXP430G2 */

#if BSP430_PLATFORM_EXP430G2 - 0
#include <bsp430/platform/exp430g2.h>
#endif /* BSP430_PLATFORM_EXP430G2 */

/* END AUTOMATICALLY GENERATED CODE [platform_decl] */
/* !BSP430! end=platform_decl */
/* !BSP430! tool=ez430 subst=tool instance=rf2500t */
/* !BSP430! insert=platform_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_decl] */
/** @def BSP430_PLATFORM_RF2500T
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/ez430-rf2500t">EZ430-RF2500T</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/rf2500t.h>.
 * If you include that header directly, #BSP430_PLATFORM_RF2500T will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/rf2500t_bsp430_config.h> for you.
 * You should not include that header directly.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_RF2500T
#define BSP430_PLATFORM_RF2500T 0
#endif /* BSP430_PLATFORM_RF2500T */

#if BSP430_PLATFORM_RF2500T - 0
#include <bsp430/platform/rf2500t.h>
#endif /* BSP430_PLATFORM_RF2500T */

/* END AUTOMATICALLY GENERATED CODE [platform_decl] */
/* !BSP430! end=platform_decl */

/* If configBSP430_CLOCK_TRIM_FLL was requested, the feature is
 * supported in principle by the peripheral, and the platform supports
 * BSP430_TIMER_CCACLK, then mark the feature as available. */
#if configBSP430_CLOCK_TRIM_FLL - 0
#if defined(__MSP430_HAS_UCS__) || defined(__MSP430_HAS_USC_RF__)
#define BSP430_CLOCK_TRIM_FLL (BSP430_TIMER_CCACLK - 0)
#else /* BSP430_CLOCK_TRIM_FLL supported by clock peripheral */
#define BSP430_CLOCK_TRIM_FLL 0
#endif /* BSP430_CLOCK_TRIM_FLL supported by clock peripheral */
#endif /* configBSP430_CLOCK_TRIM_FLL */

/** @def BSP430_PLATFORM_SPIN_FOR_JUMPER
 *
 * Define to indicate that the application or infrastructure supports
 * use of the vBSP430platformSpinForJumper_ni() function.  The value
 * is defined only if #configBSP430_PLATFORM_SPIN_FOR_JUMPER is set,
 * and is true only if the platform supports
 * vBSP430platformSpinForJumper_ni().
 *
 * @platformdefault */
#if defined(BSP430_DOXYGEN) || defined(configBSP430_PLATFORM_SPIN_FOR_JUMPER)
#ifndef BSP430_PLATFORM_SPIN_FOR_JUMPER
#define BSP430_PLATFORM_SPIN_FOR_JUMPER 0
#endif /* BSP430_PLATFORM_SPIN_FOR_JUMPER */
#endif /* BSP430_DOXYGEN */

/** @def BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS
 *
 * If defined to a true value, vBSP430platformInitialize_ni() will:
 * <ul>
 * <li> configure ACLK to source from LFXT1 if a stable crystal is available
 * <li> invoke ulBSP430clockConfigureMCLK_Hz_ni() using #BSP430_CLOCK_NOMINAL_MCLK_HZ
 * <li> invoke iBSP430clockConfigureSMCLKDividingShift_ni() using #BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT
 * </ul>
 *
 * If defined to a false value, vBSP430platformInitialize_ni() will
 * leave the clocks in their power-up configuration.
 *
 * @see #BSP430_PLATFORM_BOOT_CONFIGURE_CRYSTAL
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS
#define BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS 1
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS */

/** @def BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1
 *
 * If defined to a true value, vBSP430platformInitialize_ni() will:
 * <ul>
 * <li> invoke iBSP430clockConfigureLFXT1_ni() to enable and stabilize the crystal
 * </ul>
 *
 * If defined to a false value, vBSP430platformInitialize_ni() will
 * leave the crystal in its power-up configuration.  For most clock
 * peripherals this is a faulted mode because the XIN/XOUT pins are
 * reset to digital I/O rather than peripheral function mode.
 *
 * @see #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 1
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 */

/** @def BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC
 *
 * The number of seconds that vBSP430platformInitialize_ni() should
 * use in calculating the loop count parameter to
 * iBSP430clockConfigureLFXT1_ni() while attempting to stabilize the
 * clock.
 *
 * Set this to zero on platforms that don't have a crystal.
 *
 * @see #BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC
#define BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC 1
#endif /* BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC */

#endif /* BSP430_PLATFORM_H */
