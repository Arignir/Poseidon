################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
##
################################################################################

# Verbosity
export Q		:=
export V		?= 0
ifeq ($(V), 0)
	MAKEFLAGS	+= --silent
	Q		:= @
endif
