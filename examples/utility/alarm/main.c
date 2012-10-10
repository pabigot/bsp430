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
#include <bsp430/utility/cli.h>
#include <bsp430/periph/timer.h>
#include <bsp430/periph/flash.h>
#include <string.h>

#define MAX_TIMERS 7
sBSP430timerAlarm alarm_data[MAX_TIMERS];
hBSP430timerAlarm alarm[MAX_TIMERS];
size_t nTimers;
unsigned long wakeups;

enum {
  FLG_Active = 0x01,
  FLG_Repeating = 0x02,
  FLG_WakeFromLPM = 0x04,
};

typedef struct sAlarmStats {
  int flags;
  size_t count;
  int rc;
  unsigned long interval_tck;
  unsigned long sum_late;
  unsigned long last_late;
  unsigned long max_late;
} sAlarmStats;

volatile sAlarmStats alarm_stats[MAX_TIMERS];

static int
alarmCallback_ni (hBSP430timerAlarm alarm)
{
  volatile sAlarmStats * ap;
  unsigned long now_utt = ulBSP430uptime_ni();

  ap = alarm_stats + alarm->ccidx;
  ap->count += 1;
  ap->last_late = now_utt - alarm->setting_tck;
  if (ap->last_late > ap->max_late) {
    ap->max_late = ap->last_late;
  }
  ap->sum_late += ap->last_late;
  if (0 < ap->interval_tck) {
    ap->rc = iBSP430timerAlarmSet_ni(alarm, alarm->setting_tck + ap->interval_tck);
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
cmd_uptime (const char * command)
{
  int ci;
  volatile sBSP430hplTIMER * hpl = xBSP430hplLookupTIMER(BSP430_UPTIME_TIMER_PERIPH_HANDLE);

  cprintf("Uptime %lu with %lu wakeups\n", ulBSP430uptime(), wakeups);
  for (ci = 0; ci < nTimers; ++ci) {
    cprintf("\tCC %u: CCTL %04x CCR %04x\n", ci, hpl->cctl[ci], hpl->ccr[ci]);
  }
  return 0;
}
static sBSP430cliCommand dcmd_uptime = {
  .key = "uptime",
  .help = "# Show uptime timer info",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_uptime
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_uptime)

static int
cmd_stats (const char * command)
{
  int ai;

  cprintf("Alarm statistics:\n");
  for (ai = 0; ai < nTimers; ++ai) {
    volatile sAlarmStats * sp = alarm_stats + ai;
    cprintf("\t%u: flg 0x%02x, cnt %u, rc %d, inter %lu, late: %lu, max %lu, ave %lu\n",
            ai, sp->flags, sp->count, sp->rc, sp->interval_tck, sp->last_late, sp->max_late,
            (0 == sp->count) ? 0 : (sp->sum_late / sp->count));
  }
  return 0;
}
static sBSP430cliCommand dcmd_stats = {
  .key = "stats",
  .help = "# Show alarm stats",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_stats
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_stats)

static int
cmd_dump (const char * command)
{
  int ai;

  cprintf("Alarm internals:\n");
  for (ai = 0; ai < nTimers; ++ai) {
    hBSP430timerAlarm ap = alarm[ai];
    cprintf("\tcc %u, flg 0x%02x, setting %lu\n",
            ap->ccidx, ap->flags, ap->setting_tck);
  }
  return 0;
}
static sBSP430cliCommand dcmd_dump = {
  .key = "dump",
  .help = "# Show alarm internals",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_dump
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_dump)

static int
cmd_configure (const char * command)
{
  int rv;
  unsigned int cc;
  int enablep = 1;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);

  argstr_len = strlen(argstr);
  rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
  if ((0 == rv) && (0 < argstr_len)) {
    rv = iBSP430cliStoreExtractedI(&argstr, &argstr_len, &enablep);
    if (0 != rv) {
      cprintf("Invalid enable flag\n");
    }
  }
  if (0 == rv) {
    if ((0 <= cc) && (cc < nTimers)) {
      if (enablep) {
        rv = iBSP430timerAlarmEnable(alarm[cc]);
      } else {
        rv = iBSP430timerAlarmDisable(alarm[cc]);
      }
    } else {
      cprintf("Invalid cc value %u\n", cc);
    }
  } else {
    cprintf("Unable to extract cc index from '%s'\n", command);
  }
  return rv;
}
static sBSP430cliCommand dcmd_configure = {
  .key = "configure",
  .help = "{cc} [0|1] # enable/disable alarm cc",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_configure
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_configure)

static int
cmd_wake (const char * command)
{
  int rv;
  unsigned int cc;
  int enablep = 1;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);

  argstr_len = strlen(argstr);
  rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
  if ((0 == rv) && (0 < argstr_len)) {
    rv = iBSP430cliStoreExtractedI(&argstr, &argstr_len, &enablep);
    if (0 != rv) {
      cprintf("Invalid enable flag\n");
    }
  }
  if ((0 == rv) && (0 <= cc) && (cc < nTimers)) {
    if (enablep) {
      alarm_stats[cc].flags |= FLG_WakeFromLPM;
    } else {
      alarm_stats[cc].flags &= ~FLG_WakeFromLPM;
    }
  } else {
    cprintf("Bad input: %s\n", command);
  }
  return rv;
}
static sBSP430cliCommand dcmd_wake = {
  .key = "wake",
  .help = "{cc} [0|1] # enable/disable wake on alarm",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wake
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_wake)

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
    BSP430_CORE_INTERRUPT_STATE_T istate;
    BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
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
static sBSP430cliCommand dcmd_cancel = {
  .key = "cancel",
  .help = "{cc} # cancel alarm cc",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_cancel
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
      unsigned long abs_when;
      BSP430_CORE_INTERRUPT_STATE_T istate;
      BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
      BSP430_CORE_DISABLE_INTERRUPT();
      do {
        abs_when = ulBSP430uptime_ni() + rel_when;
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
static sBSP430cliCommand dcmd_alarm = {
  .key = "alarm",
  .help = "{cc} {rel_when} # set alarm for rel_when ticks",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_alarm
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_alarm)

static int
cmd_reset (const char * command)
{
  int rv;
  unsigned int cc;
  const char * argstr = command;
  size_t argstr_len = strlen(argstr);

  argstr_len = strlen(argstr);
  rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &cc);
  if ((0 == rv) && (0 <= cc) && (cc < nTimers)) {
    volatile sAlarmStats * sp = alarm_stats + cc;
    BSP430_CORE_INTERRUPT_STATE_T istate;

    BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      sp->count = 0;
      sp->last_late = 0;
      sp->max_late = 0;
      sp->sum_late = 0;
    } while (0);
    BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  } else {
    cprintf("Bad input: %s\n", command);
  }
  return rv;
}
static sBSP430cliCommand dcmd_reset = {
  .key = "reset",
  .help = "{cc} # reset alarm statistics",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_reset
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_reset)

