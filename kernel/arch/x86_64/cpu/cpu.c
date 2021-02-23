/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/poseidon.h>
#include <poseidon/cpu/cpu.h>
#include <arch/x86_64/apic.h>

/*
** Return the current cpu actually running this code.
**
** We can't use `apic_get_id()` if the APIC isn't set up yet, so we're
** using an other structure, `bsp`, which can be used temporarily.
** The function cpu_remap_bsp() will transfer the content of 'bsp'
** to the actuel entry of the BSP within the cpus table.
**
** TODO: Improve this using fs/gs instead of an iterative loop over `cpus`.
*/
struct cpu *
current_cpu(void)
{
    uint32 apic_id;

    if (bsp_remapped) {
        apic_id = apic_get_id();

        for (struct cpu *cpu = cpus; cpu < cpus + ncpu; ++cpu) {
            if (cpu->apic_id == apic_id) {
                return (cpu);
            }
        }

        panic("Current cpu has an unknown local APIC id\n");
    }
    else {
        return (bsp);
    }
}
