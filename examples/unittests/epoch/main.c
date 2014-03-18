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

#define POSIX_20140101T000000Z 1388534400UL

#define HALF_ERA 0x80000000UL
#define QUARTER_ERA (HALF_ERA >> 1)
#define EIGHTH_ERA (HALF_ERA >> 2)
#define FULL_ERA (HALF_ERA + (uint64_t)HALF_ERA)
#define UTT_TO_S(utt_) ((utt_) >> 15) /* 32 kiHz */

struct timeval basetv = { POSIX_20140101T000000Z, 0 }; /* 20140101T000000Z */

void
testInitialConditions (void)
{
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(0, ulBSP430uptime());
  BSP430_UNITTEST_ASSERT_TRUE(0 > iBSP430uptimeCheckEpochValidity());
}

void
testSetEpochFromTimeval (void)
{
  unsigned long utt;

  utt = ulBSP430uptime();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(0UL, utt);
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, utt));
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeCheckEpochValidity());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(basetv.tv_sec, xBSP430uptimeAsPOSIXTime(utt));
  hBSP430uptimeTimer()->overflow_count = 10;
  utt = ulBSP430uptime();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(655360UL, utt);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(basetv.tv_sec+20, xBSP430uptimeAsPOSIXTime(utt));

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, utt));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(basetv.tv_sec, xBSP430uptimeAsPOSIXTime(utt));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(basetv.tv_sec-20, xBSP430uptimeAsPOSIXTime(0UL));

  hBSP430uptimeTimer()->overflow_count = 0;
}

void
testEpochEra (void)
{
  unsigned long utt;
  struct timeval tv;

  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT, QUARTER_ERA + EIGHTH_ERA);

  /* Test boundaries and wrap into previous era */
  hBSP430uptimeTimer()->hpl->r = 0;
  hBSP430uptimeTimer()->overflow_count = EIGHTH_ERA >> 16;
  utt = ulBSP430uptime();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(EIGHTH_ERA, utt);

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, 0UL));
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeCheckEpochValidity());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(basetv.tv_sec+2*hBSP430uptimeTimer()->overflow_count, xBSP430uptimeAsPOSIXTime(utt));

  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(0));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(QUARTER_ERA));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt+BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT-1));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeEpochEra(utt+BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, iBSP430uptimeEpochEra(-1));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, iBSP430uptimeEpochEra(-EIGHTH_ERA));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, iBSP430uptimeEpochEra(-(QUARTER_ERA-1)));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeEpochEra(-QUARTER_ERA));

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(0UL, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec, tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(QUARTER_ERA, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec + UTT_TO_S(QUARTER_ERA), tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(-EIGHTH_ERA, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec - UTT_TO_S(EIGHTH_ERA), tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);

  /* Test boundaries in current era */
  hBSP430uptimeTimer()->hpl->r = 0;
  hBSP430uptimeTimer()->overflow_count = HALF_ERA >> 16;
  utt = ulBSP430uptime();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(HALF_ERA, utt);

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, 0UL));
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeCheckEpochValidity());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(basetv.tv_sec+2*hBSP430uptimeTimer()->overflow_count, xBSP430uptimeAsPOSIXTime(utt));

  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeEpochEra(0));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeEpochEra(utt-BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt-(BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT-1)));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt+BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT-1));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeEpochEra(utt+BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeEpochEra(-1));

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(QUARTER_ERA, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec + UTT_TO_S(QUARTER_ERA), tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(HALF_ERA, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec + UTT_TO_S(HALF_ERA), tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(HALF_ERA + QUARTER_ERA, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec + UTT_TO_S(HALF_ERA + QUARTER_ERA), tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);

  /* Test boundaries and wrap to next era */
  hBSP430uptimeTimer()->hpl->r = 0;
  hBSP430uptimeTimer()->overflow_count = (HALF_ERA + QUARTER_ERA + EIGHTH_ERA) >> 16;
  utt = ulBSP430uptime();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(HALF_ERA + QUARTER_ERA + EIGHTH_ERA, utt);

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, 0UL));
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeCheckEpochValidity());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(basetv.tv_sec+2*hBSP430uptimeTimer()->overflow_count, xBSP430uptimeAsPOSIXTime(utt));

  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeEpochEra(utt-BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt-(BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT-1)));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(utt));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(2, iBSP430uptimeEpochEra(utt+BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT-1));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeEpochEra(utt+BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(1, iBSP430uptimeEpochEra(-1));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(2, iBSP430uptimeEpochEra(0));

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(HALF_ERA + QUARTER_ERA, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec + UTT_TO_S(HALF_ERA + QUARTER_ERA), tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(0L, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec + UTT_TO_S(FULL_ERA), tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeAsTimeval(EIGHTH_ERA, &tv));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_sec + UTT_TO_S(FULL_ERA + EIGHTH_ERA), tv.tv_sec);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(basetv.tv_usec, tv.tv_usec);
}

