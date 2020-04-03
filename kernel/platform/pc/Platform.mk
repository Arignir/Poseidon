################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2020 - The Poseidon Authors
##
################################################################################

ldir	:= $(GET_LOCAL_DIR)

# Define the target cpu architecture
export ARCH		:= x86_64

# Define the host, build and target triplet used when compiling the cross-compiler
export HOST_TRIPLET	?= $(shell gcc -dumpmachine)
export BUILD_TRIPLET	?= $(HOST_TRIPLET)
export TARGET_TRIPLET	?= x86_64-elf

# Define a set of arguments for QEMU to emulate this platform.
export QEMU		?= qemu-system-x86_64
export QEMU_ARGS	?= -cpu Broadwell -m 512 -serial stdio

# Include the x86_64-specific make rules.
include	$(PROJECT_DIR)/kernel/arch/x86_64/Arch.mk
