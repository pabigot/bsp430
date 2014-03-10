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

# Auxiliary source files for the CC3000 host driver, when not using
# its library format.
CC3000_SRC=

# Tell BSP430's build infrastructure about the CC3000 host driver.
# Normally use the pre-built library in CC3000_ROOT; optionally build
# the source directly.  The latter is useful when you need to
# instrument the library with BSP430 operations like vBSP430setLed and
# cprintf.
ifdef CC3000_SRC_ROOT
# Source-build
AUX_CPPFLAGS=-I$(CC3000_SRC_ROOT)/include
CC3000_MODULES=cc3000_common evnt_handler hci netapp nvmem security socket wlan
CC3000_SRC=$(CC3000_MODULES:%=$(CC3000_SRC_ROOT)/%.c)
else # CC3000_SRC_ROOT
# Normal library link.
AUX_CPPFLAGS=-I$(CC3000_ROOT)/include
AUX_LDFLAGS=-L$(CC3000_ROOT)/lib
AUX_LDLIBS=-lcc3000
endif # CC3000_SRC_ROOT

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
# You may need to disable other commands to make room for
# these, or build with 20-bit support.
WITH_UPDATE ?= 0
ifneq (0,$(WITH_UPDATE))
AUX_CPPFLAGS += -DWITH_UPDATE
endif # WITH_UPDATE

# Optionally enable SmartConfig.
WITH_SMART ?= 0
ifneq (0,$(WITH_SMART))
AUX_CPPFLAGS += -DBSP430_CC3000_ENABLE_SMART
endif # WITH_SMART

# Optionally read profiles from local file
WITH_PROFILES ?= 0
ifneq (0,$(WITH_PROFILES))
AUX_CPPFLAGS += -DWITH_PROFILES
endif # WITH_PROFILES

SRC=cc3000spi.c main.c $(CC3000_SRC)
include $(BSP430_ROOT)/examples/Makefile.common
