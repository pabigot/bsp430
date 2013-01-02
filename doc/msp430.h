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
 * provided in the <a
 * href="https://sourceforge.net/projects/mspgcc/files/msp430mcu/">msp430mcu
 * package</a> at version 20120716, or from the <a
 * href="http://www.ti.com/general/docs/lit/getliterature.tsp?baseLiteratureNumber=SLAU208&track=no">MSP430
 * 5xx/6xx Family Users Guide</a>.  The set of peripherals identified
 * as available modules in this example header is both incomplete and
 * generic: not all identified peripherals can be found in an MSP430
 * MCU.
 *
 * @warning Do not assume the values in this documentation are
 * correct.  The value for your MCU may be different, but the
 * documentation will still apply.
 *
 * @homepage http://github.com/pabigot/bsp430
 */
*
/* ============================================================================ */
/* Copyright 2012, Texas Instruments Incorporated                           */
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
#define LPM1_bits           (SCG0|CPUOFF)
/** Bits set in status register to enter LPM2 */
#define LPM2_bits           (SCG1|CPUOFF)
/** Bits set in status register to enter LPM3 */
#define LPM3_bits           (SCG1|SCG0|CPUOFF)
/** Bits set in status register to enter LPM4 */
#define LPM4_bits           (SCG1|SCG0|OSCOFF|CPUOFF)

/************************************************************
* CPU
************************************************************/
/** Definition to show that MCU has MSP430XV2 CPU
 *
 * This is the defining characteristic for 5xx/6xx family devices
 * (including FR5xx devices). */
#define __MSP430_HAS_MSP430XV2_CPU__

/* Make sure nobody can accidently reference these values */
#if defined(BSP430_DOXYGEN)
#define MC_0                (0x0000)  /**< Timer mode control: 0 - Stop */
#define MC_1                (0x0010)  /**< Timer mode control: 1 - Up to CCR0 */
#define MC_2                (0x0020)  /**< Timer mode control: 2 - Continuous up */
#define MC_3                (0x0030)  /**< Timer mode control: 3 - Up/Down */
#define ID_0                (0x0000)  /**< Timer input divider: 0 - /1 */
#define ID_1                (0x0040)  /**< Timer input divider: 1 - /2 */
#define ID_2                (0x0080)  /**< Timer input divider: 2 - /4 */
#define ID_3                (0x00C0)  /**< Timer input divider: 3 - /8 */
#define CCIS_0              (0x0000) /**< Capture input select: 0 - CCIxA */
#define CCIS_1              (0x1000) /**< Capture input select: 1 - CCIxB */
#define CCIS_2              (0x2000) /**< Capture input select: 2 - GND */
#define CCIS_3              (0x3000) /**< Capture input select: 3 - Vcc */
#define CM_0                (0x0000) /**< Capture mode: 0 - disabled */
#define CM_1                (0x4000) /**< Capture mode: 1 - pos. edge */
#define CM_2                (0x8000) /**< Capture mode: 1 - neg. edge */
#define CM_3                (0xC000) /**< Capture mode: 1 - both edges */

#define SCS                 (0x0800)  /**< Capture sychronize */
#define SCCI                (0x0400)  /**< Latched capture signal (read) */
#define CAP                 (0x0100)  /**< Capture mode: 1 /Compare mode : 0 */

#define SELA_0              (0x0000)   /**< ACLK Source Select 0 (XT1CLK) */
#define SELA_1              (0x0100)   /**< ACLK Source Select 1 (VLOCLK) */
#define SELA_2              (0x0200)   /**< ACLK Source Select 2 (REFOCLK?)*/
#define SELA_3              (0x0300)   /**< ACLK Source Select 3 (DCOCLK) */
#define SELA_4              (0x0400)   /**< ACLK Source Select 4 (DCOCLKDIV?) */
#define SELA_5              (0x0500)   /**< ACLK Source Select 5 (XT2CLK?) */
#define SELA_6              (0x0600)   /**< ACLK Source Select 6 */
#define SELA_7              (0x0700)   /**< ACLK Source Select 7 */

