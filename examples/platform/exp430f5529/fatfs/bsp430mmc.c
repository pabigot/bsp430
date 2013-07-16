/*------------------------------------------------------------------------/
/  BSP430 MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2012, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/ * This version modified for MSP430 use under BSP430: http://github.com/pabigot/bsp430
/
/--------------------------------------------------------------------------/
 Features and Limitations:

 * Very Easy to Port
   It uses only 4 bit of GPIO port. No interrupt, no SPI port is used.

 * Platform Independent
   You need to modify only a few macros to control GPIO ports.

 * Low Speed
   The data transfer rate will be several times slower than hardware SPI.

/-------------------------------------------------------------------------*/

/** [BSP430 Initialization] */

/* Include BSP430 material first, which will include msp430.h. */
#include <bsp430/serial.h>
#include <bsp430/clock.h>
#include <bsp430/periph/port.h>
#include <bsp430/utility/console.h>

#ifndef BSP430_MMC_FAST_HZ
/** Desired SPI bus speed after initialization.  Limited by SMCLK. */
#define BSP430_MMC_FAST_HZ 8000000UL
#endif /* BSP430_MMC_FAST_HZ */

#include "diskio.h"		/* Common include file for FatFs and disk I/O layer */

/*-------------------------------------------------------------------------*/
/* Platform dependent macros and functions needed to be modified           */
/*-------------------------------------------------------------------------*/

#define APP_SD_CS_PORT_HPL xBSP430hplLookupPORT(APP_SD_CS_PORT_PERIPH_HANDLE)
static hBSP430halSERIAL sdspi;

/* Following BSP430/POSIX conventions, this returns 0 if successful,
 * -1 on error. */
static int
configureSPIforSD (int fastp)
{
  unsigned int init_spi_divisor;
  volatile sBSP430hplPORT * miso_port = xBSP430hplLookupPORT(APP_SD_MISO_PORT_PERIPH_HANDLE);

  /* We'll drive the SPI device using SMCLK.  For initialization, we
   * need to stay below 400 kHz, and have chosen 380 kHz in case of
   * clock variances.  After initialization, we can go faster. */
  if (fastp) {
    init_spi_divisor = uiBSP430serialSMCLKPrescaler(BSP430_MMC_FAST_HZ);
  } else {
    init_spi_divisor = uiBSP430serialSMCLKPrescaler(380000UL);
  }
  /* SPI divisor must not be zero */
  if (0 == init_spi_divisor) {
    init_spi_divisor = 1;
  }

  /* Close the device if we already opened it. */
  if (sdspi) {
    (void)iBSP430serialClose(sdspi);
  }

  /* For some SD cards, need MISO pullup, or so we're told.  Do that
   * first, hoping the platform peripheral configuration won't destroy
   * it. */
  miso_port->dir &= ~APP_SD_MISO_PORT_BIT;
  BSP430_PORT_HPL_SET_REN(miso_port, APP_SD_MISO_PORT_BIT, BSP430_PORT_REN_PULL_UP);

  /* Configure SPI.  Probably ought to have a way to return an error
   * code.  Note: Per http://elm-chan.org/docs/mmc/mmc_e.html use SPI
   * mode 0 (CPOL=CPHA=0 via UCCKPH). */
  sdspi = hBSP430serialOpenSPI(hBSP430serialLookup(APP_SD_SPI_PERIPH_HANDLE),
                               BSP430_SERIAL_ADJUST_CTL0_INITIALIZER(UCCKPH | UCMSB | UCMST | UCMODE_0),
                               UCSSEL__SMCLK, init_spi_divisor);

  /* Configure the chip-select port. */
  APP_SD_CS_PORT_HPL->sel &= ~APP_SD_CS_PORT_BIT;
  APP_SD_CS_PORT_HPL->out |= APP_SD_CS_PORT_BIT;
  APP_SD_CS_PORT_HPL->dir |= APP_SD_CS_PORT_BIT;

  return (0 != sdspi) ? 0 : -1;
}

#define INIT_PORT() configureSPIforSD(0)
#define REINIT_PORT_FAST() configureSPIforSD(1)
#define DLY_US(n_) BSP430_CORE_DELAY_CYCLES(((n_) * BSP430_CLOCK_NOMINAL_MCLK_HZ)/1000000)

#define CS_H() do { APP_SD_CS_PORT_HPL->out |= APP_SD_CS_PORT_BIT; } while (0)
#define CS_L() do { APP_SD_CS_PORT_HPL->out &= ~APP_SD_CS_PORT_BIT; } while (0)

