################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2020 - The Poseidon Authors
##
################################################################################

# Target
export target		?= release
export TARGET		?= $(target)

ifeq ($(filter $(TARGET),release debug),)
$(error Variable "target" must have the value "release" or "debug")
endif

# Create output directory
export TARGET_DIR		:= $(PROJECT_DIR)/target/$(TARGET)
ifneq ($(shell mkdir -p "$(TARGET_DIR)" && cd "$(TARGET_DIR)" && pwd), $(TARGET_DIR))
$(error Couldn't create target directory "$(TARGET_DIR)")
endif

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
