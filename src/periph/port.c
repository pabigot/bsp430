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

/* !BSP430! periph=port */
/* !BSP430! instance=PORT1,PORT2,PORT3,PORT4,PORT5,PORT6,PORT7,PORT8,PORT9,PORT10,PORT11 */

#include <bsp430/periph/port.h>

/* !BSP430! insert=hal_port_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_port_defn] */
#if configBSP430_HAL_PORT1 - 0
struct sBSP430halPORT xBSP430hal_PORT1_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (1 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT1_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT1_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (1 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT1
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P1REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT1 */

#if configBSP430_HAL_PORT2 - 0
struct sBSP430halPORT xBSP430hal_PORT2_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (2 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT2_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT2_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (2 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT2
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P2REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT2 */

#if configBSP430_HAL_PORT3 - 0
struct sBSP430halPORT xBSP430hal_PORT3_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (3 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT3_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT3_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (3 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT3
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P3REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT3 */

#if configBSP430_HAL_PORT4 - 0
struct sBSP430halPORT xBSP430hal_PORT4_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (4 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT4_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT4_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (4 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT4
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P4REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT4 */

#if configBSP430_HAL_PORT5 - 0
struct sBSP430halPORT xBSP430hal_PORT5_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (5 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT5_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT5_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (5 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT5
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P5REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT5 */

#if configBSP430_HAL_PORT6 - 0
struct sBSP430halPORT xBSP430hal_PORT6_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (6 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT6_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT6_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (6 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT6
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P6REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT6 */

#if configBSP430_HAL_PORT7 - 0
struct sBSP430halPORT xBSP430hal_PORT7_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (7 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT7_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT7_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (7 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT7
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P7REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT7 */

#if configBSP430_HAL_PORT8 - 0
struct sBSP430halPORT xBSP430hal_PORT8_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (8 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT8_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT8_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (8 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT8
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P8REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT8 */

#if configBSP430_HAL_PORT9 - 0
struct sBSP430halPORT xBSP430hal_PORT9_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (9 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT9_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT9_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (9 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT9
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P9REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT9 */

#if configBSP430_HAL_PORT10 - 0
struct sBSP430halPORT xBSP430hal_PORT10_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (10 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT10_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT10_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (10 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT10
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P10REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT10 */

#if configBSP430_HAL_PORT11 - 0
struct sBSP430halPORT xBSP430hal_PORT11_ = {
  .hal_state = {
    .cflags =
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (11 <= 2)
    BSP430_PORT_HAL_HPL_VARIANT_PORTIE
#else /* IE */
    BSP430_PORT_HAL_HPL_VARIANT_PORT
#endif /* IE */
#if configBSP430_HAL_PORT11_ISR - 0
    | BSP430_PERIPH_HAL_STATE_CFLAGS_ISR
#endif /* configBSP430_HAL_PORT11_ISR */
  },
  .hpl = {
#if (BSP430_CORE_FAMILY_IS_5XX - 0) || (11 <= 2)
    .portie
#else /* interrupt-enabled */
    .port
#endif /* interrupt-enabled */
    = BSP430_HPL_PORT11
  },
#if defined(__MSP430_HAS_PORT1_R__) && ! (BSP430_CORE_FAMILY_IS_5XX - 0)
  .renp = &P11REN,
#endif /* __MSP430_HAS_PORT1_R__ */
};
#endif /* configBSP430_HAL_PORT11 */

/* END AUTOMATICALLY GENERATED CODE [hal_port_defn] */
/* !BSP430! end=hal_port_defn */

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
port_isr (hBSP430halPORT device,
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

/* !BSP430! insert=hal_port_isr_defn */
/* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [hal_port_isr_defn] */
#if configBSP430_HAL_PORT1_ISR - 0
static void
__attribute__((__interrupt__(PORT1_VECTOR)))
isr_PORT1 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P1IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P1IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P1IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT1, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT1_ISR */

#if configBSP430_HAL_PORT2_ISR - 0
static void
__attribute__((__interrupt__(PORT2_VECTOR)))
isr_PORT2 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P2IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P2IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P2IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT2, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT2_ISR */

#if configBSP430_HAL_PORT3_ISR - 0
static void
__attribute__((__interrupt__(PORT3_VECTOR)))
isr_PORT3 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P3IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P3IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P3IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT3, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT3_ISR */

#if configBSP430_HAL_PORT4_ISR - 0
static void
__attribute__((__interrupt__(PORT4_VECTOR)))
isr_PORT4 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P4IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P4IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P4IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT4, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT4_ISR */

#if configBSP430_HAL_PORT5_ISR - 0
static void
__attribute__((__interrupt__(PORT5_VECTOR)))
isr_PORT5 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P5IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P5IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P5IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT5, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT5_ISR */

#if configBSP430_HAL_PORT6_ISR - 0
static void
__attribute__((__interrupt__(PORT6_VECTOR)))
isr_PORT6 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P6IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P6IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P6IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT6, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT6_ISR */

#if configBSP430_HAL_PORT7_ISR - 0
static void
__attribute__((__interrupt__(PORT7_VECTOR)))
isr_PORT7 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P7IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P7IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P7IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT7, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT7_ISR */

#if configBSP430_HAL_PORT8_ISR - 0
static void
__attribute__((__interrupt__(PORT8_VECTOR)))
isr_PORT8 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P8IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P8IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P8IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT8, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT8_ISR */

#if configBSP430_HAL_PORT9_ISR - 0
static void
__attribute__((__interrupt__(PORT9_VECTOR)))
isr_PORT9 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P9IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P9IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P9IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT9, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT9_ISR */

#if configBSP430_HAL_PORT10_ISR - 0
static void
__attribute__((__interrupt__(PORT10_VECTOR)))
isr_PORT10 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P10IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P10IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P10IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT10, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT10_ISR */

#if configBSP430_HAL_PORT11_ISR - 0
static void
__attribute__((__interrupt__(PORT11_VECTOR)))
isr_PORT11 (void)
{
  int iv;
  int rv;
#if BSP430_CORE_FAMILY_IS_5XX - 0
  iv = P11IV;
#else /* CPUX */
  unsigned char bit = 1;

  iv = 0;
  while (bit && !(bit & P11IFG)) {
    ++iv;
    bit <<= 1;
  }
  if (! bit) {
    return;
  }
  P11IFG &= ~bit;
#endif /* CPUX */
  rv = port_isr(BSP430_HAL_PORT11, iv);
  BSP430_HAL_ISR_CALLBACK_TAIL_NI(rv);
}
#endif /* configBSP430_HAL_PORT11_ISR */

/* END AUTOMATICALLY GENERATED CODE [hal_port_isr_defn] */
/* !BSP430! end=hal_port_isr_defn */

volatile sBSP430hplPORTIE *
xBSP430periphLookupPORTIE (tBSP430periphHandle periph)
{
  /* !BSP430! ie_test=<= subst=ie_test insert=periph_hpl_port_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_port_demux] */
#if (configBSP430_HPL_PORT1 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (1 <= 2))
  if (BSP430_PERIPH_PORT1 == periph) {
    return BSP430_HPL_PORT1;
  }
#endif /* configBSP430_HPL_PORT1 */

#if (configBSP430_HPL_PORT2 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (2 <= 2))
  if (BSP430_PERIPH_PORT2 == periph) {
    return BSP430_HPL_PORT2;
  }
#endif /* configBSP430_HPL_PORT2 */

#if (configBSP430_HPL_PORT3 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (3 <= 2))
  if (BSP430_PERIPH_PORT3 == periph) {
    return BSP430_HPL_PORT3;
  }
#endif /* configBSP430_HPL_PORT3 */

#if (configBSP430_HPL_PORT4 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (4 <= 2))
  if (BSP430_PERIPH_PORT4 == periph) {
    return BSP430_HPL_PORT4;
  }
#endif /* configBSP430_HPL_PORT4 */

#if (configBSP430_HPL_PORT5 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (5 <= 2))
  if (BSP430_PERIPH_PORT5 == periph) {
    return BSP430_HPL_PORT5;
  }
#endif /* configBSP430_HPL_PORT5 */

#if (configBSP430_HPL_PORT6 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (6 <= 2))
  if (BSP430_PERIPH_PORT6 == periph) {
    return BSP430_HPL_PORT6;
  }
#endif /* configBSP430_HPL_PORT6 */

#if (configBSP430_HPL_PORT7 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (7 <= 2))
  if (BSP430_PERIPH_PORT7 == periph) {
    return BSP430_HPL_PORT7;
  }
#endif /* configBSP430_HPL_PORT7 */

#if (configBSP430_HPL_PORT8 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (8 <= 2))
  if (BSP430_PERIPH_PORT8 == periph) {
    return BSP430_HPL_PORT8;
  }
#endif /* configBSP430_HPL_PORT8 */

#if (configBSP430_HPL_PORT9 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (9 <= 2))
  if (BSP430_PERIPH_PORT9 == periph) {
    return BSP430_HPL_PORT9;
  }
#endif /* configBSP430_HPL_PORT9 */

#if (configBSP430_HPL_PORT10 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (10 <= 2))
  if (BSP430_PERIPH_PORT10 == periph) {
    return BSP430_HPL_PORT10;
  }
#endif /* configBSP430_HPL_PORT10 */

#if (configBSP430_HPL_PORT11 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (11 <= 2))
  if (BSP430_PERIPH_PORT11 == periph) {
    return BSP430_HPL_PORT11;
  }
#endif /* configBSP430_HPL_PORT11 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_port_demux] */
  /* !BSP430! end=periph_hpl_port_demux */
  return NULL;
}

volatile sBSP430hplPORT *
xBSP430periphLookupPORT (tBSP430periphHandle periph)
{
  /* !BSP430! ie_test=> subst=ie_test insert=periph_hpl_port_demux */
  /* BEGIN AUTOMATICALLY GENERATED CODE---DO NOT MODIFY [periph_hpl_port_demux] */
#if (configBSP430_HPL_PORT1 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (1 > 2))
  if (BSP430_PERIPH_PORT1 == periph) {
    return BSP430_HPL_PORT1;
  }
#endif /* configBSP430_HPL_PORT1 */

#if (configBSP430_HPL_PORT2 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (2 > 2))
  if (BSP430_PERIPH_PORT2 == periph) {
    return BSP430_HPL_PORT2;
  }
#endif /* configBSP430_HPL_PORT2 */

#if (configBSP430_HPL_PORT3 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (3 > 2))
  if (BSP430_PERIPH_PORT3 == periph) {
    return BSP430_HPL_PORT3;
  }
#endif /* configBSP430_HPL_PORT3 */

#if (configBSP430_HPL_PORT4 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (4 > 2))
  if (BSP430_PERIPH_PORT4 == periph) {
    return BSP430_HPL_PORT4;
  }
#endif /* configBSP430_HPL_PORT4 */

#if (configBSP430_HPL_PORT5 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (5 > 2))
  if (BSP430_PERIPH_PORT5 == periph) {
    return BSP430_HPL_PORT5;
  }
#endif /* configBSP430_HPL_PORT5 */

#if (configBSP430_HPL_PORT6 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (6 > 2))
  if (BSP430_PERIPH_PORT6 == periph) {
    return BSP430_HPL_PORT6;
  }
#endif /* configBSP430_HPL_PORT6 */

#if (configBSP430_HPL_PORT7 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (7 > 2))
  if (BSP430_PERIPH_PORT7 == periph) {
    return BSP430_HPL_PORT7;
  }
#endif /* configBSP430_HPL_PORT7 */

#if (configBSP430_HPL_PORT8 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (8 > 2))
  if (BSP430_PERIPH_PORT8 == periph) {
    return BSP430_HPL_PORT8;
  }
#endif /* configBSP430_HPL_PORT8 */

#if (configBSP430_HPL_PORT9 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (9 > 2))
  if (BSP430_PERIPH_PORT9 == periph) {
    return BSP430_HPL_PORT9;
  }
#endif /* configBSP430_HPL_PORT9 */

#if (configBSP430_HPL_PORT10 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (10 > 2))
  if (BSP430_PERIPH_PORT10 == periph) {
    return BSP430_HPL_PORT10;
  }
#endif /* configBSP430_HPL_PORT10 */

#if (configBSP430_HPL_PORT11 - 0) && ((BSP430_CORE_FAMILY_IS_5XX - 0) || (11 > 2))
  if (BSP430_PERIPH_PORT11 == periph) {
    return BSP430_HPL_PORT11;
  }
#endif /* configBSP430_HPL_PORT11 */

  /* END AUTOMATICALLY GENERATED CODE [periph_hpl_port_demux] */
  /* !BSP430! end=periph_hpl_port_demux */
  return NULL;
}

