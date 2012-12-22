/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/lpm.h>
#include <bsp430/utility/led.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/periph/port.h>
#include <bsp430/periph/pmm.h>
#include <bsp430/periph/sys.h>
#include <string.h>

#if ! (BSP430_PLATFORM_BUTTON0 - 0)
#error No button available on this platform
#endif /* BSP430_PLATFORM_BUTTON0 */

/* Core voltage control is supported on 5xx/6xx modules with PMM
 * support but not on FRAM devices. */
#define APP_ENABLE_COREV (BSP430_MODULE_PMM - 0) && ! (BSP430_MODULE_PMM_FRAM - 0)

typedef struct sCommand {
  char cmd;
  const char * description;
} sCommand;

const sCommand commands[] = {
#define CMD_MODE_ACTIVE 'a'
  { CMD_MODE_ACTIVE, "Remain in active mode while idle" },
#define CMD_MODE_LPM0 '0'
  { CMD_MODE_LPM0, "Set mode to enter LPM0" },
#define CMD_MODE_LPM1 '1'
  { CMD_MODE_LPM1, "Set mode to enter LPM1" },
#define CMD_MODE_LPM2 '2'
  { CMD_MODE_LPM2, "Set mode to enter LPM2" },
#define CMD_MODE_LPM3 '3'
  { CMD_MODE_LPM3, "Set mode to enter LPM3" },
#define CMD_MODE_LPM4 '4'
  { CMD_MODE_LPM4, "Set mode to enter LPM4" },
#ifdef BSP430_PMM_ENTER_LPMXp5_NI
#define CMD_MODE_LPM3p5 '5'
  { CMD_MODE_LPM3p5, "Set mode to enter LPM3.5" },
#define CMD_MODE_LPM4p5 '6'
  { CMD_MODE_LPM4p5, "Set mode to enter LPM4.5" },
#endif /* BSP430_PMM_ENTER_LPMXp5_NI */
#define CMD_HOLD_SERIAL 's'
  { CMD_HOLD_SERIAL, "Toggle whether serial is placed on hold during LPM" },
#define CMD_HOLD_CLOCK 'u'
  { CMD_HOLD_CLOCK, "Toggle whether uptime clock is placed on hold during LPM" },
#define CMD_HELP '?'
  { CMD_HELP, "Display help" },
#define CMD_STATE '='
  { CMD_STATE, "Display system state (clock speeds, etc.)" },
#define CMD_SLEEP '$'
  { CMD_SLEEP, "Enter sleep mode" },
#if APP_ENABLE_COREV
#define CMD_COREV_INCR '>'
  { CMD_COREV_INCR, "Increment core voltage" },
#define CMD_COREV_DECR '<'
  { CMD_COREV_DECR, "Decrement core voltage" },
#endif /* APP_ENABLE_COREV */
};

typedef struct sState {
  unsigned int lpm_bits;
  const char * lpm_description;
  int hold_serial;
  int hold_clock;
} sState;

volatile int button;

static int
button_isr_ni (const struct sBSP430halISRIndexedChainNode * cb,
               void * context,
               int idx)
{
  (void)cb;
  (void)context;
  (void)idx;
  ++button;
  return BSP430_HAL_ISR_CALLBACK_DISABLE_INTERRUPT
         | BSP430_HAL_ISR_CALLBACK_BREAK_CHAIN
         | BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
}

const sBSP430halISRIndexedChainNode button_cb = {
  .next_ni = NULL,
  .callback = button_isr_ni,
};

char rx_buffer[16];
char * volatile rx_head;
char * volatile rx_tail;

static int
consume_rx_ni ()
{
  int rc;

  if (rx_head == rx_tail) {
    return -1;
  }
  rc = *rx_tail++;
  if (rx_tail > (rx_buffer + sizeof(rx_buffer) / sizeof(*rx_buffer))) {
    rx_tail = rx_buffer;
  }
  return rc;
}

