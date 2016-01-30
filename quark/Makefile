include ../py/mkenv.mk

QSTR_DEFS = qstrdefsport.h
include ../py/py.mk

all: $(BUILD)/intel_mcu.bin

PROG = $(BUILD)/intel_mcu.elf

SRC += $(wildcard *.c)
SRC += lib/utils/printf.c
SRC += lib/utils/pyexec.c
SRC += lib/libc/string0.c
SRC += lib/mp-readline/readline.c

INC += -I. -I.. -I../lib/mp-readline -I$(BUILD)

OBJ = $(PY_O) $(addprefix $(BUILD)/,$(SRC:%.c=%.o))

include sdk/sdk.mk

include ../py/mkrules.mk
