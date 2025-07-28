/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "lib/list.h"
#include "arch/target/api/cpu.h"

struct thread;

/*
** A structure representing the local-data of each CPU.
**
** This structure can be accessed easily using `arch_cpu_fetch_local_data()`.
*/
struct cpu_local_data {
    // Current CPU.
    // Should never ever be NULL after the CPU's early initialization.
    struct cpu *cpu;

    // Current thread executed by the CPU.
    // Might be NULL if the cpu is idling or in the scheduler.
    struct thread *thread;
};

/*
** A structure representing a CPU
**
** Past boot-time, this structure's content can only be modified by the current CPU.
*/
struct cpu {
    struct arch_cpu;                // Arch dependant stuff

    size_t id;                      // ID of the CPU.

    bool bsp;                       // `true` if the CPU is the boostrap processor (BSP).
    bool active;                    // `true` if the CPU is started.

    void *scheduler_stack;          // Stack of the scheduler. Also the boot stack of the CPU.
    void *scheduler_stack_top;

    // RCU related variables
    struct rcu {
        bool in_read_critical_section;  // Set if the CPU is within a read-critical section.

        uint64_t generation;            // RCU Generation when the CPU entered its read-critical
                                        // section.
                                        // Undefined if `in_read_critical_section` is false
    } rcu;

    // Local data of the CPU.
    struct cpu_local_data local_data;

    // Linked list of all available CPUs
    struct linked_list available_cpus;

    // Linked list of all active CPUs
    struct linked_list active_cpus;
};

/*
** Linked list of all available CPUs
*/
extern struct linked_list g_cpus_available;

/*
** Linked list of all active CPUs
*/
extern struct linked_list g_cpus_active;

/*
** Spin lock that protects against concurrent modifications of both linked list.
*/
extern struct spinlock g_cpus_lock;

/*
** Return the current cpu actually running this code.
*/
static inline
struct cpu *
current_cpu(
    void
) {
    return arch_cpu_fetch_local_data()->cpu;
}

/*
** Return the current thread, aka the thread the CPU was running before an
** interruption occured.
*/
static inline
struct thread *
current_thread(
    void
) {
    return arch_cpu_fetch_local_data()->thread;
}

/*
** Set the current thread, aka the thread the CPU was running before an
** interruption occured.
*/
static inline
void
set_current_thread(
    struct thread *new_thread
) {
    arch_cpu_fetch_local_data()->thread = new_thread;
}

void cpu_init(struct cpu *cpu, size_t id);
struct cpu *cpu_get_bsp(void);
