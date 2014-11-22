/** This file is in the public domain.
 *
 * Demonstrate use of the CRC module and validate its results against
 * a software implementation.  Both "normal" and "reversed" MSP430 CRC
 * are supported.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <limits.h>

#ifndef __MSP430_HAS_CRC__
#error Application requires CRC module
#endif /* __MSP430_HAS_CRC__ */

#ifndef BLOCK
#define BLOCK "test data"
//#define BLOCK { 0x00, 0x61, 0x74, 0x61, 0x64, 0x20, 0x74, 0x73, 0x65, 0x74 }
#endif /* BLOCK */

#ifndef CRC_INIT
#define CRC_INIT ~0
#endif /* CRC_INIT */

/* Software implementation of the CRC algorithm. */
uint16_t crc_ccit_update (uint16_t crc,
                          uint8_t d)
{
  const uint16_t CRC_POLY = 0x1021;
  const uint16_t MASK = 0x8000;

  uint16_t mask = MASK;
  crc ^= (d << 8);
  while (mask > 0x80) {
    bool xor_poly = !!(MASK & crc);
    crc <<= 1;
    if (xor_poly) {
      crc ^= CRC_POLY;
    }
    mask >>= 1;
  }
  return crc;
}

/* Reverse the bits in a byte, as required for MSP430 non-reversed CRC
 * implementation.
 *
 * @see https://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious */
uint8_t reverse_byte (uint8_t v)
{
  uint8_t r = v;
  int s = CHAR_BIT - 1;

  v >>= 1;
  while (v) {
    r = (r << 1) | (1 & v);
    --s;
    v >>= 1;
  }
  r <<= s; // shift when v's highest bits are zero
  return r;
}

void main ()
{
  const uint8_t block[] = BLOCK;
  const uint8_t * const block_end = block + sizeof(block);
  const uint8_t * bp = block;
  unsigned int errors = 0;
  uint16_t crc;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  /* Need to enable interrupts so timer overflow events are properly
   * acknowledged */
  BSP430_CORE_ENABLE_INTERRUPT();
  cprintf("CRC " __DATE__ " " __TIME__ "\n\n");

  cprintf("Reverse %02x is %02x\n", 0xAC, reverse_byte(0xAC));
  for (int i = 0; i < 256; ++i) {
    if (reverse_byte(reverse_byte(i)) != i) {
      cprintf("ERR reversing %02x\n", i);
    }
  }

  cprintf("Block:");
  vBSP430consoleDisplayMemory(block, sizeof(block), 0);
  cputchar('\n');

  CRCINIRES = CRC_INIT;
  crc = CRC_INIT;
  cprintf("MSP430 standard initial: %04x %04x\n", CRCINIRES, crc);
  bp = block;
  while (bp < block_end) {
    char compare = ' ';
    CRCDI_L = *bp;
    crc = crc_ccit_update(crc, reverse_byte(*bp));
    compare = (CRCINIRES != crc) ? '*' : ' ';
    cprintf("%02x => %04x%c%04x\n", *bp, CRCINIRES, compare, crc);
    errors += (' ' != compare);
    ++bp;
  }
  if (0 == errors) {
    cprintf("Results match\n");
  } else {
    cprintf("Results DIFFER at %u values\n", errors);
  }

  CRCINIRES = CRC_INIT;
  crc = CRC_INIT;
  cprintf("Bit reversed initial: %04x %04x\n", CRCINIRES, crc);
  bp = block;
  errors = 0;
  while (bp < block_end) {
    char compare = ' ';
    CRCDIRB_L = *bp;
    crc = crc_ccit_update(crc, *bp);
    compare = (CRCINIRES != crc) ? '*' : ' ';
    cprintf("%02x => %04x%c%04x\n", *bp, CRCINIRES, compare, crc);
    errors += (' ' != compare);
    ++bp;
  }
  if (0 == errors) {
    cprintf("Results match\n");
  } else {
    cprintf("Results DIFFER at %u values\n", errors);
  }

}
