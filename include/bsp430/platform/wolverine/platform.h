/* Copyright (c) 2012, Peter A. Bigot
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
 * @brief Platform-specific include for MSP430FR5969 in a TS430RGZ48C carrier.
 *
 * This board is also known as the "Fraunchpad"
 *
 * The following platform-specific features are supported: <ul>
 *
 * <li> #BSP430_PERIPH_EXPOSED_CLOCKS Clocks share LED lines, and can
 * be measured at the test points below the LEDs.  Proceeding from the
 * rightmost LED: TP10 provides SMCLK, TP11 provides MCLK, and TP12
 * provides ACLK.
 *
 * </ul>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */
#ifndef BSP430_PLATFORM_WOLVERINE_PLATFORM_H
#define BSP430_PLATFORM_WOLVERINE_PLATFORM_H

/** Unconditionally define this, so as to produce errors if there is a
 * conflict in definition. */
#define BSP430_PLATFORM_WOLVERINE 1

/* No buttons */

/* One LED on P1.0.  Unpopulated LEDs on P1.1 and P1.2. */
#define BSP430_LED_GREEN 0
#define BSP430_LED_RED 1
#define BSP430_LED_GREEN1 2

/* What to use as a console */
/* !BSP430! module=console subst=module instance=nop */
/* !BSP430! insert=module_startif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_startif] */
#if ((configBSP430_CONSOLE - 0)                                    \
     && ((! defined(configBSP430_CONSOLE_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_CONSOLE_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [module_startif] */
/* !BSP430! end=module_startif */
#define BSP430_CONSOLE_SERIAL_PERIPH_HANDLE BSP430_PERIPH_EUSCI_A1
/* !BSP430! insert=module_endif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_endif] */
#endif /* configBSP430_CONSOLE && need default */
/* END AUTOMATICALLY GENERATED CODE [module_endif] */
/* !BSP430! end=module_endif */

/* How to use ACLK as a capture/compare input source */
/* !BSP430! module=timer feature=ccaclk timer=TA1 cc_index=2 ccis=1 clk_port=PORT1 clk_pin=BIT3 */
/* !BSP430! insert=feature_startif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_startif] */
#if ((configBSP430_TIMER_CCACLK - 0)                                    \
     && ((! defined(configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [feature_startif] */
/* !BSP430! end=feature_startif */
/* !BSP430! insert=feature_ccaclk_decl subst=timer,cc_index,ccis,clk_port,clk_pin */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_ccaclk_decl] */
#define BSP430_TIMER_CCACLK 1
#define BSP430_TIMER_CCACLK_PERIPH_HANDLE BSP430_PERIPH_TA1
#define BSP430_TIMER_CCACLK_CC_INDEX 2
#define BSP430_TIMER_CCACLK_CCIS CCIS_1
#define BSP430_TIMER_CCACLK_CLK_PORT_PERIPH_HANDLE BSP430_PERIPH_PORT1
#define BSP430_TIMER_CCACLK_CLK_PORT_BIT BIT3
/* END AUTOMATICALLY GENERATED CODE [feature_ccaclk_decl] */
/* !BSP430! end=feature_ccaclk_decl */
/* !BSP430! insert=feature_endif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_endif] */
#endif /* configBSP430_TIMER_CCACLK && need default */
/* END AUTOMATICALLY GENERATED CODE [feature_endif] */
/* !BSP430! end=feature_endif */

/* Include generic file, in case this is being included directly */
#include <bsp430/platform.h>

#endif /* BSP430_PLATFORM_WOLVERINE_H */
