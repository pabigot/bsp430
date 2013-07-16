/** This file is in the public domain.
 *
 * Demonstrate the proper way to detect timer overflow given modular
 * arithmetic.  See: http://www.thetaeng.com/TimerWrap.htm
 *
 * (The "Positive Delta" algorithm is correct.)
 *
 * @homepage http://github.com/pabigot/bsp430
 */

/* Outputs expected for each comparison

Wrapping timer comparisons: Unsigned Comparison
Row indicates a scheduled time produced by adding to a running counter
Column represents a counter value
Cell indicates whether counter is considered >= time of row
    signed  unsigned       0       1   32766   32767   32768   65534   65535
        0         0        1       1       1       1       1       1       1
        1         1        0       1       1       1       1       1       1
    32766     32766        0       0       1       1       1       1       1
    32767     32767        0       0       0       1       1       1       1
   -32768     32768        0       0       0       0       1       1       1
       -2     65534        0       0       0       0       0       1       1
       -1     65535        0       0       0       0       0       0       1

Wrapping timer comparisons: Positive Delta
Row indicates a scheduled time produced by adding to a running counter
Column represents a counter value
Cell indicates whether counter is considered >= time of row
    signed  unsigned       0       1   32766   32767   32768   65534   65535
        0         0        1       1       1       1       0       0       0
        1         1        0       1       1       1       1       0       0
    32766     32766        0       0       1       1       1       0       0
    32767     32767        0       0       0       1       1       1       0
   -32768     32768        0       0       0       0       1       1       1
       -2     65534        1       1       0       0       0       1       1
       -1     65535        1       1       1       0       0       0       1

*/

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <limits.h>
#include <inttypes.h>

static const unsigned int values[] = { 0, 1,
                                       INT_MAX - 1, INT_MAX, (unsigned int)INT_MAX + 1,
                                       UINT_MAX - 1, UINT_MAX
                                     };
static const size_t nvalues = sizeof(values)/sizeof(*values);

int
ucompare (unsigned int u1,
          unsigned int u2)
{
  static const unsigned int UMAX = (~0U) >> 1;
  return (u2 - u1) < UMAX;
}

#if 0
#define UCOMPARE(u1_, u2_) ((u2_) >= (u1_))
#define UCOMPARE_DESC "Unsigned Comparison"
#endif
#if 1
#define UCOMPARE(u1_, u2_) (((u2_) - (u1_)) <= (unsigned int)INT_MAX)
#define UCOMPARE_DESC "Positive Delta"
#endif

void main ()
{
  int ri;

  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  BSP430_CORE_ENABLE_INTERRUPT();
  cputs("Wrapping timer comparisons: " UCOMPARE_DESC
        "\nRow indicates a scheduled time produced by adding to a running counter"
        "\nColumn represents a counter value"
        "\nCell indicates whether counter is considered >= time of row");
  cprintf("%10s%10s", "signed", "unsigned");
  for (ri = 0; ri < nvalues; ++ri) {
    cprintf("%8u", values[ri]);
  }
  for (ri = 0; ri < nvalues; ++ri) {
    int ci;
    cprintf("\n %8d  %8u ", values[ri], values[ri]);
    for (ci = 0; ci < nvalues; ++ci) {
      cprintf("%8u", UCOMPARE(values[ri], values[ci]));
    }
  }
  cputchar('\n');
}
