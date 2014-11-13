/** This file is in the public domain.
 *
 * Demonstrate the event infrastructure in the context of multiplexed
 * timer alarms.  Tagged events, event flags, and processing within
 * the handler without wake are all present.
 *
 * You should see repeatedly:
 * @li the red LED blinking at 150 ms without wakeup
 * @li the green LED blinking at 500 ms from Flags 0x02
 * @li tagged timestamped output at 700 ms from Flags 0x01
 * @li event/interrupt/duty cycle statistics at 10s from Flags 0x01
 *
 * The stable state active duty cycle is about 0.6%, most of which is
 * formatting the statistics output.  BSP430_CONSOLE_TX_BUFFER_SIZE
 * enables interrupt-driven output so emitting the formatted output
 * does not fully wake the MCU.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/periph/timer.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/led.h>
#include <bsp430/utility/event.h>
#include <bsp430/utility/console.h>
#include <string.h>

#ifndef USE_FULL_STATS
/* If evalutes to true at preprocessor, display full details on
 * statistics.  If false, displays a reduced set showing only
 * duty-cycle statistics. */
#define USE_FULL_STATS 1
#endif /* USE_FULL_STATS */

#ifndef SHOW_FLAGS
/* If evalutes to true at preprocessor, display the set of event flags
 * each time events are received. */
#define SHOW_FLAGS 0
#endif /* SHOW_FLAGS */

struct sExampleMuxAlarm;

/* Need a CCDIX on the uptime clock that isn't used for something
 * else. */
#ifndef UPTIME_MUXALARM_CCIDX
#define UPTIME_MUXALARM_CCIDX 3
#endif /* UPTIME_MUXALARM_CCIDX */

typedef void (* mux_process_fn) (const struct sBSP430eventTagRecord * ep,
                                 struct sExampleMuxAlarm * ap);

typedef struct sExampleMuxAlarm {
  sBSP430timerMuxAlarm alarm;
  unsigned long interval_utt;
  mux_process_fn process_fn;
  unsigned char tag;
  unsigned int flag;
} sExampleMuxAlarm;

static unsigned char mux_tag;
static unsigned char stats_tag;
static unsigned int mux_flag;
static sBSP430timerMuxSharedAlarm mux_alarm_base;
static sExampleMuxAlarm mux_alarms[4];

typedef struct sStatistics {
  unsigned long awake_utt;
  unsigned long sleep_utt;
  unsigned int mux_intr_ct;
  unsigned int sleep_ct;
  unsigned int counter[sizeof(mux_alarms)/sizeof(*mux_alarms)];
} sStatistics;

static volatile sStatistics statistics_v;

static int
mux_alarm_callback (sBSP430timerMuxSharedAlarm * shared,
                    sBSP430timerMuxAlarm * alarm)
{
  sExampleMuxAlarm * map = (sExampleMuxAlarm *)alarm;
  int rc;
  int rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
  uBSP430eventAnyType u;

  alarm->setting_tck += map->interval_utt;
  rc = iBSP430timerMuxAlarmAdd_ni(shared, alarm);
  statistics_v.counter[map - mux_alarms] += 1;
  statistics_v.mux_intr_ct += 1;
  if (map->tag) {
    u.p = map;
    xBSP430eventRecordEvent_ni(map->tag, !!rc, &u);
  } else if (map->flag) {
    vBSP430eventFlagsSet_ni(map->flag);
  } else {
    vBSP430ledSet(BSP430_LED_RED, -1);
    rv = 0;
  }
  return rv;
}

static void
process_timestamp (const struct sBSP430eventTagRecord * ep,
                   struct sExampleMuxAlarm * ap)
{
  const sBSP430eventTagConfig * tcp = hBSP430eventTagLookup(ep->tag);
  char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];

  cprintf("%s : %s %02x\n",
          xBSP430uptimeAsText(ep->timestamp_utt, as_text),
          tcp->id, ep->flags);
}

static void
process_mux_flag ()
{
  vBSP430ledSet(BSP430_LED_GREEN, -1);
}

static void
process_statistics (const struct sBSP430eventTagRecord * ep,
                    struct sExampleMuxAlarm * ap)
{
  sStatistics st;
  char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];
  unsigned long period_utt;
  unsigned long duty_ppt;
  int i;

  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    st = statistics_v;
    memset((void *)&statistics_v, 0, sizeof(statistics_v));
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  period_utt = st.awake_utt + st.sleep_utt;
  if (0 == period_utt) {
    duty_ppt = 0;
  } else {
    duty_ppt = (1000 * st.awake_utt + (period_utt / 2)) / period_utt;
  }
#if (USE_FULL_STATS - 0)
  cprintf("%s : Sleep %u ; MUX ", xBSP430uptimeAsText(ep->timestamp_utt, as_text), st.sleep_ct);
  for (i = 0; i < sizeof(st.counter)/sizeof(*st.counter); ++i) {
    cprintf(" %u", st.counter[i]);
  }
  cprintf(" = intr %u ; duty %lu ppt\n", st.mux_intr_ct, duty_ppt);
  cprintf("\tasleep %lu tck = %lu ms ; awake %lu tck = %lu ms\n",
          st.sleep_utt, BSP430_UPTIME_UTT_TO_MS(st.sleep_utt),
          st.awake_utt, BSP430_UPTIME_UTT_TO_MS(st.awake_utt));
#else /* USE_FULL_STATS */
  (void)i;
  (void)as_text;
  cprintf("sleep %lu awake %lu ; duty %lu ppt\n",
          st.sleep_utt, st.awake_utt, duty_ppt);
#endif /* USE_FULL_STATS */
}

