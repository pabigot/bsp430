/** This file is in the public domain.
 *
 * Basic demonstration using FatFs to access the micro SD card on the
 * EXP430F5529.
 *
 * You'll need to install the FatFs distribution in a subdirectory
 * named "fatfs".  This implementation has been tested with FatFs
 * R0.10 and R0.09b.
 *
 * @note: If you use a version older than R0.10 you need to add @c
 * EXT_CPPFLAGS=-DFATFS_IS_PRE_R0_10=1 to the make command line.
 *
 * You can get fatfs from http://elm-chan.org/fsw/ff/00index_e.html
 *
 * Or you can clone a copy with:
 *   git clone -b ff/master git@github.com:pabigot/FatFs.git fatfs
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>
#include <bsp430/periph/sys.h>
#include <bsp430/periph/pmm.h>
#include <stdio.h>

/* msp430.h headers define DIR which will conflict with the structure
 * definition from FatFS. */
#undef DIR

#include "diskio.h"
#include "ff.h"

/* Get definitions for FATFS_IS_PRE_R0_10 and other flags that
 * accommodate API changes. */
#include "ff_compat.h"

char buffer[1024];

void main ()
{
  unsigned int reset_flags;
  unsigned long reset_causes;
  int rv;
  FATFS fso;
  DIR dir_obj;
  FILINFO finfo;

  {
    unsigned int sysrstiv;

    /* Record all the reset causes */
    reset_causes = 0;
    while (0 != ((sysrstiv = uiBSP430sysSYSRSTGenerator_ni(&reset_flags)))) {
      reset_causes |= 1UL << (sysrstiv / 2);
    }
  }

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\nBUILD " __DATE__ " " __TIME__ "\n");

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

  rv = iBSP430pmmSetCoreVoltageLevel_ni(PMMCOREV_3);
  cprintf("Set core voltage gets %d\n", rv);

#if (FATFS_IS_PRE_R0_10 - 0)
  rv = f_mount(0, &fso);
#else /* Pre R0.10 */
  rv = f_mount(&fso, "", 1);
#endif /* Pre R0.10 */
  cprintf("mount gets %d\n", rv);
  if (FR_OK == rv) {
    rv = f_opendir(&dir_obj, "/");
    cprintf("opendir gets %d\n", rv);
  }
  while (FR_OK == rv) {
    rv = f_readdir(&dir_obj, &finfo);
    if (FR_OK != rv) {
      break;
    }
    if (0 == finfo.fname[0]) {
      break;
    }
    cprintf("%s %lu %u %u %#x\n", finfo.fname, finfo.fsize, finfo.fdate, finfo.ftime, finfo.fattrib);
  }
  {
    FIL fil_obj;

    /* Append a boot log message to the file BOOT.LOG.  NB: This had
     * failed with version combinations prior to FatFs 0.9b and BSP430
     * 20130427. */
    rv = f_open(&fil_obj, "0:BOOT.LOG", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    cprintf("Boot log open returned %d\n", rv);
    if (FR_OK == rv) {
      UINT nbytes = -1;
      rv = f_read(&fil_obj, buffer, sizeof(buffer), &nbytes);
      cprintf("Read boot log %u got %d with %u read\n",
              (unsigned int)sizeof(buffer), rv, nbytes);
      if (0 == rv) {
        buffer[nbytes] = 0;
        cputs("Contents:");
        cputs(buffer);
      }
      rv = f_lseek(&fil_obj, f_size(&fil_obj));
      cprintf("Seek to end got %d\n", rv);
      if (FR_OK == rv) {
        int nb = snprintf(buffer, sizeof(buffer), "Booted build " __DATE__ " " __TIME__ "\n");
        UINT nw;
        rv = f_write(&fil_obj, buffer, nb, &nw);
        cprintf("write %u got %d nw %u\n", nb, rv, nw);
      }
      f_close(&fil_obj);
    }
  }

#if (FATFS_IS_PRE_R0_10 - 0)
  f_mount(0, NULL);
#else /* Pre R0.10 */
  f_mount(NULL, NULL, 1);
#endif /* Pre R0.10 */
  cprintf("Exiting application\n");

}
