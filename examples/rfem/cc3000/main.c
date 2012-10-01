/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/periph/port.h>
#include <bsp430/periph/sys.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>
#include <bsp430/utility/cli.h>
#include <bsp430/utility/cc3000spi.h>
#include <cc3000/wlan.h>
#include <string.h>
#include <ctype.h>

#if 0
typedef struct sWlanSecMap {
  unsigned int val;
  const char * tag;
} sWlanSecMap;
static const sWlanSecMap wlanSecMap[] = {
  { WLAN_SEC_UNSEC, "unsec" },
  { WLAN_SEC_WEP, "wep" },
  { WLAN_SEC_WPA, "wpa" },
  { WLAN_SEC_WPA2, "wpa2" },
};
static const sWlanSecMap * wlanSecMapFromValue (unsigned int val)
{
  const sWlanSecMap * mp;
  for (mp = wlanSecMap; mp < (wlanSecMap + sizeof(wlanSecMap)/sizeof(*wlanSecMap)); ++mp) {
    if (mp->val == val) {
      return mp;
    }
  }
  return NULL;
}
static const sWlanSecMap * wlanSecMapFromTag (const char * tag)
{
  const sWlanSecMap * mp;
  for (mp = wlanSecMap; mp < (wlanSecMap + sizeof(wlanSecMap)/sizeof(*wlanSecMap)); ++mp) {
    if (0 == strcmp(mp->tag, tag)) {
      return mp;
    }
  }
  return NULL;
}
#endif

typedef struct sConnectParams {
  int security_type;
  char ssid[33];
  char key[65];
} sConnectParams;

static void wlan_cb (long event_type,
                     char * data,
                     unsigned char length)
{
  cprintf("wlan_cb %#lx %u at %p SR %#x\n", event_type, length, data, __read_status_register());
}

const sBSP430cliCommand * commandSet;
#define LAST_COMMAND NULL

#define LAST_SUB_COMMAND NULL

static int
cmd_wlan_stop (const char * argstr)
{
  wlan_stop();
  return 0;
}
static sBSP430cliCommand dcmd_wlan_stop = {
  .key = "stop",
  .help = "# Stop the CC3000 WLAN",
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_stop
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_stop

static int
cmd_wlan_status (const char * argstr)
{
  /* Contrary to the documentation, there are no macro constants
   * defined for these states */
  static const char * status_str[] = {
    "disconnected",
    "scanning",
    "connecting",
    "connected"
  };
  long rl = wlan_ioctl_statusget();
  cprintf("WLAN status %ld", rl);
  if ((0 <= rl) && (rl < (sizeof(status_str)/sizeof(*status_str)))) {
    cprintf(": %s", status_str[rl]);
  }
  cputchar_ni('\n');
  return 0;
}
static sBSP430cliCommand dcmd_wlan_status = {
  .key = "status",
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_status
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_status

static int
cmd_wlan_start (const char * argstr)
{
  unsigned long t[2];
  t[0] = ulBSP430uptime_ni();
  wlan_start(0);
  t[1] = ulBSP430uptime_ni();
  cprintf("wlan_start() took %s\n", xBSP430uptimeAsText_ni(t[1]-t[0]));
  return 0;
}
static sBSP430cliCommand dcmd_wlan_start = {
  .key = "start",
  .help = "# Start the CC3000 WLAN",
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_start
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_start

static sBSP430cliCommand dcmd_wlan = {
  .key = "wlan",
  .next = LAST_COMMAND,
  .child = LAST_SUB_COMMAND
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_wlan

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
char command[40];

#define FLG_NEED_PROMPT 0x01
#define FLG_HAVE_COMMAND 0x02

void main (void)
{
  int rv;
  unsigned int flags = 0;
  char * cp;
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

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
  cprintf("\n\nWLAN test program\n");

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
  if (reset_flags) {
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
  } else {
    cputtext_ni(" no data");
  }
  cputchar_ni('\n');
#endif /* BSP430_MODULE_SYS */

  cprintf("PWR_EN at %s.%u\n", xBSP430portName(BSP430_RFEM_PWR_EN_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RFEM_PWR_EN_PORT_BIT));
  cprintf("SPI_IRQ HAL at %s.%u\n", xBSP430portName(BSP430_RFEM_SPI_IRQ_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RFEM_SPI_IRQ_PORT_BIT));
  cprintf("CSn HAL at %s.%u\n", xBSP430portName(BSP430_RFEM_SPI0CSn_PORT_PERIPH_HANDLE), iBSP430portBitPosition(BSP430_RFEM_SPI0CSn_PORT_BIT));
  cprintf("SPI is %s\n", xBSP430serialName(BSP430_RFEM_SPI0_PERIPH_HANDLE));
#if BSP430_PLATFORM_PERIPHERAL_HELP
  cprintf("SPI Pins: %s\n", xBSP430platformPeripheralHelp(BSP430_RFEM_SPI0_PERIPH_HANDLE, BSP430_PERIPHCFG_SERIAL_SPI3));
#endif /* BSP430_PLATFORM_PERIPHERAL_HELP */
  cprintf(__DATE__ " " __TIME__ "\n");

  rv = iBSP430cc3000spiInitialize(wlan_cb, NULL, NULL, NULL);
  cprintf("%s Initialize returned %d\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), rv);

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
#if 0
  cprintf("%s wlan_start(0)\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()));
  wlan_start(0);
  cprintf("%s wlan_stop()\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()));
  wlan_stop();
  cprintf("%s Leaving program\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()));
#endif
}
