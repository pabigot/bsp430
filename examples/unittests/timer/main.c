/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/unittest.h>
#include <bsp430/utility/console.h>

hBSP430halTIMER uthal;

void
resetTimer ()
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    uthal->hpl->r = 0;
    uthal->overflow_count = 0;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

void
overflowTimer ()
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    uthal->hpl->r = 0;
    uthal->hpl->ctl |= TAIFG;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  __nop();
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    BSP430_UNITTEST_ASSERT_FALSE(TAIFG & uthal->hpl->ctl);
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

void
testInitialConditions (void)
{
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(0, ulBSP430uptime());
}

void
testOverflowTimer (void)
{
  resetTimer();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(0, ulBSP430uptime());
  overflowTimer();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(0x10000UL, ulBSP430uptime());
  resetTimer();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(0, ulBSP430uptime());
}

static void
testCorrection (const unsigned int cap_ctr,
                const unsigned int cur_ctr)
{
  const unsigned int delta_ctr = cur_ctr - cap_ctr;
  unsigned long long full_base = 0;
  unsigned long long cur_ull = cur_ctr;
  unsigned long cur_ul = cur_ull;

#define RESET_CTR() do {                         \
    uthal->hpl->r = cur_ctr;                     \
    cur_ull = full_base + cur_ctr;               \
    cur_ul = cur_ull;                            \
  } while (0)

  cprintf("# Testing correction cap 0x%04x cur 0x%04x\n", cap_ctr, cur_ctr);
  resetTimer();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(0, ulBSP430uptime());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(0, ullBSP430uptime());

  RESET_CTR();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ul, ulBSP430uptime());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull, ullBSP430uptime());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull - delta_ctr, ullBSP430uptimeCorrected(cap_ctr));

  overflowTimer();
  full_base = 1UL << 16;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(full_base, ullBSP430uptime());

  RESET_CTR();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull, ullBSP430uptime());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull - delta_ctr, ullBSP430uptimeCorrected(cap_ctr));
  uthal->hpl->r = 0;
  uthal->overflow_count = ~(unsigned int)0;
  full_base = 0xFFFF0000UL;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(full_base, ullBSP430uptime());

  RESET_CTR();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(cur_ul, ulBSP430uptime());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull, ullBSP430uptime());
  BSP430_UNITTEST_ASSERT_TRUE(cur_ul == cur_ull);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull - delta_ctr, ullBSP430uptimeCorrected(cap_ctr));

  overflowTimer();
  full_base += 0x10000UL;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(full_base, ullBSP430uptime());

  RESET_CTR();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(cur_ul, ulBSP430uptime());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull, ullBSP430uptime());
  BSP430_UNITTEST_ASSERT_FALSE(cur_ul == cur_ull);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull - delta_ctr, ullBSP430uptimeCorrected(cap_ctr));

  uthal->overflow_count =  ~(unsigned long)0;
  full_base = uthal->overflow_count;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(full_base, 0xFFFFFFFFUL);
  full_base <<= 16;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(full_base, 0xFFFFFFFF0000ULL);

  RESET_CTR();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(cur_ul, ulBSP430uptime());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull, ullBSP430uptime());
  BSP430_UNITTEST_ASSERT_FALSE(cur_ul == cur_ull);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(cur_ull - delta_ctr, ullBSP430uptimeCorrected(cap_ctr));

  overflowTimer();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(0, ulBSP430uptime());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTllx(0, ullBSP430uptime());
#undef RESET_CTR
}

void
testWrapCorrection (void)
{
}

void main ()
{
  vBSP430platformInitialize_ni();
  vBSP430uptimeStart_ni();
  vBSP430unittestInitialize();

  BSP430_CORE_DISABLE_INTERRUPT();
  /* Halt the timer without telling the infrastructure, since we
   * manage it explicitly.  Put the interrupts back on so that we can
   * synthesize overflow events and have them be handled normally. */
  uthal = hBSP430uptimeTimer();
  uthal->hpl->ctl &= ~(MC0 | MC1);
  resetTimer();
  ulBSP430uptimeSetConversionFrequency_ni(32768);
  BSP430_CORE_ENABLE_INTERRUPT();

  testInitialConditions();
  testOverflowTimer();
  testCorrection(0x4000, 0xC000);
  testCorrection(0xC000, 0x4000);
  testCorrection(0x0000, 0xFFFF);
  testCorrection(0xFFFF, 0x0000);
  testCorrection(0x0000, 0x0001);
  testCorrection(0X0001, 0x0000);

  vBSP430unittestFinalize();
}
