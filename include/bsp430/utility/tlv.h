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

/** @file
 *
 * @brief Basic support for MSP430 Tag-Length-Value device descriptors
 *
 * Two families of MSP430 use TLV descriptors for various purposes:
 *
 * @li The 2xx family provides calibration constants for various peripherals
 *
 * @li The 5xx/6xx/FR5xx family provides calibration constants as well
 * as information on which peripherals are available and the base
 * address of the corresponding peripheral register map
 *
 * The layout of the device descriptor table and the algorithm used
 * for checksum on them differ between these two implementations.  The
 * C preprocessor symbol #BSP430_TLV_IS_5XX may be used to identify
 * the supported implementation in user code.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 * @copyright Copyright 2012-2014, Peter A. Bigot.  Licensed under <a href="http://www.opensource.org/licenses/BSD-3-Clause">BSD-3-Clause</a>
 */

#ifndef BSP430_UTILITY_TLV_H
#define BSP430_UTILITY_TLV_H

#include <bsp430/core.h>

/** Define to a true value to indicate intent to use the TLV interface.
 *
 * @cppflag
 * @affects #BSP430_TLV
 * @defaulted
 */
#ifndef configBSP430_TLV
#define configBSP430_TLV 0
#endif /* configBSP430_TLV */

/** Indicate that the TLV interface is available on the platform.  The
 * value is true iff #configBSP430_TLV is true.
 *
 * This flag is defined only if #configBSP430_TLV is true.
 *
 * @cppflag
 * @dependency #configBSP430_TLV
 * @platformvalue */
#if defined(BSP430_DOXYGEN) || defined(configBSP430_TLV)
#define BSP430_TLV (configBSP430_TLV - 0)
#endif /* BSP430_TLV */

/** Fixed header for all TLV entries.  This is immediately followed by
    entry-specific data. */
typedef struct sBSP430tlvEntry {
  /** Tag identifying the structure being described */
  unsigned char tag;
  /** Length of the data portion of the structure (exclusive of this
   * header), in bytes */
  unsigned char len;
} sBSP430tlvEntry;

/** The complete device descriptor table including the header, as
 * supported on 2xx devices.  The address of the table is @c
 * 0x10C0. */
typedef struct sBSP430tlvTable2xx {
  /** 16-bit XOR CRC of data */
  unsigned int crc;
  union {
    /** Table access as a sequence of words */
    unsigned int words[1];
    /** First entry in the table */
    sBSP430tlvEntry entry;
  };
} sBSP430tlvTable2xx;

/** The complete device descriptor table including the header, as
 * supported on 5xx/6xx/FR5xx devices.  The address of
 * sBSP430tlvTable5::entry is available as #TLV_START in the MCU
 * header, and can be read from memory at address @c 0x0FF4. */
typedef struct sBSP430tlvTable5xx {
  /** Length of the table as an exponent of 2 and units of 32-bit
   * words.  Use 4 << info_len_lg2 to get the total length of the
   * table. */
  unsigned char info_len_lg2;
  /** Apparently the length of the region covered by the CRC, as an
   * exponent of 2, in units of 32-bit words, and rounded up. */
  unsigned char crc_len_lg2;
  /** The reversed-bit CRC-CCIT value over the information in the
   * table. */
  unsigned int crc;
  /** A device identifier of some sort, not documented by TI */
  unsigned int device_id;
  /** A firmware revision of some sort, not documented by TI */
  unsigned char firmware_revision;
  /** A hardware revision of some sort, not documented by TI */
  unsigned char hardware_revision;
  union {
    /** Table access as a sequence of words */
    unsigned int words[1];
    /** First entry in the table */
    sBSP430tlvEntry entry;
  };
} sBSP430tlvTable5xx;

/** Flag used to determine which variant of TLV structure is used.
 *
 * The functional presence symbol @c \__MSP430_HAS_TLV__ is missing
 * from certain headers that support TLV.  For the purposes of this
 * module, the standard BSP430 approach of using the MCU header
 * functional presence symbol @c \__MSP430_HAS_MSP430XV2_CPU__ is used
 * to identify MCUs that follow the 5xx/6xx/FR5xx interface; the 2xx
 * interface is assumed for all other MCUs.
 *
 * @cppflag */
#define BSP430_TLV_IS_5XX defined(__MSP430_HAS_MSP430XV2_CPU__)

/** Typedef for the family-specific TLV table structure for this MCU.
 * @platformdep
 */
#if defined(BSP430_DOXYGEN) || (BSP430_TLV_IS_5XX - 0)
typedef sBSP430tlvTable5xx sBSP430tlvTable;
#else /* BSP430_TLV_IS_5XX */
typedef sBSP430tlvTable2xx sBSP430tlvTable;
#endif /* BSP430_TLV_IS_5XX */

