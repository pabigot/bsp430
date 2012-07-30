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

#include <bsp430/utility/uptime.h>
#include <bsp430/periph/timer_.h>
#include <bsp430/platform.h>

#if BSP430_UPTIME - 0
/* Inhibit definition if required components were not provided. */

void
vBSP430uptimeStart_ni (void)
{
  BSP430_UPTIME_TIMER_HAL_HANDLE->timer->ctl = 0;
  vBSP430timerResetCounter_ni(BSP430_UPTIME_TIMER_HAL_HANDLE);
  BSP430_UPTIME_TIMER_HAL_HANDLE->timer->ctl =
    ((TASSEL0 | TASSEL1) & (BSP430_UPTIME_SSEL))
    | ((ID0 | ID1) & (BSP430_UPTIME_DIVIDING_SHIFT))
    | MC_2 | TACLR | TAIE;
}

void
vBSP430uptimeSuspend_ni (void)
{
  BSP430_UPTIME_TIMER_HAL_HANDLE->timer->ctl &= ~(MC0 | MC1);
}

void
vBSP430uptimeResume_ni (void)
{
  BSP430_UPTIME_TIMER_HAL_HANDLE->timer->ctl |= MC_2;
}

#endif /* BSP430_UPTIME */
