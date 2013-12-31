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
 * @brief Device support for u8glib on EXP430F5438 (HD66753)
 *
 */

#include <bsp430/platform.h>
#include <bsp430/utility/u8glib.h>
#include <bsp430/clock.h>
#include <bsp430/periph/port.h>
#include <string.h>

#if (BSP430_UTILITY_U8GLIB - 0)

/* Upright, 1/112 duty */
#define LCD_R01_DRIVER_OUTPUT_SETTING 0x0d
/* C-wave 12 rows */
#define LCD_R02_WAVEFORM_SETTING 0x004c
/* Power: 1/7 bias drive, 6x step-up, 16-divided clock, middle OpAmp current, C-wave 12 rows */
#define LCD_R03_POWER_SETTING 0x1214
#define LCD_R03_SLEEP_MASK 0x000C
#define LCD_R03_SLEEP_BITS 0x0002
#define LCD_R03_STANDBY_BITS 0x0001

#ifndef BSP430_PLATFORM_EXP430F5438_LCD_SLEEP_IS_STANDBY
/** If defined to a true value, sleep will use standby mode instead of
 * sleep mode.  Standby uses less power but requires a 10ms
 * stabilization delay on wakeup. */
#define BSP430_PLATFORM_EXP430F5438_LCD_SLEEP_IS_STANDBY 0
#endif /* BSP430_PLATFORM_EXP430F5438_LCD_SLEEP_IS_STANDBY */

/* V1REF = Vreg*5, VR = 1.30*R */
#define LCD_R04_CONTRAST_SETTING 0x0458
#define LCD_R04_CONTRAST_MASK 0x007F
#define LCD_R04_CONTRAST_SHIFT 1
/* Increment after parallel (horizontal) replace (I/D=1, AM=0, LG=0)*/
#define LCD_R05_ENTRY_MODE_SETTING 0x10
/* Rotate incoming word 8 bits (RT=4) */
#define LCD_R06_ROTATION_SETTING 0x04
/* 2/3 grayscale for bright, 1/3 grayscale for dim.  No reverse. */
#define LCD_R07_DISPLAY_CONTROL_SETTING 0x0014
/* Bit to set (clear) in R07 to turn display on (off) */
#define LCD_R07_DISPLAY_ON_BIT 0x0001
/* Black-and-white reversed blinking cursor */
#define LCD_R08_CURSOR_SETTING 0x0003
/* Bit to set (clear) in R08 to turn cursor on (off) */
#define LCD_R08_CURSOR_ON_BIT 0x0004

/* Evocative name for the register that sets the CGRAM address */
#define LCD_R_CGRAM_ADDR 0x11
/* Evocative name for the register that writes CGRAM data */
#define LCD_R_CGRAM_DATA 0x12

/* The (word) address increments by this many for each row. */
#define ROW_ADDRESS_INCREMENT 0x20

/* Two bits per column */
#define ROW_WIDTH_BYTES (BSP430_PLATFORM_EXP430F5438_LCD_COLUMNS * 2 / 8)

#define HD66753_START 0x74
/** The register select bit.  A zero indicates the index register or
 * status; a 1 indicates RAM. */
#define HD66753_RS 0x02
/** The read/write mode.  A zero indicates read; a 1 indicates
 * write. */
#define HD66753_RW 0x01

static hBSP430halSERIAL spi_;

/* Single page containing all cells, 4 pixels per byte MSB, row-major
 * order. */
static uint8_t cgram_[BSP430_PLATFORM_EXP430F5438_LCD_ROWS * BSP430_PLATFORM_EXP430F5438_LCD_COLUMNS / 4];

/** Assert the CS# signal in preparation for interacting with the
 * device. */
#define LCD_CS_ASSERT() do {                                            \
    lcd_port->out &= ~(BSP430_PLATFORM_EXP430F5438_LCD_CSn_PORT_BIT);   \
  } while (0)

/** De-assert the CS# signal after interacting with the device. */
#define LCD_CS_DEASSERT() do {                                          \
    lcd_port->out |= (BSP430_PLATFORM_EXP430F5438_LCD_CSn_PORT_BIT);    \
  } while (0)

