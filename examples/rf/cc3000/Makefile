PLATFORM ?= exp430f5438
TEST_PLATFORMS=exp430f5438 trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += periph/port
MODULES += periph/sys
MODULES += periph/flash
MODULES += utility/cli

# Set this to where you installed the cc3000 headers and library
CC3000_ROOT ?= /usr/local/msp430-cc3000

# Tell BSP430's build infrastructure about the library.
AUX_CPPFLAGS=-I$(CC3000_ROOT)/include
AUX_LDFLAGS=-L$(CC3000_ROOT)/lib
AUX_LDLIBS=-lcc3000

# Use CC3000BOOST booster pack
ifdef CC3000BOOST
AUX_CPPFLAGS+=-DconfigBSP430_RF_CC3000BOOST=1
endif # CC3000BOOST

# EXP430G2 has too little memory
ifeq ($(PLATFORM),exp430g2)
AUX_CPPFLAGS+=-DBSP430_CC3000SPI_TX_BUFFER_SIZE=8
AUX_CPPFLAGS+=-DBSP430_CC3000SPI_RX_BUFFER_SIZE=8
endif # exp430g2

SRC=cc3000spi.c main.c
include $(BSP430_ROOT)/examples/Makefile.common
