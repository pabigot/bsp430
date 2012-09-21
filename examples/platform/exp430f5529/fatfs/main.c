/** This file is in the public domain.
 *
 * Basic demonstration using FatFs to access the micro SD card on the EXP430F5529.
 *
 * You'll need to install the FatFs distribution in a subdirectory
 * named "fatfs".  This implementation has been tested with FatFs
 * R0.09a.
 * 
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/rfem.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>

/* msp430.h headers define DIR which will conflict with the structure
 * definition from FatFS. */
#undef DIR

#include "inttypes.h"
#include "diskio.h"
#include "ff.h"

unsigned char buffer[1024];

void main ()
{
  int rv;
  FATFS fso;
  DIR dir_obj;
  FILINFO finfo;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  cprintf("\nBUILD " __DATE__ " " __TIME__ "\n");
  rv = f_mount(0, &fso);
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
}
