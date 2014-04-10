/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/lpm.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/led.h>
#include <bsp430/periph.h>
#include <bsp430/periph/port.h>
#include <bsp430/periph/pmm.h>
#include <bsp430/periph/sys.h>
#include <bsp430/utility/tlv.h>
#if ! (__MSP430FR5969__ - 0)
#error This application hard-coded to MSP430FR5979
#endif /* MCU */
#if ! (BSP430_PLATFORM_BUTTON0 - 0)
#error No button available on this platform
#endif /* BSP430_PLATFORM_BUTTON0 */
#include <string.h>
#include <time.h>

#define REFVSEL__1p2 REFVSEL_0
#define REFVSEL__2p0 REFVSEL_1

const sBSP430tlvREFCAL * const cal_ref = (const sBSP430tlvREFCAL *)0x1A26;
const sBSP430tlvADCCAL * const cal_adc = (const sBSP430tlvADCCAL *)0x1A14;

typedef struct sState {
  unsigned long cumulative_active_utt;
  unsigned long cumulative_lpm3_utt;
  unsigned int boots;
  unsigned int sleeps;
  unsigned int last_elapsed_s;
  unsigned int refvsel;
  unsigned int last_vdd_mV;
} sState;

__attribute__((__section__(".rodata.app")))
sState volatile state;

/* I know button 0 is on P4.5 and button 1 is on P0.1, so I'm using
 * the same bits as the event bits to make recognizing it.  I'm checking that
 * assumption at compile-time, though. */
#define EVT_STAYUP (1U << 0)
#define EVT_BUTTON1 (1U << 1)
#define EVT_RTCRDY (1U << 2)
#define EVT_RTC1MIN (1U << 3)
#define EVT_ADC12RDY (1U << 4)
#define EVT_BUTTON0 (1U << 5)

#if ((EVT_BUTTON0 != BSP430_PLATFORM_BUTTON0_PORT_BIT) \
     | (EVT_BUTTON1 != BSP430_PLATFORM_BUTTON1_PORT_BIT))
#error Unexpected button config
#endif

volatile struct tm rtc_tm_ni;
volatile unsigned int events_ni;

unsigned int
uiElapsed_s ()
{
  return rtc_tm_ni.tm_sec + 60 * (rtc_tm_ni.tm_min + 60 * rtc_tm_ni.tm_hour);
}

#define FLG_DID_RST 0x01
#define FLG_WOKE_FROM_LPM3p5 0x02
#define FLG_WITH_CONSOLE 0x04
#define FLG_WITH_LEDS 0x8
#define FLG_ENTER_LPM3p5 0x10
#define FLG_B0_ON_BOOT 0x20
#define FLG_B1_ON_BOOT 0x40

unsigned int flags;
unsigned int vmid_raw;

#define LED_ACTIVE 1
#define LED_LPM3 0

static int
button_isr_ni (const struct sBSP430halISRIndexedChainNode * cb,
               void * context,
               int idx)
{
  (void)cb;
  (void)context;
  /* Record the event, then exit from LPM and disable the interrupt as
   * a debounce feature. */
  events_ni |= (1U << idx);
  return BSP430_HAL_ISR_CALLBACK_EXIT_LPM | BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT;
}

const sBSP430halISRIndexedChainNode button_cb = {
  .next_ni = NULL,
  .callback_ni = button_isr_ni,
};

static void
__attribute__((__interrupt__(RTC_VECTOR)))
isr_RTC (void)
{
  unsigned int rtciv = RTCIV;
  int rv = 0;
  switch (rtciv) {
    case RTCIV_RTCRDYIFG:
      RTCCTL01 &= ~RTCRDYIE;
      events_ni |= EVT_RTCRDY;
      goto finish;
    case RTCIV_RTCTEVIFG:
      events_ni |= EVT_RTC1MIN;
    finish:
      rtc_tm_ni.tm_year = RTCYEAR - 1900;
      rtc_tm_ni.tm_mon = RTCMON - 1;
      rtc_tm_ni.tm_mday = RTCDAY;
      rtc_tm_ni.tm_wday = RTCDOW;
      rtc_tm_ni.tm_hour = RTCHOUR;
      rtc_tm_ni.tm_min = RTCMIN;
      rtc_tm_ni.tm_sec = RTCSEC;
      rv |= BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
    default:
      break;
  }
  if (BSP430_HAL_ISR_CALLBACK_EXIT_LPM & rv) {
    rv |= BSP430_CORE_LPM_EXIT_MASK;
  }
  BSP430_CORE_LPM_EXIT_FROM_ISR(rv);
}

