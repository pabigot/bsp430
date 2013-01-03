#include <bsp430/core.h>

uint16_t bswap16i16o16 (uint16_t in)
{
  return BSP430_CORE_SWAP_16(in);
}
uint16_t bswap16i32o32 (uint32_t in)
{
  return BSP430_CORE_SWAP_16(in);
}
uint32_t bswap32i32o32 (uint32_t in)
{
  return BSP430_CORE_SWAP_32(in);
}
