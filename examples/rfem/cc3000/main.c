/** This file is in the public domain.
 *
 * This program is an example; see detailed discussion within the
 * BSP430 documentation.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/periph/port.h>
#include <bsp430/periph/sys.h>
#include <bsp430/periph/flash.h>
#include <sys/crtld.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>
#include <bsp430/utility/cli.h>
#include "cc3000spi.h"
#include <cc3000/wlan.h>
#include <cc3000/nvmem.h>
#include <cc3000/netapp.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Memory is extremely tight on the FR5739 (16 kB including CC3000
 * buffers).  Set these to restrict the set of commands to ones of
 * interest that will fit.  You'll probably find you'll need to
 * disable WLAN_CONNECT, which makes this pretty useless.  On the
 * EXP430F5438 the total application size is just over 20 kB. */
#define CMD_WLAN 1
#define CMD_WLAN_STOP 1
#define CMD_WLAN_STATUS 1
#define CMD_WLAN_CONNECT 1
#define CMD_WLAN_IPCONFIG 1
#define CMD_WLAN_DISCONNECT 1
#define CMD_WLAN_START 1
#define CMD_NVMEM 1
#define CMD_NVMEM_SP 1
#define CMD_NVMEM_READ 1
#define CMD_NVMEM_MAC 1
#define CMD_INFO 1
#define CMD_HELP 1

#if NO_HELP - 0
#define HELP_STRING(h_) NULL
#else /* NO_HELP */
#define HELP_STRING(h_) h_
#endif /* NO_HELP */

#define MAGIC_CONNECT_PARAMS 0x3000

typedef struct sConnectParams {
  unsigned int magic;
  int security_type;
  size_t ssid_len;
  size_t passphrase_len;
  char ssid[33];
  unsigned char passphrase[65];
} sConnectParams;

static void wlan_cb (long event_type,
                     char * data,
                     unsigned char length)
{
  cprintf("%s wlan_cb %#lx %u at %p SR %#x\n",
          xBSP430uptimeAsText_ni(ulBSP430uptime_ni()),
          event_type, length, data, __read_status_register());
}

const sBSP430cliCommand * commandSet;
#define LAST_COMMAND NULL

#if CMD_WLAN - 0
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND NULL

#if CMD_WLAN_STOP - 0
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

