/** This file is in the public domain.
 *
 * Base-2 primes for intervals: 17 31 61 127 257 509 1021
 * Base-10 primes for intervals: 11 101 499 997 4999 10007
 *
 * Define APP_SOURCE_SMCLK=1 to use SMCLK as the alarm timer source;
 * otherwise ACLK will be used.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/cli.h>
#include <bsp430/periph/timer.h>
#include <bsp430/periph/flash.h>
#include <string.h>

#define ALARM_TIMER_PERIPH_HANDLE BSP430_TIMER_CCACLK_PERIPH_HANDLE
#if (APP_SOURCE_SMCLK - 0)
#define APP_TASSEL TASSEL_2
#else /* APP_SOURCE_SMCLK */
#define APP_TASSEL TASSEL_1
#endif /* APP_SOURCE_SMCLK */

static hBSP430halTIMER alarmHAL_;
unsigned long alarm_Hz;

#define MAX_TIMERS 7
sBSP430timerAlarm alarm_data[MAX_TIMERS];
hBSP430timerAlarm alarm[MAX_TIMERS];
size_t nTimers;
unsigned long wakeups;

enum {
  FLG_SkipLost = 0x01,
  FLG_WakeFromLPM = 0x02,
};

typedef struct sAlarmStats {
  int flags;
  unsigned long count;
  int rc;
  unsigned long interval_tck;
  unsigned long sum_late;
  unsigned long last_late;
  unsigned long max_late;
  unsigned long lost;
} sAlarmStats;

volatile sAlarmStats alarm_stats[MAX_TIMERS];

static int
alarmCallback_ni (hBSP430timerAlarm alarm)
{
  volatile sAlarmStats * ap;
  unsigned long now_tck = ulBSP430timerCounter_ni(alarmHAL_, NULL);

  ap = alarm_stats + alarm->ccidx;
  ap->count += 1;
  ap->last_late = now_tck - alarm->setting_tck;
  if (ap->last_late > ap->max_late) {
    ap->max_late = ap->last_late;
  }
  ap->sum_late += ap->last_late;
  if (0 < ap->interval_tck) {
    unsigned long setting_tck = alarm->setting_tck + ap->interval_tck;
    unsigned int lost = 0;
    /* Pre-check for high-speed recurring short intervals that might
     * violate #BSP430_TIMER_ALARM_PAST_LIMIT.  This could still fail
     * if we're so far behind that now_tck gets out of date, but the
     * cost of updating it can also be significant. */
    if (setting_tck <= now_tck) {
      lost = (now_tck - setting_tck + ap->interval_tck - 1) / ap->interval_tck;
      setting_tck += lost * ap->interval_tck;
    }
    ap->rc = iBSP430timerAlarmSet_ni(alarm, setting_tck);
    while ((0 != ap->rc) && (ap->flags & FLG_SkipLost) && (1000 > lost)) {
      ++lost;
      setting_tck += ap->interval_tck;
      ap->rc = iBSP430timerAlarmSet_ni(alarm, setting_tck);
    }
    ap->lost += lost;
  } else {
  }
  return (ap->flags & FLG_WakeFromLPM) ? BSP430_HAL_ISR_CALLBACK_EXIT_LPM : 0;
}

/* Recommended pattern: a global variable that holds the entrypoint to
 * the supported commands, and a macro that is redefined after each
 * command is hooked in.  The macro allows commands to be disabled or
 * rearranged without having to change the links. */
const sBSP430cliCommand * commandSet;
#define LAST_COMMAND NULL

static int
cmd_cancel (const char * command)
{
  int rv;
  unsigned int cc;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);

  argstr_len = strlen(argstr);
  rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
  if ((0 == rv) && (0 <= cc) && (cc < nTimers)) {
    BSP430_CORE_SAVED_INTERRUPT_STATE(istate);

    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      rv = iBSP430timerAlarmCancel_ni(alarm[cc]);
    } while (0);
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
    cprintf("Cancel %u produced %d\n", cc, rv);
  } else {
    cprintf("Bad input: %s\n", command);
  }
  return rv;
}
static const sBSP430cliCommand dcmd_cancel = {
  .key = "cancel",
  .help = "{cc} # cancel alarm cc",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_cancel
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_cancel)

