/** This file is in the public domain.
 *
 * This program demonstrates use of the CMA3000-D01 accelerometer on
 * the TrxEB board.
 *
 * @warning The silkscreen showing accelerometer orientation on the
 * bottom of the board should be interpreted as if it were printed on
 * the top of the board.  I.e., positive X from the left to the right
 * of the LCD, and positive Y is from the bottom to the top of the
 * LCD.  Interpreted as printed the X axis is flipped.
 *
 * @homepage http://github.com/pabigot/bsp430
 *
 */

#include <bsp430/platform.h>
#include <bsp430/clock.h>
#include <bsp430/utility/uptime.h>
#include <bsp430/utility/console.h>
#include <bsp430/serial.h>
#include <bsp430/periph/port.h>
#include <string.h>

enum {
  R_WHO_AM_I = 0,
  R_REVID = 1,
  R_CTRL = 2,
  /* NB: Reading R_STATUS is supposed to clear PORST, but this doesn't
   * actually happen until CS is deasserted. */
  R_STATUS = 3,
  R_RSTR = 4,
  R_INT_STATUS = 5,
  R_DOUTX = 6,
  R_DOUTY = 7,
  R_DOUTZ = 8,
  R_MDTHR = 9,
  R_MDFFTMR = 10,
  R_FFTHR = 11,
  R_I2C_ADDR = 12,
  R_RegisterLimit
};

#define RESP_VALID(v_) (!!((v_) & 0x02))
#define RESP_PORST(v_) (!!((v_) & 0x38))

/** Convert a register index to the SPI command that reads the register */
#define REG_TO_READ(r_) ((r_) << 2)

/** Convert a register index to the SPI command that writes the register */
#define REG_TO_WRITE(r_) (2 | ((r_) << 2))

/* Accelerometer SPI interface */
static hBSP430halSERIAL spi_;

/* Accelerometer CSn HPL */
static volatile sBSP430hplPORT * hplCSn_;

/* Assert chip select by clearing CSn */
#define CS_ASSERT() do {                                                \
    hplCSn_->out &= ~BSP430_PLATFORM_TRXEB_ACCEL_CSn_PORT_BIT;          \
  } while (0)

/* De-assert chip select by setting CSn */
#define CS_DEASSERT() do {                                             \
    hplCSn_->out |= BSP430_PLATFORM_TRXEB_ACCEL_CSn_PORT_BIT;          \
  } while (0)


/** Bits within R_CTRL values */
enum CTRL_MODE {
  CTRL_G_RANGE_2G = 0x80,
  CTRL_I2C_DIS = 0x10,
  CTRL_OFF_DEFAULT = 0x00,
  CTRL_MEAS_100Hz = 0x02,
  CTRL_MEAS_400Hz = 0x04,
  CTRL_MEAS_40Hz = 0x06,
  CTRL_MOTION_10Hz = 0x08,
  CTRL_FF_100Hz = 0x0A,
  CTRL_FF_400Hz = 0x0C,
  CTRL_OFF = 0x0E,
  CTRL_INT_DIS = 0x01
};

/** At 2g measuring either 100Hz or 400Hz ODR the system uses 56
 * counts per g of acceleration. */
#define COUNT_PER_G_56 56

/** At at 8g measuring or slower ODRs the system uses 14 counts per g
 * of acceleration. */
#define COUNT_PER_G_14 14

#define ACCEL_mg(meas_, cpg_) ((1000L * (int8_t)(meas_)) / (cpg_))

int cma3000read (uint8_t reg,
                 uint8_t * valp)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  uint8_t resp[2];
  int rc;
  
  reg = REG_TO_READ(reg);
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    CS_ASSERT();
    rc = iBSP430spiTxRx_ni(spi_, &reg, sizeof(reg), 1, resp);
    CS_DEASSERT();
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
      
  //cprintf("send 0x%02x got %x %x res %d  ", reg, resp[0], resp[1], rc);
  if ((sizeof(resp) == rc) && RESP_VALID(resp[0])) {
    *valp = resp[1];
    rc = RESP_PORST(resp[0]);
  } else {
    rc = -1;
  }
  return rc;
}

/* Simpler read for the common case where SPI is expected to work and
 * we don't care whether the chip believes itself to have been
 * reset. */
int readreg (uint8_t reg)
{
  uint8_t val;
  int rc = cma3000read(reg, &val);
  if (0 <= rc) {
    return val;
  }
  return -1;
}

int cma3000write (uint8_t reg,
                  uint8_t val)
{
  BSP430_CORE_INTERRUPT_STATE_T istate;
  uint8_t outbuf[2];
  int rc;
  
  outbuf[0] = REG_TO_WRITE(reg);
  outbuf[1] = val;
  
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    CS_ASSERT();
    rc = iBSP430spiTxRx_ni(spi_, outbuf, sizeof(outbuf), 0, NULL);
    CS_DEASSERT();
  } while (0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  //cprintf("\nwrote 0x%02x to %d got %d\n", val, reg, rc);
  return rc;
}

int writereg (uint8_t reg, uint8_t val)
{
  int reps = 0;
  int rc;
  int readback;

  /* At least for the CTRL register when setting MODE fields, it may
   * be necessary to write multiple times before the command takes.
   * It really is multiple writes that's needed: writing once
   * (apparently successfully) then waiting for the readback to match
   * does not work. */
  do {
    ++reps;
    rc = cma3000write(reg, val);
    readback = readreg(reg);
    //cprintf("reg %u wrote %x res %d read %x\n", reg, val, rc, readback);
  } while ((0 <= rc) && (val != readback));
  //cprintf("wrote %d times to get result\n", reps);
  return 0;
}

