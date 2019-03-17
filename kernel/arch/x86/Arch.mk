################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
##
################################################################################

# This file contains the x86 related build variables.
#
# Those override the default ones and/or create new ones which are used by the
# main Makefile.

ldir	:= $(GET_LOCAL_DIR)

# Add x86-specific flags
CFLAGS	+= \
	-m32 \
	-mgeneral-regs-only \

LDFLAGS	+= \
	-T $(ldir)poseidon.ld
