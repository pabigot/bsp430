TEST_PLATFORMS=trxeb exp430f5438 exp430f5529
SHARP96 ?= 0
U8GLIB_ROOT ?= /opt/u8glib
U8GLIB_CSRC = $(U8GLIB_ROOT)/csrc
AUX_CPPFLAGS = -I$(U8GLIB_CSRC)
PLATFORM ?= trxeb
MODULES=$(MODULES_PLATFORM)
MODULES += $(MODULES_UPTIME)
MODULES += $(MODULES_CONSOLE)
MODULES += $(MODULES_SERIAL)
MODULES += periph/port
ifneq (0,$(SHARP96))
AUX_CPPFLAGS += -DconfigBSP430_PLATFORM_BOOSTERPACK_SHARP96=1
VPATH += $(BSP430_ROOT)/src/boosterpack/sharp96
MODULES += utility/sharplcd
PLATFORM_U8G_PB_C ?= $(U8GLIB_CSRC)/u8g_pb8h1.c
endif # SHARP96

U8GLIB_SRC = \
    $(U8GLIB_CSRC)/u8g_com_api.c \
    $(U8GLIB_CSRC)/u8g_ll_api.c \
    $(U8GLIB_CSRC)/u8g_font.c \
    $(U8GLIB_CSRC)/u8g_line.c \
    $(U8GLIB_CSRC)/u8g_clip.c \
    $(U8GLIB_CSRC)/u8g_page.c \
    $(U8GLIB_CSRC)/u8g_state.c \
    $(U8GLIB_CSRC)/u8g_pb.c \
    $(PLATFORM_U8G_PB_C) \
    $(U8GLIB_ROOT)/fntsrc/u8g_font_10x20.c
SRC = $(U8GLIB_SRC) u8g_bsp430.c main.c
include $(BSP430_ROOT)/examples/Makefile.common
