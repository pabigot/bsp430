/* Copyright 2012-2014, Peter A. Bigot
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
 * @brief Common implementation of platform functions
 *
 * This file provides a customizable implementations of functions
 * like vBSP430platformInitialize_ni().  It is intended to be
 * included into platform-specific implementation files.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph/pmm.h>

#ifndef BSP430_PLATFORM_STANDARD_USE_INITIALIZE
/** Define to true value to include default vBSP430platformInitialize_ni() implementation */
#define BSP430_PLATFORM_STANDARD_USE_INITIALIZE 1
#endif /* BSP430_PLATFORM_STANDARD_USE_INITIALIZE */

#ifndef BSP430_PLATFORM_STANDARD_INITIALIZE_PROLOGUE
/** Define to a C statement to do platform-specific operations in
 * vBSP340platformInitialize_ni() after initializing watchdog but
 * before anything else. */
#define BSP430_PLATFORM_STANDARD_INITIALIZE_PROLOGUE do { } while (0)
#endif /* BSP430_PLATFORM_STANDARD_INITIALIZE_PROLOGUE */

#ifndef BSP430_PLATFORM_STANDARD_INITIALIZE_EPILOGUE
/** Define to a C statement to do platform-specific operations before
 * leaving vBSP340platformInitialize_ni(). */
#define BSP430_PLATFORM_STANDARD_INITIALIZE_EPILOGUE do { } while (0)
#endif /* BSP430_PLATFORM_STANDARD_INITIALIZE_EPILOGUE */

#ifndef BSP430_PLATFORM_STANDARD_SET_CORE_VOLTAGE
/** Define to a C statement to do platform-specific voltage initialization.
 *
 * On MCUs with the @link bsp430/periph/pmm.h PMM@endlink module it is
 * recommended to adjust the processor core voltage prior to setting
 * the clock above a certain frequency.  Based on clock ranges
 * specified in the MCU data sheet the platform header file should
 * define this macro to invoke iBSP430pmmSetCoreVoltageLevel_ni() with
 * the necessary level. */
#define BSP430_PLATFORM_STANDARD_SET_CORE_VOLTAGE do { } while (0)
#endif /* BSP430_PLATFORM_STANDARD_SET_CORE_VOLTAGE */

/** @cond DOXYGEN_EXCLUDE */
#if (BSP430_PLATFORM_STANDARD_USE_INITIALIZE - 0)
void
vBSP430platformInitialize_ni (void)
{
#if (BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG - 0)
  /* Hold off watchdog */
  WDTCTL = WDTPW | WDTHOLD;
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

  BSP430_PLATFORM_STANDARD_INITIALIZE_PROLOGUE;

#if (BSP430_PLATFORM_BOOT_CONFIGURE_LEDS - 0) && (BSP430_LED - 0)
  vBSP430ledInitialize_ni();
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LEDS */

  /* Enable XT1 functions if requested or if using a clock module
   * where they are normally enabled on power-up.  (In the latter
   * case, the attempt to enable them is necessary to ensure that
   * BSP430's infrastructure can accurately detect the absence of a
   * crystal). */
#if ((BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 - 0) \
     || (BSP430_MODULE_BC2 - 0))
  (void)iBSP430clockConfigureLFXT1_ni(1, (BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC * BSP430_CLOCK_PUC_MCLK_HZ) / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_LFXT1 */

  /* Enable XT2 functions if requested */
#if (BSP430_PLATFORM_BOOT_CONFIGURE_XT2 - 0)
  (void)iBSP430clockConfigureXT2_ni(1, (BSP430_PLATFORM_BOOT_LFXT1_DELAY_SEC * BSP430_CLOCK_PUC_MCLK_HZ) / BSP430_CLOCK_LFXT1_STABILIZATION_DELAY_CYCLES);
#endif

  /* Adjust core voltage to support high-speed clocks */
#if (BSP430_PMM_SUPPORTS_COREV - 0)
  (void)iBSP430pmmSetCoreVoltageLevel_ni(BSP430_PMM_COREV_FOR_MCLK(BSP430_CLOCK_NOMINAL_MCLK_HZ));
#endif /* BSP430_PMM_SUPPORTS_COREV */

  /* Configure clocks.  Start with MCLK, which may need to use ACLK or
   * SMCLK during trimming operations.  Then finish up with SMCLK and
   * ACLK. */
#if (BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS - 0)
  ulBSP430clockConfigureMCLK_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ);
#if (configBSP430_CORE_DISABLE_FLL - 0)
  __bis_status_register(SCG0);
#endif /* configBSP430_CORE_DISABLE_FLL */
  iBSP430clockConfigureSMCLK_ni(BSP430_PLATFORM_BOOT_SMCLK_SOURCE, BSP430_PLATFORM_BOOT_SMCLK_DIVIDING_SHIFT);
  iBSP430clockConfigureACLK_ni(BSP430_PLATFORM_BOOT_ACLK_SOURCE, BSP430_PLATFORM_BOOT_ACLK_DIVIDING_SHIFT);
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS */

#if (BSP430_UPTIME - 0)
  vBSP430uptimeStart_ni();
#endif /* BSP430_UPTIME */

  BSP430_PLATFORM_STANDARD_INITIALIZE_EPILOGUE;
}
#endif /* BSP430_PLATFORM_STANDARD_USE_INITIALIZE */

/** @endcond */

/*
 * Local Variables:
 * mode:c
 * End:
 */
