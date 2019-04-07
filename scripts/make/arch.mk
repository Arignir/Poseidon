################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
##
################################################################################

# Target arch & triplet
export ARCH		?= x86
export TARGET_TRIPLET	?= i686-elf

# Compiler
CC	:= $(TARGET_TRIPLET)-gcc
AS	:= $(TARGET_TRIPLET)-as
LD	:= $(TARGET_TRIPLET)-ld
