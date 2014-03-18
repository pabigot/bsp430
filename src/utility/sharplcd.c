/* Copyright 2014, Peter A. Bigot
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

#include <bsp430/platform.h>
#include <bsp430/utility/sharplcd.h>
#include <bsp430/clock.h>
#include <stdlib.h>
#include <string.h>

hBSP430sharplcd
hBSP430sharplcdInitializePlatformDevice (hBSP430sharplcd dev)
{
  if (NULL == dev) {
    return NULL;
  }

  dev->cs = xBSP430hplLookupPORT(BSP430_PLATFORM_SHARPLCD_CS_PORT_PERIPH_HANDLE);
  dev->cs_bit = BSP430_PLATFORM_SHARPLCD_CS_PORT_BIT;
  dev->lcd_en = xBSP430hplLookupPORT(BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_PERIPH_HANDLE);
  dev->lcd_en_bit = BSP430_PLATFORM_SHARPLCD_LCD_EN_PORT_BIT;
  dev->pwr_en = xBSP430hplLookupPORT(BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_PERIPH_HANDLE);
  dev->pwr_en_bit = BSP430_PLATFORM_SHARPLCD_PWR_EN_PORT_BIT;
  dev->spi = hBSP430serialOpenSPI(hBSP430serialLookup(BSP430_PLATFORM_SHARPLCD_SPI_PERIPH_HANDLE),
                                  BSP430_SHARPLCD_CTL0_INITIALIZER,
                                  UCSSEL__SMCLK,
                                  uiBSP430serialSMCLKPrescaler(BSP430_PLATFORM_SHARPLCD_SPI_BUS_HZ));
  dev->vcom_state_ = 0;
  dev->lines = BSP430_PLATFORM_SHARPLCD_ROWS;
  dev->columns = BSP430_PLATFORM_SHARPLCD_COLUMNS;
  dev->line_size = BSP430_PLATFORM_SHARPLCD_BYTES_PER_LINE;
  if ((NULL == dev->spi) || (NULL == dev->cs)) {
    return NULL;
  }
  return dev;
}

int iBSP430sharplcdSetEnabled_ni (hBSP430sharplcd dev,
                                  int enablep)
{
  dev->cs->out &= ~dev->cs_bit;
  if (enablep) {
    dev->cs->dir |= dev->cs_bit;
    if (dev->pwr_en) {
      dev->pwr_en->out |= dev->pwr_en_bit;
      dev->pwr_en->dir |= dev->pwr_en_bit;
    }
    if (dev->lcd_en) {
      dev->lcd_en->out |= dev->lcd_en_bit;
      dev->lcd_en->dir |= dev->lcd_en_bit;
    }
    /* Allow 50us for power to rise.  Just a guess. */
    BSP430_CORE_DELAY_CYCLES((50 * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000UL);
  }
  /* Reset everything */
  iBSP430sharplcdClearDisplay_rh(dev);
  if (enablep) {
    /* Allow 60us for whatever that TCOM stuff is (two, each > 30 uS). */
    BSP430_CORE_DELAY_CYCLES((60 * BSP430_CLOCK_NOMINAL_MCLK_HZ) / 1000000UL);
  } else {
    dev->cs->dir &= ~dev->cs_bit;
    if (dev->lcd_en) {
      dev->lcd_en->out &= ~dev->lcd_en_bit;
      dev->lcd_en->dir &= ~dev->lcd_en_bit;
    }
    if (dev->pwr_en) {
      dev->pwr_en->out &= ~dev->pwr_en_bit;
      dev->pwr_en->dir &= ~dev->pwr_en_bit;
    }
  }
  return 0;
}

int
iBSP430sharplcdClearDisplay_rh (hBSP430sharplcd dev)
{
  uint8_t cmd[2];
  int rc;

  dev->vcom_state_ ^= BSP430_SHARPLCD_VCOM;
  cmd[0] = dev->vcom_state_ | BSP430_SHARPLCD_CLEAR_ALL;
  cmd[1] = 0;
  BSP430_SHARPLCD_CS_ASSERT(dev);
  do {
    rc = iBSP430spiTxRx_rh(dev->spi, cmd, sizeof(cmd), 0, NULL);
  } while (0);
  BSP430_SHARPLCD_CS_DEASSERT(dev);
  return rc;
}

