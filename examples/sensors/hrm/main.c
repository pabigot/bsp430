/** This file is in the public domain.
 *
 * Nominally what this does is measure the R-R interval using a heart
 * rate monitor such as https://www.adafruit.com/products/1077,
 * displaying per second the projected heartrate in beats per minute
 * and the measured R-R intervals in milliseconds.  It could
 * equivalently be used for any other periodic signal in the range
 * 1-10Hz.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/led.h>
#include <string.h>

#ifndef MAX_BPS
/* 10 beats per second or 600 BPM: extreme tachycardia */
#define MAX_BPS 10
#endif /* MAX_BPS */

#if (BSP430_PLATFORM_EXP430F5529LP - 0)
/* Use TA0.3 for alarm functionality */
#define ALARM_CCIDX 3
/* Use P1.5 = TA0.4A */
#define HRM_CCIDX 4
#define HRM_CCIS CCIS_0
#define HRM_PORT_BIT BIT5
#else
#error No HRM CCR specified */
#endif

static volatile unsigned int rrvals_ni[MAX_BPS];
static volatile unsigned int * rrp_ni;
static volatile unsigned int * const rrvals_end = rrvals_ni + sizeof(rrvals_ni) / sizeof(*rrvals_ni);
static int

rr_capture_isr_ni (const struct sBSP430halISRIndexedChainNode * cb,
                   void * context,
                   int idx)
{
  hBSP430halTIMER timer = (hBSP430halTIMER)context;

  vBSP430ledSet(BSP430_LED_RED, -1);
  if (rrp_ni < rrvals_end) {
    *rrp_ni = timer->hpl->ccr[idx];
    vBSP430ledSet(BSP430_LED_GREEN, -1);
    ++rrp_ni;
  }
  return 0;
}

static struct sBSP430halISRIndexedChainNode rr_chain_node;

typedef struct alarm_1Hz_type {
  sBSP430timerAlarm alarm;
  unsigned long interval_tck;
} alarm_1Hz_type;

