/** @file
 * @brief Stripped <msp430.h> header supporting Doxygen links
 *
 * This file is a fake <msp430.h> header modified so that constants
 * defined by the header are documented for reference from BSP430
 * documentation.  You do not include this one; you include <msp430.h>
 * and the compilation environment resolves this to the one
 * appropriate for your target MCU based on toolchain-specific
 * configuration.  For mspgcc, that is the @c -mmcu= target option.
 *
 * Documentation text is taken from the <msp430xgeneric.h> header
 * provided in the msp430mcu package at version 20120716, or from
 * slau208j (MSP430 5xx/6xx Family Users Guide).  The set of
 * peripherals identified as available modules in this example header
 * is both incomplete and generic: not all identified peripherals can
 * be found in an MSP430 MCU.
 *
 * @author Peter A. Bigot <bigotp@acm.org>
 * @date 2012
 * @homepage http://github.com/pabigot/freertos-mspgcc
 */
*
/* ============================================================================ */
/* Copyright (c) 2012, Texas Instruments Incorporated                           */
/*  All rights reserved.                                                        */
/*                                                                              */
/*  Redistribution and use in source and binary forms, with or without          */
/*  modification, are permitted provided that the following conditions          */
/*  are met:                                                                    */
/*                                                                              */
/*  *  Redistributions of source code must retain the above copyright           */
/*     notice, this list of conditions and the following disclaimer.            */
/*                                                                              */
/*  *  Redistributions in binary form must reproduce the above copyright        */
/*     notice, this list of conditions and the following disclaimer in the      */
/*     documentation and/or other materials provided with the distribution.     */
/*                                                                              */
/*  *  Neither the name of Texas Instruments Incorporated nor the names of      */
/*     its contributors may be used to endorse or promote products derived      */
/*     from this software without specific prior written permission.            */
/*                                                                              */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" */
/*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,       */
/*  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR      */
/*  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR            */
/*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,       */
/*  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,         */
/*  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; */
/*  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,    */
/*  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR     */
/*  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,              */
/*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                          */
/* ============================================================================ */

/********************************************************************
*
* Standard register and bit definitions for the Texas Instruments
* MSP430 microcontroller.
*
* This file supports assembler and C development for
* msp430XGENERIC device.
*
* Texas Instruments, Version 1.0 draft
*
* Rev. 1.0, Setup
*
*
********************************************************************/

/************************************************************
* STANDARD BITS
************************************************************/

#define BIT0                (0x0001) /**< Constant for bit 0 (=least significant bit) */
#define BIT1                (0x0002) /**< Constant for bit 1 (bit 0 is least significant bit) */
#define BIT2                (0x0004) /**< Constant for bit 2 (bit 0 is least significant bit) */
#define BIT3                (0x0008) /**< Constant for bit 3 (bit 0 is least significant bit) */
#define BIT4                (0x0010) /**< Constant for bit 4 (bit 0 is least significant bit) */
#define BIT5                (0x0020) /**< Constant for bit 5 (bit 0 is least significant bit) */
#define BIT6                (0x0040) /**< Constant for bit 6 (bit 0 is least significant bit) */
#define BIT7                (0x0080) /**< Constant for bit 7 (bit 0 is least significant bit) */
#define BIT8                (0x0100) /**< Constant for bit 8 (bit 0 is least significant bit) */
#define BIT9                (0x0200) /**< Constant for bit 9 (bit 0 is least significant bit) */
#define BITA                (0x0400) /**< Constant for bit 10 (bit 0 is least significant bit) */
#define BITB                (0x0800) /**< Constant for bit 11 (bit 0 is least significant bit) */
#define BITC                (0x1000) /**< Constant for bit 12 (bit 0 is least significant bit) */
#define BITD                (0x2000) /**< Constant for bit 13 (bit 0 is least significant bit) */
#define BITE                (0x4000) /**< Constant for bit 14 (bit 0 is least significant bit) */
#define BITF                (0x8000) /**< Constant for bit 15 (bit 0 is least significant bit) */

/************************************************************
* STATUS REGISTER BITS
************************************************************/

/** Carry. This bit is set when the result of an operation produced a
 * carry and cleared when no carry occurred. */
#define C                   (0x0001)
/** Zero. This bit is set when the result of an operation is 0 and
 * cleared when the result is not 0. */
#define Z                   (0x0002)
/** Negative. This bit is set when the result of an operation is
 * negative and cleared when the result is positive. */
#define N                   (0x0004)
/** Overflow. This bit is set when the result of an arithmetic
 * operation overflows the signed-variable range. */
#define V                   (0x0100)
/** General interrupt enable. This bit, when set, enables maskable
 * interrupts. When reset, all maskable interrupts are disabled. */
#define GIE                 (0x0008)
/** CPU off. This bit, when set, turns off the CPU. */
#define CPUOFF              (0x0010)
/** Oscillator off. This bit, when set, turns off the LFXT1 crystal
 * oscillator when LFXT1CLK is not used for MCLK or SMCLK. */