int
iBSP430sharplcdRefreshDisplay_rh (hBSP430sharplcd dev)
{
  uint8_t cmd[2];
  int rc;

  dev->vcom_state_ ^= BSP430_SHARPLCD_VCOM;
  cmd[0] = dev->vcom_state_;
  cmd[1] = 0;
  BSP430_SHARPLCD_CS_ASSERT(dev);
  do {
    rc = iBSP430spiTxRx_rh(dev->spi, cmd, sizeof(cmd), 0, NULL);
  } while (0);
  BSP430_SHARPLCD_CS_DEASSERT(dev);
  return rc;
}

static BSP430_CORE_INLINE
uint8_t
reverse_bits (uint8_t i)
{
  uint8_t rv;
#if (BSP430_CORE_TOOLCHAIN_GCC - 0)
  /* MSP430 version */
  __asm__ __volatile__ ("rlc.b\t%1\n"
                        "\trrc.b\t%0\n"
                        "\trlc.b\t%1\n"
                        "\trrc.b\t%0\n"
                        "\trlc.b\t%1\n"
                        "\trrc.b\t%0\n"
                        "\trlc.b\t%1\n"
                        "\trrc.b\t%0\n"
                        "\trlc.b\t%1\n"
                        "\trrc.b\t%0\n"
                        "\trlc.b\t%1\n"
                        "\trrc.b\t%0\n"
                        "\trlc.b\t%1\n"
                        "\trrc.b\t%0\n"
                        "\trlc.b\t%1\n"
                        "\trrc.b\t%0\n"
                        : "=&r" (rv) /* Output must be register, is write-only and clobbered */
                        , "+r"(i) /* Input is register and is mutated */
                        : );
#else /* GCC */
  /* Portable version */
  rv = i;
  rv = ((rv & 0xaa) >> 1) | ((rv & 0x55) << 1);
  rv = ((rv & 0xcc) >> 2) | ((rv & 0x33) << 2);
  rv = ((rv & 0xf0) >> 4) | ((rv & 0x0f) << 4);
#endif /* GCC */
  return rv;
}

int
iBSP430sharplcdUpdateDisplayLines_rh (hBSP430sharplcd dev,
                                      int start_line,
                                      int num_lines,
                                      const uint8_t * line_data)
{
  int line = start_line;
  int end_line;
  uint8_t cmd[2];
  const uint8_t * dp = line_data;
  int rc = 1;

  if (line < 1) {
    return -1;
  }
  if (0 > num_lines) {
    /* From start line to end of device */
    end_line = dev->lines + 1 - line;
  } else {
    end_line = start_line + num_lines;
  }

  dev->vcom_state_ ^= BSP430_SHARPLCD_VCOM;
  cmd[0] = dev->vcom_state_ | BSP430_SHARPLCD_MODE_DYNAMIC;

  BSP430_SHARPLCD_CS_ASSERT(dev);
  while (line < end_line) {
    cmd[1] = reverse_bits(line);
    rc = iBSP430spiTxRx_rh(dev->spi, cmd, sizeof(cmd), 0, NULL);
    if (sizeof(cmd) != rc) {
      break;
    }
    /* Schedule 8 trailing zero bits for continued multiline */
    cmd[0] = 0;
    rc = iBSP430spiTxRx_rh(dev->spi, dp, dev->line_size, 0, NULL);
    if (dev->line_size != rc) {
      break;
    }
    dp += rc;
    ++line;
  }
  if (0 < rc) {
    /* Transmit 16 trailing zero bits to complete multiline */
    cmd[1] = 0;
    rc = iBSP430spiTxRx_rh(dev->spi, cmd, sizeof(cmd), 0, NULL);
  }
  BSP430_SHARPLCD_CS_DEASSERT(dev);
  return (0 > rc) ? -1 : 0;
}
