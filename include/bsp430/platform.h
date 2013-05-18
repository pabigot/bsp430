/* Copyright 2012-2013, Peter A. Bigot
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
 * Further, if a recognized platform selection macro is defined to a
 * true value, the corresponding platform-specific header will be
 * included for you.  Platform-specific headers are included based on
 * the definitions of: <ul>
 * <li>#BSP430_PLATFORM_EXP430G2</a>
 * <li>#BSP430_PLATFORM_EXP430FG4618</a>
 * <li>#BSP430_PLATFORM_EXP430F5438</a>
 * <li>#BSP430_PLATFORM_EXP430F5529</a>
 * <li>#BSP430_PLATFORM_EXP430FR5739</a>
 * <li>#BSP430_PLATFORM_EM430</a>
 * <li>#BSP430_PLATFORM_RF2500T</a>
 * <li>#BSP430_PLATFORM_SURF</a>
 * <li>#BSP430_PLATFORM_TRXEB</a>
 * <li>#BSP430_PLATFORM_WOLVERINE</a>
 * <li>#BSP430_PLATFORM_CUSTOM</a>
 * </ul>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_H
#define BSP430_PLATFORM_H

#include <msp430.h>
#include <bsp430/periph.h>

/** Basic configuration for the platform.
 *
 * Applications should assume that invoking this in main() will
 * configure the system based on requested settings.  Most platforms
 * should use something like:
 *
 *
 * @platformdep
 */
void vBSP430platformInitialize_ni (void);

/** Basic configuration for the platform.
 *
 * This routine should:
 * @li Disable the watchdog (see #BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG)
 * @li Configure the LEDs (see #BSP430_PLATFORM_BOOT_CONFIGURE_LEDS)
 * @li Configure crystals (see #BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 and #BSP430_PLATFORM_BOOT_CONFIGURE_XT2)
 * @li Configure clocks (see #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS)
 * @li Start the system clock (see #BSP430_UPTIME)
 *
 * If your platform does not need to do anything special, you can
 * re-use the standard implementation of this function by including
 * this in your platform-specific @c platform.c file:
 *
 * @code
 * #include <bsp430/platform/standard.inc>
 * @endcode
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
 * @param periph Raw peripheral device for which pins should be
 * configured.
 *
 * @param periph_config Flag controlling which pins of the peripheral
 * will be configured.  A value of zero indicates a platform and
 * peripheral-specific default.  Generic flag values include
 * #BSP430_PERIPHCFG_SERIAL_UART, #BSP430_PERIPHCFG_SERIAL_I2C,
 * #BSP430_PERIPHCFG_SERIAL_SPI3, and #BSP430_PERIPHCFG_SERIAL_SPI4.
 *
 * @param enablep TRUE to enable for peripheral use; FALSE to disable
 * (return to general purpose IO).  When disabled, the direction and
 * output register bits are configured for low power use (output
 * zero).  Note that only the pins specified by @p periph_config will
 * be modified.
 *
 * @return 0 if configuration was successful, -1 if the device was not
 * recognized.
 */
int iBSP430platformConfigurePeripheralPins_ni (tBSP430periphHandle periph, int periph_config, int enablep);

/** @def configBSP430_PLATFORM_SPIN_FOR_JUMPER
 *
 * Define to a true value to indicate that the application or
 * infrastructure suggests use of the
 * vBSP430platformSpinForJumper_ni() function.
 *
 * This value represents an application or system request for the
 * feature; availability of the feature must be tested using
 * #BSP430_PLATFORM_SPIN_FOR_JUMPER.
 *
 * @cppflag
 * @affects #BSP430_PLATFORM_SPIN_FOR_JUMPER
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
 * @dependency #BSP430_PLATFORM_SPIN_FOR_JUMPER */
#if defined(BSP430_DOXYGEN) || (configBSP430_PLATFORM_SPIN_FOR_JUMPER - 0)
void vBSP430platformSpinForJumper_ni (void);
#endif /* configBSP430_PLATFORM_SPIN_FOR_JUMPER */

/** @def configBSP430_PLATFORM_PERIPHERAL_HELP
 *
 * Define to a true value if the application will want to invoke
 * xBSP430platformPeripheralHelp().
 *
 * @cppflag
 * @defaulted
 */