#define OSCOFF              (0x0020)
/** System clock generator 0. This bit may be used to enable/disable
 * functions in the clock system depending on the device family; for
 * example, FLL disable/enable. */
#define SCG0                (0x0040)
/** System clock generator 1. This bit may be to enable/disable
 * functions in the clock system depending on the device family; for
 * example, DCO bias enable/disable. */
#define SCG1                (0x0080)

/* Low Power Modes coded with Bits 4-7 in SR */

/** Bits set in status register to enter LPM0 */
#define LPM0_bits           (CPUOFF)
/** Bits set in status register to enter LPM1 */
#define LPM1_bits           (SCG0+CPUOFF)
/** Bits set in status register to enter LPM2 */
#define LPM2_bits           (SCG1+CPUOFF)
/** Bits set in status register to enter LPM3 */
#define LPM3_bits           (SCG1+SCG0+CPUOFF)
/** Bits set in status register to enter LPM4 */
#define LPM4_bits           (SCG1+SCG0+OSCOFF+CPUOFF)

/************************************************************
* CPU
************************************************************/
/** Definition to show that MCU has MSP430XV2 CPU
 *
 * This is the defining characteristic for 5xx/6xx family devices
 * (including FR5xx devices). */
#define __MSP430_HAS_MSP430XV2_CPU__

/************************************************************
* ADC10_A
************************************************************/
#define  __MSP430_HAS_ADC10_A__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_ADC10_B__        /**< Definition to show that Module is available */

