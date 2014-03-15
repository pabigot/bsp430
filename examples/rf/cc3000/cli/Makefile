PLATFORM ?= exp430f5438
TEST_PLATFORMS=exp430f5438 trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += periph/port
MODULES += periph/sys
MODULES += periph/flash
MODULES += utility/cli

# Add the rules to provide external library support for the CC3000
include $(BSP430_ROOT)/make/Makefile.cc3000

# Optionally enable the nvmem update infrastructure.
# This requires the presence of two files:
#  wlan_drv_patch.inc   # has patches to the CC3000 driver
#  fw_patch.inc         # has patches to the CC3000 firmware
# See the source for information on where to get these files.
# You may need to disable other commands to make room for
# these, or build with 20-bit support.
WITH_UPDATE ?= 0
ifneq (0,$(WITH_UPDATE))
AUX_CPPFLAGS += -DWITH_UPDATE
endif # WITH_UPDATE

# Optionally enable radio module comparison
# This requires the presence of the following file:
#  rm_param.inc         # has radio module default parameters
# See the source for information on where to get these files.
WITH_RMPARAM ?= 0
ifneq (0,$(WITH_RMPARAM))
AUX_CPPFLAGS += -DWITH_RMPARAM
endif # WITH_RMPARAM

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

SRC += main.c
include $(BSP430_ROOT)/make/Makefile.common
