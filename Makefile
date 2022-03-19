################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2022 - The Poseidon Authors
##
################################################################################

# Project directory
export PROJECT_DIR	:= $(shell pwd)

# Define a few rules and variables used a bit everywhere
include $(PROJECT_DIR)/scripts/make/version.mk
include $(PROJECT_DIR)/scripts/make/verbose.mk
include $(PROJECT_DIR)/scripts/make/platform.mk
include $(PROJECT_DIR)/scripts/make/profile.mk

# Targets made by other scripts or Makefiles
export ISO		:= $(TARGET_DIR)/$(NAME).iso
export ELF		:= $(TARGET_DIR)/kernel/$(NAME).elf

# Do not print "Entering directory ..."
MAKEFLAGS		+= --no-print-directory

# Number of cores to emulate (QEMU)
export SMP		?= 1


.PHONY:	all
all:	iso

.PHONY:		cross_cc
cross_cc:	$(BUILD_CC)

$(BUILD_CC):
	printf "  SH\t scripts/build-crosscompiler.sh\n"
	$(Q)./scripts/build-crosscompiler.sh $(CROSS_DIR) $(TARGET_TRIPLET)

.PHONY:	kernel
kernel:	check_kernel
	$(Q)$(MAKE) -C kernel

.PHONY: check_kernel
check_kernel: cross_cc
	$(Q)$(MAKE) -C kernel

.PHONY: iso
iso:	check_iso
	$(Q)$(MAKE) $(ISO)

.PHONY:	check_iso
check_iso: check_kernel

$(ISO):	$(ELF)
	$(Q)printf "  SH\t scripts/gen-iso.sh\n"
	$(Q)./scripts/gen-iso.sh -o $@ $<
	$(Q)printf "\nDone. You can run the iso through QEMU using 'make run'\n"

.PHONY:	run
run:	iso
	$(Q)./scripts/run.sh -s $(SMP) "$(ISO)"

.PHONY: monitor
monitor: iso
	$(Q)./scripts/run.sh -s $(SMP) -t "$(ISO)"

.PHONY: debug
debug: iso
	$(Q)./scripts/run.sh -s $(SMP) -d "$(ISO)"

.PHONY: gdb
gdb: iso
	$(Q)./scripts/run.sh -s $(SMP) -g "$(ISO)"

.PHONY: kvm
kvm: iso
	$(Q)./scripts/run.sh -s $(SMP) -k "$(ISO)"

.PHONY: clean
clean:
	$(Q)$(MAKE) -C kernel clean
	$(Q)$(RM) -rf $(ISO)

.PHONY: fclean
fclean: clean
	$(Q)$(RM) -rf $(CROSS_DIR)

.PHONY: re
re: clean
	$(Q)$(MAKE)

.PHONY: ? help
? help:
	$(Q)printf "Poseidon's main Makefile\n"
	$(Q)printf "\n"
	$(Q)printf "\t- make help        Show this help\n"
	$(Q)printf "\n"
	$(Q)printf "\t- make profile=    Indicate which target profile to use (\"debug\" or \"release\")\n"
	$(Q)printf "\t- make platform=   Indicate which platform to target (Default is \"pc\")\n"
	$(Q)printf "\n"
	$(Q)printf "\t- make all         Build all targets (cross-compiler, kernel and iso)\n"
	$(Q)printf "\t- make cross_cc    Build the cross-compiler\n"
	$(Q)printf "\t- make kernel      Build the kernel\n"
	$(Q)printf "\t- make iso         Build a complete iso using grub-mkrescue\n"
	$(Q)printf "\t- make clean       Clean object files\n"
	$(Q)printf "\t- make fclean      Clean both object files and the cross-compiler\n"
	$(Q)printf "\t- make re          Clean object files and build all targets\n"
	$(Q)printf "\n"
	$(Q)printf "\t- make run         Run Poseidon's iso through QEMU, using the default configuration\n"
	$(Q)printf "\t- make monitor     Run Poseidon's iso through QEMU, using QEMU's minotor mode\n"
	$(Q)printf "\t- make debug       Run Poseidon's iso through QEMU, using QEMU's debug mode\n"
	$(Q)printf "\t- make gdb         Run Poseidon's iso through QEMU, connecting to a GDB server on localhost:1234\n"
	$(Q)printf "\t- make kvm         Run Poseidon's iso through QEMU, using KVM\n"
