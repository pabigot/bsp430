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
WITH_UPDATE=0
endif # exp430g2

# Optionally enable the nvmem update infrastructure.
# This requires the presence of three files:
#  rm_param.inc         # has radio module default parameters
#  wlan_drv_patch.inc   # has patches to the CC3000 driver
#  fw_patch.inc         # has patches to the CC3000 firmware
# See the source for information on where to get these files.
WITH_UPDATE ?= 0
ifneq (0,$(WITH_UPDATE))
AUX_CPPFLAGS += -DWITH_UPDATE
endif # WITH_UPDATE

SRC=cc3000spi.c main.c
include $(BSP430_ROOT)/examples/Makefile.common
