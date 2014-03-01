PLATFORM ?= trxeb
# Test requires >2 kiB RAM for time conversion support
TEST_PLATFORMS_EXCLUDE=exp430fr5739 exp430g2 rf2500t wolverine exp430fr5969
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
VPATH += $(BSP430_ROOT)/src/chips
MODULES += chips/ds3231
SRC=main.c
include $(BSP430_ROOT)/examples/Makefile.common