#if CMD_WLAN_DISCONNECT - 0
static int
cmd_wlan_disconnect (const char * argstr)
{
  long rl = wlan_disconnect();
  cprintf("disconnect returned %ld\n", rl);
  return 0;
}
static sBSP430cliCommand dcmd_wlan_disconnect = {
  .key = "disconnect",
  .help = HELP_STRING("# disconnect from AP"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_disconnect
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_disconnect
#endif /* CMD_WLAN_DISCONNECT */

#if CMD_WLAN_IPCONFIG - 0
const char *
ipv4AsText (const unsigned char * ipaddr)
{
  static char buffer[16];
  snprintf(buffer, sizeof(buffer), "%u.%u.%u.%u", ipaddr[3], ipaddr[2], ipaddr[1], ipaddr[0]);
  return buffer;
}

static int
cmd_wlan_ipconfig (const char * argstr)
{
  tNetappIpconfigRetArgs ipc;
  const unsigned char * p;

  memset(&ipc, 0, sizeof(ipc));
  netapp_ipconfig(&ipc);
  cprintf("IP: %s\n", ipv4AsText(ipc.aucIP));
  cprintf("NM: %s\n", ipv4AsText(ipc.aucSubnetMask));
  cprintf("GW: %s\n", ipv4AsText(ipc.aucDefaultGateway));
  cprintf("DHCP: %s\n", ipv4AsText(ipc.aucDHCPServer));
  cprintf("DNS: %s\n", ipv4AsText(ipc.aucDNSServer));
  p = ipc.uaMacAddr;
  /* WTF?  A little-endian MAC address?  This may be because of a bug
   * fix in the host driver; the original version was completely
   * wrong. */
  cprintf("MAC: %02x.%02x.%02x.%02x.%02x.%02x\n",
          p[5], p[4], p[3], p[2], p[1], p[0]);
  cprintf("SSID: %s\n", ipc.uaSSID);
  return 0;
}
static sBSP430cliCommand dcmd_wlan_ipconfig = {
  .key = "ipconfig",
  .help = HELP_STRING("# show IP parameters"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_ipconfig
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_ipconfig
#endif /* CMD_WLAN_IPCONFIG */

#if CMD_WLAN_CONNECT - 0
typedef struct sWlanSecMap {
  unsigned int val;
  const char * tag;
} sWlanSecMap;
static const sWlanSecMap wlanSecMap[] = {
  { WLAN_SEC_UNSEC, "unsec" },
  { WLAN_SEC_WEP, "wep" },
  { WLAN_SEC_WPA, "wpa" },
#if 0
  /* WPA2 is not implemented.  Unit will transmit unsecured auth request */
  { WLAN_SEC_WPA2, "wpa2" },
#endif /* 0 */
};
static const sWlanSecMap * const wlanSecMap_end = wlanSecMap + sizeof(wlanSecMap)/sizeof(*wlanSecMap);
static const sWlanSecMap * wlanSecMapFromValue (unsigned int val)
{
  const sWlanSecMap * mp;
  for (mp = wlanSecMap; mp < wlanSecMap_end; ++mp) {
    if (mp->val == val) {
      return mp;
    }
  }
  return NULL;
}
static const sWlanSecMap * wlanSecMapFromTag (const char * tag)
{
  const sWlanSecMap * mp;
  for (mp = wlanSecMap; mp < wlanSecMap_end; ++mp) {
    if (0 == strcmp(mp->tag, tag)) {
      return mp;
    }
  }
  return NULL;
}

sConnectParams connectParams;

static int
cmd_wlan_sectype (const char * argstr)
{
  size_t remaining = strlen(argstr);
  size_t len;
  const char * tp;
  const sWlanSecMap * mp;

  tp = xBSP430cliNextQToken(&argstr, &remaining, &len);
  if (0 < len) {
    mp = wlanSecMapFromTag(tp);
    if (0 != mp) {
      connectParams.security_type = mp->val;
    } else {
      cprintf("ERR: Unrecognized sectype '%s': valid are", tp);
      for (mp = wlanSecMap; mp < wlanSecMap_end; ++mp) {
        cprintf(" %s", mp->tag);
      }
      cprintf("\n");
    }
  }
  mp = wlanSecMapFromValue(connectParams.security_type);
  cprintf("AP security type is %s (%u)\n", (mp ? mp->tag : "<UNDEF>"), connectParams.security_type);
  return 0;
}

static int
cmd_wlan_ssid (const char * argstr)
{
  size_t remaining = strlen(argstr);
  size_t len;
  const char * tp;

  tp = xBSP430cliNextQToken(&argstr, &remaining, &len);
  if (0 < len) {
    memcpy(connectParams.ssid, tp, len);
    connectParams.ssid[len] = 0;
    connectParams.ssid_len = len;
  }
  cprintf("AP SSID is %s (%u chars)\n", connectParams.ssid, connectParams.ssid_len);
  return 0;
}

static int
cmd_wlan_passphrase (const char * argstr)
{
  size_t remaining = strlen(argstr);
  size_t len;
  const char * tp;

  tp = xBSP430cliNextQToken(&argstr, &remaining, &len);
  if (0 < len) {
    if (sizeof(connectParams.passphrase) <= (len+1)) {
      cprintf("ERR: Passphrase too long (> %u chars + EOS)\n", sizeof(connectParams.passphrase));
    } else {
      memcpy(connectParams.passphrase, tp, len);
      connectParams.passphrase[len] = 0;
      connectParams.passphrase_len = len;
    }
  }
  cprintf("AP passphrase is '%s' (%u chars)\n", connectParams.passphrase, connectParams.passphrase_len);
  return 0;
}

static int
cmd_wlan_connect (const char * argstr)
{
  long rl;
  const sWlanSecMap * mp;
  unsigned long t[2];

  mp = wlanSecMapFromValue(connectParams.security_type);
  cprintf("connect to ssid '%s' by %s using '%s'\n",
          connectParams.ssid, (mp ? mp->tag : "<UNDEF>"),
          connectParams.passphrase);
  t[0] = ulBSP430uptime_ni();
  rl = wlan_connect(connectParams.security_type,
                    connectParams.ssid, connectParams.ssid_len,
                    NULL,
                    connectParams.passphrase, connectParams.passphrase_len);
  t[1] = ulBSP430uptime_ni();
  cprintf("con %ld in %s\n", rl, xBSP430uptimeAsText_ni(t[1]-t[0]));
  return 0;
}
static sBSP430cliCommand dcmd_wlan_ssid = {
  .key = "ssid",
  .help = HELP_STRING("[{ssid}] # Display or set AP SSID for connect"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_ssid
};
static sBSP430cliCommand dcmd_wlan_passphrase = {
  .key = "passphrase",
  .help = HELP_STRING("[{passphrase}] # Display or set AP passphrase"),
  .next = &dcmd_wlan_ssid,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_passphrase
};
static sBSP430cliCommand dcmd_wlan_sectype = {
  .key = "sectype",
  .help = HELP_STRING("[{sectype}] # Display or set AP sectype"),
  .next = &dcmd_wlan_passphrase,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_sectype
};
static sBSP430cliCommand dcmd_wlan_connect = {
  .key = "connect",
  .help = HELP_STRING("# connect to specified access point using AP config"),
  .next = &dcmd_wlan_sectype,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_connect
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_connect
#endif /* CMD_WLAN_CONNECT */

#if CMD_WLAN_STATUS - 0
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
#if CMD_WLAN_CONNECT - 0
  {
    const sWlanSecMap * mp;
    mp = wlanSecMapFromValue(connectParams.security_type);

    cprintf("AP configuration SSID '%s', passphrase '%s'\n",
            connectParams.ssid, connectParams.passphrase);
    cprintf("AP security type %s (%u)\n", (mp ? mp->tag : "<UNDEF>"), connectParams.security_type);
  }
#endif /* CMD_WLAN_CONNECT */
  cprintf("WLAN status %ld", rl);
  if ((0 <= rl) && (rl < (sizeof(status_str)/sizeof(*status_str)))) {
    cprintf(": %s", status_str[rl]);
  }
  cputchar('\n');
  return 0;
}
static sBSP430cliCommand dcmd_wlan_status = {
  .key = "status",
  .help = HELP_STRING("# get CC3000 status and AP config"),
  .next = LAST_SUB_COMMAND,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_wlan_status
};
#undef LAST_SUB_COMMAND
#define LAST_SUB_COMMAND &dcmd_wlan_status
#endif /* CMD_WLAN_STATUS */

#if CMD_WLAN_START - 0
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
  .help = HELP_STRING("# read firmware service pack level"),
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

#if ! (BSP430_MODULE_FLASH - 0)
/* No support for this on FRAM boards yet */
#undef CMD_INFO
#endif

#if CMD_INFO - 0

static sConnectParams * const infoConnectParams = (sConnectParams *)__infob;

static int
cmd_info_load (const char * argstr)
{
  if (MAGIC_CONNECT_PARAMS != infoConnectParams->magic) {
    cprintf("ERR: Stored connection params invalid\n");
    return -1;
  }
#if (CMD_WLAN_CONNECT - 0)
  connectParams = *infoConnectParams;
#endif /* CMD_WLAN_CONNECT */
  return 0;
}

static int
cmd_info_erase_ni (void)
{
  int rv;

  cprintf("Erasing stored params at %p...", infoConnectParams);
  rv = iBSP430flashEraseSegment_ni(infoConnectParams);
  cprintf("%d\n", rv);
  return rv;
}

static int
cmd_info_erase (const char * argstr)
{
  int rv;
  BSP430_CORE_INTERRUPT_STATE_T istate;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = cmd_info_erase_ni();
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}

static int
cmd_info_store (const char * argstr)
{
  int rv;

  BSP430_CORE_INTERRUPT_STATE_T istate;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    rv = cmd_info_erase_ni();
    if (0 == rv) {
      connectParams.magic = MAGIC_CONNECT_PARAMS;
      cprintf("Copying current configuration...");
      rv = iBSP430flashWriteData_ni(infoConnectParams, &connectParams, sizeof(connectParams));
      cprintf("%d\n", rv);
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rv;
}
static sBSP430cliCommand dcmd_info_store = {
  .key = "store",
  .help = HELP_STRING("# store wlan AP params in INFO_B"),
  .next = NULL,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_info_store
};
static sBSP430cliCommand dcmd_info_erase = {
  .key = "erase",
  .help = HELP_STRING("# erase INFO_B"),
  .next = &dcmd_info_store,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_info_erase
};
static sBSP430cliCommand dcmd_info_load = {
  .key = "load",
  .help = HELP_STRING("# load wlan AP params from INFO_B"),
  .next = &dcmd_info_erase,
  .handler = iBSP430cliHandlerSimple,
  .param = cmd_info_load
};
static sBSP430cliCommand dcmd_info = {
  .key = "info",
  .next = LAST_COMMAND,
  .child = &dcmd_info_load
};
#undef LAST_COMMAND
#define LAST_COMMAND &dcmd_info

#endif /* CMD_INFO */

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

void main (void)
{
  int rv;
  const char * command;
  int flags;
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
  vBSP430cliSetDiagnosticFunction(iBSP430cliConsoleDiagnostic);
  BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 2);
  cprintf("\ncc3000 " __DATE__ " " __TIME__ "\n");

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

  /* Initialization can be done with interrupts disabled, since the
   * function does nothing but store callbacks. */
  rv = iBSP430cc3000spiInitialize(wlan_cb, NULL, NULL, NULL);
  cprintf("%s Initialize returned %d\n", xBSP430uptimeAsText_ni(ulBSP430uptime_ni()), rv);

  commandSet = LAST_COMMAND;
  command = NULL;
  flags = eBSP430cliConsole_REPAINT;

  BSP430_CORE_ENABLE_INTERRUPT();

  /* wlan_start() MUST be invoked with interrupts enabled. */
  wlan_start(0);

  cprintf("\nAnd wlan has been started.\n");

#if CMD_INFO - 0
  if (0 == cmd_info_load("")) {
    cmd_wlan_status("");
  }
#endif

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
      /* Draw the prompt along with whatever's left in the command buffer */
      cprintf("> %s", command ? command : "");
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
    flags = iBSP430cliConsoleBufferProcessInput_ni();
    if (flags) {
      /* Got something to do; get the command contents in place */
      command = xBSP430cliConsoleBuffer_ni();
      BSP430_CORE_ENABLE_INTERRUPT();
      continue;
    }
    BSP430_CORE_LPM_ENTER_NI(LPM2_bits);
  }
}
