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
#include <cc3000/nvmem.h>
#include <string.h>
#include <ctype.h>

/* Memory is extremely tight on the FR5739.  Set these to restrict the
 * set of commands to ones of interest that will fit. */
#define CMD_WLAN 1
#define CMD_WLAN_STOP 1
#define CMD_WLAN_STATUS 1
#define CMD_WLAN_CONNECT 1
#define CMD_WLAN_START 1
#define CMD_NVMEM 1
#define CMD_NVMEM_SP 1
#define CMD_NVMEM_READ 1
#define CMD_NVMEM_MAC 1
#define CMD_HELP 1

#if NO_HELP - 0
#define HELP_STRING(h_) NULL
#else /* NO_HELP */
#define HELP_STRING(h_) h_
#endif /* NO_HELP */

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

#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND NULL

#if (CMD_WLAN - 0) && (CMD_WLAN_STOP - 0)
static int
cmd_wlan_stop (const char * argstr)
{
  wlan_stop();
  return 0;
}
static sBSP430cliCommand dcmd_wlan_stop = {
  .key = "stop",
  .help = HELP_STRING("# shut down CC3000"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_stop
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_stop
#endif /* CMD_WLAN_STOP */

#if (CMD_WLAN - 0) && (CMD_WLAN_STATUS - 0)
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
  .help = HELP_STRING("# get CC3000 status"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_status
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_status
#endif /* CMD_WLAN_STATUS */

#if (CMD_WLAN - 0) && (CMD_WLAN_CONNECT - 0)
static int
cmd_wlan_connect (const char * argstr)
{
  long rl;
  char ssid[33];
  int ssid_len = 0;
  unsigned char key[33];
  int key_len = 0;
  int security_type = WLAN_SEC_WPA;
  const char * tp;
  size_t remaining;
  size_t len;
  unsigned long t[2];

  remaining = strlen(argstr);
  tp = xBSP430cliNextQToken(&argstr, &remaining, &len);
  if (*tp) {
    memcpy(ssid, tp, len);
    ssid_len = len;
  }
  ssid[ssid_len] = 0;

  tp = xBSP430cliNextQToken(&argstr, &remaining, &len);
  if (*tp) {
    memcpy(key, tp, len);
    key_len = len;
  }
  key[key_len] = 0;
  
  cprintf("connect ssid '%s' passphrase '%s'\n", ssid, key);
  t[0] = ulBSP430uptime_ni();
  rl = wlan_connect(security_type, ssid, ssid_len, NULL, key, key_len);
  t[1] = ulBSP430uptime_ni();
  cprintf("con %ld in %s\n", rl, xBSP430uptimeAsText_ni(t[1]-t[0]));
  return 0;
}
static sBSP430cliCommand dcmd_wlan_connect = {
  .key = "connect",
  .help = HELP_STRING("ssid passphrase [(unsec|wep|*wpa|wpa2)] # connect to specified access point"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_connect
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_connect
#endif /* CMD_WLAN_CONNECT */

#if (CMD_WLAN - 0) && (CMD_WLAN_START - 0)
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
  .help = HELP_STRING("# power-up CC3000"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_start
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_start
#endif /* CMD_WLAN_START */

#if (CMD_WLAN - 0)
static sBSP430cliCommand dcmd_wlan = {
  .key = "wlan",
  .next = LAST_COMMAND,
  .child = LAST_SUB_COMMAND
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_wlan
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND NULL
#endif /* CMD_WLAN */

#if (CMD_NVMEM - 0) && (CMD_NVMEM_SP - 0)
static int
cmd_nvmem_sp (const char * argstr)
{
  unsigned char patch_version[2];
  unsigned char rv;

  memset(patch_version, -1, sizeof(patch_version));
  rv = nvmem_read_sp_version(patch_version);
  cprintf("sp ret %u: %u.%u\n", rv, patch_version[0], patch_version[1]);
  return 0;
}
static sBSP430cliCommand dcmd_nvmem_sp = {
  .key = "sp",
  .help = HELP_STRING("# read firmware server patchlevel"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_nvmem_sp
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_nvmem_sp
#endif /* CMD_NVMEM_SP */

#if (CMD_NVMEM - 0) && (CMD_NVMEM_READ - 0)
static int
cmd_nvmem_read (const char * argstr)
{
  int rc;
  unsigned int fileid = 0;
  unsigned int len = 128;
  unsigned int ofs = 0;
  unsigned char data[32];
  char asciiz[17];
  char * ap;
  unsigned int end_read;
  unsigned int ui;
  size_t argstr_len = strlen(argstr);
  unsigned int nb;
  
  rc = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &ui);
  if (0 == rc) {
    fileid = ui;
    rc = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &ui);
  }
  if (0 == rc) {
    len = ui;
    rc = iBSP430cliStoreExtractedUI(&argstr, &argstr_len, &ui);
  }
  if (0 == rc) {
    ofs = ui;
  }
  end_read = ofs + len;
  memset(asciiz, 0, sizeof(asciiz));
  ap = asciiz;
  rc = 0;
  while ((0 == rc) && (ofs < end_read)) {
    unsigned char * dp;
    unsigned char * dpe;
    nb = (end_read - ofs);
    if (sizeof(data) < nb) {
      nb = sizeof(data);
    }
    rc = nvmem_read(fileid, nb, ofs, data);
    if (0 == rc) {
      dp = data;
      dpe = dp + nb;
      while (dp < dpe) {
        if (0 == (ofs % 16)) {
          cprintf("    %s\n%x.%04x ", asciiz, fileid, ofs);
          memset(asciiz, 0, sizeof(asciiz));
          ap = asciiz;
        } else if (0 == (ofs % 8)) {
          cprintf(" ");
        }
        ++ofs;
        *ap++ = isprint(*dp) ? *dp : '.';
        cprintf(" %02x", *dp++);
      }
      len -= nb;
    }
  }
  if (0 == rc) {
    cprintf("    %s\n", asciiz);
  } else {
    cprintf("\nERR: Read returned %u for %u bytes at %u of fileid %u\n",
            rc, nb, ofs, fileid);
  }
  return 0;
}
static sBSP430cliCommand dcmd_nvmem_read = {
  .key = "read",
  .help = HELP_STRING("fileid [len(=128) [ofs(=0)]] # read block from nvmem file"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_nvmem_read
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_nvmem_read
#endif /* CMD_NVMEM_READ */

#if (CMD_NVMEM - 0) && (CMD_NVMEM_MAC - 0)
static int
cmd_nvmem_mac (const char * argstr)
{
  int rc;
  unsigned char mac[6];

  /* Could extend this to parse "set {addr}" if you wanted. */
  rc = nvmem_get_mac_address(mac);
  if (0 == rc) {
    cprintf("nvmem mac is %02x.%02x.%02x.%02x.%02x.%02x\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  } else {
    cprintf("ERR: nvmem mac read got %u\n", rc);
  }
  return 0;
}
static sBSP430cliCommand dcmd_nvmem_mac = {
  .key = "mac",
  .help = HELP_STRING("# get mac address"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_nvmem_mac
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_nvmem_mac
#endif /* CMD_NVMEM_MAC */  

#if (CMD_NVMEM - 0)
static sBSP430cliCommand dcmd_nvmem = {
  .key = "nvmem",
  .next = LAST_COMMAND,
  .child = LAST_SUB_COMMAND
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_nvmem
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND NULL
#endif /* CMD_NVMEM */

#if (CMD_HELP - 0)
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
  .next = LAST_COMMAND,
  .handler = cmd_help
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_help
#endif /* CMD_HELP */

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

#if (BSP430_MODULE_PMM - 0) && ! (BSP430_MODULE_PMM_FRAM - 0)
  rv = iBSP430pmmSetCoreVoltageLevel_ni(PMMCOREV_3);
  cprintf("Vcore at %d\n", rv);
#endif

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
      rv = iBSP430cliExecuteCommand(commandSet, 0, command);
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
