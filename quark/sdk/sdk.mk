SDK := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
TOOLCHAIN := $(SDK)/toolchain_$(shell uname -m)
CROSS_COMPILE := $(TOOLCHAIN)/bin/i686-elf-

INC += -I"$(SDK)/include"
INC += -I"$(TOOLCHAIN)/i686-elf/include"
INC += -I"$(TOOLCHAIN)/i686-elf/usr/include"
INC += -I"$(TOOLCHAIN)/lib/gcc/i686-elf/4.6.1/include"
INC += -I"$(TOOLCHAIN)/lib/gcc/i686-elf/4.6.1/include-fixed"

CFLAGS += $(INC)
CFLAGS += -fno-asynchronous-unwind-tables
CFLAGS += -fmessage-length=0
CFLAGS += -fdelete-null-pointer-checks
CFLAGS += -ffast-math
CFLAGS += -Os -s -Wall -std=gnu99

LDFLAGS += -T $(SDK)/lib/mcu.lds
LDFLAGS += -static
LDFLAGS += -L"$(SDK)/lib"
LDFLAGS += -nostdlib
LDFLAGS += -Wl,-X,-N,--gc-section,-X,-N,-Ttext,0xFF300000,-e,__Start,--no-undefined

LIB += $(SDK)/lib/intel_mcu.a
LIB += -lc -lgcc

%.raw: %.elf
	$(OBJCOPY) -j .pshinit -j .builtin_fw -O binary -j .text -j .rodata \
	    -j .data -j .bss --set-section-flags .bss=alloc,load,contents $< $@

%.bin: %.raw
	perl -e "print pack 'x724V', `stat -L -c %s $<`/4" > $@
	cat $< >> $@
