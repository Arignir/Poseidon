/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#include <poseidon/boot/init_hook.h>

static
status_t
boot_vmm_init(void)
{
    return (OK);
}

REGISTER_INIT_HOOK(boot_vmm, &boot_vmm_init, INIT_LEVEL_BOOT_VMM);