static int
cmd_alarm (const char * command)
{
  int rv;
  unsigned int cc;
  long rel_when;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);

  argstr_len = strlen(argstr);
  rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
  if (0 == rv) {
    rv = iBSP430cliStoreExtractedL(&argstr, &argstr_len, &rel_when);
  }
  if (0 == rv) {
    if ((0 <= cc) && (cc < nTimers)) {
      BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
      unsigned long abs_when;

      BSP430_CORE_DISABLE_INTERRUPT();
      do {
        abs_when = ulBSP430timerCounter_ni(alarmHAL_, NULL) + rel_when;
        rv = iBSP430timerAlarmSet_ni(alarm[cc], abs_when);
      } while (0);
      BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
      cprintf("Set %u in %ld (%lu) produced %d\n",
              cc, rel_when, abs_when, rv);
    } else {
      cprintf("Invalid cc value %u\n", cc);
    }
  } else {
    cprintf("Bad input: %s\n", command);
  }
  return rv;
}
static const sBSP430cliCommand dcmd_alarm = {
  .key = "alarm",
  .help = "{cc} {rel_when} # set alarm for rel_when ticks",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_alarm
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_alarm)

static int
cmd_flags (const char * command)
{
  int rv;
  unsigned int cc;
  const char * key;
  int enablep = 0;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);
  int flags = 0;

  argstr_len = strlen(argstr);
  key = argstr;
  rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
  if ((0 > cc) || (nTimers <= cc)) {
    rv = -1;
  }
  while (0 == rv) {
    unsigned int flag;
    size_t key_len;

    key = xBSP430cliNextToken(&argstr, &argstr_len, &key_len);
    if (0 == key_len) {
      break;
    }
    enablep = -1;
    if ('+' == *key) {
      enablep = 1;
    } else if ('-' == *key) {
      enablep = 0;
    } else {
      rv = -1;
    }
    ++key;
    --key_len;
    if (0 == strncmp("wake", key, key_len)) {
      flag = FLG_WakeFromLPM;
    } else if (0 == strncmp("skip", key, key_len)) {
      flag = FLG_SkipLost;
    } else {
      rv = -1;
    }
    if (0 == rv) {
      flags |= flag;
      if (enablep) {
        alarm_stats[cc].flags |= flag;
      } else {
        alarm_stats[cc].flags &= ~flag;
      }
    }
  }
  cprintf("Successfully modified flags 0x%x\n", flags);
  if (0 != rv) {
    cprintf("error at %s\n", key);
  }
  return rv;
}
static const sBSP430cliCommand dcmd_flags = {
  .key = "flags",
  .help = "{cc} {[+-]flg}... # enable/disable wake skip",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_flags
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_flags)

static int
cmd_interval (const char * command)
{
  int rv;
  unsigned int cc;
  unsigned long interval;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);

  argstr_len = strlen(argstr);
  rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
  if (0 == rv) {
    rv = iBSP430cliStoreExtractedUL(&argstr, &argstr_len, &interval);
  }
  if (0 == rv) {
    if ((0 <= cc) && (cc < nTimers)) {
      alarm_stats[cc].interval_tck = interval;
    } else {
      cprintf("Invalid cc value %u\n", cc);
    }
  } else {
    cprintf("Bad input: %s\n", command);
  }
  return rv;
}
static const sBSP430cliCommand dcmd_interval = {
  .key = "interval",
  .help = "{cc} {interval} # set alarm interval, 0 one-shot",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_interval
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_interval)

static int
cmd_configure_ (const char * command,
                int enablep)
{
  int rv;
  unsigned int cc;
  const char * input;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);

  argstr_len = strlen(argstr);
  do {
    input = argstr;
    rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
    if ((0 == rv) && (0 <= cc) && (cc < nTimers)) {
      if (enablep) {
        rv = iBSP430timerAlarmEnable(alarm[cc]);
      } else {
        rv = iBSP430timerAlarmDisable(alarm[cc]);
      }
      if (0 != rv) {
        cprintf("Failed cc index %d: %d\n", cc, rv);
        return rv;
      }
    } else {
      rv = -1;
    }
  } while (0 == rv);
  if (0 < argstr_len) {
    cprintf("Invalid cc index at '%s'\n", input);
    return -1;
  }
  return 0;
}

static int
cmd_disable (const char * command)
{
  return cmd_configure_(command, 0);
}
static const sBSP430cliCommand dcmd_disable = {
  .key = "disable",
  .help = "{cc}... # disable alarm cc",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_disable
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_disable)

static int
cmd_enable (const char * command)
{
  return cmd_configure_(command, 1);
}
static const sBSP430cliCommand dcmd_enable = {
  .key = "enable",
  .help = "{cc}... # enable alarm cc",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_enable
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_enable)

