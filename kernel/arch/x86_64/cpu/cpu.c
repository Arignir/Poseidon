/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/poseidon.h"
#include "poseidon/cpu/cpu.h"
#include "arch/x86_64/apic.h"

/* The bootstrap processor. Only used at boot time. */
__section(".boot_memory")
static struct cpu __bsp = { .thread = NULL };

__section(".boot_memory")
struct cpu *bsp = &__bsp;

// Set if the BSP has been remaped to its corresponding entry within the cpu table.
static bool bsp_remapped = false;

/*
** Kernel's Bootstrap Processor boot stack.
** Will be used as the scheduler's stack.
*/
extern virtaddr_t bsp_kernel_stack_top[];
extern virtaddr_t bsp_kernel_stack_bot[];

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
    struct cpu *cpu;
    uint32 apic_id;

    if (bsp_remapped) {
        apic_id = apic_get_id();

        cpu = cpus;
        while (cpu < cpus_end) {
            if (cpu->apic_id == apic_id) {
                return (cpu);
            }
            ++cpu;
        }

        panic("Current cpu has an unknown local APIC id\n");
    }
    else {
        return (bsp);
    }
}

/*
** TODO FIXME: Remove this little hack and actually use cpus[0] to represent the BSP
*/
__boot_text
void
cpu_remap_bsp(void)
{
    struct cpu *cpu;

    assert(!bsp_remapped);

    bsp_remapped = true;

    // Swap the data structure
    cpu = current_cpu();
    *cpu = *bsp;
    bsp = NULL;

    // Set the scheduler stack of the BSP
    cpu->scheduler_stack = bsp_kernel_stack_bot;
    cpu->scheduler_stack_top = bsp_kernel_stack_top;

    // Mark the current CPU as the BSP
    cpu->bsp = true;
}