#ifndef configBSP430_PLATFORM_PERIPHERAL_HELP
#define configBSP430_PLATFORM_PERIPHERAL_HELP 0
#endif /* configBSP430_PLATFORM_PERIPHERAL_HELP */

/** Provide peripheral-specific help strings for application integration
 *
 * For cross-platform applications it is helpful if the startup text
 * describes any external connections.  Since, for example, pins
 * associated with an I2C interface are platform-specific, that
 * information belongs in the platform rather than the application.
 *
 * This routine should be implemented on all platforms, and any
 * peripheral supported by vBSP430platformConfigurePeripheralPins_ni()
 * should produce a help string.
 *
 * @param periph Raw peripheral device for which help is desired
 *
 * @param periph_config Flag as passed to
 * iBSP430platformConfigurePeripheralPins_ni() as the @c periph_config
 * value.  A value of zero will describe all potential pins for the
 * peripheral.
 *
 * @return A pointer to a help string suitable for display.  A null
 * pointer is returned if the peripheral was not enabled.
 *
 * @dependency #BSP430_PLATFORM_PERIPHERAL_HELP
 */
const char * xBSP430platformPeripheralHelp (tBSP430periphHandle periph, int periph_config);

#ifndef configBSP430_PLATFORM_RF
/** Define to a true value to enable any built-in radio platform.
 *
 * For example, this enables #configBSP430_RF_RF2500T when on
 * #BSP430_PLATFORM_RF2500T.
 *
 * Platforms reference this macro only when there is a hard-wired
 * radio.  For pluggable radios specific defines like
 * #configBSP430_RF_CC2500EMK or #configBSP430_RF_ANAREN_CC110L must
 * be used.
 *
 * @cppflag
 * @defaulted
 */
#define configBSP430_PLATFORM_RF 0
#endif /* configBSP430_PLATFORM_RF */

/** @def configBSP430_PLATFORM_BUTTON0
 *
 * Define to a true value if the application will want to use the
 * primary button (if any) supported by a platform.
 *
 * @note If the platform supports more than one button, the
 * corresponding macros (e.g., @c configBSP430_PLATFORM_BUTTON1) are
 * also defined with default values of 0, up to button 3.
 *
 * @cppflag
 * @affects #BSP430_PLATFORM_BUTTON0
 */
#ifndef configBSP430_PLATFORM_BUTTON0
#define configBSP430_PLATFORM_BUTTON0 0
#endif /* configBSP430_PLATFORM_BUTTON0 */

/** @def BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE
 *
 * A platform-specific identification of a port device handle through
 * which the platform's primary button is controlled.
 *
 * The value is defined if the platform supports a button, regardless
 * of whether #configBSP430_PLATFORM_BUTTON0 is set.  The latter must
 * be set in order for the HAL interface to the button to be enabled
 * automatically.
 *
 * @note If the platform supports more than one button, the
 * corresponding macros (e.g., @c
 * BSP430_PLATFORM_BUTTON1_PORT_PERIPH_HANDLE) may also be defined, up
 * to button 3.
 *
 * @platformdefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_PLATFORM_BUTTON0_PORT_BIT
 *
 * A platform-specific identification of the bit on
 * #BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE that is associated with
 * the platform's primary button.
 *
 * The value is defined if the platform supports a button, regardless
 * of whether #configBSP430_PLATFORM_BUTTON0 is set.  The latter must
 * be set in order for the HAL interface to the button to be enabled
 * automatically.
 *
 * @note If the platform supports more than one button, the
 * corresponding macros (e.g., @c BSP430_PLATFORM_BUTTON1_PORT_BIT)
 * may also be defined, up to button 3.
 *
 * @platformdefault */
#if defined(BSP430_DOXYGEN)
#define BSP430_PLATFORM_BUTTON0_PORT_BIT include <bsp430/platform.h>
#endif /* BSP430_DOXYGEN */

/** @def BSP430_PLATFORM_BUTTON0
 *
 * Defined by the infrastructure if #configBSP430_PLATFORM_BUTTON0 is
 * true.  The defined value evaluates to true if the button is
 * supported by the platform, and to false if the button is not
 * supported.
 *
 * @note If the platform supports more than one button, the
 * corresponding macros (e.g., @c BSP430_PLATFORM_BUTTON1) are
 * also defined, up to button 3.
 *
 * @cppflag
 * @dependency #configBSP430_PLATFORM_BUTTON0 */