static int
cmd_reset (const char * command)
{
  int rv;
  unsigned int cc;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);

  do {
    rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
    if ((0 == rv) && (0 <= cc) && (cc < nTimers)) {
      BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
      volatile sAlarmStats * sp = alarm_stats + cc;

      BSP430_CORE_DISABLE_INTERRUPT();
      do {
        sp->count = 0;
        sp->last_late = 0;
        sp->max_late = 0;
        sp->sum_late = 0;
        sp->lost = 0;
      } while (0);
      BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
    }
  } while (0 == rv);
  return 0;
}
static const sBSP430cliCommand dcmd_reset = {
  .key = "reset",
  .help = "{cc}... # reset alarm statistics",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_reset
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_reset)

static int
cmd_stats (const char * command)
{
  int ai;

  cprintf("Alarm statistics:\n");
  for (ai = 0; ai < nTimers; ++ai) {
    volatile sAlarmStats * sp = alarm_stats + ai;
    if (NULL == alarm[ai]) {
      cprintf(" %u: not available\n", ai);
    } else {
      cprintf(" %u: %cset %cenab %cwake %cskip cnt %lu, rc %d, inter %lu\n"
              "    lost %lu; late: last %lu, max %lu, ave %lu\n",
              ai,
              (alarm[ai]->flags & BSP430_TIMER_ALARM_FLAG_SET) ? '+' : '-',
              (alarm[ai]->flags & BSP430_TIMER_ALARM_FLAG_ENABLED) ? '+' : '-',
              (sp->flags & FLG_WakeFromLPM) ? '+' : '-',
              (sp->flags & FLG_SkipLost) ? '+' : '-',
              sp->count, sp->rc, sp->interval_tck,
              sp->lost, sp->last_late, sp->max_late,
              (0 == sp->count) ? 0 : (sp->sum_late / sp->count));
    }
  }
  return 0;
}
static const sBSP430cliCommand dcmd_stats = {
  .key = "stats",
  .help = "# Show alarm stats",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_stats
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_stats)

static int
cmd_dump (const char * command)
{
  unsigned long now_tck;
  unsigned int overflows;
  int ai;

  now_tck = ulBSP430timerCounter(alarmHAL_, &overflows);
  cprintf("Timer ticks %lu (0x%08lx) with %u overflows and %lu wakeups\n",
          now_tck, now_tck, overflows, wakeups);
  cprintf("Alarm internals:\n");
  for (ai = 0; ai < nTimers; ++ai) {
    hBSP430timerAlarm ap = alarm[ai];

    if (NULL == ap) {
      cprintf("\tcc %u is not available\n", ai);
    } else {
      int ci = ap->ccidx;
      cprintf("\tcc %u, flg 0x%02x, setting %lu ; CCTL %04x CCR %04x\n",
              ci, ap->flags, ap->setting_tck,
              alarmHAL_->hpl->cctl[ci], alarmHAL_->hpl->ccr[ci]);
    }
  }
  return 0;
}
static const sBSP430cliCommand dcmd_dump = {
  .key = "dump",
  .help = "# Show alarm internals",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_dump
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_dump)

static int
cmd_convert (sBSP430cliCommandLink * chain,
             void * param,
             const char * command,
             size_t command_len)
{
  unsigned long when_tck;
  int rv;

  rv = iBSP430cliStoreExtractedUL(&command, &command_len, &when_tck);
  if (0 == rv) {
    unsigned long ms = (when_tck * 1000) / alarm_Hz;
    unsigned long us = (when_tck * 1000) / (alarm_Hz / 1000);
    cprintf("%lu tt at %lu Hz = %lu ms = %lu us\n", when_tck, alarm_Hz, ms, us);
  }
  return rv;
}
static const sBSP430cliCommand dcmd_convert = {
  .key = "convert",
  .help = "{tt} # show tt value in human time",
  .next = LAST_COMMAND,
  .handler = cmd_convert
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_convert)