void
testEpochValidate (void)
{
  struct timeval tv;
  unsigned long utt;

  hBSP430uptimeTimer()->hpl->r = 0;
  hBSP430uptimeTimer()->overflow_count = 0;

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, 0UL));
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeCheckEpochValidity());
  hBSP430uptimeTimer()->overflow_count += HALF_ERA >> 16;
  utt = ulBSP430uptime();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(-1, iBSP430uptimeAsTimeval(utt, &tv));
  BSP430_UNITTEST_ASSERT_TRUE(0 > iBSP430uptimeCheckEpochValidity());

  hBSP430uptimeTimer()->overflow_count = 0;
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, 0UL));
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeCheckEpochValidity());
  hBSP430uptimeTimer()->overflow_count += HALF_ERA >> 16;
  utt = ulBSP430uptime();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx((time_t)-1, xBSP430uptimeAsPOSIXTime(utt));
  BSP430_UNITTEST_ASSERT_TRUE(0 > iBSP430uptimeCheckEpochValidity());
}

void
testEpochAge (void)
{
  unsigned long utt;

  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx(BSP430_UPTIME_EPOCH_VALID_OFFSET_UTT, QUARTER_ERA + EIGHTH_ERA);

  /* Test boundaries and wrap into previous era */
  hBSP430uptimeTimer()->hpl->r = 0;
  hBSP430uptimeTimer()->overflow_count = EIGHTH_ERA >> 16;
  utt = ulBSP430uptime();
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(EIGHTH_ERA, utt);

  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, 0UL));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(utt, ulBSP430uptimeLastEpochUpdate());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(0, lBSP430uptimeEpochAge());
  hBSP430uptimeTimer()->overflow_count += QUARTER_ERA >> 16;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(utt, ulBSP430uptimeLastEpochUpdate());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(QUARTER_ERA, lBSP430uptimeEpochAge());
  hBSP430uptimeTimer()->overflow_count += HALF_ERA >> 16;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlu(utt, ulBSP430uptimeLastEpochUpdate());
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(-QUARTER_ERA, lBSP430uptimeEpochAge());
}

