################################################################################
##
##  This file is part of the Poseidon Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2018-2024 - The Poseidon Authors
##
################################################################################

ldir	:= $(GET_LOCAL_DIR)

objs-y	+= $(addprefix $(ldir), \
		interrupt.o \
		kmain.o \
		panic.o \
	)

include \
	$(ldir)boot/Makefile \
	$(ldir)cpu/Makefile \
	$(ldir)memory/Makefile \
	$(ldir)scheduler/Makefile \
	$(ldir)thread/Makefile \
