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
 * @brief BSP430 scheduler implementation for FreeRTOS
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/periph/timer.h>
#include "FreeRTOS.h"

#if (configBSP430_HAL_TA0_CC0_ISR - 0)
#error FreeRTOS override reserves TA0 CC0 for internal use
#endif /* configBSP430_HAL_TA0_CC0_ISR */

#define TIMESLICE_DURATION_tck (portACLK_FREQUENCY_HZ / configTICK_RATE_HZ)

void
vBSP430freertosSuspendScheduler (void)
{
  BSP430_HPL_TA0->cctl0 &= ~(CCIE | CCIFG);
}

void
vBSP430freertosResumeScheduler (void)
{
  BSP430_HPL_TA0->ccr0 = TIMESLICE_DURATION_tck + uiBSP430timerSafeCounterRead_ni(BSP430_HPL_TA0);
  BSP430_HPL_TA0->cctl0 |= CCIE;
}

portBASE_TYPE
xPortStartScheduler (void)
{
  /* If TA0 appears to be stopped, start it driving from ACLK in
   * continuous mode. */
  if (0 == ((MC0 | MC1) & BSP430_HPL_TA0->ctl)) {
    BSP430_HPL_TA0->ctl = TASSEL_1 | MC_2 | TACLR;
  }
  vBSP430freertosResumeScheduler();
  portRESTORE_CONTEXT();
  return pdTRUE;
}

void vPortEndScheduler (void)
{
  vBSP430freertosSuspendScheduler();
}

static void
__attribute__((__interrupt__(TIMER0_A0_VECTOR)))
#if configUSE_PREEMPTION == 1
__attribute__((__naked__))
#endif /* configUSE_PREEMPTION */
scheduler_isr (void)
{
#if configUSE_PREEMPTION == 1
  portSAVE_CONTEXT();
#endif /* configUSE_PREEMPTION */

  BSP430_HPL_TA0->ccr0 += TIMESLICE_DURATION_tck;
  vTaskIncrementTick();

#if configUSE_PREEMPTION == 1
  vTaskSwitchContext();
  portRESTORE_CONTEXT();
#endif /* configUSE_PREEMPTION */
}
