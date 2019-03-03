################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
##
################################################################################

# Compiler
CC	:= $(TARGET_TRIPLET)-gcc
AS	:= $(TARGET_TRIPLET)-as
LD	:= $(TARGET_TRIPLET)-ld

# Variables
CFLAGS	:= \
	-MD \
	-W \
	-Wall \
	-Wextra \
	-Wcast-align \
	-Winline \
	-Wno-format \
	-Wno-missing-braces \
	-Wpointer-arith \
	-Wredundant-decls \
	-Wshadow \
	-Wstrict-prototypes \
	-Wuninitialized \
	-Wwrite-strings \
	-static \
	-ffreestanding \
	-fms-extensions \
	-fno-omit-frame-pointer \
	-fno-stack-protector \
	-std=gnu11 \
	-isystem $(PROJDIR)/include

# Add new CFLAGS when building the "release" target.
ifeq ($(TARGET), release)
CFLAGS	+= \
	-O2 \
	-DRELEASE
endif

# Add new CFLAGS when building the "debug" target.
ifeq ($(TARGET), debug)
CFLAGS	+= \
	-g3 \
	-DDEBUG
endif


LDFLAGS	:= \
	-nostdlib \
	-lgcc

# Include arch-dependent variables and rules.
include arch/$(ARCH)/Arch.mk

# Objects
objs-y	:=

# Include makefiles of top subfolders
include arch/$(ARCH)/Makefile
include	lib/Makefile
include	driver/Makefile
include	poseidon/Makefile

# Prepend objects with target folder
objs-y	:= $(patsubst $(PROJDIR)%, $(OUTDIR)%, $(objs-y))
deps	:= $(objs-y:.o=.d)

# Prepare target folder & arch symlink
$(shell mkdir -p $(dir $(objs-y)))
$(shell $(RM) $(PROJDIR)/include/arch/current && ln -s $(PROJDIR)/include/arch/$(ARCH)/ $(PROJDIR)/include/arch/current)

.PHONY:	all
all:	$(ELF)

$(ELF): $(objs-y)
	$(Q)printf "  LD\t $(notdir $@)\n"
	$(Q)$(CC) $(objs-y) $(CFLAGS) $(LDFLAGS) -o $(ELF)

-include $(deps)
$(OUTDIR)/%.o: $(PROJDIR)/%.c
	$(Q)printf "  CC\t $*.c\n"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@

-include $(deps)
$(OUTDIR)/%.o: $(PROJDIR)/%.S
	$(Q)printf "  AS\t $*.S\n"
	$(Q)$(CC) $(CFLAGS) -c $< -o $@
