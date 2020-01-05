################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2020 - The Poseidon Authors
##
################################################################################

# Version
export MAJOR		:= 0
export MINOR		:= 1
export PATCH		:= 0
export EXTRA		:= -indev
export VERSION		:= $(MAJOR).$(MINOR).$(PATCH)$(EXTRA)
export NAME		:= poseidon_$(VERSION)