static unsigned int
process_events (unsigned int events)
{
#if (SHOW_FLAGS - 0)
  cprintf("Flags %02x\n", events);
#endif /* SHOW_FLAGS */
  if (uiBSP430eventFlag_EventRecord & events) {
    sBSP430eventTagRecord evt[4];
    const sBSP430eventTagRecord * ep = evt;
    int nevt = iBSP430eventTagGetRecords(evt, sizeof(evt)/sizeof(*evt));
    const sBSP430eventTagRecord * const epe = ep + nevt;

    events &= ~uiBSP430eventFlag_EventRecord;
    while (ep < epe) {
      if ((mux_tag == ep->tag)
          || (stats_tag == ep->tag)) {
        sExampleMuxAlarm * map = ep->u.p;
        map->process_fn(ep, map);
      } else {
        cprintf("ERR: Unrecognized tag %u\n", ep->tag);
      }
      ++ep;
    }
  }
  if (mux_flag & events) {
    process_mux_flag();
    events &= ~mux_flag;
  }
  return events;
}

void main ()
{
  hBSP430timerMuxSharedAlarm map;
  tBSP430periphHandle uptime_periph;
  int arc[sizeof(mux_alarms)/sizeof(*mux_alarms)];
  unsigned long last_wake_utt;
  unsigned long last_sleep_utt;
  int rc = 0;

  vBSP430platformInitialize_ni();
  last_wake_utt = ulBSP430uptime_ni();
  (void)iBSP430consoleInitialize();
  BSP430_CORE_ENABLE_INTERRUPT();

  cprintf("\n\nevent demo " __DATE__ " " __TIME__ "\n");
  mux_tag = ucBSP430eventTagAllocate("MuxAlarm");
  stats_tag = ucBSP430eventTagAllocate("Statistics");
  mux_flag = uiBSP430eventFlagAllocate();

  uptime_periph = xBSP430periphFromHPL(hBSP430uptimeTimer()->hpl);

  map = hBSP430timerMuxAlarmStartup(&mux_alarm_base, uptime_periph, UPTIME_MUXALARM_CCIDX);
  cprintf("Mux tag %u, stats tag %u, flag %x, with alarm base %p on %s.%u\n",
          mux_tag, stats_tag, mux_flag, map,
          xBSP430timerName(uptime_periph), UPTIME_MUXALARM_CCIDX);
  if (! map) {
    cprintf("ERR initializing mux shared alarm\n");
    goto err_out;
  }

  /* Processing done entirely in mux callback.  No wakeup. */
  mux_alarms[0].alarm.callback_ni = mux_alarm_callback;
  mux_alarms[0].interval_utt = BSP430_UPTIME_MS_TO_UTT(150);

  /* Processing done by an event flag */
  mux_alarms[1].alarm.callback_ni = mux_alarm_callback;
  mux_alarms[1].interval_utt = BSP430_UPTIME_MS_TO_UTT(500);
  mux_alarms[1].flag = mux_flag;

  /* Processing done by a callback with a timestamped event */
  mux_alarms[2].alarm.callback_ni = mux_alarm_callback;
  mux_alarms[2].interval_utt = BSP430_UPTIME_MS_TO_UTT(700);
  mux_alarms[2].process_fn = process_timestamp;
  mux_alarms[2].tag = mux_tag;

  /* Processing done by a callback with a timestamped event */
  mux_alarms[3].alarm.callback_ni = mux_alarm_callback;
  mux_alarms[3].interval_utt = BSP430_UPTIME_MS_TO_UTT(10000);
  mux_alarms[3].process_fn = process_statistics;
  mux_alarms[3].tag = stats_tag;

  /* Enable the multiplexed alarms */
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    int i = 0;
    for (i = 0; (0 == rc) && (i < sizeof(mux_alarms)/sizeof(*mux_alarms)); ++i) {
      mux_alarms[i].alarm.setting_tck = ulBSP430uptime_ni();
      arc[i] = iBSP430timerMuxAlarmAdd_ni(map, &mux_alarms[i].alarm);
    }
  } while (0);
  BSP430_CORE_ENABLE_INTERRUPT();

  /* Display the results.  All values should be non-negative for success. */
  {
    int i;
    cprintf("Alarm installation got:");
    rc = 0;
    for (i = 0; i < sizeof(arc)/sizeof(*arc); ++i) {
      cprintf(" %d", arc[i]);
      if (0 > arc[i]) {
        rc = arc[i];
      }
    }
    cputchar('\n');
  }
  if (0 > rc) {
    goto err_out;
  }

  last_sleep_utt = ulBSP430uptime();
  while (1) {
    last_wake_utt = ulBSP430uptime();
    unsigned int events = process_events(uiBSP430eventFlagsGet());
    if (events) {
      cprintf("ERR: Unprocessed event flags %04x\n", events);
    }

    BSP430_CORE_DISABLE_INTERRUPT();
    /* Put back any unprocessed events */
    vBSP430eventFlagsSet_ni(events);
    if (iBSP430eventFlagsEmpty_ni()) {
      /* Nothing pending: go to sleep, then jump back to the loop head
       * when we get woken. */
      statistics_v.sleep_utt += last_wake_utt - last_sleep_utt;
      last_sleep_utt = ulBSP430uptime_ni();
      statistics_v.awake_utt += last_sleep_utt - last_wake_utt;
      statistics_v.sleep_ct += 1;
      BSP430_CORE_LPM_ENTER_NI(LPM4_bits | GIE);
      continue;
    }
    BSP430_CORE_ENABLE_INTERRUPT();
  }
err_out:
  (void)iBSP430consoleFlush();
  return;
}