#define LFXT1S_0            (0x00)   /**< Mode 0 for LFXT1 : Normal operation (32 KiHz watch) */
#define LFXT1S_1            (0x10)   /**< Mode 1 for LFXT1 : Reserved */
#define LFXT1S_2            (0x20)   /**< Mode 2 for LFXT1 : VLO */
#define LFXT1S_3            (0x30)   /**< Mode 3 for LFXT1 : Digital input signal */


#define UCPEN               (0x80)    /**< UART ctl0 : Async. Mode: Parity enable */
#define UCCKPH              (0x80)    /**< SPI ctl0 : Sync. Mode: Clock Phase */
#define UCA10               (0x80)    /**< I2C ctl0 : 10-bit Address Mode */
#define UCPAR               (0x40)    /**< UART ctl0 : Async. Mode: Parity     0:odd / 1:even */
#define UCCKPL              (0x40)    /**< SPI ctl0 : Sync. Mode: Clock Polarity */
#define UCSLA10             (0x40)    /**< I2C ctl0 :  10-bit Slave Address Mode */
#define UCMSB               (0x20)    /**< UART/SPI ctl0 : Async. Mode: MSB first  0:LSB / 1:MSB */
#define UCMM                (0x20)    /**< I2C ctl0 :  Multi-Master Environment */
#define UC7BIT              (0x10)    /**< UART/SPI ctl0 : Async. Mode: Data Bits  0:8-bits / 1:7-bits */
#define UCSPB               (0x08)    /**< UART ctl0 : Async. Mode: Stop Bits  0:one / 1: two */
#define UCMST               (0x08)    /**< SPI ctl0 : Sync. Mode: Master Select */

#define UCSYNC              (0x01)    /**< ctl0 : Sync-Mode  0:UART-Mode / 1:SPI-Mode */

// I2C-Mode Bits
//#define res               (0x10)    /* reserved */
#define UCMODE_0            (0x00)    /**< ctl0 : Sync. Mode: USCI Mode: 0 (3-pin SPI) */
#define UCMODE_1            (0x02)    /**< ctl0 : Sync. Mode: USCI Mode: 1 (4-pin SPI, m/s if STE) */
#define UCMODE_2            (0x04)    /**< ctl0 : Sync. Mode: USCI Mode: 2 (4-pin SPI, m/s if !STE) */
#define UCMODE_3            (0x06)    /**< ctl0 : Sync. Mode: USCI Mode: 3 (I2C)*/

// UART-Mode Bits
#define UCRXEIE             (0x20)    /**< UART ctl1 : RX Error interrupt enable */
#define UCBRKIE             (0x10)    /**< UART ctl1 : Break interrupt enable */
#define UCTR                (0x10)    /**< I2C ctl1 : Transmit/Receive Select/Flag */
#define UCDORM              (0x08)    /**< UART ctl1 : Dormant (Sleep) Mode */
#define UCTXNACK            (0x08)    /**< I2C ctl1 : Transmit NACK */
#define UCTXADDR            (0x04)    /**< UART ctl1 : Send next Data as Address */
#define UCTXSTP             (0x04)    /**< I2C ctl1 : Transmit STOP */
#define UCTXBRK             (0x02)    /**< UART ctl1 : Send next Data as Break */
#define UCTXSTT             (0x02)    /**< I2C ctl1 : Transmit START */

#define UCSWRST             (0x01)    /**< ctl1 : USCI Software Reset */

#define UCSSEL_0            (0x00)    /**< ctl1 : USCI 0 Clock Source: 0 (NA, UCLK) */
#define UCSSEL_1            (0x40)    /**< ctl1 : USCI 0 Clock Source: 1 (ACLK) */
#define UCSSEL_2            (0x80)    /**< ctl1 : USCI 0 Clock Source: 2 (SMCLK) */
#define UCSSEL_3            (0xC0)    /**< ctl1 : USCI 0 Clock Source: 3 (SMCLK) */

/* PMMCTL0 Control Bits */

