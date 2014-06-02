/** This file is in the public domain.
 *
 * This example demonstrates use of the BSP430 infrastructure for
 * CC3000 to obtain and monitor a relationship between the uptime
 * clock and civil time as provided through NTP.
 *
 * Expected output something like:
0:00.001: WDISBdssssss!dsrE[209.114.111.1:123 adj 28467361533480014 ntp = 2147483647 ms, rtt 61533 us][ 0:04.329]
1:00.002: WDISBsr[209.114.111.1:123 adj 9120934 ntp = 2 ms, rtt 69590 us][ 0:01.711]
2:00.002: WDISBsr[209.114.111.1:123 adj 6020358 ntp = 1 ms, rtt 66629 us][ 0:01.715]
 *
 * This shows an initial setting, then once-per-minute retrieving the
 * updates, which takes about 1.7s each time, with about 70ms RTT to
 * the selected server.
 *
 * Be aware that it can take several attempts to locate an NTP server
 * that is responsive.  Finding one on the second attempt (as shown
 * above) is fairly unusual.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/periph/port.h>
#include <bsp430/periph/sys.h>
#include <bsp430/periph/flash.h>
#include <bsp430/periph/pmm.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/led.h>
#include <bsp430/rf/cc3000.h>
#include <cc3000/host_driver_version.h>
#include <cc3000/wlan.h>
#include <cc3000/nvmem.h>
#include <cc3000/netapp.h>
#include <cc3000/hci.h>
#include <cc3000/security.h>
#include <cc3000/socket.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <sys/time.h>
#include <time.h>

#if (CC3000_REPACKAGED_VERSION - 0) < 20140318
/* Original 1.12 TI release does not allow use of <sys/time.h> */
#error This program requires the CC3000 API repackaged version 20140318 or later
#endif /* CC3000_REPACKAGED_VERSION */

#ifndef NTP_SERVERS_PER_ATTEMPT
/** The code will try this many servers in an attempt to find one that
 * is responsive.  A responsive server is kept for future attempts,
 * until it becomes unresponsive. */
#define NTP_SERVERS_PER_ATTEMPT 10
#endif /* NTP_SERVERS_PER_ATTEMPT */

#ifndef NTP_REQUESTS_PER_SERVER
/** The code will send this many requests to a server before
 * determining that it is unresponsive and selecting a new server. */
#define NTP_REQUESTS_PER_SERVER 5
#endif /* NTP_REQUESTS_PER_SERVER */

#ifndef NTP_ADJUST_EACH_ITER
/** Set to a true value to cause the epoch to be updated on each NTP
 * response.  When false, the epoch is set only when it is invalid,
 * which allows you to monitor the drift of the MCU uptime clock. */
#define NTP_ADJUST_EACH_ITER 0
#endif /* NTP_ADJUST_EACH_ITER */

static const char *
net_ipv4AsText (const in_addr * ipaddr)
{
  static char buffer[16];
  const uint8_t * i8p = (const uint8_t*)&ipaddr->s_addr;
  snprintf(buffer, sizeof(buffer), "%u.%u.%u.%u", i8p[0], i8p[1], i8p[2], i8p[3]);
  return buffer;
}

/** Flags set by CC3000 callback as events occur.  They may be read
 * without locking in the main program, but interrupts should be
 * disabled before clearing them. */
volatile unsigned int event_flags_v;
#define EVENT_FLAG_WLANCONN  0x01
#define EVENT_FLAG_IPCONN 0x02
#define EVENT_FLAG_DISCONN 0x04
#define EVENT_FLAG_SHUTDOWN 0x08