/** [BSP430 Initialization] */
/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* MMC/SD command (SPI mode) */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define CMD13	(13)		/* SEND_STATUS */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD41	(41)		/* SEND_OP_COND (ACMD) */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

static
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static
BYTE CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */

/**! [BSP430 SPI TxRx] */

/*-----------------------------------------------------------------------*/
/* Transmit bytes to the card (bitbanging)                               */
/*-----------------------------------------------------------------------*/

static
void xmit_mmc (
	const BYTE* buff,	/* Data to be sent */
	UINT bc				/* Number of bytes to send */
)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);

  BSP430_CORE_DISABLE_INTERRUPT();
  (void)iBSP430spiTxRx_rh(sdspi, buff, bc, 0, 0);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}



/*-----------------------------------------------------------------------*/
/* Receive bytes from the card (bitbanging)                              */
/*-----------------------------------------------------------------------*/

static
void rcvr_mmc (
	BYTE *buff,	/* Pointer to read buffer */
	UINT bc		/* Number of bytes to receive */
)
{
  BSP430_CORE_SAVED_INTERRUPT_STATE(istate);

  BSP430_CORE_DISABLE_INTERRUPT();
  (void)iBSP430spiTxRx_rh(sdspi, NULL, 0, bc, buff);
  BSP430_CORE_RESTORE_INTERRUPT_STATE(istate);
}

/**! [BSP430 SPI TxRx] */

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
int wait_ready (void)	/* 1:OK, 0:Timeout */
{
	BYTE d;
	UINT tmr;


	for (tmr = 5000; tmr; tmr--) {	/* Wait for ready in timeout of 500ms */
		rcvr_mmc(&d, 1);
		if (d == 0xFF) break;
		DLY_US(100);
	}

	return tmr ? 1 : 0;
}