static int
consume_rx ()
{
  int rc;
  BSP430_CORE_INTERRUPT_STATE_T istate;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  rc = consume_rx_ni();
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

static int
rx_cbchain_ni (const struct sBSP430halISRVoidChainNode * cb,
               void * context)
{
  hBSP430halSERIAL device = (hBSP430halSERIAL)context;

  *rx_head++ = device->rx_byte;
  if (rx_head > (rx_buffer + sizeof(rx_buffer) / sizeof(*rx_buffer))) {
    rx_head = rx_buffer;
  }
  if (rx_head == rx_tail) {
    (void)consume_rx_ni;
  }
  /* NOTE: For this test intentionally clear all LPM bits, not just
   * the ones in BSP430_CORE_LPM_EXIT_MASK as would happen with
   * BSP430_HAL_ISR_CALLBACK_EXIT_LPM. */
  return LPM4_bits;
}

struct sBSP430halISRVoidChainNode rx_cb = {
  .callback = rx_cbchain_ni
};

void main ()
{
  hBSP430halPORT b0hal;
  hBSP430halSERIAL tty;
  volatile sBSP430hplPORTIE * b0hpl;
  int b0pin;
  sState state;
#if BSP430_MODULE_SYS - 0
  unsigned long reset_causes = 0;
  unsigned int reset_flags = 0;
#endif /* BSP430_MODULE_SYS */

#if BSP430_MODULE_SYS - 0
  {
    unsigned int sysrstiv;

    /* Record all the reset causes */
    while (0 != ((sysrstiv = uiBSP430sysSYSRSTGenerator_ni(&reset_flags)))) {
      reset_causes |= 1UL << (sysrstiv / 2);
    }

#ifdef BSP430_PMM_ENTER_LPMXp5_NI
    /* If we woke from LPMx.5, we need to clear the lock in PM5CTL0.
     * We'll do it early, since we're not really interested in
     * retaining the current IFG settings. */
    if (reset_flags & BSP430_SYS_FLAG_SYSRST_LPM5WU) {
      PMMCTL0_H = PMMPW_H;
      PM5CTL0 = 0;
      PMMCTL0_H = 0;
    }
#endif /* BSP430_PMM_ENTER_LPMXp5_NI */
  }
#endif /* BSP430_MODULE_SYS */

#if APP_CONFIGURE_PORTS_FOR_LPM
  vBSP430lpmConfigurePortsForLPM_ni();
#endif /* APP_CONFIGURE_PORTS_FOR_LPM */
  vBSP430platformInitialize_ni();

  (void)iBSP430consoleInitialize();

  cprintf("Application is running\n");
#if BSP430_MODULE_SYS - 0
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

  cputtext_ni("System reset included:");
  if (reset_flags & BSP430_SYS_FLAG_SYSRST_BOR) {
    cputtext_ni(" BOR");
  }
  if (reset_flags & BSP430_SYS_FLAG_SYSRST_LPM5WU) {
    cputtext_ni(" LPM5WU");
  }
  if (reset_flags & BSP430_SYS_FLAG_SYSRST_POR) {
    cputtext_ni(" POR");
  }
  if (reset_flags & BSP430_SYS_FLAG_SYSRST_PUC) {
    cputtext_ni(" PUC");
  }
  cputchar_ni('\n');

#endif

  b0hal = hBSP430portLookup(BSP430_PLATFORM_BUTTON0_PORT_PERIPH_HANDLE);
  b0pin = iBSP430portBitPosition(BSP430_PLATFORM_BUTTON0_PORT_BIT);
  b0hpl = BSP430_PORT_HAL_GET_HPL_PORTIE(b0hal);
  b0hal->pin_cbchain_ni[b0pin] = &button_cb;
  b0hpl->sel &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
  b0hpl->dir &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;

#if BSP430_PORT_SUPPORTS_REN - 0
  BSP430_PORT_HAL_HPL_REN(b0hal) |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
  BSP430_PORT_HAL_HPL_OUT(b0hal) |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
#endif /* BSP430_PORT_SUPPORTS_REN */

  rx_head = rx_tail = rx_buffer;
  /* A careful coder would check to return values in the following */
  tty = hBSP430console();
  (void)iBSP430serialSetHold_ni(tty, 1);
  rx_cb.next_ni = tty->rx_cbchain_ni;
  tty->rx_cbchain_ni = &rx_cb;
  (void)iBSP430serialSetHold_ni(tty, 0);
  *rx_head++ = CMD_MODE_ACTIVE;
  *rx_head++ = CMD_STATE;

  memset(&state, 0, sizeof(state));
  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    int enter_sleep = 0;
    unsigned long int sleep_utt;
    unsigned long int wake_utt;
    do {
      int c;

      while (0 <= ((c = consume_rx()))) {
        const sCommand * cmdp = commands;
        const sCommand * const commands_end = commands + sizeof(commands) / sizeof(*commands);
        while ((cmdp < commands_end) && (cmdp->cmd != c)) {
          ++cmdp;
        }
        if (cmdp->cmd == c) {
          cputs(cmdp->description);
          switch (cmdp->cmd) {
            case CMD_MODE_ACTIVE:
              state.lpm_bits = 0;
              state.lpm_description = "Active";
              break;
            case CMD_MODE_LPM0:
              state.lpm_bits = LPM0_bits;
              state.lpm_description = "LPM0";
              break;
            case CMD_MODE_LPM1:
              state.lpm_bits = LPM1_bits;
              state.lpm_description = "LPM1";
              break;
            case CMD_MODE_LPM2:
              state.lpm_bits = LPM2_bits;
              state.lpm_description = "LPM2";
              break;
            case CMD_MODE_LPM3:
              state.lpm_bits = LPM3_bits;
              state.lpm_description = "LPM3";
              break;
            case CMD_MODE_LPM4:
              state.lpm_bits = LPM4_bits;
              state.lpm_description = "LPM4";
              break;
#ifdef BSP430_PMM_ENTER_LPMXp5_NI
            case CMD_MODE_LPM3p5:
              state.lpm_bits = BSP430_CORE_LPM_LPMXp5 | LPM3_bits;
              state.lpm_description = "LPM3.5";
              break;
            case CMD_MODE_LPM4p5:
              state.lpm_bits = BSP430_CORE_LPM_LPMXp5 | LPM4_bits;
              state.lpm_description = "LPM4.5";
              break;
#endif /* BSP430_PMM_ENTER_LPMXp5_NI */
            case CMD_HELP: {
              cmdp = commands;
              cprintf("Available commands:\n");
              while (cmdp < commands_end) {
                cprintf("\t%c : %s\n", cmdp->cmd, cmdp->description);
                ++cmdp;
              }
              break;
            }
            case CMD_STATE:
              cprintf("MCLK rate: %lu Hz\n", ulBSP430clockMCLK_Hz_ni());
              cprintf("SMCLK rate: %lu Hz\n", ulBSP430clockSMCLK_Hz_ni());
              cprintf("ACLK rate: %u Hz\n", uiBSP430clockACLK_Hz_ni());
              cprintf("LFXT1: %s\n", BSP430_CLOCK_LFXT1_IS_FAULTED_NI() ? "FAULTED" : "stable");
              cprintf("Uptime (ACLK ticks): %lu\n", ulBSP430uptime_ni());
              cprintf("Selected idle state: %s\n", state.lpm_description);
              cprintf("Clocks will %s\n", state.hold_clock ? "freeze" : "run");
              cprintf("Serial will %s\n", state.hold_serial ? "be held" : "be active");
#if APP_ENABLE_COREV
              cprintf("Core voltage level %d\n", PMMCTL0 & PMMCOREV_3);
#endif /* APP_ENABLE_COREV */
              break;
            case CMD_HOLD_SERIAL:
              state.hold_serial = ! state.hold_serial;
              break;
            case CMD_HOLD_CLOCK:
              state.hold_clock = ! state.hold_clock;
              break;
            case CMD_SLEEP:
              enter_sleep = 1;
              break;
#if APP_ENABLE_COREV
            case CMD_COREV_INCR:
            case CMD_COREV_DECR: {
              int delta = 0;
              int rc = 0;
              BSP430_CORE_DISABLE_INTERRUPT();
              do {
                unsigned int level = PMMCTL0 & PMMCOREV_3;
                if ((cmdp->cmd == CMD_COREV_INCR) && (level < PMMCOREV_3)) {
                  delta = 1;
                }
                if ((cmdp->cmd == CMD_COREV_DECR) && (level > PMMCOREV_0)) {
                  delta = -1;
                }
                if (0 != delta) {
                  rc = iBSP430pmmSetCoreVoltageLevel_ni(level + delta);
                }
              } while (0);
              BSP430_CORE_ENABLE_INTERRUPT();
              if (0 == delta) {
                cprintf("Core voltage at limit\n");
              } else {
                cprintf("Core voltage adjusted %d to %d rv %d\n", delta, PMMCTL0 & PMMCOREV_3, rc);
              }
              cprintf("PMM: CTL0 %04x CTL1 %04x\n", PMMCTL0, PMMCTL1);
              break;
            }
#endif /* APP_ENABLE_COREV */
          }
        } else {
          cprintf("Unrecognized command: %c\n", c);
        }

      }
    } while (! enter_sleep);
    BSP430_CORE_DISABLE_INTERRUPT();
    sleep_utt = ulBSP430uptime_ni();
    cprintf("Entering idle mode at %lu: %s\n", sleep_utt, state.lpm_description);
    if (state.lpm_bits & BSP430_CORE_LPM_LPMXp5) {
      cprintf("NOTE: Will use LPMx.5: press button to exit\n");
    }
    if (state.hold_clock) {
      cprintf("Suspending clock\n");
      vBSP430uptimeSuspend_ni();
    }
    if (state.hold_serial) {
      cprintf("Disabling serial; press button to wake\n\n");
      iBSP430serialSetHold_ni(tty, 1);
    }
    if (b0hpl->in & BSP430_PLATFORM_BUTTON0_PORT_BIT) {
      b0hpl->ies |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
    } else {
      b0hpl->ies &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
    }
    b0hpl->ifg &= ~BSP430_PLATFORM_BUTTON0_PORT_BIT;
    b0hpl->ie |= BSP430_PLATFORM_BUTTON0_PORT_BIT;
    if (0 == state.lpm_bits) {
      button = 0;
      BSP430_CORE_ENABLE_INTERRUPT();
      while ((rx_head == rx_tail) && (! button)) {
      }
    } else {
#ifdef BSP430_PMM_ENTER_LPMXp5_NI
      if (state.lpm_bits & BSP430_CORE_LPM_LPMXp5) {
        BSP430_PMM_ENTER_LPMXp5_NI();
      }
#endif /* BSP430_PMM_ENTER_LPMXp5_NI */
      BSP430_CORE_LPM_ENTER_NI(state.lpm_bits | GIE);
    }
    BSP430_CORE_DISABLE_INTERRUPT();
    wake_utt = ulBSP430uptime_ni();
    if (state.hold_serial) {
      iBSP430serialSetHold_ni(tty, 0);
      cprintf("Serial now awake\n");
    }
    if (state.hold_clock) {
      vBSP430uptimeResume_ni();
      cprintf("Clock resumed\n");
    }
    cprintf("Left idle mode at %lu: %lu asleep\n", wake_utt, wake_utt - sleep_utt);
    BSP430_CORE_ENABLE_INTERRUPT();
  }
}
