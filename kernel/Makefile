################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2022 - The Poseidon Authors
##
################################################################################

# Project directory
export PROJECT_DIR	:= $(realpath $(shell pwd)/../)

include $(PROJECT_DIR)/scripts/make/version.mk
include $(PROJECT_DIR)/scripts/make/verbose.mk
include $(PROJECT_DIR)/scripts/make/platform.mk
include $(PROJECT_DIR)/scripts/make/profile.mk
include $(PROJECT_DIR)/scripts/make/localdir.mk

# Target
export ELF		:= $(TARGET_DIR)/kernel/$(NAME).elf

# Variables
CFLAGS	+= \
	-MD \
	-pipe \
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
	-isystem $(PROJECT_DIR)/include \
	-DKCONFIG_ARCH_$(ARCH) \
	-DKCONFIG_PLATFORM_$(PLATFORM) \

LDFLAGS	:= \
	-nostdlib \
	-lgcc \

# Objects
objs-y	:=

# Include platform-dependent variables and rules.
include platform/$(PLATFORM)/Platform.mk

# Include makefiles of top subfolders
include arch/$(ARCH)/Makefile
include	poseidon/Makefile
include platform/$(PLATFORM)/Makefile
include	lib/Makefile
include	driver/Makefile

# Prepend objects with target folder
objs-y	:= $(patsubst $(PROJECT_DIR)%, $(TARGET_DIR)%, $(objs-y))
deps	:= $(objs-y:.o=.d)

# Prepare target/ folder and include/arch/target symlink
$(shell mkdir -p $(dir $(objs-y)))
$(shell rm -f $(PROJECT_DIR)/include/arch/target)
$(shell ln -s $(PROJECT_DIR)/include/arch/$(ARCH)/ $(PROJECT_DIR)/include/arch/target)
$(shell rm -f $(PROJECT_DIR)/include/platform/target)
$(shell ln -s $(PROJECT_DIR)/include/platform/$(PLATFORM)/ $(PROJECT_DIR)/include/platform/target)


.PHONY:	all
all:	$(ELF)

.PHONY: clean
clean:
	$(Q)$(RM) -r $(TARGET_DIR)/kernel

.PHONY: re
re: clean
	$(Q)$(MAKE)

$(ELF): $(objs-y)
	$(Q)printf "  LD\t $(notdir $@)\n"
	$(Q)$(BUILD_CC) $(objs-y) $(CFLAGS) $(LDFLAGS) -o $(ELF)

-include $(deps)
$(TARGET_DIR)/%.o: $(PROJECT_DIR)/%.c
	$(Q)printf "  CC\t $*.c\n"
	$(Q)$(BUILD_CC) $(CFLAGS) -c $< -o $@

-include $(deps)
$(TARGET_DIR)/%.o: $(PROJECT_DIR)/%.S
	$(Q)printf "  AS\t $*.S\n"
	$(Q)$(BUILD_CC) $(CFLAGS) -c $< -o $@