#if defined(BSP430_DOXYGEN)
#define BSP430_PLATFORM_BUTTON0 include <bsp430/platform.h>
#endif /* configBSP430_PLATFORM_BUTTON0 */


/* !BSP430! tool=msp subst=tool instance=exp430f5438,exp430f5529,exp430fr5739,exp430fg4618,exp430g2 */
/* !BSP430! insert=platform_decl */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_decl] */
/** @def BSP430_PLATFORM_EXP430F5438
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430f5438">MSP-EXP430F5438</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430f5438/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430F5438 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430f5438/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430F5438
#define BSP430_PLATFORM_EXP430F5438 0
#endif /* BSP430_PLATFORM_EXP430F5438 */

#if BSP430_PLATFORM_EXP430F5438 - 0
#include <bsp430/platform/exp430f5438/platform.h>
#endif /* BSP430_PLATFORM_EXP430F5438 */

/** @def BSP430_PLATFORM_EXP430F5529
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430f5529">MSP-EXP430F5529</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430f5529/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430F5529 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430f5529/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430F5529
#define BSP430_PLATFORM_EXP430F5529 0
#endif /* BSP430_PLATFORM_EXP430F5529 */

#if BSP430_PLATFORM_EXP430F5529 - 0
#include <bsp430/platform/exp430f5529/platform.h>
#endif /* BSP430_PLATFORM_EXP430F5529 */

/** @def BSP430_PLATFORM_EXP430FR5739
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430fr5739">MSP-EXP430FR5739</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430fr5739/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430FR5739 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430fr5739/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430FR5739
#define BSP430_PLATFORM_EXP430FR5739 0
#endif /* BSP430_PLATFORM_EXP430FR5739 */

#if BSP430_PLATFORM_EXP430FR5739 - 0
#include <bsp430/platform/exp430fr5739/platform.h>
#endif /* BSP430_PLATFORM_EXP430FR5739 */

/** @def BSP430_PLATFORM_EXP430FG4618
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430fg4618">MSP-EXP430FG4618</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430fg4618/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430FG4618 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430fg4618/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430FG4618
#define BSP430_PLATFORM_EXP430FG4618 0
#endif /* BSP430_PLATFORM_EXP430FG4618 */

#if BSP430_PLATFORM_EXP430FG4618 - 0
#include <bsp430/platform/exp430fg4618/platform.h>
#endif /* BSP430_PLATFORM_EXP430FG4618 */

/** @def BSP430_PLATFORM_EXP430G2
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-exp430g2">MSP-EXP430G2</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/exp430g2/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_EXP430G2 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/exp430g2/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EXP430G2
#define BSP430_PLATFORM_EXP430G2 0
#endif /* BSP430_PLATFORM_EXP430G2 */

#if BSP430_PLATFORM_EXP430G2 - 0
#include <bsp430/platform/exp430g2/platform.h>
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
 * platform-specific header <bsp430/platform/rf2500t/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_RF2500T will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/rf2500t/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_RF2500T
#define BSP430_PLATFORM_RF2500T 0
#endif /* BSP430_PLATFORM_RF2500T */

#if BSP430_PLATFORM_RF2500T - 0
#include <bsp430/platform/rf2500t/platform.h>
#endif /* BSP430_PLATFORM_RF2500T */

/* END AUTOMATICALLY GENERATED CODE [platform_decl] */
/* !BSP430! end=platform_decl */
/* !BSP430! url=http://www.ti.com/tool/em430f5137rf900 subst=url instance=em430 */
/* !BSP430! insert=platform_decl_url */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_decl_url] */
/** @def BSP430_PLATFORM_EM430
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/em430f5137rf900">EM430</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/em430/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_EM430 will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/em430/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_EM430
#define BSP430_PLATFORM_EM430 0
#endif /* BSP430_PLATFORM_EM430 */

#if BSP430_PLATFORM_EM430 - 0
#include <bsp430/platform/em430/platform.h>
#endif /* BSP430_PLATFORM_EM430 */

