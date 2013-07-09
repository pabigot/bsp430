/** This file is in the public domain.
 *
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/console.h>
#include <bsp430/utility/tlv.h>

void dumpRegion (const char * header,
                 const unsigned char * addr,
                 size_t len)
{
  size_t i;

  cprintf("%s:", header);
  for (i = 0; i < len; ++i) {
    if (0 == (i % 16)) {
      cprintf("\n%p", addr+i);
    }
    if (0 == (i % 8)) {
      cputchar(' ');
    }
    cprintf(" %02x", addr[i]);
  }
  cputchar('\n');
}

void main ()
{
  const unsigned char * tlvt_start = BSP430_TLV_TABLE_START;
  const unsigned char * tlvt_end = 1 + (const unsigned char *)TLV_END;
  const unsigned char * crc_start = tlvt_start + BSP430_TLV_TABLE_DATA_OFFSET;
  const unsigned char * crc_end = tlvt_end;
  const sBSP430tlvTable * tp = (const sBSP430tlvTable *)BSP430_TLV_TABLE_START;
  const sBSP430tlvEntry * ep = (const sBSP430tlvEntry *)TLV_START;
  unsigned int crc;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  cprintf("\n\n\n");
  dumpRegion("Legacy device table", (const unsigned char *)0xFF0, 16);
  cprintf("TLV table starts at %p and ends at %p\n", tlvt_start, tlvt_end);
  dumpRegion("TLV table", tlvt_start, tlvt_end - tlvt_start);
  crc = uiBSP430tlvChecksum(crc_start, crc_end - crc_start);
  cprintf("CRC for [%p, %p] expect %04x get %04x: %s\n",
          crc_start, crc_end, tp->crc, crc,
          (crc == tp->crc) ? "valid data" : "CORRUPTED DATA");
  while (ep < (const sBSP430tlvEntry*)TLV_END) {
    cprintf("%p tag %02x len %u\n", ep, ep->tag, ep->len);
#if (BSP430_TLV_IS_5XX - 0)
    if (TLV_DIERECORD == ep->tag) {
      const sBSP430tlvDIERECORD * rp = (const sBSP430tlvDIERECORD *)ep;
      cprintf("\tLot ID: 0x%08lx ; position %u, %u ; test results 0x%04x\n",
              rp->lot_id, rp->die_x_position, rp->die_y_position, rp->test_results);
    }
    if (TLV_REFCAL == ep->tag) {
      const sBSP430tlvREFCAL * calp = (const sBSP430tlvREFCAL *)ep;
      cprintf("\tREF factors: 1.5V %d ; 2.0V %d ; 2.5V %d\n",
              calp->cal_adc_15vref_factor,
              calp->cal_adc_20vref_factor,
              calp->cal_adc_25vref_factor);
    }
    if ((TLV_ADC10CAL == ep->tag)
        || (TLV_ADC12CAL == ep->tag)
#if defined(TLV_ADCCAL)
        || (TLV_ADCCAL == ep->tag)
#endif /* TLV_ADCCAL */
       ) {
      const sBSP430tlvADCCAL * calp = (const sBSP430tlvADCCAL *)ep;
      cprintf("\tADC: Gain factor %d, offset %d\n", calp->cal_adc_gain_factor, calp->cal_adc_offset);
      cprintf("\t 1.5V T30 %u T85 %u\n",
              calp->cal_adc_15t30, calp->cal_adc_15t85);
      cprintf("\t 2.0V T30 %u T85 %u\n",
              calp->cal_adc_20t30, calp->cal_adc_20t85);
      cprintf("\t 2.5V T30 %u T85 %u\n",
              calp->cal_adc_25t30, calp->cal_adc_25t85);
    }
    if (TLV_PDTAG == ep->tag) {
      const unsigned char * cp = (const unsigned char *)(ep+1);
      unsigned int periph_addr = 0;
      unsigned int pc;

      while (*cp) {
        unsigned int me = *(unsigned int *)cp;
        unsigned char memory_type = (me >> 13) & 0x07;
        unsigned char memory_size = (me >> 9) & 0x0F;
        unsigned long unit_size = ((me >> 7) & 0x01) ? 0x10000L : 0x200;
        unsigned char address_value = me & 0x3F;
        const char * memory_type_str[] = { "None", "RAM", "EEPROM", "Reserved",
                                           "FLASH", "ROM", "app", "Undefined"
                                         };

        cp += sizeof(me);
        cprintf("\tMemory entry %04x: ", me);
        if (0x0F == memory_size) {
          cprintf("UNDEF ");
        } else if (0x0E == memory_size) {
          cprintf("[app] ");
        } else if (0x04 <= memory_size) {
          cprintf("%u KB ", (1 << (memory_size - 4)));
        } else {
          cprintf("%u B ", 64 << memory_size);
        }
        cprintf("%s at 0x%lx\n", memory_type_str[memory_type], address_value * unit_size);
      }
      ++cp;
      pc = *cp++;
      while (0 < pc--) {
        unsigned int pe = *(unsigned int *)cp;
        unsigned char pid = (pe >> 8);
        unsigned int unit_size = (pe & 0x80) ? 0x800 : 0x10;
        unsigned char address_value = pe & 0x7F;
        cprintf("\tPeripheral entry %04x: ", pe);
        cp += sizeof(pe);
        if (0 == pid) {
          cprintf("No module");
        } else {
          cprintf("Module pid 0x%02x", pid);
        }
        periph_addr += address_value * unit_size;
        cprintf(" at 0x%04x\n", periph_addr);
      }
      cprintf("\tInterrupts from highest to lowest priority:\n");
      cprintf("\t\tRSTIV\n");
      cprintf("\t\tSNIV\n");
      cprintf("\t\tUNIV\n");
      while (*cp) {
        /* The constants used to map PIDs to peripherals are
         * apparently MCU-specific, so we can't tell.  However, if
         * (say) 0x90 refers to a USCI module, then 0x92 would be
         * USCI_A1 and 0x93 would be USCI_B1. */
        cprintf("\t\tInterrupt for module pid 0x%02x\n", *cp);
        ++cp;
      }
    }
