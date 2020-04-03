################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2020 - The Poseidon Authors
##
################################################################################

# Target platform (default to pc)
platform		?= pc
export PLATFORM		?= $(platform)

# A list of all supported platforms.
export ALL_PLATFORMS	:= \
	pc

# Ensure the target platform is supported.
ifeq ($(filter $(PLATFORM),$(ALL_PLATFORMS)),)
$(error The platform "$(PLATFORM)" isn't supported.)
endif

# Include the platform-specific make rules, including the cross-compiler triplets or
# any CPU-specific C flags.
include	$(PROJECT_DIR)/kernel/platform/$(PLATFORM)/Platform.mk

# Add the cross compiler to the PATH
export CROSS_DIR	?= $(PROJECT_DIR)/target/cross-$(TARGET_TRIPLET)/

# Build compiler
BUILD_CC		:= $(CROSS_DIR)/install/bin/$(TARGET_TRIPLET)-gcc
BUILD_AS		:= $(CROSS_DIR)/install/bin/$(TARGET_TRIPLET)-as

# Host compiler
HOST_CC			:= $(HOST_TRIPLET)-cc
HOST_AS			:= $(HOST_TRIPLET)-as
