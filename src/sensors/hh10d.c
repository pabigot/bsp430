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

#include <bsp430/platform.h>
#include <bsp430/serial.h>
#include <bsp430/sensors/hh10d.h>

int
iBSP430sensorsHH10DperiodicCallback_ni (const struct sBSP430halISRIndexedChainNode *cb,
                                        void *context,
                                        int idx)
{
  volatile sBSP430sensorsHH10Dstate * hh10d = (sBSP430sensorsHH10Dstate *)cb;
  unsigned int capture;
  hBSP430halTIMER timer = (hBSP430halTIMER)context;
  int rv = 0;

  /* Record the HH10D counter, schedule the next wakeup, then return
   * waking the MCU and inhibiting further interrupts when active. */
  capture = uiBSP430timerSafeCounterRead_ni(hh10d->freq_timer);
  if (hh10d->flags & BSP430_SENSORS_HH10D_FLAG_VALID_CAPTURE) {
    hh10d->last_period_count = capture - hh10d->last_capture;
    hh10d->flags |= BSP430_SENSORS_HH10D_FLAG_VALID_COUNT;
    if (hh10d->flags & BSP430_SENSORS_HH10D_FLAG_WAKE_ON_COUNT) {
      rv |= BSP430_HAL_ISR_CALLBACK_EXIT_LPM;
    }
  }
  hh10d->last_capture = capture;
  hh10d->flags |= BSP430_SENSORS_HH10D_FLAG_VALID_CAPTURE;
  if (hh10d->flags & BSP430_SENSORS_HH10D_FLAG_AUTOSAMPLE) {
    timer->hpl->ccr[idx] += hh10d->interval_tck;
  }
  return rv;
}

int
iBSP430sensorsHH10DgetCalibration (hBSP430halSERIAL i2c,
                                   hBSP430sensorsHH10Dstate hh10d)
{
  int rv = -1;
  int reset_mode;

  if (! (hh10d && i2c)) {
    return rv;
  }
  reset_mode = iBSP430serialSetReset_rh(i2c, -1);
  if (0 > reset_mode) {
    return rv;
  }
  do {
    int rc;
    uint8_t addr = 10;
    uint8_t data[4];

    rc = iBSP430i2cSetAddresses_rh(i2c, -1, BSP430_SENSORS_HH10D_I2C_ADDRESS);
    if (0 != rc) {
      break;
    }
    rc = iBSP430serialSetReset_rh(i2c, 0);
    if (0 > rc) {
      break;
    }
    rc = iBSP430i2cTxData_rh(i2c, &addr, sizeof(addr));
    if (sizeof(addr) != rc) {
      break;
    }
    rc = iBSP430i2cRxData_rh(i2c, data, sizeof(data));
    if (sizeof(data) != rc) {
      break;
    }
    hh10d->cal_sens = (data[0] << 8) | data[1];
    hh10d->cal_offs = (data[2] << 8) | data[3];
    rv = 0;
  } while (0);
  if (0 < reset_mode) {
    iBSP430serialSetReset_rh(i2c, reset_mode);
  }

  return rv;
}
