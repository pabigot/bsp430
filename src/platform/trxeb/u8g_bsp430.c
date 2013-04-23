/* Copyright 2012-2013, Peter A. Bigot
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
 * @brief Device support for u8glib on TrxEB (DOGM128E-6)
 *
 */

#include <bsp430/platform.h>
#include <bsp430/utility/u8glib.h>
#include <bsp430/clock.h>
#include <bsp430/periph/port.h>

#if (BSP430_UTILITY_U8GLIB - 0)

static hBSP430halSERIAL spi_;

/** Assert the CS# signal in preparation for interacting with the
 * device. */
#define LCD_CS_ASSERT() do {                                            \
    csna0_port->out &= ~(BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_BIT);      \
  } while (0)

/** De-assert the CS# signal after interacting with the device. */
#define LCD_CS_DEASSERT() do {                                          \
    csna0_port->out |= (BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_BIT);       \
  } while (0)

/** Switch device to command mode */
#define LCD_MODE_COMMAND() do {                                         \
    csna0_port->out &= ~(BSP430_PLATFORM_TRXEB_LCD_A0_PORT_BIT);       \
  } while (0)

/** Switch device to data mode */
#define LCD_MODE_DATA() do {                                           \
    csna0_port->out |= (BSP430_PLATFORM_TRXEB_LCD_A0_PORT_BIT);       \
  } while (0)

