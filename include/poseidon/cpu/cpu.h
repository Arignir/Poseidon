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

    struct thread *thread;          // Current thread executed by the CPU.
                                    // Might be NULL if the cpu is idling or in the scheduler.

    struct spin_rwlock lock;         // Lock to protect this structure
};

/* Number of CPUs on the current system. */
extern uint ncpu;

/* An array for all cpus. */
extern struct cpu cpus[KCONFIG_MAX_CPUS];

/* The end of `cpus`. Used for iterating over `cpus`. */
extern struct cpu const *cpus_end;

size_t              cpu_get_id(struct cpu *cpu);

/*
** Return the current cpu actually running this code.
**
** The returned `cpu` structure isn't locked, make sure you have the permissions
** to do whatever operations you want to do.
**
** NOTE: This function is implemented by the architecture-dependant code.
*/
struct cpu          *current_cpu(void);

/*
** Return the current cpu actually running this code, with read-only permissions.
*/
static inline
struct cpu const *
current_cpu_acquire_read(void)
{
    struct cpu *cpu;

    cpu = current_cpu();
    spin_rwlock_acquire_read(&cpu->lock);
    return (cpu);
}

/*
** Release the read-only access to the current CPU.
*/
static inline
void
current_cpu_release_read(void)
{
    struct cpu *cpu;

    cpu = current_cpu();
    spin_rwlock_release_read(&cpu->lock);
}

/*
** Return the current cpu actually running this code, with read-write permissions.
*/
static inline
struct cpu *
current_cpu_acquire_write(void)
{
    struct cpu *cpu;

    cpu = current_cpu();
    spin_rwlock_acquire_write(&cpu->lock);
    return (cpu);
}

/*
** Release the read-write access to the current CPU.
*/
static inline
void
current_cpu_release_write(void)
{
    struct cpu *cpu;

    cpu = current_cpu();
    spin_rwlock_release_write(&cpu->lock);
}

#endif /* !_POSEIDON_CPU_CPU_H_ */
