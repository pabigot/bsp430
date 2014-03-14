LPM ?= 4
LED ?= 0
AUX_CPPFLAGS += -DAPP_LPM=$(LPM) -DconfigBSP430_LED=$(LED)
PLATFORM ?= exp430fr5739
MODULES=$(MODULES_PLATFORM)
MODULES += lpm
SRC=main.c
include $(BSP430_ROOT)/make/Makefile.common
