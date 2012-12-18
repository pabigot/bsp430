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
 * @brief Platform-specific BSP430 configuration directives for <a href="http://www.ti.com/tool/em430f5137rf900">EM430</a>
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PLATFORM_EM430_BSP430_CONFIG_H
#define BSP430_PLATFORM_EM430_BSP430_CONFIG_H

/* Use native USCI5 for genericized serial port unless told not to */
#ifndef configBSP430_SERIAL_USE_USCI5
#define configBSP430_SERIAL_USE_USCI5 1
#endif /* configBSP430_SERIAL_USE_USCI5 */

/* Enable buttons as requested */
#if ((configBSP430_PLATFORM_BUTTON0 - 0)        \
     || (configBSP430_PLATFORM_BUTTON1 - 0))
#if !defined(configBSP430_HAL_PORT1)
#define configBSP430_HAL_PORT1 1
#else /* configBSP430_HAL_PORT1 */
#if ! defined(configBSP430_HPL_PORT1)
#define configBSP430_HPL_PORT1 1
#endif /* BUTTON[01] */
#endif /* configBSP430_HAL_PORT1 */
#endif /* configBSP430_PLATFORM_BUTTON[01] */
#if ((configBSP430_PLATFORM_BUTTON2 - 0)        \
     || (configBSP430_PLATFORM_BUTTON3 - 0))
#if !defined(configBSP430_HAL_PORT2)
#define configBSP430_HAL_PORT2 1
#else /* configBSP430_HAL_PORT2 */
#if ! defined(configBSP430_HPL_PORT2)
#define configBSP430_HPL_PORT2 1
#endif /* BUTTON[23] */
#endif /* configBSP430_HAL_PORT2 */
#endif /* configBSP430_PLATFORM_BUTTON[23] */

/* What to use as a console */
/* !BSP430! module=console subst=module instance=nop */
/* !BSP430! insert=module_startif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_startif] */
#if ((configBSP430_CONSOLE - 0)                                    \
     && ((! defined(configBSP430_CONSOLE_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_CONSOLE_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [module_startif] */
/* !BSP430! end=module_startif */
#if !defined(configBSP430_HAL_USCI5_A0)
#define configBSP430_HAL_USCI5_A0 1
#endif /* configBSP430_HAL_USCI5_A0 */
/* !BSP430! insert=module_endif */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [module_endif] */
#endif /* configBSP430_CONSOLE && need default */
/* END AUTOMATICALLY GENERATED CODE [module_endif] */
/* !BSP430! end=module_endif */

/* How to use ACLK as a capture/compare input source */
/* !BSP430! module=timer feature=ccaclk timer=TA1 cc_index=2 ccis=1 clk:pp=2.0 */
/* !BSP430! insert=feature_startif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_startif] */
#if ((configBSP430_TIMER_CCACLK - 0)                                    \
     && ((! defined(configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE))    \
         || (configBSP430_TIMER_CCACLK_USE_DEFAULT_RESOURCE - 0)))
/* END AUTOMATICALLY GENERATED CODE [feature_startif] */
/* !BSP430! end=feature_startif */
/* !BSP430! insert=feature_ccaclk_cfg subst=timer,cc_index,ccis,clk_port,clk_pin */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_ccaclk_cfg] */
#if configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL - 0
#if !defined(configBSP430_HAL_TA1)
#define configBSP430_HAL_TA1 1
#endif /* configBSP430_HAL_TA1 */
#if configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR - 0
#if !defined(configBSP430_HAL_TA1_CC0_ISR)
#define configBSP430_HAL_TA1_CC0_ISR 1
#endif /* configBSP430_HAL_TA1_CC0_ISR */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_CC0_ISR */
#else /* configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL */
#if !defined(configBSP430_HPL_TA1)
#define configBSP430_HPL_TA1 1
#endif /* configBSP430_HPL_TA1 */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_TIMER_HAL */

#if configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL - 0
#if !defined(configBSP430_HAL_PORT2)
#define configBSP430_HAL_PORT2 1
#endif /* configBSP430_HAL_PORT2 */
#else /* configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL */
#if !defined(configBSP430_HPL_PORT2)
#define configBSP430_HPL_PORT2 1
#endif /* configBSP430_HPL_PORT2 */
#endif /* configBSP430_TIMER_CCACLK_USE_DEFAULT_PORT_HAL */
/* END AUTOMATICALLY GENERATED CODE [feature_ccaclk_cfg] */
/* !BSP430! end=feature_ccaclk_cfg */
/* !BSP430! insert=feature_endif subst=module,feature */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [feature_endif] */
#endif /* configBSP430_TIMER_CCACLK && need default */
/* END AUTOMATICALLY GENERATED CODE [feature_endif] */
/* !BSP430! end=feature_endif */

#endif /* BSP430_PLATFORM_EM430_BSP430_CONFIG_H */
