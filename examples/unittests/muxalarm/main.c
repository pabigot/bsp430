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
#include <bsp430/periph/timer.h>

void
testOnOff (void)
{
  sBSP430timerMuxSharedAlarm sd;
  hBSP430timerMuxSharedAlarm hs;
  int rc;

  memset(&sd, 0x96, sizeof(sd));
  hs = hBSP430timerMuxAlarmStartup(&sd, BSP430_TIMER_CCACLK_PERIPH_HANDLE, 1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&sd, hs);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(sd.dedicated.ccidx, 1);

  rc = iBSP430timerMuxAlarmShutdown(hs);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, rc);
}

void
testAddRemove (void)
{
  sBSP430timerMuxSharedAlarm sd;
  hBSP430timerMuxSharedAlarm hs;
  sBSP430timerMuxAlarm alarms[5];
  int i;

  memset(&sd, 0x96, sizeof(sd));
  hs = hBSP430timerMuxAlarmStartup(&sd, BSP430_TIMER_CCACLK_PERIPH_HANDLE, 1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(&sd, hs);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, hs->alarms);
  /* For unit tests, make sure underlying timer is not running and is
   * at counter zero */
  hs->dedicated.timer->hpl->ctl &= ~(MC0 | MC1);
  vBSP430timerResetCounter_ni(hs->dedicated.timer);

  for (i = 0; i < sizeof(alarms)/sizeof(*alarms); ++i) {
    hBSP430timerMuxAlarm mp = alarms + i;
    mp->setting_tck = 100 * (i+1);
  }

  BSP430_UNITTEST_ASSERT_FALSE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);

  i = iBSP430timerMuxAlarmAdd_ni(hs, alarms+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[1].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmAdd_ni(hs, alarms+3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+3, alarms[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[3].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmAdd_ni(hs, alarms+2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+2, alarms[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+3, alarms[2].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[3].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmAdd_ni(hs, alarms+0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+0, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, alarms[0].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+2, alarms[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+3, alarms[2].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[3].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmAdd_ni(hs, alarms+4);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+0, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, alarms[0].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+2, alarms[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+3, alarms[2].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+4, alarms[3].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[4].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmRemove_ni(hs, alarms+0);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+2, alarms[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+3, alarms[2].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+4, alarms[3].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[4].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmRemove_ni(hs, alarms+2);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+3, alarms[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+4, alarms[3].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[4].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmRemove_ni(hs, alarms+4);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+3, alarms[1].next);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[3].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmRemove_ni(hs, alarms+3);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(alarms+1, hs->alarms);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, alarms[1].next);
  BSP430_UNITTEST_ASSERT_TRUE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTld(hs->alarms->setting_tck, hs->dedicated.setting_tck);

  i = iBSP430timerMuxAlarmRemove_ni(hs, alarms+1);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTd(0, i);
  BSP430_UNITTEST_ASSERT_EQUAL_FMTp(NULL, hs->alarms);
  BSP430_UNITTEST_ASSERT_FALSE(BSP430_TIMER_ALARM_FLAG_SET & hs->dedicated.flags);
}

void main ()
{
  vBSP430platformInitialize_ni();
  vBSP430unittestInitialize();

  testOnOff();
  testAddRemove();

  vBSP430unittestFinalize();
}
