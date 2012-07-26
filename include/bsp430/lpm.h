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
 * @brief Support for low power mode execution.
 *
 * #SCG1 : Status register bit that, when set, turns off the DCO generator if
 * DCOCLK is not used for MCLK or SMCLK.  It is set in LPM modes 2, 3,
 * and 4.
 *
 * #SCG0 : Status register bit that, when set, turns off FLL loop
 * control.  It is set in LPM modes 1, 3, and 4.
 *
 * #OSCOFF : Status register bit that, when set, turns off the LFXT1
 * crystal oscillator when LFXT1CLK is not used for MCLK or SMCLK.  It
 * is set in LPM mode 4 and in LPMx.5.
 *
 * #CPUOFF : Status register bit that, when set, turns off the CPU.  It
 * is set in all LPM modes.
 *
 * #GIE : Status register bit that, when set, enables maskable
 * interrrupts.  It should be set in any LPM mode for which exit is
 * signalled by an interrupt (which is most of them).  It may be left
 * set on wakeup if interrupts are normally disabled in the main
 * application.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_LPM_H
#define BSP430_LPM_H

#include <bsp430/common.h>
#include <msp430.h>

/** Bitmask isolating LPM-related bits recorded in status register.
 *
 * These are #SCG1, #SCG0, #OSCOFF, #CPUOFF, and (just in case) #GIE.
 * Other bits are eliminated from any LPM bit value before mutating a
 * status register setting. */
#define BSP430_LPM_SR_MASK (SCG1 | SCG0 | OSCOFF | CPUOFF | GIE)

/** Bit indicating that LPMx.5 should be entered.
 *
 * This bit is set along with #LPM3_bits or #LPM4_bits to define a low
 * power mode. */
#define BSP430_LPM_LPM5_BIT 0x1000

/** Bit indicating that the scheduler should be suspended.
 *
 * This bit may be set along with the other LPM bits to indicate that
 * the infrastructure should disable the periodic tick that controls
 * task switching.  It should only be set when it is known that all
 * tasks are woken only as a result of an external interrupt.
 *
 * In FreeRTOS, this bit should not be set if the application uses a
 * blocking semaphore or queue operation with a timeout specified as
 * something other than @c portMAX_DELAY.
 *
 * @note This only disables the interval interrupt that wakes the
 * scheduler.  It does not inhibit the low frequency clock counters
 * that retain time-since-boot. */
#define BSP430_LPM_SUSPEND_BIT 0x2000

/** Reset all port pins for low power applications.
 *
 * This function should be invoked at application power-up so that
 * only peripheral pins required by the application are in a state
 * that consumes energy.  Without doing this, connected pins tend to
 * float and keep the MCU power consumption much higher than it should
 * be.
 * 
 * All digital I/O ports on the MCU (PORTA through PORTJ or their
 * 8-bit equivalents, PORT1 through PORT12) are configured to their
 * i/o function, output direction, and outputs set to zero.
 */
void vBSP430lpmConfigurePortsForLPM_ni ();

#endif /* BSP430_LPM_H */
