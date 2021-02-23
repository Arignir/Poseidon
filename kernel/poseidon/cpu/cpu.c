/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/cpu/cpu.h>
#include <poseidon/memory/memory.h>

/*
** Kernel's Bootstrap Processor boot stack.
** Will be used as the scheduler's stack.
*/
extern virtaddr_t bsp_kernel_stack_top[];
extern virtaddr_t bsp_kernel_stack_bot[];

/* The bootstrap processor. Only used at boot time. */
__section(".boot_memory")
static struct cpu __bsp;
__section(".boot_memory")
struct cpu *bsp = &__bsp;

/* Number of CPUs on the current system. */
uint ncpu = 0;

/* An array for all cpus. */
struct cpu cpus[KCONFIG_MAX_CPUS] = { 0 };

/* Set if the BSP has been remaped to its corresponding entry within the cpu table. */
bool bsp_remapped = false;

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

/*
** Architecture-independent API to manipulate the CPU structure.
**
** The following functions are default implementations, usually overwritten by
** the architecture.
*/

/*
** Return the current cpu actually running this code.
**
** The returned `cpu` structure isn't locked, make sure you have the permissions
** to do whatever operations you want to do.
*/
__weak
struct cpu *
current_cpu(void)
{
    unimplemented();
}

/*
** Return the current cpu actually running this code, with read-only permissions.
*/
struct cpu const *
current_cpu_acquire_read(void)
{
    struct cpu *cpu;

    cpu = current_cpu();
    spinrwlock_acquire_read(&cpu->lock);
    return (cpu);
}

/*
** Release the read-only access to the current CPU.
*/
void
current_cpu_release_read(void)
{
    struct cpu *cpu;

    cpu = current_cpu();
    spinrwlock_release_read(&cpu->lock);
}

/*
** Return the current cpu actually running this code, with read-write permissions.
*/
struct cpu *
current_cpu_acquire_write(void)
{
    struct cpu *cpu;

    cpu = current_cpu();
    spinrwlock_acquire_write(&cpu->lock);
    return (cpu);
}

/*
** Release the read-write access to the current CPU.
*/
void
current_cpu_release_write(void)
{
    struct cpu *cpu;

    cpu = current_cpu();
    spinrwlock_release_write(&cpu->lock);
}