#else /* BSP430_TLV_IS_5XX */
    if (TAG_EMPTY == ep->tag) {
      cprintf("\tBlank\n");
    } else if (TAG_DCO_30 == ep->tag) {
      const sBSP430tlvDCO * calp = (const sBSP430tlvDCO *)ep;
      cprintf("\t16 MHz: DCOCTL %02x BCSCTL1 %02x\n", calp->caldco_16MHz, calp->calbc1_16MHz);
      cprintf("\t12 MHz: DCOCTL %02x BCSCTL1 %02x\n", calp->caldco_12MHz, calp->calbc1_12MHz);
      cprintf("\t8 MHz: DCOCTL %02x BCSCTL1 %02x\n", calp->caldco_8MHz, calp->calbc1_8MHz);
      cprintf("\t1 MHz: DCOCTL %02x BCSCTL1 %02x\n", calp->caldco_1MHz, calp->calbc1_1MHz);
    } else {
      const sBSP430tlvADC * calp = (const sBSP430tlvADC *)ep;
      cprintf("\tADC: Gain factor %d, offset %d\n", calp->cal_adc_gain_factor, calp->cal_adc_offset);
      cprintf("\t 1.5V ref: Factor %#x, T30 %u T85 %u\n",
              calp->cal_adc_15vref_factor, calp->cal_adc_15t30, calp->cal_adc_15t85);
      cprintf("\t 2.5V ref: Factor %#x, T30 %u T85 %u\n",
              calp->cal_adc_25vref_factor, calp->cal_adc_25t30, calp->cal_adc_25t85);
    }
#endif /* BSP430_TLV_IS_5XX */

    ep = BSP430_TLV_NEXT_ENTRY(ep);
  }
  cprintf("End of TLV structure\n\n");

}