volatile unsigned int can_shutdown = 0;
static void wlan_cb (long event_type,
                     char * data,
                     unsigned char length)
{
  int rv = BSP430_HAL_ISR_CALLBACK_EXIT_LPM;

  switch (event_type) {
    case HCI_EVNT_WLAN_UNSOL_CONNECT:
      event_flags_v |= EVENT_FLAG_WLANCONN;
      break;
    case HCI_EVNT_WLAN_UNSOL_DHCP:
      event_flags_v |= EVENT_FLAG_IPCONN;
      break;
    case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
      event_flags_v |= EVENT_FLAG_DISCONN;
      break;
    case HCI_EVENT_CC3000_CAN_SHUT_DOWN:
      event_flags_v |= EVENT_FLAG_SHUTDOWN;
      ++can_shutdown;
      break;
    default:
      cprintf("%s wlan_cb evnt %#lx len %u at %p\n",
              xBSP430uptimeAsText_ni(ulBSP430uptime_ni()),
              event_type, length, data);
      /*FALLTHRU*/
    case HCI_EVNT_WLAN_KEEPALIVE:
      /* No wakeup for uninteresting events */
      rv = 0;
  }

  /* Wake up the main program to respond to whatever just happened. */
  iBSP430cc3000IRQrv = rv;
}

/** Union for type-safe aliasing of socket address formats */
typedef union sockaddr_u {
  sockaddr sa;
  sockaddr_in sai;
} sockaddr_u;

/** Union for type-safe aliasing of 32-bit integers and bytes */
typedef union uint32_u {
  uint32_t u32;
  uint8_t u8[4];
} uint32_u;

sockaddr_u local_addr;
sockaddr_u remote_addr;

