OBJS = detection.o

BINARY = main

OPENCM3_DIR	= ../.opencm3/libopencm3
LDSCRIPT	= $(OPENCM3_DIR)/lib/stm32/f1/stm32f103x8.ld

# Target configuration
LIBNAME		= opencm3_stm32f1
DEFS		+= -DSTM32F1
FP_FLAGS	?= -msoft-float
ARCH_FLAGS	= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd

# OpenOCD specific variables
OOCD		?= openocd
OOCD_INTERFACE	?= stlink-v2
OOCD_TARGET	?= stm32f1x

include ../.opencm3/libopencm3.rules.mk