/************************************************************
* ADC12 PLUS
************************************************************/
#define  __MSP430_HAS_ADC12_PLUS__     /**< Definition to show that Module is available */
/************************************************************
* ADC12_B
************************************************************/
#define  __MSP430_HAS_ADC12_B__         /**< Definition to show that Module is available */
/************************************************************
* AES256 Accelerator
************************************************************/
#define  __MSP430_HAS_AES256__          /**< Definition to show that Module is available */
/*************************************************************
* Backup RAM Module
*************************************************************/
#define  __MSP430_HAS_BACKUP_RAM__      /**< Definition to show that Module is available */
/*************************************************************
* Battery Charger Module
*************************************************************/
#define  __MSP430_HAS_BATTERY_CHARGER__   /**< Definition to show that Module is available */
/************************************************************
* CapSence_IO 0
************************************************************/
#define  __MSP430_HAS_CAP_SENSE_IO_0__ /**< Definition to show that Module is available */
/************************************************************
* CapSence_IO 1
************************************************************/
#define  __MSP430_HAS_CAP_SENSE_IO_1__ /**< Definition to show that Module is available */
/************************************************************
* Comparator B
************************************************************/
#define  __MSP430_HAS_COMPB__          /**< Definition to show that Module is available */
/************************************************************
* Comparator D
************************************************************/
#define  __MSP430_HAS_COMPD__          /**< Definition to show that Module is available */
/*************************************************************
* CRC Module
*************************************************************/
#define  __MSP430_HAS_CRC__            /**< Definition to show that Module is available */
/************************************************************
* CLOCK SYSTEM
************************************************************/
#define  __MSP430_HAS_CS__              /**< Definition to show that Module is available */
/************************************************************
* CLOCK SYSTEM
************************************************************/
#define  __MSP430_HAS_CS_A__            /**< Definition to show that Module is available */
/************************************************************
* DAC12
************************************************************/
#define  __MSP430_HAS_DAC12_2__          /**< Definition to show that Module is available */
/************************************************************
* DMA_X
************************************************************/
#define  __MSP430_HAS_DMAX_3__           /**< Definition to show that Module is available */
/************************************************************
* DMA_X
************************************************************/
#define  __MSP430_HAS_DMAX_6__           /**< Definition to show that Module is available */
/*************************************************************
* Flash Memory
*************************************************************/
#define  __MSP430_HAS_FLASH__         /**< Definition to show that Module is available */
/*************************************************************
* FRAM Memory
*************************************************************/
#define  __MSP430_HAS_FRAM_FR5XX__     /**< Definition to show that Module is available */
/*************************************************************
* FRAM Memory
*************************************************************/
#define  __MSP430_HAS_FRAM__           /**< Definition to show that Module is available */
/************************************************************
* Memory Protection Unit
************************************************************/
#define  __MSP430_HAS_MPU__             /**< Definition to show that Module is available */
/************************************************************
* Memory Protection Unit A
************************************************************/
#define  __MSP430_HAS_MPU_A__           /**< Definition to show that Module is available */
/************************************************************
* LCD_B
************************************************************/
#define  __MSP430_HAS_LCD_B__          /**< Definition to show that Module is available */
/************************************************************
* HARDWARE MULTIPLIER 32Bit
************************************************************/
#define  __MSP430_HAS_MPY32__          /**< Definition to show that Module is available */
/************************************************************
* DIGITAL I/O Port1/2 Pull up / Pull down Resistors
************************************************************/
#define  __MSP430_HAS_PORT1_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORT2_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORTA_R__        /**< Definition to show that Module is available */
/************************************************************
* DIGITAL I/O Port3/4 Pull up / Pull down Resistors
************************************************************/
#define  __MSP430_HAS_PORT3_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORT4_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORTB_R__        /**< Definition to show that Module is available */
/************************************************************
* DIGITAL I/O Port5/6 Pull up / Pull down Resistors
************************************************************/
#define  __MSP430_HAS_PORT5_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORT6_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORTC_R__        /**< Definition to show that Module is available */
/************************************************************
* DIGITAL I/O Port7/8 Pull up / Pull down Resistors
************************************************************/
#define  __MSP430_HAS_PORT7_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORT8_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORTD_R__        /**< Definition to show that Module is available */
/************************************************************
* DIGITAL I/O Port9/10 Pull up / Pull down Resistors
************************************************************/
#define  __MSP430_HAS_PORT9_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORT10_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORTE_R__        /**< Definition to show that Module is available */
/************************************************************
* DIGITAL I/O Port11 Pull up / Pull down Resistors
************************************************************/
#define  __MSP430_HAS_PORT11_R__        /**< Definition to show that Module is available */
#define  __MSP430_HAS_PORTF_R__        /**< Definition to show that Module is available */
/************************************************************
* DIGITAL I/O PortJ Pull up / Pull down Resistors
************************************************************/
#define  __MSP430_HAS_PORTJ_R__        /**< Definition to show that Module is available */
/************************************************************
* PORT MAPPING CONTROLLER
************************************************************/
#define  __MSP430_HAS_PORT_MAPPING__   /**< Definition to show that Module is available */
/************************************************************
* PORT 2 MAPPING CONTROLLER
************************************************************/
#define  __MSP430_HAS_PORT2_MAPPING__   /**< Definition to show that Module is available */
/************************************************************
* PMM - Power Management System
************************************************************/
#define  __MSP430_HAS_PMM__            /**< Definition to show that Module is available */
/************************************************************
* PMM - Power Management System
************************************************************/
#define  __MSP430_HAS_PMM_FR5xx__      /**< Definition to show that Module is available */
/************************************************************
* PMM - Power Management System for FRAM
************************************************************/
#define  __MSP430_HAS_PMM_FRAM__       /**< Definition to show that Module is available */
/************************************************************
* Port U
************************************************************/
#define  __MSP430_HAS_PU__             /**< Definition to show that Module is available */
/*************************************************************
* RAM Control Module
*************************************************************/
#define  __MSP430_HAS_RC__             /**< Definition to show that Module is available */
/************************************************************
* Shared Reference
************************************************************/
#define  __MSP430_HAS_REF__          /**< Definition to show that Module is available */
/************************************************************
* Shared Reference
************************************************************/
#define  __MSP430_HAS_REF_A__           /**< Definition to show that Module is available */
/************************************************************
* Real Time Clock
************************************************************/
#define  __MSP430_HAS_RTC__            /**< Definition to show that Module is available */
/************************************************************
* Real Time Clock
************************************************************/
#define  __MSP430_HAS_RTC_B__          /**< Definition to show that Module is available */
/************************************************************
* Real Time Clock
************************************************************/
#define  __MSP430_HAS_RTC_C__          /**< Definition to show that Module is available */
/************************************************************
* Real Time Clock
************************************************************/
#define  __MSP430_HAS_RTC_CE__         /**< Definition to show that Module is available */
/************************************************************
* SFR - Special Function Register Module
************************************************************/
#define  __MSP430_HAS_SFR__            /**< Definition to show that Module is available */
/************************************************************
* SYS - System Module
************************************************************/
#define  __MSP430_HAS_SYS__            /**< Definition to show that Module is available */
/************************************************************
* Timerx_A7
************************************************************/
#define  __MSP430_HAS_TxA7__            /**< Definition to show that Module is available */
/************************************************************
* Timerx_B3
************************************************************/
#define  __MSP430_HAS_TxB7__            /**< Definition to show that Module is available */
/************************************************************
* Timerx_D7
************************************************************/
#define  __MSP430_HAS_TxD7__            /**< Definition to show that Module is available */
/************************************************************
* UNIFIED CLOCK SYSTEM
************************************************************/
#define  __MSP430_HAS_UCS__            /**< Definition to show that Module is available */
/************************************************************
* USB
************************************************************/
#define  __MSP430_HAS_USB__            /**< Definition to show that Module is available */
/************************************************************
* USCI Ax
************************************************************/
#define  __MSP430_HAS_USCI_Ax__       /**< Definition to show that Module is available */
/************************************************************
* USCI Ax
************************************************************/
#define  __MSP430_HAS_EUSCI_Ax__      /**< Definition to show that Module is available */
/************************************************************
* WATCHDOG TIMER A
************************************************************/
#define  __MSP430_HAS_WDT_A__          /**< Definition to show that Module is available */
/************************************************************
* TLV Descriptors
************************************************************/
#define __MSP430_HAS_TLV__              /**< Definition to show that Module is available */
