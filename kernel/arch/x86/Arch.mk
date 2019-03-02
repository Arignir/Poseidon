################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
##
################################################################################

# This file contains x86 related build variables.
# Those override the default ones and/or create new one which are used by the
# main Makefile.

incdir	:= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

# Add x86-specific flags
CFLAGS	+= \
	-m32 \
	-mgeneral-regs-only \

LDFLAGS	+= \
	-T $(incdir)poseidon.ld
