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
 * @brief Hardware presentation/abstraction for SYS peripheral (SYS).
 *
 * The System Control Module (SYS) is present on 5xx/6xx/FR5xx
 * devices.
 *
 * @section h_periph_sys_opt Module Configuration Options
 *
 * None supported.
 *
 * @section h_periph_sys_hpl Hardware Presentation Layer
 *
 * As there can be only one instance of SYS on any MCU, there is no
 * structure supporting a SYS @HPL.  Manipulate the peripheral through its
 * registers directly.
 *
 * @section h_periph_sys_hal Hardware Adaptation Layer
 *
 * As there can be only one instance of SYS on any MCU, there is no
 * structure supporting a SYS @HAL.
 *
 * The functions provided by this module simplify the extraction of
 * reset causes, notification of type of reset, and translation from
 * MCU-specific reset codes to text cause descriptions.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 * @copyright <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_SYS_H
#define BSP430_PERIPH_SYS_H

#include <bsp430/periph.h>

/** @def BSP430_MODULE_SYS
 *
 * Defined on inclusion of <bsp430/periph/sys.h>.  The value evaluates
 * to true if the target MCU supports the System Control Module, and
 * false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_SYS defined(__MSP430_HAS_SYS__)

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_SYS - 0)

/** Flag indicating a BOR occurred during the last reset.
 *
 * Set in the output reset flags parameter by
 * #uiBSP430sysSYSRSTGenerator_ni when a reset cause with a priority at
 * least as high as security violation has been recorded. */
#define BSP430_SYS_FLAG_SYSRST_BOR 0x0001

/** Flag indicating an LPMx.5 wakeup occurred during the last reset.
 *
 * Set in the output reset flags parameter by
 * #uiBSP430sysSYSRSTGenerator_ni when SYSRSTIV_LPM5WU has been
 * recorded. */
#define BSP430_SYS_FLAG_SYSRST_LPM5WU 0x0002

/** Flag indicating a POR occurred during the last reset.
 *
 * Set in the output reset flags parameter by
 * #uiBSP430sysSYSRSTGenerator_ni when a reset cause with a priority at
 * least as high as PMM_DOPOR has been recorded. */
#define BSP430_SYS_FLAG_SYSRST_POR 0x0004

/** Flag indicating a PUC occurred during the last reset.
 *
 * Set in the output reset flags parameter by
 * #uiBSP430sysSYSRSTGenerator_ni when any reset cause was recorded. */
#define BSP430_SYS_FLAG_SYSRST_PUC 0x0008

/** Get the text describing the reset cause
 *
 * The integer value returned by uiBSP430sysSYSRSTGenerator_ni()
 * identifies an MCU-specific constant such as #SYSRSTIV_WDTTO, but
 * the value of that constant is different on different MCUs.  This
 * routine will map the integer value to the text description of the
 * reset cause.
 *
 * @param sysrstiv an integer reset cause value returned from
 * uiBSP430sysSYSRSTGenerator_ni()
 *
 * @return a text description of the reset cause that is being
 * returned, or a null pointer if @a sysrstiv is not recognized.
 */
const char * xBSP430sysSYSRSTIVDescription (unsigned int sysrstiv);

/** Generate the events recorded within the system reset vector.
 *
 * This routine can be used to determine the cause of a reset.  It can
 * also detect whether the reset involves a brownout reset (BOR),
 * which restores everything to its power-on condition, or is a result
 * of an LPMx.5 wakeup, or several other classes of reset.
 *
 * Recall that an MSP430 power on reset (POR) does not in fact return
 * all values to power on defaults, nor does a power up clear (PUC).
 * See vBSP430pmmInduceBOR().
 *
 * @param puiResetFlags Optional pointer to a variable that indicates
 * the class of reset causes encountered.  On initial call, the value
 * of the pointed-to variable should be zero; it is updated with
 * #BSP430_SYS_FLAG_SYSRST_BOR and related flags as each reset cause
 * is returned.
 *
 * @return A positive integer value corresponding to a SYSRST_type
 * supported on the current microcontroller, or zero if all such
 * values have been returned.  Values are even and constrained to be
 * no greater than 31. */
unsigned int uiBSP430sysSYSRSTGenerator_ni (unsigned int * puiResetFlags);

#endif /* BSP430_MODULE_SYS */

#endif /* BSP430_PERIPH_SYS_H */
