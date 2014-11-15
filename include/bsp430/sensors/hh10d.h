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

#ifndef BSP430_SENSORS_HH10D_H
#define BSP430_SENSORS_HH10D_H

/** @file
 *
 * @brief Interface to <a
 * href="http://www.hoperf.com/sensor/app/hh10d.htm">Hope RF HH10D
 * Humidity Sensor</a>.
 *
 * This device is a humidity-to-pulse converter, which represents
 * humidity as a linear function of the frequency, ranging from 5 kHz
 * to 10 kHz.  Each device is calibrated, with offset and sensitivity
 * constants accessed through I2C.
 *
 * To use this with BSP430 you will need an I2C device, a timer that
 * will use the HH10D @c FOUT as its clock signal, and a
 * capture/compare index on another timer to periodically read the
 * first clock and estimate frequency.  See @c examples/sensors/hh10d
 * for an example of how to use it.
 *
 * @warning Because the humidity estimate is based on frequency, an
 * accurate measurement of frequency is critical.  A wildly inaccurate
 * humidity estimate is likely when this sensor is used with a board
 * that does not have a provide an accurate clock.
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#include <bsp430/serial.h>
#include <bsp430/periph/timer.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** The 7-bit I2C slave address for the device.  This is not
 * configurable. */
#define BSP430_SENSORS_HH10D_I2C_ADDRESS 0x51

/** Bit set in sBSP430sensorsHH10Dstate::flags when
 * sBSP430sensorsHH10Dstate::last_capture has a valid value. */
#define BSP430_SENSORS_HH10D_FLAG_VALID_CAPTURE 0x01

/** Bit set in sBSP430sensorsHH10Dstate::flags when
 * sBSP430sensorsHH10Dstate::last_period_count has a valid value. */
#define BSP430_SENSORS_HH10D_FLAG_VALID_COUNT 0x02

/** Bit set in sBSP430sensorsHH10Dstate::flags to cause
 * iBSP430sensorsHH10DperiodicCallback_ni to include
 * #BSP430_HAL_ISR_CALLBACK_EXIT_LPM in its return value. */
#define BSP430_SENSORS_HH10D_FLAG_WAKE_ON_COUNT 0x04

/** Bit set in sBSP430sensorsHH10Dstate::flags to cause
 * iBSP430sensorsHH10DperiodicCallback_ni to automatically increment
 * the compare index by sBSP430sensorsHH10Dstate::interval_tck to
 * schedule the next callback. */
#define BSP430_SENSORS_HH10D_FLAG_AUTOSAMPLE 0x08

/** State used to interact with the HH10D sensor. */
typedef struct sBSP430sensorsHH10Dstate {
  /** The callback node used for periodic sampling of the @c FOUT
   * counter.  The sBSP430halISRIndexedChainNode::callback_ni field
   * should be set to */
  sBSP430halISRIndexedChainNode cb;

  /** Device-specific humidity calculation offset parameter by
   * iBSP430sensorsHH10DgetCalibration(). */
  unsigned int cal_offs;

  /** Device-specific humidity calculation sensitivity parameter by
   * iBSP430sensorsHH10DgetCalibration(). */
  unsigned int cal_sens;

  /** Reference to the timer that is counting device @c FOUT ticks. */
  volatile sBSP430hplTIMER * freq_timer;

  /** The interval between periodic captures.  Normal practice would
   * be to set this to the frequency of the controlling alarm timer,
   * so that captures are made at 1Hz.  See
   * #BSP430_SENSORS_HH10D_FLAG_AUTOSAMPLE. */
  unsigned int interval_tck;

  /** The value from #freq_timer at the last capture.  Invalid unless
   * #BSP430_SENSORS_HH10D_FLAG_VALID_CAPTURE is set in #flags. */
  unsigned int last_capture;

  /** The number of @c FOUT pulses within the last #interval_tck
   * period. Invalid unless #BSP430_SENSORS_HH10D_FLAG_VALID_COUNT is
   * set in #flags. */
  unsigned int last_period_count;

  /** Flags controlling behavior of the callback. */
  unsigned int flags;
} sBSP430sensorsHH10Dstate;

/** Handle for an HH10D state instance. */
typedef sBSP430sensorsHH10Dstate * hBSP430sensorsHH10Dstate;

/** A timer callback to handle HH10D periodic state updates.
 *
 * This callback should be configured for periodic invocation on a
 * timer.  It should be installed in the @link
 * sBSP430halISRIndexedChainNode::callback_ni cb.callback_ni @endlink
 * field of a #sBSP430sensorsHH10Dstate instance.  There is probably
 * no value in invoking it directly.
 *
 * @param cb a pointer to the sBSP430sensorsHH10Dstate::cb field of an
 * HH10D state instance.
 * @param context as with iBSP430halISRCallbackIndexed_ni()
 * @param idx as with iBSP430halISRCallbackIndexed_ni()
 * @return as with iBSP430halISRCallbackIndexed_ni()
 */
int iBSP430sensorsHH10DperiodicCallback_ni (const struct sBSP430halISRIndexedChainNode *cb,
                                            void *context,
                                            int idx);

/** Read the calibration data from an HH10D device and store it in the state structure.
 *
 * @note @li The caller must hold the @p i2c resource
 *
 * @note @li @p i2c may be in @link iBSP430serialSetReset_rh reset
 * mode @endlink on entry, in which case it will be put back into
 * reset mode before returning.  @p i2c must @b not be in @link
 * iBSP430serialSetHold_rh hold mode @endlink.
 *
 * @note @li The slave address of the @p i2c resource will be reconfigured by
 * this function.

 * @param i2c the I2C bus on which the HH10D device can be contacted.
 *
 * @param hh10d the state structure for the device.
 *
 * return 0 if the calibration constants could be read; otherwise a
 * negative error code.
 */
int iBSP430sensorsHH10DgetCalibration (hBSP430halSERIAL i2c,
                                       hBSP430sensorsHH10Dstate hh10d);

/** Convert the most recent HH10D sample into a humidity value.
 *
 * @param hh10d the HH10D state
 *
 * @param interval_s the number of seconds covered by each sampling
 * period.  Be sure that this value (and the
 * sBSP430sensorsHH10Dstate::interval_tck that produces it) does not
 * exceed the duration at which the counter may wrap (max rate 10 kHz
 * with 16-bit counter so a little over five seconds).
 *
 * @return -1 if there is no available data, otherwise the measured
 * relative humidity in parts-per-thousand.
 */
static BSP430_CORE_INLINE
int iBSP430sensorsHH10Dconvert_ppth_ni (hBSP430sensorsHH10Dstate hh10d,
                                        unsigned int interval_s)
{
  unsigned long ul;

  if (! hh10d) {
    return -1;
  }
  if (! (BSP430_SENSORS_HH10D_FLAG_VALID_COUNT & hh10d->flags)) {
    return -1;
  }
  ul = hh10d->last_period_count / interval_s;
  ul = hh10d->cal_offs - ul;
  ul *= 10UL * hh10d->cal_sens;
  return (unsigned int) (ul / 4096);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BSP430_SENSORS_HH10D_H */