static void
hd66753SetRegister_ni (unsigned int reg,
                       unsigned int data)
{
  uint8_t cmdbuf[3];
  volatile sBSP430hplPORT * lcd_port;

  lcd_port = xBSP430hplLookupPORT(BSP430_PLATFORM_EXP430F5438_LCD_PORT_PERIPH_HANDLE);
  cmdbuf[0] = HD66753_START;
  cmdbuf[1] = (reg >> 8) & 0xFF;
  cmdbuf[2] = reg & 0xFF;
  LCD_CS_ASSERT();
  (void)iBSP430spiTxRx_rh(spi_, cmdbuf, sizeof(cmdbuf), 0, NULL);
  LCD_CS_DEASSERT();
  cmdbuf[0] |= HD66753_RS;
  cmdbuf[1] = (data >> 8) & 0xFF;
  cmdbuf[2] = data & 0xFF;
  LCD_CS_ASSERT();
  (void)iBSP430spiTxRx_rh(spi_, cmdbuf, sizeof(cmdbuf), 0, NULL);
  LCD_CS_DEASSERT();
}

static void
hd66753WriteCGRAM_ni (void)
{
  unsigned int row;
  uint8_t cmdbuf[3];
  uint8_t * cgp;
  volatile sBSP430hplPORT * lcd_port;

  lcd_port = xBSP430hplLookupPORT(BSP430_PLATFORM_EXP430F5438_LCD_PORT_PERIPH_HANDLE);
  cmdbuf[1] = 0;
  cmdbuf[2] = LCD_R_CGRAM_DATA;
  cgp = cgram_;
  for (row = 0; row < BSP430_PLATFORM_EXP430F5438_LCD_ROWS; ++row) {
    hd66753SetRegister_ni(LCD_R_CGRAM_ADDR, row * ROW_ADDRESS_INCREMENT);
    cmdbuf[0] = HD66753_START;
    LCD_CS_ASSERT();
    (void)iBSP430spiTxRx_rh(spi_, cmdbuf, sizeof(cmdbuf), 0, NULL);
    LCD_CS_DEASSERT();
    cmdbuf[0] |= HD66753_RS;
    LCD_CS_ASSERT();
    (void)iBSP430spiTxRx_rh(spi_, cmdbuf, sizeof(cmdbuf[0]), 0, NULL);
    (void)iBSP430spiTxRx_rh(spi_, cgp, ROW_WIDTH_BYTES, 0, NULL);
    cgp += ROW_WIDTH_BYTES;
    LCD_CS_DEASSERT();
  }
}