/** Read acceleration data without using an interrupt.
 *
 * Because the reads are not synchronized with updates in this mode,
 * reads must be repeated until the same XYZ sample is read twice. */
int cma3000sample (int cpg,
                   int * px_mg,
                   int * py_mg,
                   int * pz_mg)
{
  static const uint8_t req_in[6] = { REG_TO_READ(R_DOUTX), 0,
                                     REG_TO_READ(R_DOUTY), 0,
                                     REG_TO_READ(R_DOUTZ), 0 };
  uint8_t req_out[6];
  uint8_t last[3];
  int reps = 0;
  int rc;
  BSP430_CORE_INTERRUPT_STATE_T istate;
  
  BSP430_CORE_SAVE_INTERRUPT_STATE(istate);
  BSP430_CORE_DISABLE_INTERRUPT();
  do {
    ++reps;
    last[0] = req_out[1];
    last[1] = req_out[3];
    last[2] = req_out[5];
    /* SPI timing mandates 22us delay between cycles.  However, it is
     * legal to have multiple 16-bit frames within a single cycle. */
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_US_TO_NOMINAL_MCLK(22));
    CS_ASSERT();
    rc = iBSP430spiTxRx_ni(spi_, req_in, sizeof(req_in), 0, req_out);
    CS_DEASSERT();
  } while ((sizeof(req_in) == rc)
           && ((1 == reps)
               || (last[0] != req_out[1])
               || (last[1] != req_out[3])
               || (last[2] != req_out[5])));
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
  // cprintf("rep %u res %d cpg %d raw: %x %x %x %x %x %x\n", reps, rc, cpg, req_out[0], req_out[1], req_out[2], req_out[3], req_out[4], req_out[5]);
  if (sizeof(req_in) != rc) {
    return -1;
  }
  if (px_mg) {
    *px_mg = ACCEL_mg(last[0], cpg);
  }
  if (py_mg) {
    *py_mg = ACCEL_mg(last[1], cpg);
  }
  if (pz_mg) {
    *pz_mg = ACCEL_mg(last[2], cpg);
  }
  return 0;
}

void dumpState (void)
{
  int i;
    
  for (i = 0; i < R_RegisterLimit ; ++i) {
    uint8_t v = -1;
    int rc;
    
    rc = cma3000read(i, &v);
    cprintf("Reg %u got %d val 0x%02x\n", i, rc, v);
  }
}

void main ()
{
  uint8_t ctrl;
  int cpg;
  unsigned int divisor;
  int rc;
  
  vBSP430platformInitialize_ni();
  (void)iBSP430consoleInitialize();

  divisor = (BSP430_CLOCK_NOMINAL_MCLK_HZ >> BSP430_CLOCK_NOMINAL_SMCLK_DIVIDING_SHIFT) / 300000U;
  if (0 == divisor) {
    divisor = 1;
  }
  spi_ = hBSP430serialOpenSPI(hBSP430serialLookup(BSP430_PLATFORM_TRXEB_ACCEL_SPI_PERIPH_HANDLE),
                              BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCMSB | UCMST),
                              UCSSEL_2, divisor);
  cprintf("SPI at %p is %s divisor %u\n", spi_, xBSP430serialName(BSP430_PLATFORM_TRXEB_ACCEL_SPI_PERIPH_HANDLE), divisor);

  /* Deassert chip select for accelerometer */
  hplCSn_ = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_ACCEL_CSn_PORT_PERIPH_HANDLE);
  hplCSn_->dir |= BSP430_PLATFORM_TRXEB_ACCEL_CSn_PORT_BIT;
  CS_DEASSERT();

  {
    volatile sBSP430hplPORT * hpl;

    /* Turn on power */
    hpl = xBSP430hplLookupPORT(BSP430_PLATFORM_TRXEB_ACCEL_PWR_PORT_PERIPH_HANDLE);
    hpl->out &= ~BSP430_PLATFORM_TRXEB_ACCEL_PWR_PORT_BIT;
    hpl->dir |= BSP430_PLATFORM_TRXEB_ACCEL_PWR_PORT_BIT;
    hpl->out |= BSP430_PLATFORM_TRXEB_ACCEL_PWR_PORT_BIT;
  }

  /* The first few reads fail.  Data sheet does not provide a power-up
   * initialization time. */
  {
    int reps = 0;
    do {
      ++reps;
      rc = readreg(R_CTRL);
    } while (0 > rc);
    cprintf("Valid read after %u attempts\n", reps);
  }

  cprintf("CMA3000 ID %02x rev %02x state %02x CTRL %02x\n",
          readreg(R_WHO_AM_I), readreg(R_REVID), readreg(R_STATUS), readreg(R_CTRL));
#if 1
  /* "High-resolution" */
  cpg = COUNT_PER_G_56;
  ctrl = CTRL_G_RANGE_2G | CTRL_MEAS_100Hz | CTRL_INT_DIS;
#else
  cpg = COUNT_PER_G_14;
  ctrl = CTRL_G_RANGE_2G | CTRL_MEAS_40Hz | CTRL_INT_DIS;
#endif
  rc = writereg(R_CTRL, ctrl);
  cprintf("CTRL assign %02x got %d readback %02x\n", ctrl, rc, readreg(R_CTRL));

  dumpState();
  
  /* Need to enable interrupts so timer overflow events are properly
   * acknowledged */
  BSP430_CORE_ENABLE_INTERRUPT();
  while (1) {
    int x_mg;
    int y_mg;
    int z_mg;

    rc = cma3000sample(cpg, &x_mg, &y_mg, &z_mg);
    cprintf("%d %d %d\n", x_mg, y_mg, z_mg);
    BSP430_CORE_DELAY_CYCLES(BSP430_CLOCK_NOMINAL_MCLK_HZ);
  }
}
