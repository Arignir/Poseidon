/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _POSEIDON_CPU_CPU_H_
# define _POSEIDON_CPU_CPU_H_

# include <poseidon/poseidon.h>
# include <poseidon/kconfig.h>
# include <poseidon/interrupt.h>
# include <arch/target/api/cpu.h>
# include <lib/sync/spinrwlock.h>

struct thread;

/* A structure representing a CPU */
struct cpu
{
    struct arch_cpu;                // Arch dependant stuff

    bool bsp;                       // `true` if the CPU is the boostrap processor (BSP).
    uint started;                   // `true` if the CPU is started.

    void *scheduler_stack;          // Stack for the scheduler. Also the boot stack of the CPU.
    void *scheduler_stack_top;

    struct spinrwlock lock;         // Lock to protect this structure
};

/* Number of CPUs on the current system. */
extern uint ncpu;

/* An array for all cpus. */
extern struct cpu cpus[KCONFIG_MAX_CPUS];

/* The bootstrap processor. Used only at boot time. */
extern struct cpu *bsp;

/* Set if the BSP has been remapped to its corresponding entry within the cpu table. */
extern bool bsp_remapped;

void                cpu_remap_bsp(void);

struct cpu          *current_cpu(void);
struct cpu const    *current_cpu_acquire_read(void);
struct cpu          *current_cpu_acquire_write(void);
void                current_cpu_release_read(void);
void                current_cpu_release_write(void);

#endif /* !_POSEIDON_CPU_CPU_H_ */