static uint8_t
u8g_com_fn (u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  int rc;

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
      case U8G_COM_MSG_WRITE_SEQ:
      default:
        while (1) {
          /* Spin to detect mis-use. */
        }
        break;
      case U8G_COM_MSG_INIT:
        /* Maximum clock period during reads is 250ns so limit this to
         * 4 MHz */
        spi_ = hBSP430serialOpenSPI(hBSP430serialLookup(BSP430_PLATFORM_EXP430F5438_LCD_SPI_PERIPH_HANDLE),
                                    BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPL | UCMSB | UCMST),
                                    UCSSEL__SMCLK, uiBSP430serialSMCLKPrescaler(4000000UL));
        rc = (0 != spi_);
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
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);
  volatile sBSP430hplPORT * lcd_port;
  int rc = 0;

  BSP430_CORE_DISABLE_INTERRUPT();

  /* Sanity check on port lookup */
  lcd_port = xBSP430hplLookupPORT(BSP430_PLATFORM_EXP430F5438_LCD_PORT_PERIPH_HANDLE);
  while (0 == lcd_port) {
  }
  do {
    /* Device should always be left in command mode. */
    switch (msg) {
      case U8G_DEV_MSG_PAGE_NEXT:
        hd66753WriteCGRAM_ni();
      /*FALLTHRU*/
      default:
        /* Anything not specifically handled is delegated to the base function */
        rc = u8g_dev_pb8h2_base_fn(u8g, dev, msg, arg);
        break;
      case U8G_DEV_MSG_INIT: {
        /* Configure port */
        lcd_port->out &= ~(BSP430_PLATFORM_EXP430F5438_LCD_RSTn_PORT_BIT | BSP430_PLATFORM_EXP430F5438_LCD_CSn_PORT_BIT);
        lcd_port->sel &= ~(BSP430_PLATFORM_EXP430F5438_LCD_RSTn_PORT_BIT | BSP430_PLATFORM_EXP430F5438_LCD_CSn_PORT_BIT);
        lcd_port->dir |= (BSP430_PLATFORM_EXP430F5438_LCD_RSTn_PORT_BIT | BSP430_PLATFORM_EXP430F5438_LCD_CSn_PORT_BIT);

        /* This call delegates to USG_COM_MSG_INIT, which initializes
         * the spi_ handle and csn_port.  Initialization failed if we
         * don't have a SPI interface.  Otherwise power is left off,
         * display is held in reset, and chip select is deasserted.  */
        u8g_InitCom(u8g, dev);
        if (! spi_) {
          rc = 0;
          break;
        }

        /* Continue to hold RSTn for 1 ms, then release it. */
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 1000);
        lcd_port->out |= BSP430_PLATFORM_EXP430F5438_LCD_RSTn_PORT_BIT;

        /* Start oscillation, then delay for 10ms  */
        hd66753SetRegister_ni(0, 1);    /* Start oscillation */
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 100);
        hd66753SetRegister_ni(1, LCD_R01_DRIVER_OUTPUT_SETTING);
        hd66753SetRegister_ni(2, LCD_R02_WAVEFORM_SETTING);
        hd66753SetRegister_ni(3, LCD_R03_POWER_SETTING);
        hd66753SetRegister_ni(4, LCD_R04_CONTRAST_SETTING);
        hd66753SetRegister_ni(5, LCD_R05_ENTRY_MODE_SETTING);
        hd66753SetRegister_ni(6, LCD_R06_ROTATION_SETTING);
        hd66753SetRegister_ni(7, LCD_R07_DISPLAY_CONTROL_SETTING | LCD_R07_DISPLAY_ON_BIT);

        memset(cgram_, 0, sizeof(cgram_));

        break;
      }
      case U8G_DEV_MSG_CONTRAST:
        hd66753SetRegister_ni(4, (LCD_R04_CONTRAST_SETTING & ~LCD_R04_CONTRAST_MASK) | (LCD_R04_CONTRAST_MASK & (*(uint8_t*)arg) >> LCD_R04_CONTRAST_SHIFT));
        break;
      case U8G_DEV_MSG_SLEEP_ON:
        hd66753SetRegister_ni(3, (LCD_R03_SLEEP_MASK & LCD_R03_POWER_SETTING)
#if (BSP430_PLATFORM_EXP430F5438_LCD_SLEEP_IS_STANDBY - 0)
                              | LCD_R03_STANDBY_BITS
#else /* BSP430_PLATFORM_EXP430F5438_LCD_SLEEP_IS_STANDBY */
                              | LCD_R03_SLEEP_BITS
#endif /* BSP430_PLATFORM_EXP430F5438_LCD_SLEEP_IS_STANDBY */
                             );
        rc = 1;
        break;
      case U8G_DEV_MSG_SLEEP_OFF:
#if (BSP430_PLATFORM_EXP430F5438_LCD_SLEEP_IS_STANDBY - 0)
        hd66753SetRegister_ni(0, 1);    /* Start oscillation */
        BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ / 100);
#endif /* BSP430_PLATFORM_EXP430F5438_LCD_SLEEP_IS_STANDBY */
        hd66753SetRegister_ni(3, LCD_R03_POWER_SETTING);
        rc = 1;
        break;
    }
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  return rc;
}

static u8g_pb_t pb_ = { {
    BSP430_PLATFORM_EXP430F5438_LCD_ROWS,
    BSP430_PLATFORM_EXP430F5438_LCD_ROWS,
    0, 0, 0
  },
  BSP430_PLATFORM_EXP430F5438_LCD_COLUMNS,
  cgram_
};

u8g_dev_t xBSP430u8gDevice = { u8g_dev_fn, &pb_, u8g_com_fn };

hBSP430halSERIAL
hBSP430u8gSPI (void)
{
  return spi_;
}

#endif /* BSP430_UTILITY_U8GLIB */
