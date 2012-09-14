
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
 * @brief A unit test facility.
 *
 * This abstracts the unit-test specific portions of <a
 * href="https://sourceforge.net/projects/mspgcc/files/test430/">test430</a>,
 * since BSP430 itself replaces the platform support portions of that
 * package.
 *
 * It is not a full-featured xUnit style infrastructure, as there is
 * no framework concept of test suites or setup/teardown functionality
 * to restore a known configuration.  Basically all it supports is
 * verifying intermediate values and displaying unexpected results for
 * further analysis.
 *
 * @note This facility depends on #BSP430_CONSOLE and #BSP430_LED.
 * The corresponding modules must be linked into any application that
 * uses it.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_UNITTEST_H
#define BSP430_UTILITY_UNITTEST_H

#include <bsp430/platform.h>
#include <inttypes.h>

/** @def configBSP430_UNITTEST
 *
 * Define to a true value to enable the uni ttest infrastructure.  In
 * addition, the <bsp430/utility/unittest.h> header must be included,
 * and the @c utility/unittest module linked in.
 *
 * @cppflag
 * @affects #BSP430_UNITTEST
 * @defaulted
 */
#ifndef configBSP430_UNITTEST
#define configBSP430_UNITTEST 0
#endif /* configBSP430_UNITTEST */

/** @def BSP430_UNITTEST
 *
 * Indicate that the unit test interface is available on the platform.
 * This is normally set by <bsp430/utility/unittest.h> when
 * #configBSP430_UNITTEST is true.
 *
 * @cppflag
 * @dependency #configBSP430_UNITTEST
 * @defaulted */
#define BSP430_UNITTEST (configBSP430_UNITTEST - 0)

#if defined(BSP430_DOXYGEN) || (defined(BSP430_LED_GREEN) && defined(BSP430_LED_RED))
/** @def BSP430_UNITTEST_LED_FAILED
 *
 * The LED that is lit if the tests failed.  This is normally the
 * first red LED; if either the red or green LED is missing, it is the
 * first available LED so that on a one-LED platform failure is
 * indicated by a lit LED. */
#define BSP430_UNITTEST_LED_FAILED BSP430_LED_RED

/** @def BSP430_UNITTEST_LED_PASSED
 *
 * The LED that is lit if the tests failed.  This is normally the
 * first green LED; if either the red or green LED is missing, it is
 * the second available LED.  On a one-LED platform, success is not
 * visibly indicated. */
#define BSP430_UNITTEST_LED_PASSED BSP430_LED_GREEN
#else
#define BSP430_UNITTEST_LED_FAILED 0
#define BSP430_UNITTEST_LED_PASSED 1
#endif

/** @def configBSP430_UNITTEST_FAILFAST
 *
 * Define to a true value to bypass subsequent tests after a failure.
 * In this case, the internals of the unittest framework immediately
 * execute the finalization routine on detection of a failure.
 *
 * By default this is defined to a false value, and the framework
 * continues to run the remaining tests in the application.
 *
 * @cppflag
 * @defaulted
 */
#ifndef configBSP430_UNITTEST_FAILFAST
#define configBSP430_UNITTEST_FAILFAST 0
#endif /* configBSP430_UNITTEST_FAILFAST */

/** Initialize the unittest framework.
 *
 * This clears the history of previous tests and emits a distinguished
 * sequence (<c>sync...</c>) to the console for use in external
 * monitor applications. */
void vBSP430unittestInitialize (void);

/** Display a unit test result.
 *
 * @param line the line number on which the test occurred.
 * 
 * @param passp a non-zero value if the test passed; a zero value
 * indicates the test failed.
 * 
 * @param format a #cprintf format statement for any diagnostic or
 * status to be displayed for the test.  A newline will be
 * appended automatically. */
void vBSP430unittestResult_ (int line,
                             int passp,
                             const char * format,
                             ...);

/** Finalize the unittest framework.
 *
 * This displays the results of the tests and emits a distinguished
 * sequence (<c>...done</c>) to the console for use in external
 * monitor applications.
 *
 * If tests pass, #BSP430_UNITTEST_LED_PASSED is lit, and this
 * function returns.
 *
 * If at least one test failed, #BSP430_UNITTEST_LED_FAILED flashes at
 * 1Hz and this function does not return. */
void vBSP430unittestFinalize (void);

/** Diagnose an unconditional failure */
#define BSP430_UNITTEST_FAIL(msg_) do {         \
    vBSP430unittestResult_(__LINE__, 0, msg_);  \
  } while (0)

