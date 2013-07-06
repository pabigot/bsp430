/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/timer.h>
#include <bsp430/periph/pmm.h>
#include <string.h>
#include <stdlib.h>

#ifndef NMUXALARMS
#define NMUXALARMS 10
#endif /* NMUXALARMS */

static struct sBSP430timerMuxSharedAlarm muxbase;
static struct sBSP430timerMuxAlarm alarms[NMUXALARMS];

typedef struct sAlarmQueue {
  unsigned int id;
  unsigned long setting_tck;
} sAlarmQueue;

static sAlarmQueue queue[NMUXALARMS];
static volatile unsigned int fired_bits_v;
static volatile unsigned int errors_v;
static volatile unsigned int nfired_v;
static unsigned int total_errors;
static unsigned int total_fired;
static volatile unsigned long late_min_v;
static volatile unsigned long late_max_v;

size_t
cacheQueue_ni (hBSP430timerMuxSharedAlarm sap)
{
  hBSP430timerMuxAlarm map = sap->alarms;
  sAlarmQueue * qp = queue;
  memset(queue, -1, sizeof(queue));
  while (map) {
    qp->id = (map - alarms);
    qp->setting_tck = map->setting_tck;
    map = map->next;
    ++qp;
  }
  return qp - queue;
}

void
displayQueue (hBSP430timerMuxSharedAlarm sap)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  unsigned int fired_bits;
  unsigned long now_utt;
  unsigned long now_tck;
  unsigned int nfired;
  unsigned int errors;
  long late_min;
  long late_max;
  char * sep = "";
  sAlarmQueue * qp = queue;
  sAlarmQueue * qpe;
  size_t nq;

  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    fired_bits = fired_bits_v;
    late_min = late_min_v;
    late_max = late_max_v;
    errors = errors_v;
    nfired = nfired_v;
    fired_bits_v = 0;
    late_min_v = 0;
    late_max_v = 0;
    errors_v = 0;
    nfired_v = 0;
    now_utt = ulBSP430uptime_ni();
    now_tck = ulBSP430timerCounter_ni(sap->dedicated.timer, NULL);

    nq = cacheQueue_ni(sap);
    qpe = queue + nq;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  total_fired += nfired;
  total_errors += errors;

  cprintf("Now %lu hires %lu\n"
          "\tfired %u of %u bits %x ; %u in queue\n"
          "\terrors %u of %u\n"
          "\tlate %ld .. %ld\n\t",
          now_utt, now_tck,
          nfired, total_fired, fired_bits, nq,
          errors, total_errors,
          late_min, late_max);
  while (qp < qpe) {
    cprintf("%s%u @ %lu", sep, qp->id, qp->setting_tck);
    ++qp;
    sep = " ; ";
  }
  cputchar('\n');
  cprintf("Base %x %ld : ctl %04x cctl %04x\n",
          sap->dedicated.flags, sap->dedicated.setting_tck,
          sap->dedicated.timer->hpl->ctl,
          sap->dedicated.timer->hpl->cctl[sap->dedicated.ccidx]);
}

static int
handle_mux_alarm (hBSP430timerMuxSharedAlarm shared,
                  hBSP430timerMuxAlarm alarm)
{
  unsigned int ofs;
  long now_tck = ulBSP430timerCounter_ni(shared->dedicated.timer, NULL);
  long late_tck = now_tck - alarm->setting_tck;

  ++nfired_v;
  fired_bits_v |= 1 << (alarm - alarms);
  if ((0 == late_min_v) && (0 == late_max_v)) {
    late_min_v = late_tck;
    late_max_v = late_tck;
  } else {
    if (late_tck < late_min_v) {
      late_min_v = late_tck;
    }
    if (late_tck > late_max_v) {
      late_max_v = late_tck;
    }
  }
  ofs = rand();
  if (ofs & 7) {
    int rc;
    alarm->setting_tck += ofs;
    rc = iBSP430timerMuxAlarmAdd_ni(shared, alarm);
    if (0 > rc) {
      ++errors_v;
    }
  }
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

void main ()
{
  int rc;
  hBSP430timerMuxSharedAlarm sap;
  hBSP430timerMuxAlarm map;
  const hBSP430timerMuxAlarm alarms_end = alarms + sizeof(alarms)/sizeof(*alarms);

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

#if (BSP430_PMM_SUPPORTS_SVSM - 0)
  BSP430_PMM_SET_SVSMCTL_NI(SVSMHCTL & ~(SVMHE | SVSHE), SVSMLCTL & ~(SVMLE | SVSLE));
#endif /* BSP430_PMM_SUPPORTS_SVSM */

  cputchar('\n');
  BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000);
  cprintf("muxalarm " __DATE__ " " __TIME__ "\n");

  for (map = alarms; map < alarms_end; ++map) {
    memset(map, 0, sizeof(&map));
    map->callback_ni = handle_mux_alarm;
    map->setting_tck = rand();
  }

  sap = hBSP430timerMuxAlarmStartup(&muxbase, BSP430_TIMER_CCACLK_PERIPH_HANDLE, 1);
  cprintf("Shared alarm handle %p\n", sap);
  if (NULL == sap) {
    return;
  }
  vBSP430timerResetCounter_ni(sap->dedicated.timer);

  for (map = alarms; map < alarms_end; ++map) {
    memset(map, 0, sizeof(&map));
    map->callback_ni = handle_mux_alarm;
    map->setting_tck = (unsigned int)rand();
    rc = iBSP430timerMuxAlarmAdd_ni(sap, map);
    cprintf("Add %u at %lu got %d\n", map-alarms, map->setting_tck, rc);
  }
  displayQueue(sap);
  sap->dedicated.timer->hpl->ctl = TASSEL_2 | ID_3 | TACLR | TAIE;
  cprintf("Mux alarm base timer running at %lu Hz\n", ulBSP430timerFrequency_Hz_ni(BSP430_TIMER_CCACLK_PERIPH_HANDLE));
  sap->dedicated.timer->hpl->ctl |= MC_2;
  while (1) {
    BSP430_CORE_DISABLE_INTERRUPT();
    if (0 == fired_bits_v) {
      BSP430_CORE_LPM_ENTER_NI(LPM2_bits | GIE);
    }
    BSP430_CORE_ENABLE_INTERRUPT();
    displayQueue(sap);
  }
}
