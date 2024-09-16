################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2018-2024 - The Poseidon Authors
##
################################################################################

# This file contains the x86_64 related build variables.
#
# Those override the default ones and/or create new ones which are used by the
# main Makefile.

ldir	:= $(GET_LOCAL_DIR)

CFLAGS	+= \
	-m64 \
	-mgeneral-regs-only \
	-mno-red-zone \

LDFLAGS	+= \
	-T $(ldir)poseidon.ld \
	-z max-page-size=0x1000 \
