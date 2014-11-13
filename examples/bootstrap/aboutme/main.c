/** This file is in the public domain.
 *
 * Simple program to dump as much information as can be determined at
 * compile time or runtime.  Mostly this is compiler vendor and
 * version, dependent package version information, material inferred
 * from compiler flags, and object sizes.
 *
 * @homepage http://github.com/pabigot/bsp430
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#if (BSP430_CORE_TOOLCHAIN_GCC_MSPGCC - 0)
#include <msp430libc.h>
#endif /* GCC_MSPGCC */

/** Convert token to a string.  Parameter may be a macro; the string
 * is the value of that macro. */
#define XSTR(_x) _XSTR(_x)
#define _XSTR(_x) #_x

void main ()
{
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();
  BSP430_CORE_ENABLE_INTERRUPT();

  cprintf("\naboutme " __DATE__ " " __TIME__ "\n");
  cputs("BSP430 version: " XSTR(BSP430_VERSION));
#if defined(__GNUC__)
  cputs("GNU compiler, "
#if (BSP430_CORE_TOOLCHAIN_GCC_MSPGCC - 0)
        "mspgcc"
#elif (BSP430_CORE_TOOLCHAIN_GCC_MSP430_ELF - 0)
        "msp430-elf"
#else
        "UNKNOWN"
#endif
        " variant, version "
        XSTR(__GNUC__) "." XSTR(__GNUC_MINOR__) "." XSTR(__GNUC_PATCHLEVEL__));
#endif

#if (BSP430_CORE_TOOLCHAIN_GCC_MSPGCC - 0)
  cputs("# mspgcc features");
  cputs("MSPGCC " XSTR(__MSPGCC__) "\n"
        "MSP430LIBC " XSTR(__MSP430_LIBC__) "\n"
        "MSP430MCU " XSTR(__MSP430MCU__) "\n"
        "MSP430MCU_UPSTREAM " XSTR(__MSP430MCU_UPSTREAM__) "\n"
#if 20110716 < __MSPGCC__
        "CPU "
#if MSP430_CPU_MSP430XV2 == __MSP430_CPU__
        "MSP430XV2"
#elif MSP430_CPU_MSP430X == __MSP430_CPU__
        "MSP430X"
#elif MSP430_CPU_MSP430 == __MSP430_CPU__
        "MSP430"
#else
#error Unrecognized CPU
#endif
        "\n"
        "MPY "
#if MSP430_MPY_NONE == __MSP430_MPY__
        "none"
#elif MSP430_MPY_16 == __MSP430_MPY__
        "16"
#elif MSP430_MPY_16SE == __MSP430_MPY__
        "16SE"
#elif MSP430_MPY_32 == __MSP430_MPY__
        "32"
#elif MSP430_MPY_32DW == __MSP430_MPY__
        "32DW"
#else
#error Unrecognized MPY
#endif
#if __MSP430X__
        "\nCPUX_TARGET"
#if __MSP430X__ & __MSP430_CPUX_TARGET_SR20__
        " SR20"
#endif /* SR20 */
#if __MSP430X__ & __MSP430_CPUX_TARGET_ISR20__
        " ISR20"
#endif /* ISR20 */
#if __MSP430X__ & __MSP430_CPUX_TARGET_A20__
        " A20"
#endif /* A20 */
#if __MSP430X__ & __MSP430_CPUX_TARGET_C20__
        " C20"
#endif /* C20 */
#if __MSP430X__ & __MSP430_CPUX_TARGET_D20__
        " D20"
#endif /* D20 */
#endif /* __MSP430X__ */
        "\n"
#endif /* post LTS-20110716 */
       );
#endif /* mspgcc */

#if (BSP430_CORE_TOOLCHAIN_GCC_MSP430_ELF - 0)
  cputs("# msp430-elf features");
  cputs("MSP430 "
#ifdef __MSP430__
        XSTR(__MSP430__)
#else
        " not defined"
#endif
       );
  cputs("MSP430X "
#ifdef __MSP430X__
        XSTR(__MSP430X__)
#else
        " not defined"
#endif
       );
  cputs("MSP430X_LARGE "
#ifdef __MSP430X_LARGE__
        XSTR(__MSP430X_LARGE__)
#else
        " not defined"
#endif
       );
#endif /* msp430-elf */

#define SHOW_SIZEOF(_t) do { cprintf("sizeof(" #_t ") = %u\n", (unsigned int)sizeof(_t)); } while (0)
  cputs("# object size information");
  SHOW_SIZEOF(unsigned int);
  SHOW_SIZEOF(unsigned long);
  SHOW_SIZEOF(unsigned long long);
  SHOW_SIZEOF(void *);
  SHOW_SIZEOF(void (*) ());
}
