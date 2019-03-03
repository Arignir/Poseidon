################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
##
################################################################################

incdir	:= $(dir $(realpath $(lastword $(MAKEFILE_LIST))))

objs-y	+= $(addprefix $(incdir), \
		handler_bootstrap.o \
		handler.o \
		idt.o \
	)
