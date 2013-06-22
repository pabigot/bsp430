/* Copyright 2012-2013, Peter A. Bigot
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
 * @brief Implementation for the BSP430 unit test facility
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2013, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/utility/unittest.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>
#include <bsp430/clock.h>
#include <stdarg.h>
#include <stdlib.h>
#include <bsp430/platform.h>

#if (BSP430_UNITTEST - 0)

static unsigned int num_passed;
static unsigned int num_failed;

void
vBSP430unittestInitialize (void)
{
  int ctr;

  if (NULL == hBSP430console()) {
    iBSP430consoleInitialize();
  }
  num_passed = num_failed = 0;
  ctr = 5;
  while (0 <= --ctr) {
    cputs("# sync...");
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 100);
  }
}

void
vBSP430unittestResult_ (int line,
                        int passp,
                        const char * format,
                        ...)
{
  va_list argp;

  if (passp) {
    ++num_passed;
  } else {
    ++num_failed;
  }
  cprintf("%s [%u]: ", passp ? "pass" : "FAIL", line);
  va_start(argp, format);
  vcprintf(format, argp);
  cputchar_ni('\n');
  va_end(argp);
#if (configBSP430_UNITTEST_FAILFAST - 0)
  cputs("# FAILFAST");
  vBSP430unittestFinalize();
#endif /* configBSP430_UNITTEST_FAILFAST */
}

void
vBSP430unittestFinalize (void)
{
  int ctr;

  if (0 < num_failed) {
    cprintf("# FAIL %u pass %u\n", num_failed, num_passed);
  } else if (0 < num_passed) {
    cprintf("# PASSED %u\n", num_passed);
  }
  ctr = 5;
  while (0 <= --ctr) {
    cputs("# ...done");
  }
  if (0 < num_failed) {
    BSP430_CORE_DISABLE_INTERRUPT();
    vBSP430ledSet(BSP430_UNITTEST_LED_PASSED, 0);
    vBSP430ledSet(BSP430_UNITTEST_LED_FAILED, 1);
    while (1) {
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
      vBSP430ledSet(BSP430_UNITTEST_LED_FAILED, -1);
    }
  } else if (0 < num_passed) {
    vBSP430ledSet(BSP430_UNITTEST_LED_PASSED, 1);
    vBSP430ledSet(BSP430_UNITTEST_LED_FAILED, 0);
  }
}

#endif /* BSP430_UNITTEST */
