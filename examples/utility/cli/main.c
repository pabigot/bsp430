/** This file is in the public domain.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/cli.h>
#include <bsp430/utility/led.h>
#include <string.h>
#include <ctype.h>

struct data_t {
  int ival;
  unsigned int uival;
  long lval;
  unsigned long ulval;
} data;

void
dumpData (void)
{
  cprintf("ival %d\n", data.ival);
  cprintf("uival %u (0x%x)\n", data.uival, data.uival);
  cprintf("lval %ld\n", data.lval);
  cprintf("ulval %lu (0x%lx)\n", data.ulval, data.ulval);
}

/* Recommended pattern: a global variable that holds the entrypoint to
 * the supported commands, and a macro that is redefined after each
 * command is hooked in.  The macro allows commands to be disabled or
 * rearranged without having to change the links. */
const sBSP430cliCommand * commandSet;
#define LAST_COMMAND NULL

static int
cmd_h234 (sBSP430cliCommandLink * chain,
          void * param,
          const char * argstr,
          size_t argstr_len)
{
  cputtext("Display: ");
  vBSP430cliConsoleDisplayChain(chain, argstr);
  if (0 == argstr_len) {
    cputs("\nWTH are we fighting for? Walk!");
  }
  cputchar('\n');
  return 0;
}

static int
cmd_hup_two (const char * argstr)
{
  cprintf("Give me a 'three'\n");
  return 0;
}

