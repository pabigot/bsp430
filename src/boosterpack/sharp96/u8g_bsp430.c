/* Copyright 2012-2014, Peter A. Bigot
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the software nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without
 *   specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** @file
 *
 * @brief Device support for u8glib on using the 430BOOST-SHARP96
 * booster pack as a platform device.
 *
 * Use $(U8GLIB_CSRC)/u8g_pb8h1.c with this driver.
 *
 */

#include <bsp430/platform.h>
#include <bsp430/utility/u8glib.h>
#include <bsp430/utility/sharplcd.h>
#include <bsp430/clock.h>
#include <bsp430/periph/port.h>
#include <string.h>
#include <stddef.h>

#if (BSP430_UTILITY_U8GLIB - 0)

/* Single-pixel U8GLib apparently cannot handle more than 8 rows per
 * page. */
#define ROWS_PER_PAGE 8

/* Single page containing all pixels */
static uint8_t cgram_[ROWS_PER_PAGE * BSP430_PLATFORM_SHARPLCD_BYTES_PER_LINE];

typedef struct sDeviceData {
  sBSP430sharplcd device;       /**< The resource and state for the LCD */
  u8g_pb_t pb;                  /**< The page buffer required by u8glib */
  char in_use;                  /**< Non-zero if the driver is using the device */
} sDeviceData;

typedef sDeviceData * hDeviceData;

static BSP430_CORE_INLINE
hDeviceData
get_device_data (u8g_t *u8g)
{
  return (hDeviceData)((char *)(u8g->dev->dev_mem) - offsetof(sDeviceData, pb));
}

/* The LS013B4DN04 presents text as white (clear) on silver (set).  Or
 * vice versa.  Since u8glib doesn't have the notion of an inverted
 * display, this routine may be called before and after updating
 * display lines to invert the image. */
static BSP430_CORE_INLINE
void invert_cgram (void)
{
  uint8_t * dp = cgram_;
  const uint8_t * const edp = cgram_ + sizeof(cgram_);
  while (dp < edp) {
    *dp = ~*dp;
    ++dp;
  }
}

static uint8_t
u8g_com_fn (u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  hDeviceData dd = get_device_data(u8g);
  int rc = 1;

  do {
    switch (msg) {
      /* None of these are implemented because the BSP430-supported
       * devices use neither the u8glib devices nor WriteEscSeqP.  To
       * detect changes in that situation, invocations of these
       * operations will hang. */
      case U8G_COM_MSG_STOP:
      case U8G_COM_MSG_ADDRESS:
      case U8G_COM_MSG_CHIP_SELECT:
      case U8G_COM_MSG_RESET:
      case U8G_COM_MSG_WRITE_BYTE:
      case U8G_COM_MSG_WRITE_SEQ_P:
      default:
        while (1) {
          /* Spin to detect mis-use. */
        }
        break;
      case U8G_COM_MSG_INIT: {
        hBSP430sharplcd dev = NULL;

        BSP430_CORE_DISABLE_INTERRUPT();
        do {
          while (dd->in_use) {
            /* bug */
          }
          dd->in_use = 1;
          dev = hBSP430sharplcdInitializePlatformDevice(&dd->device);
          dd->in_use = 0;
        } while (0);
        BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
        rc = (0 != dev);
        break;
      }
      case U8G_COM_MSG_WRITE_SEQ: {
        u8g_pb_t * pb = &dd->pb;
        int start_line = 1+pb->p.page_y0;
        int num_lines = (pb->p.page_y1 - pb->p.page_y0) + 1;

        (void)invert_cgram;     /* reference to avoid compiler warnings */
        BSP430_CORE_DISABLE_INTERRUPT();
        do {
          while (dd->in_use) {
            /* bug */
          }
          dd->in_use = 1;
          rc = ! iBSP430sharplcdUpdateDisplayLines_rh(&dd->device, start_line, num_lines, cgram_);
          dd->in_use = 0;
        } while (0);
        BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
        break;
      }
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

static uint8_t
u8g_dev_fn (u8g_t * u8g,
            u8g_dev_t * dev,
            uint8_t msg,
            void * arg)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  hDeviceData dd = get_device_data(u8g);
  int rc = 0;

  /* Sanity check on port lookup */
  do {
    /* Device should always be left in command mode. */
    switch (msg) {
      default:
        /* Anything not specifically handled is delegated to the base function */
        rc = u8g_dev_pb8h1_base_fn(u8g, dev, msg, arg);
        break;
      case U8G_DEV_MSG_INIT: {
        /* This call delegates to USG_COM_MSG_INIT, which initializes
         * the spi_ handle and csn_port.  Initialization failed if we
         * don't have a SPI interface.  Otherwise power is left off,
         * display is held in reset, and chip select is deasserted.  */
        u8g_InitCom(u8g, dev, U8G_SPI_CLK_CYCLE_NONE);
        if (! (dd->device.spi && dd->device.cs)) {
          rc = 0;
          break;
        }
        BSP430_CORE_DISABLE_INTERRUPT();
        do {
          while (dd->in_use) {
            /* bug */
          }
          dd->in_use = 1;
          rc = ! iBSP430sharplcdSetEnabled_ni(&dd->device, 1);
          dd->in_use = 0;
        } while (0);
        BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
        memset(cgram_, 0, sizeof(cgram_));
        break;
      }
      case U8G_DEV_MSG_PAGE_NEXT: {
        u8g_pb_t * pb = &dd->pb;

        /* This call delegates to U8G_COM_MSG_WRITE_SEQ */
        rc = u8g_pb_WriteBuffer(pb, u8g, dev);
        if (rc) {
          rc = u8g_dev_pb8h1_base_fn(u8g, dev, msg, arg);
        }
        break;
      }
    }
  } while (0);
  return rc;
}

static sDeviceData device_data = {
  { /* sBSP430sharplcd */ },
  { /* u8g_pb_t */
    { /* u8g_page_t */
      ROWS_PER_PAGE,
      BSP430_PLATFORM_SHARPLCD_ROWS,
      0, 0, 0
    },
    BSP430_PLATFORM_SHARPLCD_COLUMNS,
    cgram_,
  }
};

u8g_dev_t xBSP430u8gDevice = { u8g_dev_fn, &device_data.pb, u8g_com_fn };

int
iBSP430u8gRefresh (u8g_t * u8g)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  hDeviceData dd = get_device_data(u8g);

  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    if (! dd->in_use) {
      dd->in_use = 1;
      iBSP430sharplcdRefreshDisplay_rh(&dd->device);
      dd->in_use = 0;
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return 0;
}

hBSP430halSERIAL
hBSP430u8gSPI (void)
{
  return device_data.device.spi;
}

#endif /* BSP430_UTILITY_U8GLIB */