/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void deselect (void)
{
	BYTE d;

	CS_H();
	rcvr_mmc(&d, 1);	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static
int select (void)	/* 1:OK, 0:Timeout */
{
	BYTE d;

	CS_L();
	rcvr_mmc(&d, 1);	/* Dummy clock (force DO enabled) */

	if (wait_ready()) return 1;	/* OK */
	deselect();
	return 0;			/* Failed */
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from the card                                   */
/*-----------------------------------------------------------------------*/

static
int rcvr_datablock (	/* 1:OK, 0:Failed */
	BYTE *buff,			/* Data buffer to store received data */
	UINT btr			/* Byte count */
)
{
	BYTE d[2];
	UINT tmr;


	for (tmr = 1000; tmr; tmr--) {	/* Wait for data packet in timeout of 100ms */
		rcvr_mmc(d, 1);
		if (d[0] != 0xFF) break;
		DLY_US(100);
	}
	if (d[0] != 0xFE) return 0;		/* If not valid data token, return with error */

	rcvr_mmc(buff, btr);			/* Receive the data block into buffer */
	rcvr_mmc(d, 2);					/* Discard CRC */

	return 1;						/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to the card                                        */
/*-----------------------------------------------------------------------*/

static
int xmit_datablock (	/* 1:OK, 0:Failed */
	const BYTE *buff,	/* 512 byte data block to be transmitted */
	BYTE token			/* Data/Stop token */
)
{
	BYTE d[2];


	if (!wait_ready()) return 0;

	d[0] = token;
	xmit_mmc(d, 1);				/* Xmit a token */
	if (token != 0xFD) {		/* Is it data token? */
		xmit_mmc(buff, 512);	/* Xmit the 512 byte data block to MMC */
		rcvr_mmc(d, 2);			/* Xmit dummy CRC (0xFF,0xFF) */
		rcvr_mmc(d, 1);			/* Receive data response */
		if ((d[0] & 0x1F) != 0x05)	/* If not accepted, return with error */
			return 0;
	}

	return 1;
}



/*-----------------------------------------------------------------------*/
/* Send a command packet to the card                                     */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (		/* Returns command response (bit7==1:Send failed)*/
	BYTE cmd,		/* Command byte */
	DWORD arg		/* Argument */
)
{
	BYTE n, d, buf[6];


	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		n = send_cmd(CMD55, 0);
		if (n > 1) return n;
	}

	/* Select the card and wait for ready */
	deselect();
	if (!select()) return 0xFF;

	/* Send a command packet */
	buf[0] = 0x40 | cmd;			/* Start + Command index */
	buf[1] = (BYTE)(arg >> 24);		/* Argument[31..24] */
	buf[2] = (BYTE)(arg >> 16);		/* Argument[23..16] */
	buf[3] = (BYTE)(arg >> 8);		/* Argument[15..8] */
	buf[4] = (BYTE)arg;				/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* (valid CRC for CMD0(0)) */
	if (cmd == CMD8) n = 0x87;		/* (valid CRC for CMD8(0x1AA)) */
	buf[5] = n;
	xmit_mmc(buf, 6);

	/* Receive command response */
	if (cmd == CMD12) rcvr_mmc(&d, 1);	/* Skip a stuff byte when stop reading */
	n = 10;								/* Wait for a valid response in timeout of 10 attempts */
	do
		rcvr_mmc(&d, 1);
	while ((d & 0x80) && --n);

	return d;			/* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE drv			/* Drive number (always 0) */
)
{
	DSTATUS s;
	BYTE d;


	if (drv) return STA_NOINIT;

	/* Check if the card is kept initialized */
	s = Stat;
	if (!(s & STA_NOINIT)) {
		if (send_cmd(CMD13, 0))	/* Read card status */
			s = STA_NOINIT;
		rcvr_mmc(&d, 1);		/* Receive following half of R2 */
		deselect();
	}
	Stat = s;

	return s;
}



/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	BYTE n, ty, cmd, buf[4];
	UINT tmr;
	DSTATUS s;


	if (drv) return RES_NOTRDY;

	if (0 != INIT_PORT()) {				/* Initialize control port */
		return RES_ERROR;
	}

	for (n = 10; n; n--) rcvr_mmc(buf, 1);	/* 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
			rcvr_mmc(buf, 4);							/* Get trailing return value of R7 resp */
			if (buf[2] == 0x01 && buf[3] == 0xAA) {		/* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state (ACMD41 with HCS bit) */
					if (send_cmd(ACMD41, 1UL << 30) == 0) break;
					DLY_US(1000);
				}
				if (tmr && send_cmd(CMD58, 0) == 0) {	/* Check CCS bit in the OCR */
					rcvr_mmc(buf, 4);
					ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			for (tmr = 1000; tmr; tmr--) {			/* Wait for leaving idle state */
				if (send_cmd(cmd, 0) == 0) break;
				DLY_US(1000);
			}
			if (!tmr || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
				ty = 0;
		}
	}
	CardType = ty;
	s = ty ? 0 : STA_NOINIT;
	Stat = s;

	deselect();

        /* If initialization succeeded, speed up the SPI clock */
        if (! (Stat & STA_NOINIT)) {
          if (0 != REINIT_PORT_FAST()) {
            return RES_ERROR;
          }
        }

	return s;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,			/* Physical drive nmuber (0) */
	BYTE *buff,			/* Pointer to the data buffer to store read data */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..128) */
)
{
	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;
	if (!count) return RES_PARERR;
	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert LBA to byte address if needed */

	if (count == 1) {	/* Single block read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512))
			count = 0;
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0) */
	const BYTE *buff,	/* Pointer to the data to be written */
	DWORD sector,		/* Start sector number (LBA) */
	BYTE count			/* Sector count (1..128) */
)
{
	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;
	if (!count) return RES_PARERR;
	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert LBA to byte address if needed */

	if (count == 1) {	/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */
			&& xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16];
	DWORD cs;


	if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;	/* Check if card is in the socket */

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :		/* Make sure that no pending write process */
			if (select()) {
				deselect();
				res = RES_OK;
			}
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
					cs = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 8) + 1;
					*(DWORD*)buff = cs << 10;
				} else {					/* SDC ver 1.XX or MMC */
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					cs = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = cs << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
			*(DWORD*)buff = 128;
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
	}

	deselect();

	return res;
}



/*-----------------------------------------------------------------------*/
/* This function is defined for only project compatibility               */

void disk_timerproc (void)
{
	/* Nothing to do */
}

DWORD
get_fattime (void)
{
  union {
    DWORD dword;
    struct {
      DWORD year:7;
      DWORD month:4;
      DWORD dom:5;
      DWORD hour:5;
      DWORD minute:6;
      DWORD sec_div_2:5;
    };
  } bit_time;
  bit_time.dword = 0;
  bit_time.year = 0;
  bit_time.month = 1;
  bit_time.dom = 1;

  return bit_time.dword;
}