static const sBSP430cliCommand dcmd_hup_two_three_four = {
  .key = "four",
  .handler = cmd_h234
};
static const sBSP430cliCommand dcmd_hup_two_three = {
  .key = "three",
  .help = "# invalid if no four",
  .child = &dcmd_hup_two_three_four,
};
static const sBSP430cliCommand dcmd_hup_two = {
  .key = "two",
  .help = "# valid if no three",
  .child = &dcmd_hup_two_three,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_hup_two
};
static const sBSP430cliCommand dcmd_hup = {
  .key = "hup",
  .child = &dcmd_hup_two,
  .next = LAST_COMMAND,
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_hup

static int
cmd_uptime (const char * argstr)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  cprintf("Up %s\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()));
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
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
#define LAST_COMMAND &dcmd_uptime

static int
cmd_expand_ (sBSP430cliCommandLink * chain,
             void * param,
             const char * argstr,
             size_t argstr_len)
{
  cputtext("Expanded: ");
  vBSP430cliConsoleDisplayChain(chain, argstr);
  cputchar('\n');
  return 0;
}
static int
cmd_expand (const char * argstr)
{
  return iBSP430cliParseCommand(commandSet, NULL, argstr, NULL, cmd_expand_);
}
static const sBSP430cliCommand dcmd_expand = {
  .key = "expand",
  .help = "[command...] # Show the expansion of the command without executing it",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_expand
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_expand

static int
cmd_show (const char * argstr)
{
  dumpData();
  return 0;
}
static const sBSP430cliCommand dcmd_show = {
  .key = "show",
  .help = "# Display the value of variables",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_show
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_show

static int
cmd_set_all (const char * argstr)
{
  size_t argstr_len = strlen(argstr);
  int rv;

  rv = iBSP430cliStoreExtractedI(&argstr, &argstr_len, &data.ival);
  if (0 == rv) {
    rv = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &data.uival);
  }
  if (0 == rv) {
    rv = iBSP430cliStoreExtractedL(&argstr, &argstr_len, &data.lval);
  }
  if (0 == rv) {
    rv = iBSP430cliStoreExtractedUL(&argstr, &argstr_len, &data.ulval);
  }
  return rv;
}
static const sBSP430cliCommand dcmd_set_all = {
  .key = "all",
  .help = "[ival] [ [uival] [ [lval] [ [ulval] ] ] ]",
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_set_all
};
static const sBSP430cliCommand dcmd_set_ival = {
  .key = "ival",
  .help = "[signed 16-bit integer in octal, decimal, or hex]",
  .next = &dcmd_set_all,
  .handler = iBSP430cliHandlerStoreI,
  .param.ptr = &data.ival
};
static const sBSP430cliCommand dcmd_set_uival = {
  .key = "uival",
  .help = "[unsigned 16-bit integer in octal, decimal, or hex]",
  .next = &dcmd_set_ival,
  .handler = iBSP430cliHandlerStoreUI,
  .param.ptr = &data.uival
};
static const sBSP430cliCommand dcmd_set_lval = {
  .key = "lval",
  .help = "[signed 32-bit integer in octal, decimal, or hex]",
  .next = &dcmd_set_uival,
  .handler = iBSP430cliHandlerStoreL,
  .param.ptr = &data.lval
};
static const sBSP430cliCommand dcmd_set_ulval = {
  .key = "ulval",
  .help = "[unsigned 32-bit integer in octal, decimal, or hex]",
  .next = &dcmd_set_lval,
  .handler = iBSP430cliHandlerStoreUL,
  .param.ptr = &data.ulval
};
static const sBSP430cliCommand dcmd_set = {
  .key = "set",
  .child = &dcmd_set_ulval,
  .next = LAST_COMMAND,
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_set

static int
cmd_quote (const char * argstr)
{
  size_t arglen = strlen(argstr);
  size_t len;

  cprintf("Extracting text tokens from %u characters\n", arglen);
  while (0 < arglen) {
    const char * tp = xBSP430cliNextQToken(&argstr, &arglen, &len);
    cprintf("%u-char token <", len);
    while (len--) {
      cputchar(*tp++);
    }
    cprintf(">\n");
  }
  return 0;
}
static const sBSP430cliCommand dcmd_quote = {
  .key = "quote",
  .help = "[qstr]... # Extract one or more quoted tokens",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_quote
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_quote

static int
cmd_dummy (const char * argstr)
{
  cprintf("dummy %s\n", argstr);
  return 0;
}

#undef LAST_SUBCOMMAND
#define LAST_SUBCOMMAND NULL
static const sBSP430cliCommand dcmd_complete_common = {
  .key = "common",
  .next = LAST_SUBCOMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_dummy
};
#undef LAST_SUBCOMMAND
#define LAST_SUBCOMMAND &dcmd_complete_common
static const sBSP430cliCommand dcmd_complete_component = {
  .key = "component",
  .next = LAST_SUBCOMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param.simple_handler = cmd_dummy
};
#undef LAST_SUBCOMMAND
#define LAST_SUBCOMMAND &dcmd_complete_component
static const sBSP430cliCommand dcmd_complete = {
  .key = "complete",
  .next = LAST_COMMAND,
  .child = LAST_SUBCOMMAND
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_complete

static const char * const numbers[] = {
  "zero",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten"
};
static const size_t number_len = sizeof(numbers)/sizeof(*numbers);
#if configBSP430_CLI_COMMAND_COMPLETION_HELPER - 0
static const sBSP430cliCompletionHelperStrings completion_helper_say = {
  .completion_helper = { .helper = vBSP430cliCompletionHelperStrings },
  .strings = numbers,
  .len = sizeof(numbers)/sizeof(*numbers)
};
#endif /* configBSP430_CLI_COMMAND_COMPLETION_HELPER */

static int
cmd_say (sBSP430cliCommandLink * chain,
         void * param,
         const char * command,
         size_t command_len)
{
  const char * const * np = &command;
  int nmatches = 0;

  while (NULL != np) {
    size_t ocl = command_len;
    np = xBSP430cliHelperStringsExtract(&completion_helper_say, &command, &command_len);
    if (NULL != np) {
      ++nmatches;
      cprintf("Match %s for %u consumed position %u\n", *np, ocl - command_len, np - numbers);
    }
  }
  cprintf("%u matches found\n", nmatches);
  return 0;
}

static const sBSP430cliCommand dcmd_say = {
  .key = "say",
  .help = "{word}... # Customized completion demonstration",
#if configBSP430_CLI_COMMAND_COMPLETION_HELPER - 0
  .completion_helper = &completion_helper_say.completion_helper,
#endif /* configBSP430_CLI_COMMAND_COMPLETION_HELPER */
  .next = LAST_COMMAND,
  .handler = cmd_say,
};
#undef LAST_COMMAND
#define LAST_COMMAND (&dcmd_say)

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
#define LAST_COMMAND &dcmd_help

void main ()
{
  const char * command;
  int flags;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  vBSP430cliSetDiagnosticFunction(iBSP430cliConsoleDiagnostic);
  cprintf("\ncli example " __DATE__ " " __TIME__ "\n");
#if configBSP430_CLI_COMMAND_COMPLETION - 0
  cprintf("Command completion is available.\n");
#endif /* configBSP430_CLI_COMMAND_COMPLETION */
  vBSP430ledSet(0, 1);
  cprintf("\nLED lit when not awaiting input\n");

  /* NOTE: The control flow in this is a bit tricky, as we're trying
   * to leave interrupts enabled during the main body of the loop,
   * while they must be disabled when processing input to recognize a
   * command.  The flags variable preserves state across multiple loop
   * iterations until all relevant activities have completed. */
  commandSet = LAST_COMMAND;
  command = NULL;
  flags = eBSP430cliConsole_REPAINT;

  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    if (flags & eBSP430cliConsole_ANY_ESCAPE) {
      int c;
      while (0 <= ((c = cgetchar()))) {
        cprintf("escape char 0x%02x (%u) '%c'\n", c, c, isprint(c) ? c : '.');
        /* Technically CSI is a single character 0x9b representing
         * ESC+[.  In the two-character mode accepted here, we use the
         * value for the second character. */
#define KEY_CSI '['
        if ((KEY_CSI == c) && (flags & eBSP430cliConsole_PROCESS_ESCAPE)) {
          flags &= ~eBSP430cliConsole_PROCESS_ESCAPE;
          flags |= eBSP430cliConsole_IN_ESCAPE;
        } else if ((64 <= c) && (c <= 126)) {
          flags &= ~eBSP430cliConsole_ANY_ESCAPE;
          cprintf("Leaving escape mode\n");
          break;
        }
      }
    }
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
    do {
      if (flags & eBSP430cliConsole_READY) {
        /* Clear the command we just completed */
        vBSP430cliConsoleBufferClear_ni();
        flags &= ~eBSP430cliConsole_READY;
      }
      do {
        /* Unless we're processing application-specific escape
         * characters let iBSP430cliConsoleBufferProcessInput_ni()
         * process any input characters that have already been
         * received. */
        if (! (flags & eBSP430cliConsole_ANY_ESCAPE)) {
          flags |= iBSP430cliConsoleBufferProcessInput_ni();
        }
        if (0 == flags) {
          /* Sleep until something wakes us, such as console input.
           * Then turn off interrupts and loop back to read that
           * input. */
          vBSP430ledSet(0, 0);
          BSP430_CORE_LPM_ENTER_NI(LPM0_bits);
          BSP430_CORE_DISABLE_INTERRUPT();
          vBSP430ledSet(0, 1);
        }
        /* Repeat if still nothing to do */
      } while (! flags);

      /* Got something to do; get the command contents in place so
       * we can update the screen. */
      command = xBSP430cliConsoleBuffer_ni();
    } while (0);
    BSP430_CORE_ENABLE_INTERRUPT();
  }
}
