/* Copyright 2014, Peter A. Bigot
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

#ifndef BSP430_SENSORS_UTILITY_H
#define BSP430_SENSORS_UTILITY_H

/** @file
 *
 * @brief Miscellaneous constants, macros, functions useful for sensor
 * applications.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/core.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** Convert a temperature, expressed in dK (tenths of one degree
 * Kelvin) into dC (tenths of one degree Celcius). */
#define BSP430_SENSORS_CONVERT_dK_TO_dC(dk_) ((dk_) - 2732)

/** Convert a temperature, expressed in dC (tenths of one degree
 * Celcius) into dK (tenths of one degree Kelvin). */
#define BSP430_SENSORS_CONVERT_dC_TO_dK(dc_) ((dc_) + 2732)

/** Convert a temperature, expressed in dK (tenths of one degree
 * Kelvin) into d[Fahr] (tenths of one degree Fahrenheit). */
#define BSP430_SENSORS_CONVERT_dK_TO_dFahr(dk_) (320 + (9 * ((dk_) - 2732)) / 5)

/** Convert a temperature, expressed in cK (hundredths of one degree
 * Kelvin) into d[Fahr] (hundredths of one degree Fahrenheit). */
#define BSP430_SENSORS_CONVERT_cK_TO_cFahr(ck_) (3200 + (9 * ((ck_) - 27315)) / 5)

/** Convert a temperature, expressed in dC (tenths of one degree
 * Celcius) into d[Fahr] (tenths of one degree Fahrenheit). */
#define BSP430_SENSORS_CONVERT_dC_TO_dFahr(dc_) BSP430_SENSORS_CONVERT_dK_TO_dFahr(BSP430_SENSORS_CONVERT_dC_TO_dK(dc_))

/** Convert a pressure, expressed in Pa (Pascals) into cinHg
 * (hundredths of inches of mercury). */
#define BSP430_SENSORS_CONVERT_Pa_TO_cinHg(pa_) ((int)(((pa_) * 100L) / 3386))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP430_SENSORS_UTILITY_H */