static int
cmd_clocks (const char * argstr)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  unsigned long now_tck;
  unsigned long timer_overhead;

  cprintf("CPU clocks (Hz): MCLK %lu ; SMCLK %lu ; ACLK %lu\n",
          ulBSP430clockMCLK_Hz(), ulBSP430clockSMCLK_Hz(),
          ulBSP430clockACLK_Hz());

  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    unsigned long t0;

    t0 = ulBSP430timerCounter_ni(alarmHAL_, NULL);
    now_tck = ulBSP430timerCounter_ni(alarmHAL_, NULL);
    timer_overhead = now_tck - t0;
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  cprintf("Timer %s freq %lu Hz now %lu\n", xBSP430timerName(ALARM_TIMER_PERIPH_HANDLE), alarm_Hz, now_tck);
  cprintf("\tResolution %lu ns/tt, overhead %lu tt @ 32bit\n",
          1000000000UL/alarm_Hz, timer_overhead);
  return 0;
}
static const sBSP430cliCommand dcmd_clocks = {
  .key = "clocks",
  .help = "# Clock/timer information",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_clocks
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_clocks

static int
cmd_uptime (const char * command)
{
  cprintf("Up %s\n", xBSP430uptimeAsText_ni(ulBSP430uptime()));
  return 0;
}
static const sBSP430cliCommand dcmd_uptime = {
  .key = "uptime",
  .help = "# Show system uptime",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_uptime
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_uptime)

static int
cmd_help (sBSP430cliCommandLink * chain,
          void * param,
          const char * command,
          size_t command_len)
{
  vBSP430cliConsoleDisplayHelp(chain->cmd);
  return 0;
}
static const sBSP430cliCommand dcmd_help = {
  .key = "help",
  .help = "[cmd] # Show help on cmd or all commands",
  .next = LAST_COMMAND,
  .handler = cmd_help
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_help)

void main ()
{
  const char * command;
  int flags;
  int ai;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  vBSP430cliSetDiagnosticFunction(iBSP430cliConsoleDiagnostic);
  cprintf("alarm: " __DATE__ " " __TIME__ "\n");

  alarmHAL_ = hBSP430timerLookup(ALARM_TIMER_PERIPH_HANDLE);
  if (! alarmHAL_) {
    cprintf("ERROR -- Timer not available\n");
    return;
  }
  vBSP430timerResetCounter_ni(alarmHAL_);
  alarmHAL_->hpl->ctl = APP_TASSEL | MC_2 | TACLR | TAIE;
  vBSP430timerInferHints_ni(alarmHAL_);
  alarm_Hz = ulBSP430timerFrequency_Hz_ni(ALARM_TIMER_PERIPH_HANDLE);

  nTimers = iBSP430timerSupportedCCs(ALARM_TIMER_PERIPH_HANDLE);
  if (nTimers > MAX_TIMERS) {
    nTimers = MAX_TIMERS;
  }

  for (ai = 0; ai < nTimers; ++ai) {
    alarm[ai] = hBSP430timerAlarmInitialize(alarm_data + ai, ALARM_TIMER_PERIPH_HANDLE, ai, alarmCallback_ni);
  }

  cprintf("Alarm testing with timer %s having %u CCs\n",
          xBSP430timerName(ALARM_TIMER_PERIPH_HANDLE), nTimers);
  cmd_clocks("");

  /* NOTE: The control flow in this is a bit tricky, as we're trying
   * to leave interrupts enabled during the main body of the loop,
   * while they must be disabled when processing input to recognize a
   * command. */
  commandSet = LAST_COMMAND;
  command = NULL;
  flags = eBSP430cliConsole_REPAINT;

  wakeups = 0;
  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    if (flags & eBSP430cliConsole_DO_COMPLETION) {
      flags &= ~eBSP430cliConsole_DO_COMPLETION;
      flags |= iBSP430cliConsoleBufferCompletion(commandSet, &command);
    }
    if (flags & eBSP430cliConsole_READY) {
      int rv;

      rv = iBSP430cliExecuteCommand(commandSet, 0, command);
      if (0 != rv) {
        cprintf("Command execution returned %d\n", rv);
      }
      /* Ensure prompt is rewritten, but not the command we just
       * ran */
      flags |= eBSP430cliConsole_REPAINT;
      command = NULL;
    }
    if (flags & eBSP430cliConsole_REPAINT) {
      /* Draw the prompt along with whatever's left in the command
       * buffer.  Note use of leading carriage return in case an edit
       * left material on the current line. */
      cprintf("\r> %s", command ? command : "");
      flags &= ~eBSP430cliConsole_REPAINT;
    }
    if (flags & eBSP430cliConsole_REPAINT_BEL) {
      cputchar('\a');
      flags &= ~eBSP430cliConsole_REPAINT_BEL;
    }
    BSP430_CORE_DISABLE_INTERRUPT();
    if (flags & eBSP430cliConsole_READY) {
      /* Clear the command we just completed */
      vBSP430cliConsoleBufferClear_ni();
    }
    /* Unless we're processing escape characters, let
     * iBSP430cliConsoleBufferProcessInput_ni() handle the blocking
     * for input characters. */
    if (! (flags & eBSP430cliConsole_ANY_ESCAPE)) {
      flags = iBSP430cliConsoleBufferProcessInput_ni();
      if (flags) {
        /* Got something to do; get the command contents in place */
        command = xBSP430cliConsoleBuffer_ni();
        BSP430_CORE_ENABLE_INTERRUPT();
        continue;
      }
    }
    BSP430_CORE_LPM_ENTER_NI(LPM2_bits);
    ++wakeups;
  }

}