void main (void)
{
  unsigned long wake_utt;
  int rc;
  long lrc;
  char as_text[BSP430_UPTIME_AS_TEXT_LENGTH];
  uint32_u ntp_addr;
  uint32_u self_addr;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  cprintf("\nntp " __DATE__ " " __TIME__ "\n");

  /* Initialization can be done with interrupts disabled, since the
   * function does nothing but store callbacks.  We use the same
   * callback for all three update capabilities. */
  rc = iBSP430cc3000spiInitialize(wlan_cb, NULL, NULL, NULL);
  if (0 > rc) {
    cprintf("ERR: Initialization failed: %d\n", rc);
    return;
  }

  BSP430_CORE_ENABLE_INTERRUPT();

  /* Local addresses use all zeros for inet addr.  bind does not
   * support dynamic assignment of unused port through sin_port=0. */
  memset(&local_addr, 0, sizeof(local_addr));
  local_addr.sai.sin_family = AF_INET;
  local_addr.sai.sin_port = htons(60123);

  /* Remote server will be determined by DNS from the NTP pool once we
   * start. */
  remote_addr = local_addr;
  remote_addr.sai.sin_port = htons(123);

  ntp_addr.u32 = 0;
  self_addr.u32 = 0;

  cprintf("Remote: %s:%u\n", net_ipv4AsText(&remote_addr.sai.sin_addr), ntohs(remote_addr.sai.sin_port));
  rc = sizeof(sBSP430uptimeNTPPacketHeader);
  if (48 != rc) {
    cprintf("ERR: NTP header size %d\n", rc);
    return;
  }
  wake_utt = ulBSP430uptime();

  (void)rc;
  while (1) {
    unsigned long timeout_utt;

    do {
      tNetappIpconfigRetArgs ipc;
      unsigned long start_utt;
      unsigned long finished_utt;
      int sfd;
      int nfds;
      fd_set rfds;
      int servers_left;
      int retries_left;

      /* Clear everything as we're starting a cycle */
      BSP430_CORE_DISABLE_INTERRUPT();
      do {
        event_flags_v = 0;
        start_utt = ulBSP430uptime_ni();
      } while (0);
      BSP430_CORE_ENABLE_INTERRUPT();

      /* Start the WAN process.  This is asynchronous; wait up to 2
       * seconds for it to complete. */
      cprintf("%s: ", xBSP430uptimeAsText(start_utt, as_text));
      cputchar('W');
      wlan_start(0);
      vBSP430ledSet(BSP430_LED_RED, 1);
      (void)wlan_set_event_mask(0UL);
      lrc = BSP430_UPTIME_MS_TO_UTT(2000);
      timeout_utt = ulBSP430uptime() + lrc;
      while ((! (EVENT_FLAG_WLANCONN & event_flags_v))
             && (0 < ((lrc = lBSP430uptimeSleepUntil(timeout_utt, LPM0_bits))))) {
      }
      if (! (EVENT_FLAG_WLANCONN & event_flags_v)) {
        cprintf("WLAN start failed\n");
        break;
      }

      /* Wait for IP connectivity (signalled by a DHCP event).
       * Continue using the previous timeout. */
      cputchar('D');
      while ((! (EVENT_FLAG_IPCONN & event_flags_v))
             && (0 < ((lrc = lBSP430uptimeSleepUntil(timeout_utt, LPM0_bits))))) {
      }
      if (! (EVENT_FLAG_IPCONN & event_flags_v)) {
        cprintf("IP conn failed\n");
        break;
      }

      /* Inspect the IP configuration.  Sometimes we get the event,
       * but there's no IP assigned. */
      netapp_ipconfig(&ipc);
      memcpy(self_addr.u8, ipc.aucIP, sizeof(self_addr));
      if (! self_addr.u32) {
        cprintf("IP assignment failed\n");
        break;
      }
      vBSP430ledSet(BSP430_LED_GREEN, 1);

      /* Obtain a UDP socket and bind it for local operations. */
      cputchar('I');
      sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (0 > sfd) {
        cprintf("socket() failed: %d\n", sfd);
        break;
      }
      cputchar('S');
      lrc = bind(sfd, &local_addr.sa, sizeof(local_addr.sa));
      if (0 > lrc) {
        cprintf("bind() failed: %ld\n", lrc);
        break;
      }
      cputchar('B');

      servers_left = NTP_SERVERS_PER_ATTEMPT;
      retries_left = NTP_REQUESTS_PER_SERVER;
      do {
        sBSP430uptimeNTPPacketHeader ntp0;
        sBSP430uptimeNTPPacketHeader ntp1;
        int have_invalid_epoch;
        struct timeval tv;
        sockaddr_u src;
        socklen_t slen = sizeof(src);
        unsigned long recv_utt;
        uint64_t recv_ntp;
        int64_t adjustment_ntp;
        long adjustment_ms;
        unsigned long rtt_us;

        have_invalid_epoch = 0 != iBSP430uptimeCheckEpochValidity();
        if (! remote_addr.sai.sin_addr.s_addr) {
          const char ntp_fqdn[] = "0.pool.ntp.org";
          ntp_addr.u32 = 0;
          rc = gethostbyname((char *)ntp_fqdn, sizeof(ntp_fqdn)-1, &ntp_addr.u32);
          cputchar('d');
          if (-95 == rc) { /* ARP request failed; retry usually works */
            rc = gethostbyname((char *)ntp_fqdn, sizeof(ntp_fqdn)-1, &ntp_addr.u32);
            cputchar('d');
          }
          if (0 == ntp_addr.u32) {
            cprintf("gethostbyname(%s) failed: %d\n", ntp_fqdn, rc);
            rc = -1;
            break;
          }
          remote_addr.sai.sin_addr.s_addr = htonl(ntp_addr.u32);
          cprintf("{%s}", net_ipv4AsText(&remote_addr.sai.sin_addr));
          retries_left = NTP_REQUESTS_PER_SERVER;
        }

        /* Configure the NTP request and send it */
        iBSP430uptimeInitializeNTPRequest(&ntp0);
        iBSP430uptimeSetNTPXmtField(&ntp0, NULL);
        BSP430_CORE_DISABLE_INTERRUPT();
        do {
          /* Clear the shutdown bit, so we know when it's ok to shut
           * down after this send */
          event_flags_v &= ~EVENT_FLAG_SHUTDOWN;
        } while (0);
        BSP430_CORE_ENABLE_INTERRUPT();
        rc = sendto(sfd, &ntp0, sizeof(ntp0), 0, &remote_addr.sa, sizeof(remote_addr.sai));
        if (sizeof(ntp0) != rc) {
          cprintf("sendto %s:%u failed: %d\n", net_ipv4AsText(&remote_addr.sai.sin_addr), ntohs(remote_addr.sai.sin_port), rc);
          rc = -1;
          break;
        }
        cputchar('s');

        /* If we get an answer it should be here in less than 100
         * ms, but give it 400 ms just to be kind. */
        tv.tv_sec = 0;
        tv.tv_usec = 400000UL;
        FD_ZERO(&rfds);
        FD_SET(sfd, &rfds);
        nfds = sfd+1;
        rc = select(nfds, &rfds, NULL, NULL, &tv);
        if (! FD_ISSET(sfd, &rfds)) {
          /* We didn't get an answer.  If there are any retries left, use them. */
          if (0 < retries_left--) {
            rc = 1;
            continue;
          }
          /* No retries left on this server: forget about it.  If
           * there are any servers left, try another. */
          cputchar('!');
          remote_addr.sai.sin_addr.s_addr = 0;
          if (0 < servers_left--) {
            rc = 1;
            continue;
          }
          /* No retries from all available servers.  Fail this attempt */
          cprintf("no responsive NTP server found\n");
          rc = -1;
          break;
        }

        /* Got a response.  Record the time it came in and then read
         * it (no high-resolution packet RX time available, but we
         * believe it's here already so set the RX time first).  The
         * message is unacceptable if it isn't an NTP packet. */
        recv_utt = ulBSP430uptime();
        rc = recvfrom(sfd, &ntp1, sizeof(ntp1), 0, &src.sa, &slen);
        if (sizeof(ntp1) != rc) {
          cprintf("recv failed: %d\n", rc);
          rc = -1;
          break;
        }
        cputchar('r');

        /* Convert the RX time to NTP, then process the message to
         * determine the offset. */
        rc = iBSP430uptimeAsNTP(recv_utt, &recv_ntp, have_invalid_epoch);
        if (0 != rc) {
          cprintf("NTP decode failed: %d\n", rc);
          continue;
        }
        rc = iBSP430uptimeProcessNTPResponse(&ntp0, &ntp1, recv_ntp, &adjustment_ntp, &adjustment_ms, &rtt_us);
        if (0 != rc) {
          cprintf("Process failed: %d\n", rc);
          continue;
        }
        if (have_invalid_epoch) {
          rc = iBSP430uptimeSetEpochFromNTP(BSP430_UPTIME_BYPASS_EPOCH_NTP + adjustment_ntp);
          cputchar('E');
          if (0 != rc) {
            cprintf("\nERR: SetEpoch failed: %d\n", rc);
          }
#if (NTP_ADJUST_EACH_ITER - 0)
        } else {
          rc = iBSP430uptimeAdjustEpochFromNTP(adjustment_ntp);
          cputchar('A');
          if (0 != rc) {
            cprintf("\nERR: AdjustEpoch failed: %d\n", rc);
          }
#endif
        }
        cprintf("[%s:%u adj %lld ntp = %ld ms, rtt %lu us]",
                net_ipv4AsText(&remote_addr.sai.sin_addr), ntohs(remote_addr.sai.sin_port),
                adjustment_ntp, adjustment_ms, rtt_us);
      } while (0 != rc);
      if (0 != rc) {
        cprintf("NTP query failed\n");
        break;
      }
#if 0
      /* The shutdown OK seems to arrive about 1000 ms after the last
       * transmit, which is unnecessarily long.  As we're not doing
       * TCP, there's no reason to wait for it. */
      lrc = BSP430_UPTIME_MS_TO_UTT(4000);
      timeout_utt = ulBSP430uptime() + lrc;
      while ((! (EVENT_FLAG_SHUTDOWN & event_flags_v))
             && (0 < ((lrc = lBSP430uptimeSleepUntil(timeout_utt, LPM0_bits))))) {
      }
      if (! (EVENT_FLAG_SHUTDOWN & event_flags_v)) {
        cprintf("SHUTDOWN ok never received\n");
        break;
      }
#endif
      finished_utt = ulBSP430uptime();
      cprintf("[%s]\n", xBSP430uptimeAsText(finished_utt - start_utt, as_text));
    } while (0);

    BSP430_CORE_DISABLE_INTERRUPT();
    do {
      event_flags_v = 0;
    } while (0);
    BSP430_CORE_ENABLE_INTERRUPT();
    wlan_stop();
    vBSP430ledSet(BSP430_LED_GREEN, 0);
    vBSP430ledSet(BSP430_LED_RED, 0);
    wake_utt += 60 * ulBSP430uptimeConversionFrequency_Hz();
    while (0 < lBSP430uptimeSleepUntil(wake_utt, LPM2_bits)) {
    }
  }
  cprintf("Fell off end\n");
}
