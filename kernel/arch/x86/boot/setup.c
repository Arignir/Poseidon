/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Continue the initialisation of the CPU, before calling `kmain()`.
*/

#include <poseidon/boot/init_hook.h>
#include <driver/pic/pic8259.h>

/*
** Continue the initialisation of the CPU.
*/
static
void
setup(void)
{
	pic8259_init();
}

REGISTER_INIT_HOOK(setup, &setup, INIT_LEVEL_ARCH_EARLY);