/** Return a pointer to the next entry in the table.
 *
 * @warning This routine does not account for the possibility of an
 * extended tag.  Extended tags are only used on 5xx/6xx device
 * descriptor tables, and are not adequately documented in the user's
 * guide. */
#define BSP430_TLV_NEXT_ENTRY(ep_) ((sBSP430tlvEntry *)(sizeof(sBSP430tlvEntry) + (ep_)->len + (unsigned char *)(ep_)))

/** Check whether a TLV entry corresponds to an ADC.
 *
 * On 5xx/6xx MCUs this may be either #TLV_ADC10CAL or #TLV_ADC12CAL.  No MCU has both.
 *
 * On 2xx MCUs, the tag value is not constant across the family for
 * either peripheral type, so an ADC is detected by a tag that is
 * neither #TAG_DCO_30 nor #TAG_EMPTY. */
#if defined(BSP430_DOXYGEN) || (BSP430_TLV_IS_5XX - 0)
#define BSP430_TLV_ENTRY_IS_ADC(ep_) ((TLV_ADC10CAL == (ep_)->tag) || (TLV_ADC12CAL == (ep_)->tag))
#else /* BSP430_TLV_IS_5XX */
#define BSP430_TLV_ENTRY_IS_ADC(ep_) ((TAG_DCO_30 != (ep_)->tag) && (TAG_EMPTY != (ep_)->tag))
#endif /* BSP430_TLV_IS_5XX */

#if defined(BSP430_DOXYGEN) || (BSP430_TLV_IS_5XX - 0)

/* Standard definitions when missing from header */
#ifndef TLV_LDTAG
#define TLV_LDTAG             (0x01)      /**<  Legacy descriptor (1xx, 2xx, 4xx families) */
#endif /* TLV_LDTAG */
#ifndef TLV_PDTAG
#define TLV_PDTAG             (0x02)      /**<  Peripheral discovery descriptor */
#endif /* TLV_PDTAG */
#ifndef TLV_BLANK
#define TLV_BLANK             (0x05)      /**<  Blank descriptor */
#endif /* TLV_BLANK */
#ifndef TLV_DIERECORD
#define TLV_DIERECORD         (0x08)      /**<  Die Record  */
#endif /* TLV_DIERECORD */
#ifndef TLV_ADC12CAL
#define TLV_ADC12CAL          (0x11)      /**<  ADC12 calibration */
#endif /* TLV_ADC12CAL */
#ifndef TLV_REFCAL
#define TLV_REFCAL            (0x12)      /**<  REF calibration */
#endif /* TLV_REFCAL */
#ifndef TLV_ADC10CAL
#define TLV_ADC10CAL          (0x13)      /**<  ADC10 calibration */
#endif /* TLV_ADC10CAL */
#ifndef TLV_TAGEXT
#define TLV_TAGEXT            (0xFE)      /**<  Tag extender */
#endif /* TLV_TAGEXT */

/** TLV entry for #TLV_DIERECORD on 5xx MCUs.
 *
 * This structure is not described in the users' guide, but can be
 * found in certain MCU data sheets, including the one for the
 * MSP430F5438a */
typedef struct sBSP430tlvDIERECORD {
  sBSP430tlvEntry hdr;          /**< Standard tag/len pair */
  unsigned long lot_id;         /**< Lot/Wafer ID */
  unsigned int die_x_position;  /**< Die X position */
  unsigned int die_y_position;  /**< Die Y position */
  unsigned int test_results;    /**< Test Results */
} sBSP430tlvDIERECORD;

/** TLV entry for #TLV_REFCAL on 5xx MCUs.
 *
 * These are the constants for ADC adjustment based on the internal
 * reference voltage.   See the family user guide for details. */
typedef struct sBSP430tlvREFCAL {
  sBSP430tlvEntry hdr;          /**< Standard tag/len pair */
  unsigned int cal_adc_15vref_factor; /**< Factor for 1.5V reference */
  unsigned int cal_adc_20vref_factor; /**< Factor for 2.0V reference */
  unsigned int cal_adc_25vref_factor; /**< Factor for 2.5V reference */
} sBSP430tlvREFCAL;

/** TLV entry for #TLV_ADCCAL, #TLV_ADC12CAL, and #TLV_ADC10CAL on 5xx MCUs
 *
 * These are ADC gain and offset calibration constants, as well as
 * constants for the internal temperature sensor.  See the family user
 * guide for details.
 *
 * @warning: On FR4xx/2xx ADC_10 variants the structure stops after 10
 * bytes, at the cal_adc_20t30 field, since that variant supports only
 * a 1.5V reference. */