static void
__attribute__((__interrupt__(ADC12_VECTOR)))
isr_ADC12 (void)
{
  unsigned int adc12iv = ADC12IV;
  int rv = 0;
  switch (adc12iv) {
    case ADC12IV_ADC12IFG0:
      events_ni |= EVT_ADC12RDY;
      rv |= BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
      vmid_raw = ADC12MEM0;
      ADC12CTL0 &= ~(ADC12ON | ADC12ENC);
      REFCTL0 &= ~REFON;
      break;
    default:
      break;
  }
  if (BSP430_HAL_ISR_CALLBACK_EXIT_LPM & rv) {
    rv |= BSP430_CORE_LPM_EXIT_MASK;
  }
  BSP430_CORE_LPM_EXIT_FROM_ISR(rv);
}

static void
resetState (void)
{
  memset((void*)&state, 0, sizeof(state));
  state.refvsel = REFVSEL__2p0;
}

void main ()
{
  unsigned int sysrstiv;
  unsigned long reset_causes = 0;
  unsigned int reset_flags = 0;
  hBSP430halSERIAL tty = 0;
  unsigned int elapsed_s;
  unsigned int vdd_mV;
  unsigned long active_up_utt = 0;
  unsigned int iters = 0;

  flags = 0;
  //flags |= FLG_WITH_LEDS;
  events_ni = 0;
  //events_ni |= EVT_STAYUP;

  /* Record all the reset causes */
  while (0 != ((sysrstiv = uiBSP430sysSYSRSTGenerator_ni(&reset_flags)))) {
    reset_causes |= 1UL << (sysrstiv / 2);
  }

  /* Put all pins into low-power mode.  We'll only use the ones we
   * need. */
  vBSP430lpmConfigurePortsForLPM_ni();

  if ((1U << (SYSRSTIV_RSTNMI / 2)) & reset_causes) {
    flags |= FLG_DID_RST;
  }
  if (BSP430_SYS_FLAG_SYSRST_LPM5WU & reset_flags) {
    flags |= FLG_WOKE_FROM_LPM3p5;

    /* Restore required register settings before
     * BSP430_PMM_CLEAR_LOCKLPM5() is invoked in
     * vBSP430platformInitialize_ni(). */

    /* Record the current time, clear the 1MIN wake flag, and restart
     * the RTC enabling another interrupt for the next minute. */
    elapsed_s = RTCSEC + 60 * (RTCMIN + 60 * RTCHOUR);
    RTCCTL01 &= ~RTCTEVIFG;
    RTCCTL01 = RTCOFIE | RTCTEVIE;
  } else {
    RTCCTL01 = RTCHOLD | RTCOFIE | RTCTEVIE;
    /* Initialize the clock to the POSIX epoch. */
    RTCYEAR = 1970;
    RTCMON = 1;
    RTCDAY = 1;
    RTCDOW = 4;             /* 1970-01-01T00:00:00Z was a Thursday. */
    RTCHOUR = 0;
    RTCMIN = 0;
    RTCSEC = 0;
    elapsed_s = 0;
    RTCCTL01 &= ~RTCHOLD;
  }

  vBSP430platformInitialize_ni();

  /* Button 0 is on P4.5 active-low.  See if it's pushed.
   * Button 1 is on P1.1 active-low.  See if it's pushed.
   * EUSCI_A0.RX is on P2.1, pulled high by remote.  See if it's connected. */
  {
    unsigned char p2out = P2OUT;
    unsigned char p2ren = P2REN;
    unsigned char p2dir = P2DIR;
    unsigned char p4out = P4OUT;
    unsigned char p4ren = P4REN;
    unsigned char p4dir = P4DIR;
    unsigned char p1out = P1OUT;
    unsigned char p1ren = P1REN;
    unsigned char p1dir = P1DIR;

    P2OUT &= ~BIT1;             /* pull down */
    P2REN |= BIT1;
    P2DIR &= ~BIT1;
    P4OUT |= BIT5;              /* pull up */
    P4REN |= BIT5;
    P4DIR &= ~BIT5;
    P1OUT |= BIT1;              /* pull up */
    P1REN |= BIT1;
    P1DIR &= ~BIT1;
    if (P2IN & BIT1) {          /* remote pull up? */
      flags |= FLG_WITH_CONSOLE;
    }
    if (! (P4IN & BIT5)) {      /* button pressed? */
      flags |= FLG_B0_ON_BOOT;
    }
    if (! (P1IN & BIT1)) {      /* button pressed? */
      flags |= FLG_B1_ON_BOOT;
    }
    P1REN = p1ren;
    P1OUT = p1out;
    P1DIR = p1dir;
    P4REN = p4ren;
    P4OUT = p4out;
    P4DIR = p4dir;
    P2REN = p2ren;
    P2OUT = p2out;
    P2DIR = p2dir;
  }

  if ((FLG_WITH_CONSOLE & flags) && (0 == iBSP430consoleInitialize())) {
    tty = hBSP430console();
  } else {
    flags &= ~FLG_WITH_CONSOLE;
  }

  /* If powered-up from code start and there's state, display it then
   * lock up. */
  if (((FLG_B0_ON_BOOT | FLG_B1_ON_BOOT) & flags)
      || (! (FLG_WOKE_FROM_LPM3p5 & flags))) {
    if (tty) {
      char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];

      cputchar('\n');
      BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 10);
      cprintf("\nlpm3p5 " __DATE__ " " __TIME__ "\n");
      cprintf("Flags: %x\n", flags);
      cprintf("System reset bitmask %lx; causes:\n", reset_causes);
      {
        int bit = 0;
        while (bit < (8 * sizeof(reset_causes))) {
          if (reset_causes & (1UL << bit)) {
            cprintf("\t%s\n", xBSP430sysSYSRSTIVDescription(2*bit));
          }
          ++bit;
        }
      }

      cputtext("System reset included:");
      if (reset_flags & BSP430_SYS_FLAG_SYSRST_BOR) {
        cputtext(" BOR");
      }
      if (reset_flags & BSP430_SYS_FLAG_SYSRST_LPM5WU) {
        cputtext(" LPM5WU");
      }
      if (reset_flags & BSP430_SYS_FLAG_SYSRST_POR) {
        cputtext(" POR");
      }
      if (reset_flags & BSP430_SYS_FLAG_SYSRST_PUC) {
        cputtext(" PUC");
      }
      cputchar('\n');
      cprintf("Preserved state:\n");
      cprintf("Total elapsed time (s): %u\n", state.last_elapsed_s);
      cprintf("Last Vdd (mV): %u\n", state.last_vdd_mV);
      cprintf("Boots: %u\n", state.boots);
      cprintf("Sleeps: %u\n", state.sleeps);
      cprintf("Cumulative active time %s (utt): %lu\n",
              xBSP430uptimeAsText(state.cumulative_active_utt, as_text),
              state.cumulative_active_utt);
      cprintf("Cumulative LPM3 time %s (utt): %lu\n",
              xBSP430uptimeAsText(state.cumulative_lpm3_utt, as_text),
              state.cumulative_lpm3_utt);
    }

    if ((FLG_B0_ON_BOOT & flags) || (0 == state.boots)) {
      resetState();
    } else if (! (FLG_B1_ON_BOOT & flags)) {
      cprintf("\n\nPower-up/reset with B0 held to clear state\n");
      while (1) {
        vBSP430ledSet(BSP430_LED_RED, -1);
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 4);
      }
    }
  }

  ++state.boots;

  REFCTL0 = state.refvsel | REFON;
  BSP430_CORE_DELAY_CYCLES((75 * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000UL);
  ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
  /* Start collection with conversion register zero
   * ADC12SHS_0: Trigger on ADC12SC bit
   * ADC12SHP: Pulsed sampling
   * No sample-input signal inversion
   * Divide clock by 1
   * ADC12SSEL_0: Clock source is MODCLK (nominal 5MHz)
   * ADC12CONSEQ_0: Single-channel, single-conversion
   */
  ADC12CTL1 = ADC12SHS_0 | ADC12SHP | ADC12SSEL_0 | ADC12CONSEQ_0;
  /* No predivider
   * ADC12RES: 12-bit resolution
   * Binary unsigned read-back
   * Low-power sample rate */
  ADC12CTL2 = ADC12RES_2 | ADC12PWRMD;
  /* Disable channel mapping.
   * Use internal voltage channel */
  ADC12CTL3 = ADC12BATMAP;
  /* End of sequence (one conversion)
   * Start at 31 (Vmid)
   * VR+ = VREF buffered, VR- = AVSS */
  ADC12MCTL0 = ADC12EOS | ADC12VRSEL_1 | ADC12INCH_31;
  /* Interrupt on completion of ADC into MEM0*/
  ADC12IER0 = ADC12IE0;
  /* Start the conversion process while we get other things set up. */
  ADC12CTL0 |= ADC12ENC + ADC12SC;
  vdd_mV = 0;

  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    unsigned int events = 0;
    struct tm rtc_tm;

    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      events |= events_ni;
      events_ni = 0;
      rtc_tm = rtc_tm_ni;
      if (! events) {
        unsigned long active_down_utt;

        if (FLG_ENTER_LPM3p5 & flags) {
          if (0 && tty) {
            cprintf("Sleep LPM3.5\n");
          }
          BSP430_PMM_ENTER_LPMXp5_NI();
        } else {
          if (FLG_WITH_LEDS & flags) {
            vBSP430ledSet(LED_LPM3, 1);
          }
          if (0 && tty) {
            cprintf("Sleep LPM3\n");
          }
        }
        if (tty) {
          /* NB: Power consumption anomalies if pin associated with RX
           * is taken out of peripheral function mode. */
          iBSP430serialSetReset_rh(tty, -1);
          //iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_EUSCI_A0, BSP430_PERIPHCFG_SERIAL_UART, 0);
          P2OUT &= ~(BIT0 | BIT1);
          P2DIR |= BIT0 | BIT1;
          //P2SEL1 &= ~(BIT0 | BIT1);
        }
        if (FLG_WITH_LEDS & flags) {
          vBSP430ledSet(LED_ACTIVE, 0);
        }
        state.last_elapsed_s = elapsed_s;
        state.last_vdd_mV = vdd_mV;
        state.sleeps += 1;
        active_down_utt = ulBSP430uptime_ni();
        state.cumulative_active_utt += (active_down_utt - active_up_utt);
#define DO_SUSPEND_UPTIME 1
#if (DO_SUSPEND_UPTIME - 0)
        vBSP430uptimeSuspend_ni();
#endif
        BSP430_CORE_LPM_ENTER_NI(LPM3_bits);
        iters = 0;
#if (DO_SUSPEND_UPTIME - 0)
        vBSP430uptimeResume_ni();
#endif /* DO_SUSPEND_UPTIME */
        active_up_utt = ulBSP430uptime_ni();
#if ! (DO_SUSPEND_UPTIME - 0)
        state.cumulative_lpm3_utt += (active_up_utt - active_down_utt);
#endif /* DO_SUSPEND_UPTIME */
        REFCTL0 = state.refvsel | REFON;
        BSP430_CORE_DELAY_CYCLES((75 * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000UL);
        ADC12CTL0 |= ADC12ON | ADC12ENC | ADC12SC;
        flags &= ~FLG_ENTER_LPM3p5;
        if (FLG_WITH_LEDS & flags) {
          vBSP430ledSet(LED_LPM3, 0);
          vBSP430ledSet(LED_ACTIVE, 1);
        }
        if (tty) {
          iBSP430platformConfigurePeripheralPins_ni(BSP430_PERIPH_EUSCI_A0, BSP430_PERIPHCFG_SERIAL_UART, 1);
          iBSP430serialSetReset_rh(tty, 0);
        }
      }
    } while (! events); // (EVT_ADC12RDY & events));
    BSP430_CORE_ENABLE_INTERRUPT();
    ++iters;
    if ((EVT_RTCRDY | EVT_RTC1MIN) & events) {
      elapsed_s = rtc_tm.tm_sec + 60 * (rtc_tm.tm_min + 60 * rtc_tm.tm_hour);
      if (0 && tty) {
        cprintf("Time: %4u-%02u-%02uT%02u:%02u:%02uZ = %u\n",
                rtc_tm.tm_year + 1900,
                rtc_tm.tm_mon + 1,
                rtc_tm.tm_mday,
                rtc_tm.tm_hour,
                rtc_tm.tm_min,
                rtc_tm.tm_sec,
                elapsed_s);
      }
    }
    if (EVT_ADC12RDY & events) {
      uint32_t r32 = vmid_raw;
      unsigned int vref_scale;

      if (REFVSEL__1p2 == state.refvsel) {
        r32 = ((r32 << 1) * cal_ref->cal_adc_15vref_factor) >> 16;
        vref_scale = 1200;
      } else {
        r32 = ((r32 << 1) * cal_ref->cal_adc_20vref_factor) >> 16;
        vref_scale = 2000;
      }
      r32 = ((r32 << 1) * cal_adc->cal_adc_gain_factor) >> 16;
      vdd_mV = (vref_scale * r32) >> 11;
      if ((REFVSEL__1p2 == state.refvsel) && (vdd_mV > 2200)) {
        state.refvsel = REFVSEL__2p0;
      } else if ((REFVSEL__2p0 == state.refvsel) && (vdd_mV < 2100)) {
        state.refvsel = REFVSEL__1p2;
      }
      if (0 && tty) {
        cprintf("vmid raw %u ; vdd %u mV\n", vmid_raw, vdd_mV);
      }
    }
    if (tty) {
      cprintf("b %u s %u i %u e %x ; el %u (sec) ; lpm3 %lu act %lu (utt) ; vdd %u (mV)\n",
              state.boots, state.sleeps, iters, events,
              elapsed_s,
              state.cumulative_lpm3_utt,
              state.cumulative_active_utt,
              vdd_mV);
    }
    flags |= FLG_ENTER_LPM3p5;
  }
}
