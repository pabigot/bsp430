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

const sBSP430cliCommand * commandSet;
#define LAST_COMMAND NULL

static int
cmd_h234 (sBSP430cliCommandLink * chain,
          void * param,
          const char * argstr,
          size_t argstr_len)
{
  cputtext_ni("Display: ");
  vBSP430cliConsoleDisplayChain(chain, argstr);
  if (0 == argstr_len) {
    cputs("\nWTH are we fighting for? Walk!");
  }
  cputchar_ni('\n');
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
  .param = cmd_hup_two
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
static sBSP430cliCommand dcmd_uptime = {
  .key = "uptime",
  .help = "# Show system uptime",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_uptime
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_uptime

static int
cmd_expand_ (sBSP430cliCommandLink * chain,
             void * param,
             const char * argstr,
             size_t argstr_len)
{
  cputtext_ni("Expanded: ");
  vBSP430cliConsoleDisplayChain(chain, argstr);
  cputchar_ni('\n');
  return 0;
}

static int
cmd_expand (const char * argstr)
{
  return iBSP430cliParseCommand(commandSet, NULL, argstr, cmd_expand_);
}
static sBSP430cliCommand dcmd_expand = {
  .key = "expand",
  .help = "[command...] # Show the expansion of the command without executing it",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_expand
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_expand

static int
cmd_show (const char * argstr)
{
  dumpData();
  return 0;
}
static sBSP430cliCommand dcmd_show = {
  .key = "show",
  .help = "# Display the value of variables",
  .next = LAST_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_show
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
  .param = cmd_set_all
};
static const sBSP430cliCommand dcmd_set_ival = {
  .key = "ival",
  .help = "[signed 16-bit integer in octal, decimal, or hex]",
  .next = &dcmd_set_all,
  .handler = iBSP430cliHandlerStoreI,
  .param = &data.ival
};
static const sBSP430cliCommand dcmd_set_uival = {
  .key = "uival",
  .help = "[unsigned 16-bit integer in octal, decimal, or hex]",
  .next = &dcmd_set_ival,
  .handler = iBSP430cliHandlerStoreUI,
  .param = &data.uival
};
static const sBSP430cliCommand dcmd_set_lval = {
  .key = "lval",
  .help = "[signed 32-bit integer in octal, decimal, or hex]",
  .next = &dcmd_set_uival,
  .handler = iBSP430cliHandlerStoreL,
  .param = &data.lval
};
static const sBSP430cliCommand dcmd_set_ulval = {
  .key = "ulval",
  .help = "[unsigned 32-bit integer in octal, decimal, or hex]",
  .next = &dcmd_set_lval,
  .handler = iBSP430cliHandlerStoreUL,
  .param = &data.ulval
};
static const sBSP430cliCommand dcmd_set = {
  .key = "set",
  .child = &dcmd_set_ulval,
  .next = LAST_COMMAND,
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_set

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
#define LAST_COMMAND &dcmd_help

#define KEY_BS '\b'
#define KEY_LF '\n'
#define KEY_CR '\r'
#define KEY_BEL '\a'
#define KEY_ESC '\e'
#define KEY_FF '\f'
#define KEY_TAB '\t'
#define KEY_KILL_LINE 0x15
#define KEY_KILL_WORD 0x17
char command[30];

#define FLG_NEED_PROMPT 0x01
#define FLG_HAVE_COMMAND 0x02

void main ()
{
  unsigned int flags = 0;
  char * cp;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  vBSP430cliSetDiagnosticFunction(iBSP430cliConsoleDiagnostic);
  cprintf("\n\n\nAnd we're up and running.\n");
  flags = FLG_NEED_PROMPT;
  memset(command, 0, sizeof(command));
  cp = command;

  commandSet = LAST_COMMAND;
  while (1) {
    int c;

    if (flags & FLG_NEED_PROMPT) {
      *cp = 0;
      cprintf("> %s", command);
      flags &= ~FLG_NEED_PROMPT;
    }
    while (0 <= ((c = cgetchar_ni()))) {
      if (KEY_BS == c) {
        if (cp == command) {
          cputchar_ni(KEY_BEL);
        } else {
          --cp;
          cputtext_ni("\b \b");
        }
      } else if (KEY_CR == c) {
        flags |= FLG_HAVE_COMMAND;
        break;
      } else if (KEY_KILL_LINE == c) {
        cprintf("\e[%uD\e[K", cp - command);
        cp = command;
        *cp = 0;
      } else if (KEY_KILL_WORD == c) {
        char * kp = cp;
        while (--kp > command && isspace(*kp)) {
        }
        while (--kp > command && !isspace(*kp)) {
        }
        ++kp;
        cprintf("\e[%uD\e[K", cp-kp);
        cp = kp;
        *cp = 0;
      } else if (KEY_FF == c) {
        cputchar_ni(c);
        flags |= FLG_NEED_PROMPT;
      } else {
        if ((1+cp) >= (command + sizeof(command))) {
          cputchar_ni(KEY_BEL);
        } else {
          *cp++ = c;
          cputchar_ni(c);
        }
      }
    }
    if (flags & FLG_HAVE_COMMAND) {
      int rv;
      
      cputchar_ni('\n');
      *cp = 0;
      rv = iBSP430cliExecuteCommand(&dcmd_help, 0, command);
      if (0 != rv) {
        cprintf("Command execution returned %d\n", rv);
      }
      cp = command;
      *cp = 0;
      flags &= ~FLG_HAVE_COMMAND;
      flags |= FLG_NEED_PROMPT;
    }
    if (flags) {
      continue;
    }
    BSP430_CORE_LPM_ENTER_NI(LPM2_bits | GIE);
  }
}