void
testNTPSequence (void)
{
  const uint8_t data0[] = {
    0x24, 0x03, 0x03, 0xea, 0x00, 0x00, 0x11, 0x1d, 0x00, 0x00, 0x11, 0xcb, 0x32, 0x74, 0x26, 0x9d,
    0xd6, 0xd1, 0xc5, 0x6c, 0x55, 0x49, 0x1f, 0x0a, 0xd6, 0x6d, 0xd9, 0x01, 0xbf, 0xf4, 0x00, 0x00,
    0xd6, 0xd1, 0xc8, 0xda, 0xfc, 0x61, 0x70, 0x5f, 0xd6, 0xd1, 0xc8, 0xda, 0xfc, 0x65, 0x26, 0x4f,
  };
  const uint64_t recv0_ntp = 15451244498192695296ULL;
  const int64_t adj0_ntp = 28129738957212503LL;
  const int32_t adj0_ms = 2147483647L;
  const uint32_t rtt0_us = 17643;
  /*
    recvfrom got 48 from 192.168.65.10:123 fam 2
    Recv time 57918 is 15451244498192695296 rc 0 ; invalid epoch 1
    t1=15451244498116673536
    t2=15479374237111775327
    t3=15479374237112018511
    t4=15451244498192695296
    Process got 0, adjustment 28129738957212503 ntp -2040469365 ms, rtt 17643 us
  */
  const uint8_t data1[] = {
    0x24, 0x03, 0x03, 0xea, 0x00, 0x00, 0x11, 0x1d, 0x00, 0x00, 0x12, 0x06, 0x32, 0x74, 0x26, 0x9d,
    0xd6, 0xd1, 0xc5, 0x6c, 0x55, 0x49, 0x1f, 0x0a, 0xd6, 0xd1, 0xc9, 0x16, 0xfb, 0xe9, 0x4b, 0x57,
    0xd6, 0xd1, 0xc9, 0x16, 0xfd, 0xf6, 0x2d, 0x28, 0xd6, 0xd1, 0xc9, 0x16, 0xfd, 0xf8, 0x75, 0x2b,
  };
  const uint64_t recv1_ntp = 15479374494877961047ULL;
  const int64_t adj1_ntp = -3537453LL;
  const int32_t adj1_ms = -1;
  const uint32_t rtt1_us = 17665;
  /*
    recvfrom got 48 from 192.168.65.10:123 fam 2
    Recv time 2024227 is 15479374494877961047 rc 0 ; invalid epoch 0
    t1=15479374494801939287
    t2=15479374494836337960
    t3=15479374494836487467
    t4=15479374494877961047
    Process got 0, adjustment -3537453 ntp -1 ms, rtt 17665 us
  */
  const sBSP430uptimeNTPPacketHeader * ph;
  int64_t adjustment_ntp;
  int32_t adjustment_ms;
  uint32_t rtt_us;

  hBSP430uptimeTimer()->hpl->r = 0;
  hBSP430uptimeTimer()->overflow_count = 0;
  BSP430_UNITTEST_ASSERT_TRUE(0 == iBSP430uptimeSetEpochFromTimeval(&basetv, 0UL));
  BSP430_UNITTEST_ASSERT_EQUAL_FMTlx((time_t)-1, xBSP430uptimeAsPOSIXTime(HALF_ERA));
  BSP430_UNITTEST_ASSERT_TRUE(0 > iBSP430uptimeCheckEpochValidity());

  ph = (const sBSP430uptimeNTPPacketHeader *)data0;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, iBSP430uptimeProcessNTPResponse(NULL, ph, recv0_ntp,
                                                                       &adjustment_ntp, &adjustment_ms, &rtt_us));
  BSP430_UNITTEST_ASSERT_EQUAL(adj0_ntp, adjustment_ntp);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(adj0_ms, adjustment_ms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(rtt0_us, rtt_us);

  ph = (const sBSP430uptimeNTPPacketHeader *)data1;
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, iBSP430uptimeProcessNTPResponse(NULL, ph, recv1_ntp,
                                                                       &adjustment_ntp, &adjustment_ms, &rtt_us));
  BSP430_UNITTEST_ASSERT_EQUAL(adj1_ntp, adjustment_ntp);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(adj1_ms, adjustment_ms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(rtt1_us, rtt_us);
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
  hBSP430uptimeTimer()->hpl->ctl &= ~(MC0 | MC1);
  hBSP430uptimeTimer()->hpl->r = 0;
  hBSP430uptimeTimer()->overflow_count = 0;
  ulBSP430uptimeSetConversionFrequency_ni(32768);
  BSP430_CORE_ENABLE_INTERRUPT();

  testInitialConditions();
  testSetEpochFromTimeval();
  testEpochEra();
  testEpochValidate();
  testEpochAge();
  testNTPSequence();

  vBSP430unittestFinalize();
}
