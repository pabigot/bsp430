/** @file
 *
 * Declarations for port-specific extensions to the serial interface.
 * 
 * @author Peter A. Bigot <bigotp@acm.org>
 * @copyright @link http://www.opensource.org/licenses/BSD-3-Clause BSD 3-Clause @endlink
 */

#ifndef FREERTOS_MSPGCC_BSP430_PORTSERIAL_H
#define FREERTOS_MSPGCC_BSP430_PORTSERIAL_H

#include "serial.h"

/** Assign MCU-specific pins to the given port.
 *
 * This is normally invoked in the platform initialization, and must
 * be invoked before attempting to allocate or use a given serial
 * port.
 *
 * @param ePort the port to which the pin assignment applies.  E.g.,
 * serCOM1 (the first eCOMPort) is likely to refer to USCI_A0.
 *
 * @param pcPxSEL a reference to the PxSEL register for the digital
 * i/o port that controls the pins used for transmit and receive.
 * Note that some MCUs have multiple PxSEL registers; identification
 * and configuration of those is the responsibility of the
 * implementation: the caller's responsibility is to identify the
 * first one.  E.g., P5SEL or P1SEL0.
 *
 * @param ucBitTX the bit (mask, not index) identifying the pin used
 * for transmission.  E.g., BIT6.
 *
 * @aram ucBitRX the bit (mask, not index) identifying the pin used
 * for reception.  E.g., BIT7.
 *
 * @return pdSUCCESS if the port could be located; pdFAIL if something
 * went wrong.  If the pins could not be configured, the port cannot
 * be used. */
signed portBASE_TYPE
portSerialAssignPins (eCOMPort ePort,
					  int (* configurator) (int));

#endif /* FREERTOS_MSPGCC_BSP430_PORTSERIAL_H */