/** Validate that the expression is true */
#define BSP430_UNITTEST_ASSERT_TRUE(expr_) do {                 \
    static const char * expr_str_ = #expr_;                     \
    vBSP430unittestResult_(__LINE__, !!(expr_), expr_str_);     \
  } while (0)

/** Validate that the expression is false */
#define BSP430_UNITTEST_ASSERT_FALSE(expr_) do {                        \
    static const char * expr_str_ = #expr_;                             \
    vBSP430unittestResult_(__LINE__, !(expr_), "NOT %s", expr_str_);    \
  } while (0)

/** Validate that the expressions are equal */
#define BSP430_UNITTEST_ASSERT_EQUAL(v1_,v2_) do {                      \
    static const char * expr_str_ = #v1_ "==" #v2_;                     \
    vBSP430unittestResult_(__LINE__, (v1_) == (v2_), expr_str_);        \
  } while (0)

/** Diagnose equality failures.
 *
 * @param v1_ an expression of some type
 **
 ** @param v2_ an expression of the same type as @a v1_
 *
 * @param type_ a type suitable for holding the values of @a v1_ and
 * @a v2_ for equality comparison purposes
 *
 * @param pri_ a print(3) format specifier, exclusive of the leading
 * @c %, to be used to format the passed values of @a v1_ and @a v2_
 * in the case where the test fails */
#define BSP430_UNITTEST_ASSERT_EQUAL_FMT_(v1_,v2_,type_,pri_) do {      \
    static const char * expr_str_ = #v1_ "==" #v2_;                     \
    type_ v1v_ = (v1_);                                                 \
    type_ v2v_ = (v2_);                                                 \
    if (v1v_ == v2v_) {                                                 \
      vBSP430unittestResult_(__LINE__, 1, "%s\n", expr_str_);           \
    } else {                                                            \
      vBSP430unittestResult_(__LINE__, 0, "%s: %" pri_ " != %" pri_, expr_str_, v1v_, v2v_); \
    }                                                                   \
  } while (0)


/** Validate that the expressions are equal.
 *
 * If they are not, emit a diagnostic showing both values formatted as
 * signed 16-bit integers. */
#define BSP430_UNITTEST_ASSERT_EQUAL_FMTd16(v1_,v2_) \
  BSP430_UNITTEST_ASSERT_EQUAL_FMT_(v1_,v2_,int,PRId16)

/** Validate that the expressions are equal.
 *
 * If they are not, emit a diagnostic showing both values formatted as
 * unsigned 16-bit decimal integers. */
#define BSP430_UNITTEST_ASSERT_EQUAL_FMTu16(v1_,v2_) \
  BSP430_UNITTEST_ASSERT_EQUAL_FMT_(v1_,v2_,unsigned int,PRIu16)

/** Validate that the expressions are equal.
 *
 * If they are not, emit a diagnostic showing both values formatted as
 * unsigned 16-bit hexadecimal integers. */
#define BSP430_UNITTEST_ASSERT_EQUAL_FMTx16(v1_,v2_) \
  BSP430_UNITTEST_ASSERT_EQUAL_FMT_(v1_,v2_,unsigned int,PRIx16)

/** Validate that the expressions are equal.
 *
 * If they are not, emit a diagnostic showing both values formatted as
 * signed 32-bit decimal integers. */
#define BSP430_UNITTEST_ASSERT_EQUAL_FMTd32(v1_,v2_) \
  BSP430_UNITTEST_ASSERT_EQUAL_FMT_(v1_,v2_,int,PRId32)

/** Validate that the expressions are equal.
 *
 * If they are not, emit a diagnostic showing both values formatted as
 * unsigned 32-bit decimal integers. */
#define BSP430_UNITTEST_ASSERT_EQUAL_FMTu32(v1_,v2_) \
  BSP430_UNITTEST_ASSERT_EQUAL_FMT_(v1_,v2_,unsigned int,PRIu32)

/** Validate that the expressions are equal.
 *
 * If they are not, emit a diagnostic showing both values formatted as
 * unsigned 32-bit hexadecimal integers. */
#define BSP430_UNITTEST_ASSERT_EQUAL_FMTx32(v1_,v2_) \
  BSP430_UNITTEST_ASSERT_EQUAL_FMT_(v1_,v2_,unsigned int,PRIx32)

/** Validate that the pointer expressions are equal.
 *
 * If they are not, emit a diagnostic showing both pointer values. */
#define BSP430_UNITTEST_ASSERT_EQUAL_FMTp(v1_,v2_) \
  BSP430_UNITTEST_ASSERT_EQUAL_FMT_(v1_,v2_,uintptr_t,PRIxPTR)

#endif /* BSP430_UTILITY_UNITTEST_H */