typedef struct sBPS430tlvADCCAL {
  sBSP430tlvEntry hdr;          /**< Standard tag/len pair */
  unsigned int cal_adc_gain_factor; /**< ADC gain factor */
  unsigned int cal_adc_offset; /**< ADC gain offset */
  unsigned int cal_adc_15t30; /**< INCH_11 sample with 1.5V reference at 30Cel */
  unsigned int cal_adc_15t85; /**< INCH_11 sample with 1.5V reference at 85Cel */
  unsigned int cal_adc_20t30; /**< INCH_11 sample with 2.0V reference at 30Cel */
  unsigned int cal_adc_20t85; /**< INCH_11 sample with 2.0V reference at 85Cel */
  unsigned int cal_adc_25t30; /**< INCH_11 sample with 2.5V reference at 30Cel */
  unsigned int cal_adc_25t85; /**< INCH_11 sample with 2.5V reference at 85Cel */
} sBSP430tlvADCCAL;

#if 0
/* Documented in FR5XX user guide, but no examples available.  This
 * structure may need to be packed to correctly locate
 * bsl_cif_config. */
typedef struct sBSP430tlvBSL {
  sBSP430tlvEntry hdr;          /**< Standard tag/len pair */
  unsigned char bsl_com_if;
  unsigned char bsl_cif_config[1];
} sBSP430tlvBSL;
#endif /* 0 */

#endif /* BSP430_TLV_IS_5XX */

#if defined(BSP430_DOXYGEN) || ! (BSP430_TLV_IS_5XX - 0)

/* Standard definitions when missing from header */
#ifndef TAG_DCO_30
#define TAG_DCO_30             (0x01)    /**< (2xx) Tag for DCO30  Calibration Data */
#endif /* TAG_DCO_30 */
#ifndef TAG_EMPTY
#define TAG_EMPTY              (0xFE)    /**< (2xx) Tag for Empty Data Field in Calibration Data */
#endif /* TAG_EMPTY */

/** TLV entry for #TAG_DCO_30 on 2xx MCUs.
 *
 * These are BC2 calibration constants for fixed frequencies of MCLK.
 * The #TAG_DCO_30 is not defined on all MCUs, but all observed MCUs
 * have used a tag value @c 1 for this when a tag is present.  The
 * address of sBSP430tlvDCO::caldco_16mhz is fixed at @c 0x10F8. */
typedef struct sBSP430tlvDCO {
  sBSP430tlvEntry hdr;          /**< Standard tag/len pair */
  unsigned char caldco_16MHz; /**< Value for the DCOCTL register for 16 MHz, TA = 25Cel */
  unsigned char calbc1_16MHz; /**< Value for the BCSCTL1 register for 16 MHz, TA = 25Cel */
  unsigned char caldco_12MHz; /**< Value for the DCOCTL register for 12 MHz, TA = 25Cel */
  unsigned char calbc1_12MHz; /**< Value for the BCSCTL1 register for 12 MHz, TA = 25Cel */
  unsigned char caldco_8MHz; /**< Value for the DCOCTL register for 8 MHz, TA = 25Cel */
  unsigned char calbc1_8MHz; /**< Value for the BCSCTL1 register for 8 MHz, TA = 25Cel */
  unsigned char caldco_1MHz; /**< Value for the DCOCTL register for 1 MHz, TA = 25Cel */
  unsigned char calbc1_1MHz; /**< Value for the BCSCTL1 register for 1 MHz, TA = 25Cel */
} sBSP430tlvDCO;

/** TLV entry for #TAG_ADC10_1 and #TAG_ADC12_1 on 2xx MCUs
 *
 * These are ADC gain and offset calibration constants, as well as
 * constants for the internal temperature sensor.  The tag values are
 * not defined on all MCUs, and the observed value is @c 8 on some and
 * @c 0x10 on others.  The address of
 * sBSP430tlvADC::cal_adc_gain_factor is fixed at @c 0x10DC.
 *
 * When processing a 2xx descriptor table, it is reasonable to assume
 * that any tag that is not #TAG_DCO_30 nor #TAG_EMPTY corresponds to
 * an ADC structure. */
typedef struct sBPS430tlvADC {
  sBSP430tlvEntry hdr;          /**< Standard tag/len pair */
  unsigned int cal_adc_gain_factor; /**< VeREF = 2.5 V, TA = 85Cel +/- 2Cel, fADC12CLK = 5 MHz */
  unsigned int cal_adc_offset; /**< VeREF = 2.5 V, TA = 85Cel +/- 2Cel, fADC12CLK = 5 MHz */
  unsigned int cal_adc_15vref_factor; /**< VREF2_5 = 0, TA = 30Cel +/- 2Cel */
  unsigned int cal_adc_15t30; /**< VREF2_5 = 0, TA = 30Cel +/- 2Cel */
  unsigned int cal_adc_15t85; /**< VREF2_5 = 0, TA = 85Cel +/- 2Cel */
  unsigned int cal_adc_25vref_factor; /**< VREF2_5 = 1, TA = 30Cel +/- 2Cel */
  unsigned int cal_adc_25t30; /**< VREF2_5 = 1, TA = 30Cel +/- 2Cel */
  unsigned int cal_adc_25t85; /**< VREF2_5 = 1, TA = 85Cel +/- 2Cel */
} sBSP430tlvADC;