/* END AUTOMATICALLY GENERATED CODE [platform_decl_url] */
/* !BSP430! end=platform_decl_url */
/* !BSP430! url=http://en.wikipedia.org/wiki/HTTP_404 subst=url instance=surf */
/* !BSP430! insert=platform_decl_url */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_decl_url] */
/** @def BSP430_PLATFORM_SURF
 * Define to a true value if application is being built for the
 * <a href="http://en.wikipedia.org/wiki/http_404">SURF</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/surf/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_SURF will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/surf/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_SURF
#define BSP430_PLATFORM_SURF 0
#endif /* BSP430_PLATFORM_SURF */

#if BSP430_PLATFORM_SURF - 0
#include <bsp430/platform/surf/platform.h>
#endif /* BSP430_PLATFORM_SURF */

/* END AUTOMATICALLY GENERATED CODE [platform_decl_url] */
/* !BSP430! end=platform_decl_url */
/* !BSP430! url=http://www.ti.com/tool/cc1125dk subst=url instance=trxeb */
/* !BSP430! insert=platform_decl_url */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_decl_url] */
/** @def BSP430_PLATFORM_TRXEB
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/cc1125dk">TRXEB</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/trxeb/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_TRXEB will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/trxeb/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_TRXEB
#define BSP430_PLATFORM_TRXEB 0
#endif /* BSP430_PLATFORM_TRXEB */

#if BSP430_PLATFORM_TRXEB - 0
#include <bsp430/platform/trxeb/platform.h>
#endif /* BSP430_PLATFORM_TRXEB */

/* END AUTOMATICALLY GENERATED CODE [platform_decl_url] */
/* !BSP430! end=platform_decl_url */
/* !BSP430! url=http://www.ti.com/tool/msp-fet430u48c subst=url instance=wolverine */
/* !BSP430! insert=platform_decl_url */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [platform_decl_url] */
/** @def BSP430_PLATFORM_WOLVERINE
 * Define to a true value if application is being built for the
 * <a href="http://www.ti.com/tool/msp-fet430u48c">WOLVERINE</a> platform.
 *
 * A true value causes <bsp430/platform.h> to include the corresponding
 * platform-specific header <bsp430/platform/wolverine/platform.h>.
 * If you include that header directly, #BSP430_PLATFORM_WOLVERINE will be
 * defined for you.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/wolverine/bsp430_config.h> for you.
 * You should not include that header directly, as it coordinates with
 * the generic platform version.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_WOLVERINE
#define BSP430_PLATFORM_WOLVERINE 0
#endif /* BSP430_PLATFORM_WOLVERINE */

#if BSP430_PLATFORM_WOLVERINE - 0
#include <bsp430/platform/wolverine/platform.h>
#endif /* BSP430_PLATFORM_WOLVERINE */

/* END AUTOMATICALLY GENERATED CODE [platform_decl_url] */
/* !BSP430! end=platform_decl_url */

/** @def BSP430_PLATFORM_CUSTOM

 * Define to a true value if application is being built for a custom
 * (out-of-tree) platform.
 *
 * A true value causes <bsp430/platform.h> to include  corresponding
 * platform-specific header <bsp430/platform/custom/platform.h>.
 *
 * A true value also causes <bsp430/platform/bsp430_config.h> to
 * include <bsp430/platform/custom/bsp430_config.h> for you.
 *
 * Custom platform files should not be placed within the BSP430 source
 * or install area, but are included using a path that makes them
 * appear to be there.  This is primarily to avoid any confusion due
 * to accidental resolution of an unadorned include of @c "platform.h"
 * or @c "bsp430_config.h" to the wrong file.  It is the
 * responsibility of the application developer to ensure that the
 * include directories provided to the compiler allow the custom
 * platform headers to be located at the necessary paths.
 *
 * Alternatively, you could have your application include the custom
 * platform files directly and bypass any reference to
 * <bsp430/platform.h> or <bsp430/platform/bsp430_config.h>.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_CUSTOM
#define BSP430_PLATFORM_CUSTOM 0
#endif /* BSP430_PLATFORM_CUSTOM */

#if BSP430_PLATFORM_CUSTOM - 0
#include <bsp430/platform/custom/platform.h>
#endif /* BSP430_PLATFORM_CUSTOM */

/* If configBSP430_TIMER_CCACLK was requested, then mark the feature as
 * available or not based on whether the platform provided a serial
 * peripheral for the console's use. */
