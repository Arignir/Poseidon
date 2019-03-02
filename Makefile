################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
##
################################################################################

# Version
export MAJOR		:= 0
export MINOR		:= 1
export PATCH		:= 0
export EXTRA		:= -indev
export VERSION		:= $(MAJOR).$(MINOR).$(PATCH)$(EXTRA)
export NAME		:= poseidon_$(VERSION)

# Verbosity
export Q		:=
export V		?= 0
ifeq ($(V), 0)
	MAKEFLAGS	+= --silent
	Q		:= @
endif

# Target arch & triplet
export ARCH		?= x86
export TARGET_TRIPLET	?= i686-elf

# Project directory
export PROJDIR		:= $(shell pwd)

# Create output directory
export OUTDIR		:= $(PROJDIR)/target
ifneq ($(shell mkdir -p $(OUTDIR) && cd $(OUTDIR) && pwd), $(OUTDIR))
	$(error Couldn't create output directory "$(OUTDIR)")
endif

# Targets
export ISO		:= $(OUTDIR)/$(NAME).iso
export ELF		:= $(OUTDIR)/kernel/$(NAME).elf

# Do not print "Entering directory ..."
MAKEFLAGS		+= --no-print-directory

.PHONY:	all
all:	iso

.PHONY:	kernel
kernel:	check_kernel
	$(Q)$(MAKE) -C kernel

.PHONY: check_kernel
check_kernel:
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
	$(Q)./scripts/run.sh -a "$(ARCH)" "$(ISO)"

.PHONY: monitor
monitor: iso
	$(Q)./scripts/run.sh -t -a "$(ARCH)" "$(ISO)"

.PHONY: debug
debug: iso
	$(Q)./scripts/run.sh -d -t -a "$(ARCH)" "$(ISO)"

.PHONY: kvm
kvm: iso
	$(Q)./scripts/run.sh -k -a "$(ARCH)" "$(ISO)"

.PHONY: kvm_monitor
kvm_monitor: iso
	$(Q)./scripts/run.sh -t -k -a "$(ARCH)" "$(ISO)"


.PHONY: clean
clean:
	$(Q)$(RM) -r $(OUTDIR)

.PHONY: re
re: clean
	$(Q)$(MAKE)

.PHONY: ? help
? help:
	$(Q)printf "Poseidon's main Makefile\n"
	$(Q)printf "\n"
	$(Q)printf "\t- make help     Shows this help\n"
	$(Q)printf "\n"
	$(Q)printf "\t- make all      Builds all targets (kernel and iso)\n"
	$(Q)printf "\t- make kernel   Builds the kernel\n"
	$(Q)printf "\t- make iso      Builds a complete iso using grub-mkrescue\n"
	$(Q)printf "\t- make clean    Cleans object files\n"
	$(Q)printf "\t- make re       Cleans object files and builds all targets\n"
	$(Q)printf "\n"
	$(Q)printf "\t- make run      Runs Poseidon's iso through QEMU, using the default configuration\n"
	$(Q)printf "\t- make monitor  Runs Poseidon's iso through QEMU, using QEMU's minotor mode\n"
	$(Q)printf "\t- make debug    Runs Poseidon's iso through QEMU, using QEMU's debug mode\n"
	$(Q)printf "\t- make kvm      Runs Poseidon's iso through QEMU, using KVM\n"
