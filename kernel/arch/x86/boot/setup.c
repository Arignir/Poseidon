/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Continue the initialisation of the CPU.
*/

#include <poseidon/boot/init_hook.h>
#include <poseidon/boot/multiboot2.h>
#include <poseidon/memory/boot_pmm.h>
#include <driver/pic/pic8259.h>

/*
** Continue the initialisation of the CPU.
*/
__boot_text
static
status_t
setup(void)
{
	multiboot_load();
	boot_pmm_init();
	pic8259_init();
	return (OK);
}

REGISTER_INIT_HOOK(setup, &setup, INIT_LEVEL_ARCH_EARLY);