#if configBSP430_TIMER_CCACLK - 0
#if BSP430_PERIPH_CPPID_NONE != BSP430_TIMER_CCACLK_PERIPH_CPPID
#define BSP430_TIMER_CCACLK 1
#else /* BSP430_TIMER_CCACLK_PERIPH_CPPID */
#define BSP430_TIMER_CCACLK 0
#endif /* BSP430_TIMER_CCACLK_PERIPH_CPPID */
#endif /* configBSP430_TIMER_CCACLK */

/* If configBSP430_UCS_TRIM_DCOCLKDIV was requested and the platform
 * supports BSP430_TIMER_CCACLK, then mark the feature as
 * available. */
#if configBSP430_UCS_TRIM_DCOCLKDIV - 0
#if BSP430_TIMER_CCACLK - 0
#define BSP430_UCS_TRIM_DCOCLKDIV 1
#else /* BSP430_TIMER_CCACLK */
#define BSP430_UCS_TRIM_DCOCLKDIV 0
#endif /* BSP430_TIMER_CCACLK */
#endif /* configBSP430_UCS_TRIM_DCOCLKDIV */

/* If configBSP430_BC2_TRIM_TO_MCLK was requested and the platform
 * supports BSP430_TIMER_CCACLK, then mark the feature as
 * available. */
#if configBSP430_BC2_TRIM_TO_MCLK - 0
#if BSP430_TIMER_CCACLK - 0
#define BSP430_BC2_TRIM_TO_MCLK 1
#else /* BSP430_TIMER_CCACLK */
#define BSP430_BC2_TRIM_TO_MCLK 0
#endif /* BSP430_TIMER_CCACLK */
#endif /* configBSP430_BC2_TRIM_TO_MCLK */

/* If configBSP430_CONSOLE was requested, then mark the feature as
 * available or not based on whether the platform provided a serial
 * peripheral for the console's use. */
#if configBSP430_CONSOLE - 0
#if BSP430_PERIPH_CPPID_NONE != BSP430_CONSOLE_SERIAL_PERIPH_CPPID
#define BSP430_CONSOLE 1
#else /* BSP430_CONSOLE_SERIAL_PERIPH_CPPID */
#define BSP430_CONSOLE 0
#endif /* BSP430_CONSOLE_SERIAL_PERIPH_CPPID */
#endif /* configBSP430_CONSOLE */

/* If configBSP430_UPTIME was requested, then mark the feature as
 * available or not based on whether the platform provided a timer for
 * its use. */
#if (configBSP430_UPTIME - 0) && !defined(BSP430_UPTIME)
#if BSP430_PERIPH_CPPID_NONE != BSP430_UPTIME_TIMER_PERIPH_CPPID
#define BSP430_UPTIME 1
#else /* BSP430_UPTIME_TIMER_PERIPH_CPPID */
#define BSP430_UPTIME 0
#endif /* BSP430_UPTIME_TIMER_PERIPH_CPPID */
#endif /* configBSP430_UPTIME */

/* If configBSP430_PLATFORM_BUTTON0 was requested, then mark the
 * feature as available or not based on whether the platform provided
 * a peripheral handle for the button's use.
 *
 * NOTE: This file is responsible only for processing button 0.
 * Additional buttons must be handled in the platform-specific
 * header. */
#if configBSP430_PLATFORM_BUTTON0 - 0
#ifdef BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE
#define BSP430_PLATFORM_BUTTON0 1
#else /* BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE */
#define BSP430_PLATFORM_BUTTON0 0
#endif /* BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE */
#endif /* configBSP430_PLATFORM_BUTTON0 */

/* If configBSP430_PLATFORM_M25P was requested, then mark the feature
 * as available or not based on whether the platform provided a
 * peripheral handle to use to access the device. */
#if configBSP430_PLATFORM_M25P - 0
#ifdef BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE
#define BSP430_PLATFORM_M25P 1
#else /* BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE */
#define BSP430_PLATFORM_M25P 0
#endif /* BSP430_PLATFORM_M25P_SPI_PERIPH_HANDLE */
#endif /* configBSP430_PLATFORM_M25P */

/* Set other M25P defaults */
#if (BSP430_PLATFORM_M25P - 0)
#endif /* BSP430_PLATFORM_M25P */