#endif /* ! BSP430_TLV_IS_5XX */

#if defined(BSP430_DOXYGEN) || (BSP430_TLV_IS_5XX - 0)
/** Offset from start of TLV table to skip past the CRC value */
#define BSP430_TLV_TABLE_DATA_OFFSET 4
/** Offset from start of TLV table to the first #sBSP430tlvEntry entry
 * in the table. */
#define BSP430_TLV_TABLE_PREFIX_LENGTH 8
#ifndef TLV_START

/** Integer corresponding to address of first TLV table entry.
 *
 * This constant is normally part of the MCU header, but is missing in
 * some cases.  It can be reconstructed from the address stored in
 * 0xFF4 on 5xx-family MCUs.  It is normally @c 0x1A08.
 *
 * @platformdep */
#define TLV_START (BSP430_TLV_TABLE_PREFIX_LENGTH + *(const unsigned int *)0x0FF4)
/** Integer corresponding to the address of the last byte in the TLV
 * table.
 *
 * It is normally part of the MCU header, but is missing in some
 * cases.  It can be reconstructed from the encoded table length.
 *
 * @platformdep */
#define TLV_END (TLV_START - BSP430_TLV_TABLE_PREFIX_LENGTH + (4 << ((const sBSP430tlvTable *)BSP430_TLV_TABLE_START)->info_len_lg2) - 1)
#endif /* TLV_START */
#else /* BSP430_TLV_IS_5XX */
#define BSP430_TLV_TABLE_DATA_OFFSET 2
#define BSP430_TLV_TABLE_PREFIX_LENGTH 2
#define TLV_START 0x10C2
#define TLV_END 0x10FF
#endif /* BSP430_TLV_IS_5XX */

/** The offset of the device descriptor table.
 *
 * This begins with the CRC and any other family-specific prefix
 * information.  The value has type <c>const unsigned char *</c>, and
 * may be cast to #sBSP430tlvTable to access relevant information. */
#define BSP430_TLV_TABLE_START ((const unsigned char *)TLV_START - BSP430_TLV_TABLE_PREFIX_LENGTH)

/** Check whether the table is valid.
 *
 * This references #BSP430_TLV_TABLE_START and #TLV_END and other
 * constants to verify that the stored checksum matches the checksum
 * calculated over the data area of the descriptor table. */
#define BSP430_TLV_TABLE_IS_VALID()                                     \
  (((const sBSP430tlvTable *)BSP430_TLV_TABLE_START)->crc               \
   == uiBSP430tlvChecksum(BSP430_TLV_TABLE_START + BSP430_TLV_TABLE_DATA_OFFSET, \
                          (1 + (const unsigned char *)TLV_END) - (BSP430_TLV_TABLE_START + BSP430_TLV_TABLE_DATA_OFFSET)))

/** Calculate the TLV-compatible checksum for a region of memory.
 *
 * The algorithm that is used is family-specific.
 * @li For 2xx it is the negative of the 16-bit XOR of the data.
 * @li For 5xx/6xx it is the value computed by the CRC16 peripheral
 * module, which uses the CRC-CCITT polynomial but reverses the bit
 * order.  Currently the implementation uses that module on these
 * MCUs.
 *
 * Other families do not explicitly support the TLV infrastructure,
 * though it may be used for application purposes.
 *
 * @warning Reversed CRC support is not present in certain NRND MCUs
 * like the MSP430F5438; all such MCUs have been superseded by newer
 * ones such as the MSP430F5438A.  Because the implementation in the
 * 5xx/6xx family depends on the CRC module, this function will not
 * compile on those MCUs.
 *
 * @note 5xx/6xx MCU headers provide #TLV_START and #TLV_END, which
 * provide the addresses of the first and last byte of the TLV entry
 * region.  The CRC should be calculated starting at @p data = #TLV_START-4, and
 * the length of the region is @p len = <c>5+#TLV_END-#TLV_START</c>.
 *
 * @param data The address of the data over which the checksum is calculated.
 *
 * @param len The number of octets in the data that contribute to the checksum.
 *
 * @return the 16-bit checksum
 */
unsigned int uiBSP430tlvChecksum (const unsigned char * data,
                                  size_t len);

#endif /* BSP430_UTILITY_TLV_H */
