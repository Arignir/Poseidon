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

    struct thread *thread;          // Current thread executed by the CPU.
                                    // Might be NULL if the cpu is idling or in the scheduler.

    size_t cpu_id;                  // ID of the CPU.

};

/* Number of CPUs on the current system. */
extern uint ncpu;

/* An array for all cpus. */
extern struct cpu cpus[KCONFIG_MAX_CPUS];

/* The end of `cpus`. Used for iterating over `cpus`. */
extern struct cpu const *cpus_end;

/*
** Return the current cpu actually running this code.
**
** NOTE: This function is implemented by the architecture-dependant code.
*/
struct cpu *current_cpu(void);