static uint8_t
u8g_com_fn (u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
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
        volatile sBSP430hplPORT * csna0_port;

        /* TODO: Share this with M25P initialization */
        spi_ = hBSP430serialOpenSPI(hBSP430serialLookup(BSP430_PLATFORM_TRXEB_LCD_SPI_PERIPH_HANDLE),
                                    BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPL | UCMSB | UCMST),
                                    UCSSEL__SMCLK, 0);
        if (0 == spi_) {
          rc = 0;
          break;
        }

        /* Sanity check on port sharing and lookup */
        csna0_port = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_PERIPH_HANDLE);
        while ((BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_PERIPH_HANDLE != BSP430_PLATFORM_TRXEB_LCD_A0_PORT_PERIPH_HANDLE)
               || (0 == csna0_port)) {
        }

        /* Configure with chip select deasserted and A0 in command mode */
        csna0_port->out &= ~(BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_BIT | BSP430_PLATFORM_TRXEB_LCD_A0_PORT_BIT);
        csna0_port->sel &= ~(BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_BIT | BSP430_PLATFORM_TRXEB_LCD_A0_PORT_BIT);
        csna0_port->dir |= (BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_BIT | BSP430_PLATFORM_TRXEB_LCD_A0_PORT_BIT);
        rc = 1;
        break;
      }
      case U8G_COM_MSG_WRITE_SEQ:
        rc = iBSP430spiTxRx_ni(spi_, arg_ptr, arg_val, 0, NULL);
        rc = (arg_val == rc);
        break;
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
  BSP430_CORE_INTERRUPT_STATE_T istate;
  volatile sBSP430hplPORT * csna0_port;
  const uint8_t * cmdp = NULL;
  size_t cmdlen;
  uint8_t cmdbuf[4];
  uint8_t * cp;
  int rc;

  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  csna0_port = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_LCD_CSn_PORT_PERIPH_HANDLE);
  while (0 == csna0_port) {
    /* Sanity check */
  }
  do {
    /* Device should always be left in command mode. */
    switch (msg) {
      default:
        /* Anything not specifically handled is delegated to the base function */
        rc = u8g_dev_pb8v1_base_fn(u8g, dev, msg, arg);
        break;
      case U8G_DEV_MSG_INIT: {
        static const uint8_t cmds[] = {
          /* Display start line set: 0 (0x40 | (0x3f & ra)) */
          0x40,
          /* ADC select: reverse (0xA0 | (0x01 & reverse)) */
          0xa1,
          /* Common output direction: normal (0xC0 | (0x80 & reverse)) */
          0xc0,
          /* Display normal/reverse: normal (0xA6 | (0x01 & reverse)) */
          0xa6,
          /* LCD bias set: 1/9 (0xA2 | (0x01 & use_seventh) */
          0xa2,
          /* Power controller set: all (0x28 | ((0x04 & booster) | (0x02 & v_reg) | (0x01 & v_foll))) */
          0x2f,
          /* Booster ratio set: 2x (0xF8 ; 0x03 & something) */
          0xf8, 0x00,
          /* Internal resistor ratio set: maximum (0x20 | (0x07 & level)) */
          0x027,
          /* Electric volume set: 22/64 (0x81 ; 0x3F & level) */
          0x81, 0x16,
          /* Note: No hardware support for indicator on TrxEB, and the
           * thing is off by default on reset, so just leave it that
           * way. */
          /* Display on/off: on (0xAE | (0x01 & on)) */
          0xaf,
        };
        volatile sBSP430hplPORT * pwrrst_port;

        /* Sanity check. */
        while (BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_PERIPH_HANDLE != BSP430_PLATFORM_TRXEB_LCD_RSTn_PORT_PERIPH_HANDLE) {
        }

        pwrrst_port = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_PERIPH_HANDLE);
        pwrrst_port->out &= ~(BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_BIT | BSP430_PLATFORM_TRXEB_LCD_RSTn_PORT_BIT);
        pwrrst_port->sel &= ~(BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_BIT | BSP430_PLATFORM_TRXEB_LCD_RSTn_PORT_BIT);
        pwrrst_port->dir |= (BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_BIT | BSP430_PLATFORM_TRXEB_LCD_RSTn_PORT_BIT);

        /* This call delegates to USG_COM_MSG_INIT, which initializes
         * the spi_ handle and csna0_port.  Initialization failed if we
         * don't have a SPI interface.  Otherwise power is left off,
         * display is held in reset, and chip select is deasserted.  */
        u8g_InitCom(u8g, dev);
        if (! spi_) {
          rc = 0;
          break;
        }

        /* Apply power, then wait 100ms for power to stabilize.  NB: If
         * you're tracking this with a logic analyzer, it actually takes a
         * few hundred microseconds before the PWR signal goes high; some EE
         * thing related to inductance no doubt.  (100ms is what is used in
         * the TrxEB firmware.) */
        pwrrst_port->out |= BSP430_PLATFORM_TRXEB_LCD_PWR_PORT_BIT;
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 10);

        /* Take it out of reset mode and wait 1 ms. */
        pwrrst_port->out |= BSP430_PLATFORM_TRXEB_LCD_RSTn_PORT_BIT;
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000);

        /* Transmit the initialization sequence */
        cmdp = cmds;
        cmdlen = sizeof(cmds);
        break;
      }
      case U8G_DEV_MSG_PAGE_NEXT: {
        u8g_pb_t * pb = (u8g_pb_t *)(dev->dev_mem);

        cp = cmdbuf;
        /* Set page as directed */
        *cp++ = 0xb0 | pb->p.page;
        /* Set column address 0 */
        *cp++ = 0x10;
        *cp++ = 0x00;
        LCD_CS_ASSERT();
        do {
          /* Configure the page. */
          rc = iBSP430spiTxRx_ni(spi_, cmdbuf, cp-cmdbuf, 0, NULL);
          rc = (cp-cmdbuf == rc);
          if (! rc) {
            break;
          }
          /* Transmit the page contents. */
          LCD_MODE_DATA();
          /* This call delegates to U8G_COM_MSG_WRITE_SEQ */
          rc = u8g_pb_WriteBuffer(pb, u8g, dev);
          LCD_MODE_COMMAND();
        } while (0);
        LCD_CS_DEASSERT();
        if (rc) {
          /* NB: You do need to pass this on to the base implementation so
           * it moves to the next page. */
          rc = u8g_dev_pb8v1_base_fn(u8g, dev, msg, arg);
        }
        break;
      }
      case U8G_DEV_MSG_CONTRAST:
        /* Set up the contrast configuration command */
        cp = cmdbuf;
        *cp++ = 0x81;
        *cp++ = (*(uint8_t*)arg) >> 2;
        cmdp = cmdbuf;
        cmdlen = cp - cmdbuf;
        break;
      case U8G_DEV_MSG_SLEEP_ON: {
        static const uint8_t cmds[] = {
          0xae,                   /* Display off */
          0xa5,                   /* Display all points on */
        };
        cmdp = cmds;
        cmdlen = sizeof(cmds);
        break;
      }
      case U8G_DEV_MSG_SLEEP_OFF: {
        static const uint8_t cmds[] = {
          0xa4,                   /* Display all points off */
          0xaf,                   /* Display on */
        };
        cmdp = cmds;
        cmdlen = sizeof(cmds);
        break;
      }
    }
    /* Handle any final SPI transmission required for the command */
    if (cmdp) {
      /* Write the initialization sequence defined above. */
      LCD_CS_ASSERT();
      rc = iBSP430spiTxRx_ni(spi_, cmdp, cmdlen, 0, NULL);
      LCD_CS_DEASSERT();
      rc = (cmdlen == rc);
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

static uint8_t pageBuffer_[BSP430_PLATFORM_TRXEB_LCD_COLUMNS];
static u8g_pb_t pb_ = { {
    BSP430_PLATFORM_TRXEB_LCD_PAGES,
    BSP430_PLATFORM_TRXEB_LCD_PAGES * BSP430_PLATFORM_TRXEB_LCD_ROWS_PER_PAGE,
    0, 0, 0
  },
  BSP430_PLATFORM_TRXEB_LCD_COLUMNS,
  pageBuffer_
};

u8g_dev_t xBSP430u8gDevice = { u8g_dev_fn, &pb_, u8g_com_fn };

hBSP430halSERIAL
hBSP430u8gSPI (void)
{
  return spi_;
}

#endif /* BSP430_UTILITY_U8GLIB */
