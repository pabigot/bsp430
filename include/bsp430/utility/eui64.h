/* Copyright 2013, Peter A. Bigot
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
 * @brief Support for EUI-64 retrieval.
 *
 * An Extended Unique Identifier in 64-bits (EUI-64) is a 64-bit value
 * intended to uniquely identify a device.  In some cases uniqueness
 * may be restricted to instances of a given device type.  These
 * values are commonly used for interface identifiers in IPv6 and
 * other situations where it is necessary to distinguish between nodes
 * in a system.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_EUI64_H
#define BSP430_UTILITY_EUI64_H

#include <bsp430/core.h>

/** @def configBSP430_EUI64
 *
 * Define to a true value to indicate intent to use the EUI-64 interface.
 *
 * @cppflag
 * @defaulted
 */
#ifndef configBSP430_EUI64
#define configBSP430_EUI64 0
#endif /* configBSP430_EUI64 */

/** @def BSP430_EUI64
 *
 * Indicate that the EUI-64 interface is available on the platform.
 * This is normally set to reflect whether #configBSP430_EUI64 was
 * requested, but it may be explicitly set to false by the platform if
 * it does not support the EUI-64 functionality.
 *
 * This flag is defined only if #configBSP430_EUI64 is true.
 *
 * @dependency #configBSP430_EUI64
 * @platformdefault */
#if defined(BSP430_DOXYGEN) || defined(configBSP430_EUI64)
#ifndef BSP430_EUI64
#define BSP430_EUI64 (configBSP430_EUI64 - 0)
#endif /* BSP430_EUI64 */
#endif /* BSP430_DOXYGEN */

/** Define to a true value to use the platform-provided EUI-64 infrastructure.
 *
 * Many platforms will provide an implementation of iBSP430eui64(),
 * either the common one that creates a value from device-specific
 * calibration data or a custom one derived from available peripherals
 * like a DS18B20.  If this macro is defined as false by the
 * application these platform-provided implementations will be omitted
 * during builds, allowing the application to provide its own
 * implementation.
 *
 * @note If you define this to @c 0 be sure to define @c MODULE_EUI64
 * in your application @c Makefile to be empty or the name of the source file that provides the function.
 *
 * @cppflag
 * @defaulted
 */
#ifndef configBSP430_EUI64_USE_PLATFORM
#define configBSP430_EUI64_USE_PLATFORM 1
#endif /*  configBSP430_EUI64_USE_PLATFORM */

/** Value indicating stored EUI-64 is valid and was permanently
 * assigned by something that guarantees uniqueness within the
 * platform. */
#define BSP430_EUI64_PERM 0

/** Value indicating stored EUI-64 was randomly generated.  Uniqueness
 * is not guaranteed.  If this value was returned by iBSP430eui64(),
 * there is no guarantee that a subsequent invocation of the function
 * will return the same EUI-64. */
#define BSP430_EUI64_RANDOM 1

/** Value indicating stored EUI-64 was generated from non-random data
 * but may not be unique. */
#define BSP430_EUI64_STOLEN 2

/** Bit mask for first octet in canonical-order EUI-64, identifying
 * the individual/group marker.  The bit is set to indicate group, and
 * cleared to indiate individual. */
#define BSP430_EUI64_MASK_GROUP 0x01

/** Bit mask for first octet in canonical-order EUI-64, identifying
 * the universal/local marker.  The bit is set to indicate local, and
 * cleared to indicate universal. */
#define BSP430_EUI64_MASK_LOCAL 0x02

/** A type holding the EUI-64 in canonical form. */
typedef union uBSP430eui64 {
  /** The 8 octets comprising the 64-bit value in canonical form.
   * I.e., the OUI-24 component appears in elements 0, 1, and 2 of the
   * array. */
  uint8_t bytes[8];
  /** The EUI-64 broken down into components as derived from an
   * underlying 48-bit value (EUI-48 or MAC-48). */
  struct as48 {
    /** The 24-bit Organizationally Unique Identifier */
    uint8_t oui[3];
    /** A label indicating how the value was derived from a 48-bit
     * value (FF FF from MAC-48, FF FE from EUI-48). */
    uint8_t label[2];
    /** The manufacturer-assigned extension identifier */
    uint8_t extension_id[3];
  } as48;
} uBSP430eui64;

/** A handle for an EUI64. */
typedef uBSP430eui64 * hBSP430eui64;

/** Test whether an EUI-64 is the distinct null identifier (all bits
 * are set) */
#define BSP430_EUI64_IS_NULL(_h) ((0xFF == (_h)->bytes[0])      \
                                  & (0xFF == (_h)->bytes[1])    \
                                  & (0xFF == (_h)->bytes[2])    \
                                  & (0xFF == (_h)->bytes[3])    \
                                  & (0xFF == (_h)->bytes[4])    \
                                  & (0xFF == (_h)->bytes[5])    \
                                  & (0xFF == (_h)->bytes[6])    \
                                  & (0xFF == (_h)->bytes[7]))

/** Obtain an EUI-64 for the board.
 *
 * This routine is provided by the platform implementation and returns
 * a 64-bit value consistent with IEEE EUI-64 standards.  Whether the
 * returned value is a valid EUI-64 or is (partially) randomly
 * generated depends on the platform.  The default implementation
 * generates an EUI-64 by performing an XOR of whatever flash region
 * contains device-specific values such as calibration constants.  If
 * successful this EUI-64 will characterized as #BSP430_EUI64_STOLEN.
 *
 * The value returned by this function may be recalculated on each
 * call.  Under normal circumstances the function should be invoked
 * once and the @a eui64 contents retained for the life of the
 * program.
 *
 * @note This is an EUI-64, not the modified EUI-64 expected by @link
 * http://tools.ietf.org/html/rfc4291#appendix-A Appendix A of RFC
 * 4291@endlink.  To convert between them invert
 * #BSP430_EUI64_MASK_LOCAL.
 *
 * @param eui64 a pointer to a memory location into which the EUI-64
 * will be written.
 *
 * @return A negative value to indicate an error, or one of
 * #BSP430_EUI64_PERM, #BSP430_EUI64_RANDOM, or
 * #BSP430_EUI64_STOLEN.
 *
 * @dependency BSP430_EUI64
 */
#if defined(BSP430_DOXYGEN) || (BSP430_EUI64 - 0)
int iBSP430eui64 (hBSP430eui64 eui64);
#endif /* BSP430_EUI64 */

#endif /* BSP430_UTILITY_EUI64_H */
