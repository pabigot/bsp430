#ifndef BSP430_PLATFORM_EXP430G2_H
#define BSP430_PLATFORM_EXP430G2_H

#include <bsp430/common/platform.h>

/** Block until the jumper between P4.0 and GND has been removed.
 *
 * The USB device used for the EXP430FR5739, like that for the
 * Launchpad, must not transmit any data while the driver is
 * initializing.  We need a way to hold off an installed application
 * from writing to the serial port until that's happened.  What we do
 * is, if P4.0 is grounded, block until it's released.  When plugging
 * in a device for the first time, place a jumper connect P4.0 to
 * ground, wait for the driver to load, then remove the jumper.
 *
 * @note vBSP430ledInit_ni() must have been invoked before this function
 * is called. */
void vBSP430platformSpinForJumper_ni (void);

#endif /* BSP430_PLATFORM_EXP430G2_H */