#define PMMCOREV_0          (0x0000)  /**< PMM Core Voltage 0 (1.35V) */
#define PMMCOREV_1          (0x0001)  /**< PMM Core Voltage 1 (1.55V) */
#define PMMCOREV_2          (0x0002)  /**< PMM Core Voltage 2 (1.75V) */
#define PMMCOREV_3          (0x0003)  /**< PMM Core Voltage 3 (1.85V) */

/* TLV-related offsets and constants */

#define TLV_START             (0x1A08)    /**< Start Address of the TLV structure (EXCLUDES HEADER) */
#define TLV_END               (0x1AFF)    /**< End Address of the TLV structure (INCLUSIVE) */

#define TLV_LDTAG             (0x01)      /**<  Legacy descriptor (1xx, 2xx, 4xx families) */
#define TLV_PDTAG             (0x02)      /**<  Peripheral discovery descriptor */
#define TLV_BLANK             (0x05)      /**<  Blank descriptor */
#define TLV_DIERECORD         (0x08)      /**<  Die Record  */
#define TLV_ADCCAL            (0x11)      /**<  ADC12 calibration */
#define TLV_ADC12CAL          (0x11)      /**<  ADC12 calibration */
#define TLV_ADC10CAL          (0x13)      /**<  ADC10 calibration */
#define TLV_REFCAL            (0x12)      /**<  REF calibration */
#define TLV_TAGEXT            (0xFE)      /**<  Tag extender */

#define TAG_DCO_30             (0x01)    /**< (2xx) Tag for DCO30  Calibration Data */
#define TAG_ADC10_1            (0x08)    /**< (2xx) Tag for ADC10_1 Calibration Data */
#define TAG_ADC12_1            (0x08)    /**< (2xx) Tag for ADC12_1 Calibration Data */
#define TAG_EMPTY              (0xFE)    /**< (2xx) Tag for Empty Data Field in Calibration Data */

/* FLASH-related information */
#define LOCKA               (0x0040)  /**< Segment A Lock bit: read = 1 - Segment is locked (read only) */
#define LOCKINFO            (0x0080)  /**< Lock INFO Memory bit: read = 1 - Segment is locked (read only) */

