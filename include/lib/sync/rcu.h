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
#include "poseidon/atomic.h"
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
    generation = atomic_fetch_add(&g_rcu_generation, 1, ATOMIC_RELAXED);

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

#include "lib/log.h"

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
    uint64_t current_gen;
    struct cpu *current;
    struct linked_list *cursor;
    bool done;

    current = current_cpu();
    current_gen = atomic_load(&g_rcu_generation, ATOMIC_SEQ_CST);

    rcu_acquire_read();

    do {
        done = true;

        list_for_each(cursor, &g_cpus_active) {
            struct cpu *cpu;

            cpu = list_entry(cursor, struct cpu, active_cpus);

            if (
                cpu->id != current->id // Skip current CPU
                && atomic_load(&cpu->rcu.in_read_critical_section, ATOMIC_SEQ_CST)
                && cpu->rcu.generation <= current_gen
            ) {
                done = false;
                break;
            }
        }
    } while (!done);

    rcu_release_read();
}
