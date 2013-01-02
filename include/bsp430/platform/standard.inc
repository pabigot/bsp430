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

#ifndef BSP430_PLATFORM_STANDARD_INC
#define BSP430_PLATFORM_STANDARD_INC

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/led.h>

#ifndef BSP430_PLATFORM_STANDARD_USE_INITIALIZE
/** Define to true value to include default initialization routine */
#define BSP430_PLATFORM_STANDARD_USE_INITIALIZE 1
#endif /* BSP430_PLATFORM_STANDARD_USE_INITIALIZE */

#if (BSP430_PLATFORM_STANDARD_USE_INITIALIZE - 0)
void
vBSP430platformInitialize_ni (void)
{
#if BSP430_PLATFORM_BOOT_DISABLE_WATCHDOG - 0
  /* Hold off watchdog */
  WDTCTL = WDTPW | WDTHOLD;
#endif /* configBSP430_CORE_SUPPORT_WATCHDOG */

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

  /* Configure clocks.  Start with MCLK, which may need to use ACLK or
   * SMCLK during trimming operations.  Then finish up with SMCLK and
   * ACLK. */
#if BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS - 0
  ulBSP430clockConfigureMCLK_ni(BSP430_CLOCK_NOMINAL_MCLK_HZ);
#if configBSP430_CORE_DISABLE_FLL - 0
  __bis_status_register(SCG0);
#endif /* configBSP430_CORE_DISABLE_FLL */
  iBSP430clockConfigureSMCLK_ni(BSP430_PLATFORM_BOOT_SMCLK_SOURCE, BSP430_PLATFORM_BOOT_SMCLK_DIVIDING_SHIFT);
  iBSP430clockConfigureACLK_ni(BSP430_PLATFORM_BOOT_ACLK_SOURCE, BSP430_PLATFORM_BOOT_ACLK_DIVIDING_SHIFT);
#endif /* BSP430_PLATFORM_BOOT_CONFIGURE_CLOCKS */

#if BSP430_UPTIME - 0
  vBSP430uptimeStart_ni();
#endif /* BSP430_UPTIME */
}
#endif /* BSP430_PLATFORM_STANDARD_USE_INITIALIZE */

#endif /* BSP430_PLATFORM_STANDARD_INC */

/*
 * Local Variables:
 * mode:c
 * End:
 */
