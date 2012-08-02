/* Copyright (c) 2012, Peter A. Bigot <bigotp@acm.org>
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

#include <bsp430/periph/port_.h>

/* !BSP430! periph=port */
/* !BSP430! instance=PORT1,PORT2,PORT3,PORT4,PORT5,PORT6,PORT7,PORT8,PORT9,PORT10,PORT11 */

/* !BSP430! insert=hal_port_5xx_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_port_5xx_defn] */
#if configBSP430_HAL_PORT1 - 0
static struct sBSP430portState state_PORT1 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT1_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT1 = &state_PORT1;
#endif /* configBSP430_HAL_PORT1 */

#if configBSP430_HAL_PORT2 - 0
static struct sBSP430portState state_PORT2 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT2_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT2 = &state_PORT2;
#endif /* configBSP430_HAL_PORT2 */

#if configBSP430_HAL_PORT3 - 0
static struct sBSP430portState state_PORT3 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT3_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT3 = &state_PORT3;
#endif /* configBSP430_HAL_PORT3 */

#if configBSP430_HAL_PORT4 - 0
static struct sBSP430portState state_PORT4 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT4_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT4 = &state_PORT4;
#endif /* configBSP430_HAL_PORT4 */

#if configBSP430_HAL_PORT5 - 0
static struct sBSP430portState state_PORT5 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT5_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT5 = &state_PORT5;
#endif /* configBSP430_HAL_PORT5 */

#if configBSP430_HAL_PORT6 - 0
static struct sBSP430portState state_PORT6 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT6_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT6 = &state_PORT6;
#endif /* configBSP430_HAL_PORT6 */

#if configBSP430_HAL_PORT7 - 0
static struct sBSP430portState state_PORT7 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT7_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT7 = &state_PORT7;
#endif /* configBSP430_HAL_PORT7 */

#if configBSP430_HAL_PORT8 - 0
static struct sBSP430portState state_PORT8 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT8_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT8 = &state_PORT8;
#endif /* configBSP430_HAL_PORT8 */

#if configBSP430_HAL_PORT9 - 0
static struct sBSP430portState state_PORT9 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT9_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT9 = &state_PORT9;
#endif /* configBSP430_HAL_PORT9 */

#if configBSP430_HAL_PORT10 - 0
static struct sBSP430portState state_PORT10 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT10_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT10 = &state_PORT10;
#endif /* configBSP430_HAL_PORT10 */

#if configBSP430_HAL_PORT11 - 0
static struct sBSP430portState state_PORT11 = {
  .port = (volatile struct sBSP430periphPORTIE *)BSP430_PERIPH_PORT11_BASEADDRESS_,
};
tBSP430portHandle const hBSP430port_PORT11 = &state_PORT11;
#endif /* configBSP430_HAL_PORT11 */

/* END AUTOMATICALLY GENERATED CODE [hal_port_5xx_defn] */
/* !BSP430! end=hal_port_5xx_defn */

#if ((configBSP430_HAL_PORT1_ISR - 0)           \
     || (configBSP430_HAL_PORT2_ISR - 0)        \
     || (configBSP430_HAL_PORT3_ISR - 0)        \
     || (configBSP430_HAL_PORT4_ISR - 0)        \
     || (configBSP430_HAL_PORT5_ISR - 0)        \
     || (configBSP430_HAL_PORT6_ISR - 0)        \
     || (configBSP430_HAL_PORT7_ISR - 0)        \
     || (configBSP430_HAL_PORT8_ISR - 0)        \
     || (configBSP430_HAL_PORT9_ISR - 0)        \
     || (configBSP430_HAL_PORT10_ISR - 0)       \
     || (configBSP430_HAL_PORT12_ISR - 0)       \
     )
static int
#if __MSP430X__
__attribute__ ( ( __c16__ ) )
#endif /* CPUX */
/* __attribute__((__always_inline__)) */
port_isr (tBSP430portHandle device,
          int iv)
{
  int rv = 0;
  if (0 != iv) {
    int bit = (iv - 2) / 2;
    rv = iBSP430callbackInvokeISRIndexed_ni(device->pin_callback + bit, device, bit, rv);
  }
  return rv;
}
#endif /* PORT ISR */

