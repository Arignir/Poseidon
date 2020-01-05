################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2020 - The Poseidon Authors
##
################################################################################

# Target arch & triplet
export ARCH		?= x86
export TARGET_TRIPLET	?= i686-elf
export BUILD_TRIPLET	?= i686-elf
export HOST_TRIPLET	?= $(gcc -dumpmachine)
export CROSS_DIR	?= $(PROJECT_DIR)/target/cross-$(BUILD_TRIPLET)/
export PATH		:= $(CROSS_DIR)/install/bin:$(PATH)

# Build compiler
BUILD_CC		:= $(TARGET_TRIPLET)-gcc
BUILD_AS		:= $(TARGET_TRIPLET)-as
BUILD_CC_PATH		:= $(CROSS_DIR)/install/bin/$(TARGET_TRIPLET)-gcc

# Host compiler
HOST_CC			:= $(HOST_TRIPLET)-cc
HOST_AS			:= $(HOST_TRIPLET)-as
