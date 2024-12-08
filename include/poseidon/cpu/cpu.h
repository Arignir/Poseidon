/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "poseidon/poseidon.h"
#include "poseidon/kconfig.h"
#include "arch/target/api/cpu.h"

struct thread;

/*
** A structure representing the data local to each CPU.
**
** This structure can be accessed using `arch_get_current_cpu_local_data()`.
*/
struct cpu_local_data {
    // Current CPU.
    // Should never be NULL after the CPU's early initialization.
    struct cpu *cpu;

    // Current thread executed by the CPU.
    // Might be NULL if the cpu is idling or in the scheduler.
    struct thread *thread;
};

/*
** A structure representing a CPU
**
** The lock policy for this structure is that it's only allowed to be written by the CPU it belongs to.
** Therefore any read should be considered potentially reads of a partial write.
*/
struct cpu {
    struct arch_cpu;                // Arch dependant stuff

    bool bsp;                       // `true` if the CPU is the boostrap processor (BSP).
    bool started;                   // `true` if the CPU is started.

    void *scheduler_stack;          // Stack for the scheduler. Also the boot stack of the CPU.
    void *scheduler_stack_top;

    size_t cpu_id;                  // ID of the CPU.
};

// Number of CPUs on the current system.
extern uint g_cpus_len;

// An array of all cpu structures.
extern struct cpu g_cpus[KCONFIG_MAX_CPUS];

// The end of `g_cpus`. Used for iterating over `g_cpus`.
extern struct cpu const *g_cpus_end;

// An array of all cpu-local data .
extern struct cpu_local_data g_cpus_local_data[KCONFIG_MAX_CPUS];

/*
** Return the current cpu actually running this code.
*/
static inline
struct cpu *
current_cpu(
    void
) {
    return arch_fetch_current_cpu_local_data()->cpu;
}

struct cpu *cpu_get_bsp(void);
void cpu_init(void);