/** @def BSP430_PLATFORM_SPIN_FOR_JUMPER
 *
 * Defined to indicate that the application or infrastructure supports
 * use of the vBSP430platformSpinForJumper_ni() function.  The value
 * is defined only if #configBSP430_PLATFORM_SPIN_FOR_JUMPER is set,
 * and is true only if the platform supports
 * vBSP430platformSpinForJumper_ni().
 *
 * @platformdefault */
#if defined(BSP430_DOXYGEN) || defined(configBSP430_PLATFORM_SPIN_FOR_JUMPER)
#ifndef BSP430_PLATFORM_SPIN_FOR_JUMPER
#define BSP430_PLATFORM_SPIN_FOR_JUMPER 0 /* False unless platform explicitly enabled it */
#endif /* BSP430_PLATFORM_SPIN_FOR_JUMPER */
#endif /* BSP430_DOXYGEN */

/** @def BSP430_PLATFORM_PERIPHERAL_HELP
 *
 * Defined to indicate that the application or infrastructure supports
 * use of the vBSP430platformSpinForJumper_ni() function.  The value
 * is defined only if #configBSP430_PLATFORM_PERIPHERAL_HELP is set,
 * and is true only if the platform supports
 * xBSP430platformPeripheralHelp().
 *
 * @dependency #configBSP430_PLATFORM_PERIPHERAL_HELP
 * @platformdefault */
#if defined(BSP430_DOXYGEN) || defined(configBSP430_PLATFORM_PERIPHERAL_HELP)
#ifndef BSP430_PLATFORM_PERIPHERAL_HELP
#define BSP430_PLATFORM_PERIPHERAL_HELP 1 /* True unless platform explicitly disabled it */
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
#endif /* BSP430_DOXYGEN */

/** @def BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG
 *
 * If defined to a true value vBSP430platformInitialize_ni() will set
 * the watchdog infrastructure into a reset state prior to any other
 * activities.
 *
 * If #configBSP430_CORE_SUPPORT_WATCHDOG is true then this is
 * defaulted to false; otherwise it is defaulted to true.
 *
 * @warning If #configBSP430_CORE_SUPPORT_WATCHDOG is true and
 * #BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG is also true, the watchdog
 * will be disabled, but subsequent invocations of
 * #BSP430_CORE_WATCHDOG_CLEAR() are likely to re-enable it, possibly
 * before control returns from vBSP430platformInitialize_ni().
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG
#define BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG (! (configBSP430_CORE_SUPPORT_WATCHDOG - 0))
#endif /* BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG */

/** @def BSP430_PLATFORM_BOOT_CONFIGURE_LEDS
 *
 * If defined to a true value and #BSP430_LED is also true,
 * vBSP430platformInitialize_ni() will invoke
 * vBSP430ledInitialize_ni() prior to the crystal stabilization phase
 * of platform initialization.
 *
 * @see #BSP430_LED
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_CONFIGURE_LEDS
#if defined(BSP430_PLATFORM_BOOT_CONFIGURE_LED)
#warning BSP430_PLATFORM_BOOT_CONFIGURE_LEDS is not spelled BSP430_PLATFORM_BOOT_CONFIGURE_LED
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LED */
#define BSP430_PLATFORM_BOOT_CONFIGURE_LEDS 1
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LEDS */

/** @def BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS
 *
 * If defined to a true value, vBSP430platformInitialize_ni() will
 * configure the MCLK, SMCLK, and ACLK clock infrastructure.
 *
 * If defined to a false value, vBSP430platformInitialize_ni() will
 * leave the clocks in their power-up configuration.
 *
 * @see #BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1
 * @see #BSP430_PLATFORM_BOOT_CONFIGURE_XT2
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
 * If defined to a false value, platforms which power-up with the crystal
 * pins configured for their peripheral mode (specifically, 2xx family
 * devices including the @link bsp430/platform/exp430g2/platform.h
 * EXP430G2 "Launchpad" @endlink), should:
 * <ul>
 * <li> invoke iBSP430platformConfigurePeripheralPins_ni() to set the
 * #BSP430_PERIPH_LFXT1 pins to their I/O function.
 * </ul>
 * Other platforms may treat this as if
 * #BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 was left undefined.
 *
 * If #BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 is left undefined,
 * vBSP430platformInitialize_ni() will leave the crystal in its
 * power-up configuration.  For most clock peripherals this is a
 * faulted mode because the XIN/XOUT pins are reset to digital I/O
 * rather than peripheral function mode.  For 2xx-family devices the
 * pins are defaulted to peripheral function mode, and the @link
 * bsp430/periph/bc2.h BC2 @endlink configuration will normally use the
 * crystal if it is populated.
 *
 * @see #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1
#define BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 0
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 */

