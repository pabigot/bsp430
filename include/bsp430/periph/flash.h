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
 * @brief Hardware presentation/abstraction for flash memory peripheral (FLASH).
 *
 * A flash memory peripherals is available on almost every MSP430 MCU,
 * and may be identified as either FLASH or FLASH2.  Different MCUs in
 * the same family are one or the other.  The differences between
 * these peripherals are visible in the interface provided by this
 * module.
 *
 * @section h_periph_flash_opt Module Configuration Options
 *
 * None supported.
 *
 * @section h_periph_flash_hpl Hardware Presentation Layer
 *
 * As there can be only one instance of the flash memory peripheral on
 * any MCU, there is no structure supporting a FLASH @HPL.  Manipulate
 * the peripheral through its registers directly.
 *
 * @section h_periph_flash_hal Hardware Adaptation Layer
 *
 * As there can be only one instance of FLASH on any MCU, there is no
 * structure supporting a FLASH @HAL.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_PERIPH_FLASH_H
#define BSP430_PERIPH_FLASH_H

#include <bsp430/periph.h>

/** Defined on inclusion of <bsp430/periph/flash.h>.  The value
 * evaluates to true if the target MCU supports the Flash Memory
 * module, and false if it does not.
 *
 * @cppflag
 */
#define BSP430_MODULE_FLASH (defined(__MSP430_HAS_FLASH__) || defined(__MSP430_HAS_FLASH2__))

#if defined(BSP430_DOXYGEN) || (BSP430_MODULE_FLASH - 0)

/** Erase the flash segment holding the given address
 *
 * The MSP430 flash segment erase function is invoked.
 *
 * @note This function is not responsible for managing either #LOCKA
 * or #LOCKINFO.  If you wish to erase or modify information memory
 * segment A, you will have to clear #LOCKA.  Similarly if you have
 * set #LOCKINFO and wish to modify any information memory segment,
 * you will have to do so yourself.
 *
 * @note This function does not disable the watchdog, but does reset
 * it prior to invoking the erase operation.  If the duration of the
 * erase operation exceeds the watchdog timeout, the board may reset.
 *
 * @param addr an address within the segment to be erased
 *
 * @return 0 if the erase was successful, or a negative error code.
 */
int iBSP430flashEraseSegment_ni (const void * addr);

/** Copy data into flash memory
 *
 * Rather like <c>memcpy()</c>, but the destination is assumed to be
 * in a flash memory segment.  The write must not cross a segment
 * boundary.
 *
 * @note This function is not responsible for managing either #LOCKA
 * or #LOCKINFO.  If you wish to erase or modify information memory
 * segment A, you will have to clear #LOCKA.  Similarly if you have
 * set #LOCKINFO and wish to modify any information memory segment,
 * you will have to do so yourself.
 *
 * @note This function does not disable the watchdog, but does reset
 * it prior to copying the data.  If the duration of the write
 * operation exceeds the watchdog timeout, the board may reset.
 *
 * @param dest an address in a flash segment.  The region into which
 * the data will be written must have already been erased.
 *
 * @param src the address of the data to be copied into flash
 *
 * @param len the number of bytes to be copied
 *
 * @return the number of bytes successfully copied, or a negative
 * error code. */
int iBSP430flashWriteData_ni (void * dest,
                              const void * src,
                              size_t len);

#endif /* BSP430_MODULE_FLASH */

#endif /* BSP430_PERIPH_FLASH_H */
