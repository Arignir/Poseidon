/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "poseidon/interrupt.h"
#include "poseidon/cpu/cpu.h"

static uint64_t g_rcu_generation = 0;

/*
** Enter a read-side critical section.
**
** NOTE:
**   - Interrupts must be **disabled** when entering a read-side critical section
**   - This function is re-entrant.
*/
static inline
void
rcu_acquire_read(
    void
) {
    uint64_t generation;
    struct cpu *cpu;

    cpu = current_cpu();

    // Interrupts must be disabled before entering an RCU read-side critical section.
    assert(!interrupts_enabled());

    // Increase the generation by one.
    generation = atomic_fetch_add(&g_rcu_generation, 1, ATOMIC_ACQ_REL);

    // Set the CPU's RCU generation
    cpu->rcu.generation = generation;
    atomic_store(&cpu->rcu.in_read_critical_section, true, ATOMIC_SEQ_CST);
}

/*
** Leave a read-side critical section.
*/
static inline
void
rcu_release_read(
    void
) {
    struct cpu *cpu;

    cpu = current_cpu();

    // Interrupts must be disabled before releasing an RCU read-side critical section.
    assert(!interrupts_enabled());

    atomic_store(&cpu->rcu.in_read_critical_section, false, ATOMIC_SEQ_CST);
    cpu->rcu.generation = 0;
}

/*
** Wait for all the remaining read-side critical sections entered by the other cores to finish.
**
** NOTE:This is a blocking function that might enable the interrupts and yield the current
** thread.
*/
static inline
void
rcu_synchronize(
    void
) {
    size_t i;
    uint64_t current_gen;
    struct cpu *current;
    bool done;

    // If we're running in a mono-core context, the equation is pretty simple:
    // there's nothing to synchronize and we can early return.
    if (g_cpus_len <= 1) {
        return ;
    }

    current = current_cpu();
    current_gen = atomic_load(&g_rcu_generation, ATOMIC_SEQ_CST);

    do {
        done = true;
        for (i = 0; i < g_cpus_len; ++i) {
            struct cpu *cpu;

            cpu = &g_cpus[i];
            if (
                cpu->started
                && cpu->cpu_id != current->cpu_id
                && atomic_load(&cpu->rcu.in_read_critical_section, ATOMIC_SEQ_CST)
                && cpu->rcu.generation <= current_gen
            ) {
                done = false;
                break;
            }
        }
    } while (!done);
}