static int
cmd_repeat (const char * command)
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
static sBSP430cliCommand dcmd_repeat = {
  .key = "repeat",
  .help = "{cc} {interval} # set alarm interval, 0 one-shot",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_repeat
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_repeat)

static int
cmd_help (sBSP430cliCommandLink * chain,
          void * param,
          const char * command,
          size_t command_len)
{
  vBSP430cliConsoleDisplayHelp(chain->cmd);
  return 0;
}
static sBSP430cliCommand dcmd_help = {
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

  nTimers = iBSP430timerSupportedCCs(BSP430_UPTIME_TIMER_PERIPH_HANDLE);
  cprintf("Alarm testing with timer %s having %u CCs\n",
          xBSP430timerName(BSP430_UPTIME_TIMER_PERIPH_HANDLE), nTimers);
  if (nTimers > MAX_TIMERS) {
    nTimers = MAX_TIMERS;
  }

  for (ai = 0; ai < nTimers; ++ai) {
    alarm[ai] = hBSP430timerAlarmInitialize(alarm_data + ai, BSP430_UPTIME_TIMER_PERIPH_HANDLE, ai, alarmCallback_ni);
  }

  /* NOTE: The control flow in this is a bit tricky, as we're trying
   * to leave interrupts enabled during the main body of the loop,
   * while they must be disabled when processing input to recognize a
   * command.  Both flags and command serve as signals as well as
   * values. */
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
      cputchar_ni('\a');
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
    BSP430_CORE_LPM_ENTER_NI(LPM2_bits | GIE);
    ++wakeups;
  }

}
