/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Continue the initialisation of the CPU.
*/

#include <poseidon/boot/init_hook.h>
#include <poseidon/memory/pmm.h>
#include <arch/x86_64/interrupt.h>
#include <arch/x86_64/cpuid.h>
#include <lib/log.h>

/*
** Continue the initialisation of the CPU.
*/
__boot_text
static
status_t
setup_cpu(void)
{
    setup_idt();

    load_cpuid_features();

    logln("Dumping CPUID:");
    dump_cpuid();

    return (OK);
}

REGISTER_INIT_HOOK(setup_cpu, &setup_cpu, INIT_LEVEL_ARCH_EARLY);

/*
** On PC, the first mega-byte of physical memory is usually used by hardware,
** the bios, the bootloader and other stuff we don't want to mess with.
*/
REGISTER_PMM_RESERVED_AREA(pc_bios, 0x0, 0x100000);