static alarm_1Hz_type alarm_1Hz_state;
static int
alarm_1Hz_callback (hBSP430timerAlarm alarm)
{
  alarm_1Hz_type * ap = (alarm_1Hz_type *)alarm;
  (void)iBSP430timerAlarmSet_ni(alarm, alarm->setting_tck + ap->interval_tck);
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

void main ()
{
  hBSP430halTIMER uptime_hal;
  tBSP430periphHandle uptime_ph;
  hBSP430halPORT hrm_hal;
  unsigned long uptime_Hz;
  hBSP430timerAlarm alarm_1Hz;
  unsigned int nccr;
  unsigned int cci;
  unsigned int rrvals[1+MAX_BPS];
  unsigned int * rrvals_next;
  unsigned int * rrvals_end;
  int rc = 0;

  vBSP430platformInitialize_ni();
  BSP430_CORE_ENABLE_INTERRUPT();

  (void)iBSP430consoleInitialize();
  cprintf("HRM " __DATE__ " " __TIME__ "\n\n");

  uptime_hal = hBSP430uptimeTimer();
  uptime_ph = xBSP430periphFromHPL(uptime_hal->hpl);
  nccr = iBSP430timerSupportedCCs(uptime_ph);
  cprintf("Uptime timer is %s with %u CCRs\n",
          xBSP430timerName(uptime_ph), nccr);
#ifdef BSP430_UPTIME_DELAY_CCIDX
  cprintf("CCR %u reserved for delay function\n", BSP430_UPTIME_DELAY_CCIDX);
#endif /* BSP430_UPTIME_DELAY_CCIDX */
#ifdef BSP430_TIMER_VALID_COUNTER_READ_CCIDX
  cprintf("CCR %u reserved for valid counter read function\n", BSP430_TIMER_VALID_COUNTER_READ_CCIDX);
#endif /* BSP430_TIMER_VALID_COUNTER_READ_CCIDX */
  for (cci = 0; cci < nccr; ++cci) {
    cprintf("CC %u callback %p\n", cci, uptime_hal->cc_cbchain_ni[cci]);
  }
  hrm_hal = hBSP430portLookup(HRM_PORT_PERIPH_HANDLE);
  if (! hrm_hal) {
    cprintf("ERROR: HRM port HAL not available\n");
    return;
  }

  cprintf("HRM monitor via %u%c on %s.%u\n",
          HRM_CCIDX, 'A' + (HRM_CCIS / CCIS0),
          xBSP430portName(BSP430_PORT_HAL_GET_PERIPH_HANDLE(hrm_hal)),
          iBSP430portBitPosition(HRM_PORT_BIT));

  /* Set HRM port for input as CC trigger, with weak pull-down */
  BSP430_PORT_HAL_HPL_DIR(hrm_hal) &= ~HRM_PORT_BIT;
#if BSP430_PORT_SUPPORTS_REN
  BSP430_PORT_HAL_HPL_OUT(hrm_hal) &= ~HRM_PORT_BIT;
  BSP430_PORT_HAL_HPL_REN(hrm_hal) |= HRM_PORT_BIT;
#endif /* BSP430_PORT_SUPPORTS_REN */
  BSP430_PORT_HAL_HPL_SEL(hrm_hal) |= HRM_PORT_BIT;

  rr_chain_node.callback_ni = rr_capture_isr_ni;
  rrp_ni = rrvals_ni;
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    BSP430_HAL_ISR_CALLBACK_LINK_NI(sBSP430halISRIndexedChainNode,
                                    uptime_hal->cc_cbchain_ni[HRM_CCIDX],
                                    rr_chain_node,
                                    next_ni);
    /* Capture synchronously on rising edge and interrupt */
    uptime_hal->hpl->cctl[HRM_CCIDX] = HRM_CCIS | CM_1 | SCS | CAP | CCIE;
  } while (0);
  BSP430_CORE_ENABLE_INTERRUPT();

  /* Configure the 1Hz alarm */
  uptime_Hz = ulBSP430uptimeConversionFrequency_Hz();
  alarm_1Hz_state.interval_tck = uptime_Hz;
  alarm_1Hz = hBSP430timerAlarmInitialize(&alarm_1Hz_state.alarm, uptime_ph, ALARM_CCIDX, alarm_1Hz_callback);
  if (alarm_1Hz) {
    rc = iBSP430timerAlarmEnable(alarm_1Hz);
  }
  if (0 == rc) {
    unsigned long now_utt = ulBSP430uptime();
    now_utt -= (now_utt % uptime_Hz);
    rc = iBSP430timerAlarmSet(alarm_1Hz, now_utt + alarm_1Hz_state.interval_tck);
  }
  cprintf("alarm %p interval %lu rc %d\n", alarm_1Hz, alarm_1Hz_state.interval_tck, rc);

  rrvals_next = rrvals;
  while (1) {
    char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];
    unsigned long now_utt;
    unsigned int nrr;

    BSP430_CORE_DISABLE_INTERRUPT();
    BSP430_CORE_LPM_ENTER_NI(LPM3_bits | GIE);

    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      unsigned int nrr = (unsigned int)(rrp_ni - rrvals_ni);
      now_utt = ulBSP430uptime_ni();
      if (0 == nrr) {
        rrvals_next = rrvals_end = rrvals;
      } else {
        memcpy (rrvals_next, (void*)rrvals_ni, nrr * sizeof(*rrp_ni));
        rrvals_end = rrvals_next + nrr;
      }
      rrp_ni = rrvals_ni;
    } while (0);
    BSP430_CORE_ENABLE_INTERRUPT();
    nrr = rrvals_end - rrvals;
    cprintf("%s: ", xBSP430uptimeAsText(now_utt, as_text));
    if (1 < nrr) {
      unsigned int idx;
      unsigned long sum_rri = 0;
      unsigned int rri[sizeof(rrvals) / sizeof(*rrvals)];
      unsigned int * rrip = rri;

      for (idx = 1; idx < nrr; ++idx) {
        *rrip = rrvals[idx] - rrvals[idx-1];
        sum_rri += *rrip;
        ++rrip;
      }
      cprintf("%u BPM : ", (unsigned int)((60 * uptime_Hz * (nrr - 1)) / sum_rri));
      for (idx = 0; idx < nrr-1; ++idx) {
        cprintf("%u ", (unsigned int)((1000UL * rri[idx]) / uptime_Hz));
      }
      cputchar('\n');
      rrvals[0] = rrvals[nrr-1];
      rrvals_next = rrvals + 1;
    } else {
      cprintf("No beats captured\n");
    }
  }
}