/* !BSP430! insert=hal_port_5xx_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_port_5xx_isr_defn] */
#if configBSP430_HAL_PORT1_ISR - 0
static void
__attribute__((__interrupt__(PORT1_VECTOR)))
isr_PORT1 (void)
{
  int rv = port_isr(hBSP430port_PORT1, P1IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT1_ISR */

#if configBSP430_HAL_PORT2_ISR - 0
static void
__attribute__((__interrupt__(PORT2_VECTOR)))
isr_PORT2 (void)
{
  int rv = port_isr(hBSP430port_PORT2, P2IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT2_ISR */

#if configBSP430_HAL_PORT3_ISR - 0
static void
__attribute__((__interrupt__(PORT3_VECTOR)))
isr_PORT3 (void)
{
  int rv = port_isr(hBSP430port_PORT3, P3IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT3_ISR */

#if configBSP430_HAL_PORT4_ISR - 0
static void
__attribute__((__interrupt__(PORT4_VECTOR)))
isr_PORT4 (void)
{
  int rv = port_isr(hBSP430port_PORT4, P4IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT4_ISR */

#if configBSP430_HAL_PORT5_ISR - 0
static void
__attribute__((__interrupt__(PORT5_VECTOR)))
isr_PORT5 (void)
{
  int rv = port_isr(hBSP430port_PORT5, P5IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT5_ISR */

#if configBSP430_HAL_PORT6_ISR - 0
static void
__attribute__((__interrupt__(PORT6_VECTOR)))
isr_PORT6 (void)
{
  int rv = port_isr(hBSP430port_PORT6, P6IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT6_ISR */

#if configBSP430_HAL_PORT7_ISR - 0
static void
__attribute__((__interrupt__(PORT7_VECTOR)))
isr_PORT7 (void)
{
  int rv = port_isr(hBSP430port_PORT7, P7IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT7_ISR */

#if configBSP430_HAL_PORT8_ISR - 0
static void
__attribute__((__interrupt__(PORT8_VECTOR)))
isr_PORT8 (void)
{
  int rv = port_isr(hBSP430port_PORT8, P8IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT8_ISR */

#if configBSP430_HAL_PORT9_ISR - 0
static void
__attribute__((__interrupt__(PORT9_VECTOR)))
isr_PORT9 (void)
{
  int rv = port_isr(hBSP430port_PORT9, P9IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT9_ISR */

#if configBSP430_HAL_PORT10_ISR - 0
static void
__attribute__((__interrupt__(PORT10_VECTOR)))
isr_PORT10 (void)
{
  int rv = port_isr(hBSP430port_PORT10, P10IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT10_ISR */

#if configBSP430_HAL_PORT11_ISR - 0
static void
__attribute__((__interrupt__(PORT11_VECTOR)))
isr_PORT11 (void)
{
  int rv = port_isr(hBSP430port_PORT11, P11IV);
  BSP430_PERIPH_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT11_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_port_5xx_isr_defn] */
/* !BSP430! end=hal_port_5xx_isr_defn */

volatile xBSP430periphPORTIE *
xBSP430periphLookupPORTIE (tBSP430periphHandle periph)
{
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
  /* !BSP430! insert=periph_ba_hpl_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_ba_hpl_demux] */
#if configBSP430_HPL_PORT1 - 0
  if (BSP430_PERIPH_PORT1 == periph) {
    return hBSP430periph_PORT1;
  }
#endif /* configBSP430_HPL_PORT1 */

#if configBSP430_HPL_PORT2 - 0
  if (BSP430_PERIPH_PORT2 == periph) {
    return hBSP430periph_PORT2;
  }
#endif /* configBSP430_HPL_PORT2 */

#if configBSP430_HPL_PORT3 - 0
  if (BSP430_PERIPH_PORT3 == periph) {
    return hBSP430periph_PORT3;
  }
#endif /* configBSP430_HPL_PORT3 */

#if configBSP430_HPL_PORT4 - 0
  if (BSP430_PERIPH_PORT4 == periph) {
    return hBSP430periph_PORT4;
  }
#endif /* configBSP430_HPL_PORT4 */

#if configBSP430_HPL_PORT5 - 0
  if (BSP430_PERIPH_PORT5 == periph) {
    return hBSP430periph_PORT5;
  }
#endif /* configBSP430_HPL_PORT5 */

#if configBSP430_HPL_PORT6 - 0
  if (BSP430_PERIPH_PORT6 == periph) {
    return hBSP430periph_PORT6;
  }
#endif /* configBSP430_HPL_PORT6 */

#if configBSP430_HPL_PORT7 - 0
  if (BSP430_PERIPH_PORT7 == periph) {
    return hBSP430periph_PORT7;
  }
#endif /* configBSP430_HPL_PORT7 */

#if configBSP430_HPL_PORT8 - 0
  if (BSP430_PERIPH_PORT8 == periph) {
    return hBSP430periph_PORT8;
  }
#endif /* configBSP430_HPL_PORT8 */

#if configBSP430_HPL_PORT9 - 0
  if (BSP430_PERIPH_PORT9 == periph) {
    return hBSP430periph_PORT9;
  }
#endif /* configBSP430_HPL_PORT9 */

#if configBSP430_HPL_PORT10 - 0
  if (BSP430_PERIPH_PORT10 == periph) {
    return hBSP430periph_PORT10;
  }
#endif /* configBSP430_HPL_PORT10 */

#if configBSP430_HPL_PORT11 - 0
  if (BSP430_PERIPH_PORT11 == periph) {
    return hBSP430periph_PORT11;
  }
#endif /* configBSP430_HPL_PORT11 */

  /* END AUTOMATICALLY GENERATED CODE [periph_ba_hpl_demux] */
  /* !BSP430! end=periph_ba_hpl_demux */
#else /* 5xx */
#error Not implemented
#endif /* 5xx */
  return NULL;
}

volatile xBSP430periphPORT *
xBSP430periphLookupPORT (tBSP430periphHandle periph)
{
#if defined(__MSP430_HAS_MSP430XV2_CPU__)
  return NULL;
#else /* 5xx */
#error Not implemented
#endif /* 5xx */
  return NULL;
}