/* SYSRSTIV aggregated candidate names */
#define SYSRSTIV_BOR       (0x0002)    /**< SYSRSTIV : BOR */
#define SYSRSTIV_RSTNMI    (0x0004)    /**< SYSRSTIV : RST/NMI */
#define SYSRSTIV_SVMBOR    (0x0004)    /**< SYSRSTIV : SVMBOR */
#define SYSRSTIV_DOBOR     (0x0006)    /**< SYSRSTIV : Do BOR */
#define SYSRSTIV_RSTNMI    (0x0006)    /**< SYSRSTIV : RST/NMI */
#define SYSRSTIV_DOBOR     (0x0008)    /**< SYSRSTIV : Do BOR */
#define SYSRSTIV_LPM5WU    (0x0008)    /**< SYSRSTIV : Port LPM5 Wake Up */
#define SYSRSTIV_SECYV     (0x000A)    /**< SYSRSTIV : Security violation */
#define SYSRSTIV_DOPOR     (0x000C)    /**< SYSRSTIV : Do POR */
#define SYSRSTIV_SVSL      (0x000C)    /**< SYSRSTIV : SVSL */
#define SYSRSTIV_SVSLIFG   (0x000C)    /**< SYSRSTIV : SVSLIFG */
#define SYSRSTIV_SVSH      (0x000E)    /**< SYSRSTIV : SVSH */
#define SYSRSTIV_SVSHIFG   (0x000E)    /**< SYSRSTIV : SVSHIFG */
#define SYSRSTIV_WDTTO     (0x000E)    /**< SYSRSTIV : WDT Time out */
#define SYSRSTIV_SVML_OVP  (0x0010)    /**< SYSRSTIV : SVML_OVP */
#define SYSRSTIV_WDTKEY    (0x0010)    /**< SYSRSTIV : WDTKEY violation */
#define SYSRSTIV_CCSKEY    (0x0012)    /**< SYSRSTIV : CCS Key violation */
#define SYSRSTIV_SVMH_OVP  (0x0012)    /**< SYSRSTIV : SVMH_OVP */
#define SYSRSTIV_DOPOR     (0x0014)    /**< SYSRSTIV : Do POR */
#define SYSRSTIV_PMMKEY    (0x0014)    /**< SYSRSTIV : PMMKEY violation */
#define SYSRSTIV_WDTTO     (0x0016)    /**< SYSRSTIV : WDT Time out */
#define SYSRSTIV_PERF      (0x0016)    /**< SYSRSTIV : peripheral/config area fetch */
#define SYSRSTIV_WDTKEY    (0x0018)    /**< SYSRSTIV : WDTKEY violation */
#define SYSRSTIV_FRCTLPW   (0x001A)    /**< SYSRSTIV : FRAM Key violation */
#define SYSRSTIV_KEYV      (0x001A)    /**< SYSRSTIV : Flash Key violation */
#define SYSRSTIV_FLLUL     (0x001C)    /**< SYSRSTIV : FLL unlock */
#define SYSRSTIV_DBDIFG    (0x001C)    /**< SYSRSTIV : FRAM Double bit Error */
#define SYSRSTIV_UBDIFG    (0x001C)    /**< SYSRSTIV : FRAM Uncorrectable bit Error */
#define SYSRSTIV_PLLUL     (0x001C)    /**< SYSRSTIV : PLL unlock */
#define SYSRSTIV_PERF      (0x001E)    /**< SYSRSTIV : peripheral/config area fetch */
#define SYSRSTIV_PSSKEY    (0x0020)    /**< SYSRSTIV : Legacy: PMMKEY violation */
#define SYSRSTIV_PMMPW     (0x0020)    /**< SYSRSTIV : PMM Password violation */
#define SYSRSTIV_PMMKEY    (0x0020)    /**< SYSRSTIV : PMMKEY violation */
#define SYSRSTIV_MPUPW     (0x0022)    /**< SYSRSTIV : MPU Password violation */
#define SYSRSTIV_MPUKEY    (0x0022)    /**< SYSRSTIV : MPUKEY violation */
#define SYSRSTIV_CSPW      (0x0024)    /**< SYSRSTIV : CS Password violation */
#define SYSRSTIV_CSKEY     (0x0024)    /**< SYSRSTIV : CSKEY violation */
#define SYSRSTIV_MPUSEGIIFG (0x0026)   /**< SYSRSTIV : MPUSEGIIFG violation */
#define SYSRSTIV_MPUSEGPIFG (0x0026)   /**< SYSRSTIV : MPUSEGPIFG violation */
#define SYSRSTIV_MPUSEG1IFG (0x0028)   /**< SYSRSTIV : MPUSEG1IFG violation */
#define SYSRSTIV_MPUSEG2IFG (0x002A)   /**< SYSRSTIV : MPUSEG2IFG violation */
#define SYSRSTIV_MPUSEG3IFG (0x002C)   /**< SYSRSTIV : MPUSEG3IFG violation */

#define SELREF_0            (0x0000)    /**< UCS : FLL Reference Clock Select 0 */
#define SELREF_1            (0x0010)    /**< UCS : FLL Reference Clock Select 1 */
#define SELREF_2            (0x0020)    /**< UCS : FLL Reference Clock Select 2 */
#define SELREF_3            (0x0030)    /**< UCS : FLL Reference Clock Select 3 */
#define SELREF_4            (0x0040)    /**< UCS : FLL Reference Clock Select 4 */
#define SELREF_5            (0x0050)    /**< UCS : FLL Reference Clock Select 5 */
#define SELREF_6            (0x0060)    /**< UCS : FLL Reference Clock Select 6 */
#define SELREF_7            (0x0070)    /**< UCS : FLL Reference Clock Select 7 */
#define SELREF__XT1CLK      (0x0000)    /**< UCS : Multiply Selected Loop Freq. By XT1CLK */
#define SELREF__REFOCLK     (0x0020)    /**< UCS : Multiply Selected Loop Freq. By REFOCLK */
#define SELREF__XT2CLK      (0x0050)    /**< UCS : Multiply Selected Loop Freq. By XT2CLK */

#endif /* BSP430_DOXYGEN */

/** @cond DOXYGEN_EXCLUDE */
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
/** @endcond */