/** Same as #BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 except configures XT2
 *
 * If defined to a true value, vBSP430platformInitialize_ni() will
 * invoke iBSP430clockConfigureXT2_ni() to enable and stabilize the
 * crystal.
 *
 * This has no effect if XT2 is not supported on the platform.
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_CONFIGURE_XT2
#define BSP430_PLATFORM_BOOT_CONFIGURE_XT2 0
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_XT2 */

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

/** The ACLK source configured at boot if #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS is true.
 *
 * On most MCUs, the power-up-clear default would be equivalent to
 * using #eBSP430clockSRC_XT1CLK.  However, on MCUs that use BC2 the
 * absence of a crystal will cause this setting to leave the LFXT1
 * fault present, which causes ulBSP430clockConfigureMCLK_ni() to hang
 * because it cannot distinguish that fault from a DCO fault.
 * Consequently, the BSP430 default will fall back to a non-crystal
 * source if the crystal is unstable.  This is particularly important
 * for pre-5xx MCUs where the peripheral pins associated with the
 * crystal power-up in their peripheral mode enabling the crystal.
 *
 * Also consider #BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 since 5xx/6xx
 * family MCUs do not automatically configure the peripheral pins to
 * enable XIN/XOUT to function even if the crystal is present.
 *
 * @see iBSP430clockConfigureACLK_ni()
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_ACLK_SOURCE
#define BSP430_PLATFORM_BOOT_ACLK_SOURCE eBSP430clockSRC_XT1CLK_FALLBACK
#endif /* BSP430_PLATFORM_BOOT_ACLK_SOURCE */

/** The ACLK dividing shift configured at boot if #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS is true.
 *
 * Several clock peripherals support dividing the input signal to
 * reduce jitter or slow the clock.  This option corresponds to the
 * DIVA field in such a peripheral.
 *
 * The option is ignored unless the clock peripheral supports dividing
 * the ACLK source.
 *
 * @see iBSP430clockConfigureACLK_ni()
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_ACLK_DIVIDING_SHIFT
#define BSP430_PLATFORM_BOOT_ACLK_DIVIDING_SHIFT 0
#endif /* BSP430_PLATFORM_BOOT_ACLK_DIVIDING_SHIFT */

/** The SMCLK source configured at boot if #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS is true.
 *
 * On most MCUs, the power-up-clear default will be equivalent having
 * SMCLK and MCLK run at the same rate.  In some cases it might be
 * desirable to use an external crystal as the source for SMCLK.
 *
 * @see iBSP430clockConfigureSMCLK_ni()
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_SMCLK_SOURCE
#define BSP430_PLATFORM_BOOT_SMCLK_SOURCE eBSP430clockSRC_SMCLK_PU_DEFAULT
#endif /* BSP430_PLATFORM_BOOT_SMCLK_SOURCE */

/** The SMCLK dividing shift configured at boot if #BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS is true.
 *
 * Several clock peripherals support dividing the input signal to
 * reduce jitter or slow the clock.  This option corresponds to the
 * DIVS field in such a peripheral.
 *
 * The option is ignored unless the clock peripheral supports dividing
 * the SMCLK source.
 *
 * @see iBSP430clockConfigureSMCLK_ni()
 *
 * @defaulted */
#ifndef BSP430_PLATFORM_BOOT_SMCLK_DIVIDING_SHIFT
#define BSP430_PLATFORM_BOOT_SMCLK_DIVIDING_SHIFT 0
#endif /* BSP430_PLATFORM_BOOT_SMCLK_DIVIDING_SHIFT */

#if (BSP430_RFEM - 0)
/* If using the RFEM feature, incorporate the header that maps RFEM
 * constants to specific EMK functions. */
#include <bsp430/rf/emk_platform.h>
#endif /* BSP430_RFEM */

#endif /* BSP430_PLATFORM_H */
