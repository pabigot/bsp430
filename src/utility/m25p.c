/* Copyright 2013, Peter A. Bigot
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
#include <bsp430/utility/m25p.h>
#include <stdlib.h>
#include <string.h>

#include <bsp430/utility/console.h>

hBSP430m25p
hBSP430m25pInitialize (hBSP430m25p dev,
                       unsigned char ctl0_byte,
                       unsigned char ctl1_byte,
                       unsigned int prescaler)
{
  if ((NULL == dev) || (NULL == dev->spi) || (NULL == dev->csn_port)) {
    return NULL;
  }
  if (NULL == hBSP430serialOpenSPI(dev->spi, ctl0_byte, ctl1_byte, prescaler)) {
    return NULL;
  }
  /* Set the signal before making the port an output */
  BSP430_M25P_CS_DEASSERT(dev);
  dev->csn_port->dir |= dev->csn_bit;
  if (NULL != dev->rstn_port) {
    BSP430_M25P_RESET_SET(dev);
    dev->rstn_port->dir |= dev->rstn_bit;
  }
  return dev;
}

int
iBSP430m25pStatus_rh (hBSP430m25p dev)
{
  const uint8_t cmd = BSP430_M25P_CMD_RDSR;
  uint8_t res[2];
  int rc;

  BSP430_M25P_CS_ASSERT(dev);
  rc = iBSP430spiTxRx_rh(dev->spi, &cmd, sizeof(cmd), sizeof(res[1]), res);
  BSP430_M25P_CS_DEASSERT(dev);
  if (sizeof(cmd) + sizeof(res[1]) == rc) {
    return res[1];
  }
  return -1;
}

int
iBSP430m25pStrobeCommand_rh (hBSP430m25p dev,
                             uint8_t cmd)
{
  int rc = -1;

  BSP430_M25P_CS_ASSERT(dev);
  rc = iBSP430spiTxRx_rh(dev->spi, &cmd, sizeof(cmd), 0, NULL);
  if (sizeof(cmd) == rc) {
    rc = 0;
  }
  BSP430_M25P_CS_DEASSERT(dev);
  return rc;
}

int
iBSP430m25pStrobeAddressCommand_rh (hBSP430m25p dev,
                                    uint8_t cmd,
                                    unsigned long addr)
{
  int rv = iBSP430m25pInitiateAddressCommand_rh(dev, cmd, addr);
  /* Safe enough to deassert regardless of success/failure */
  BSP430_M25P_CS_DEASSERT(dev);
  return rv;
}

int
iBSP430m25pInitiateCommand_rh (hBSP430m25p dev,
                               uint8_t cmd)
{
  int rc;

  BSP430_M25P_CS_ASSERT(dev);
  rc = iBSP430spiTxRx_rh(dev->spi, &cmd, sizeof(cmd), 0, NULL);
  if (sizeof(cmd) == rc) {
    return 0;
  }
  BSP430_M25P_CS_DEASSERT(dev);
  return -1;
}

int
iBSP430m25pInitiateAddressCommand_rh (hBSP430m25p dev,
                                      uint8_t cmd,
                                      unsigned long addr)
{
  uint8_t cmdb[5];              /* 1 cmd, 3 addr, optional 1 dummy */
  uint8_t * cbp;
  size_t len;
  int rc;

  cbp = cmdb;
  *cbp++ = cmd;
  *cbp++ = 0xFF & (addr >> 16);
  *cbp++ = 0xFF & (addr >> 8);
  *cbp++ = 0xFF & addr;
  if (BSP430_M25P_CMD_FAST_READ == cmd) {
    /* This command requires a dummy byte */
    *cbp++ = 0;
  }
  len = cbp - cmdb;
  BSP430_M25P_CS_ASSERT(dev);
  rc = iBSP430spiTxRx_rh(dev->spi, cmdb, len, 0, NULL);
  if (len == rc) {
    return 0;
  }
  BSP430_M25P_CS_DEASSERT(dev);
  return -1;
}

int
iBSP430m25pCompleteTxRx_rh (hBSP430m25p dev,
                            const uint8_t * tx_data,
                            size_t tx_len,
                            size_t rx_len,
                            uint8_t * rx_data)
{
  int rv;
  rv = iBSP430spiTxRx_rh(dev->spi, tx_data, tx_len, rx_len, rx_data);
  BSP430_M25P_CS_DEASSERT(dev);
  return rv;
}

