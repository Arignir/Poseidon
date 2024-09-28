################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2018-2024 - The Poseidon Authors
##
################################################################################

# Target
export profile		?= release
export PROFILE		?= $(profile)

ifeq ($(filter $(PROFILE),release debug),)
$(error Variable "profile" must have the value "release" or "debug")
endif

# Create output directory
export TARGET_DIR		:= $(PROJECT_DIR)/target/$(PROFILE)
ifneq ($(shell mkdir -p "$(TARGET_DIR)" && cd "$(TARGET_DIR)" && pwd), $(TARGET_DIR))
$(error Couldn't create directory "$(TARGET_DIR)")
endif

# Add new CFLAGS when building the "release" profile.
ifeq ($(PROFILE), release)
CFLAGS	+= \
	-O2 \
	-DRELEASE
endif

# Add new CFLAGS when building the "debug" target.
ifeq ($(PROFILE), debug)
CFLAGS	+= \
	-O2 \
	-g3 \
	-DDEBUG
endif
