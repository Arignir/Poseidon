/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _POSEIDON_THREAD_THREAD_H_
# define _POSEIDON_THREAD_THREAD_H_

# include <poseidon/memory/memory.h>
# include <poseidon/poseidon.h>
# include <poseidon/cpu/cpu.h>
# include <lib/sync/spinrwlock.h>

typedef uint32 tid_t;
typedef int (*thread_entry)();

/*
** An enum of all states a thread can be in.
*/
enum thread_state
{
    NONE = 0,
    RUNNABLE,
    RUNNING,
    ZOMBIE,
};

/*
** Strings to print each thread state
*/
static char const *thread_state_str[] =
{
    [NONE]              = "NONE",
    [RUNNABLE]          = "RUNNABLE",
    [RUNNING]           = "RUNNING",
    [ZOMBIE]            = "ZOMBIE",
};

/*
** Exit status a process can exit with.
*/
enum exit_status
{
    EXIT_SUCCESS        = 0,
    EXIT_FAILURE        = 1,
    EXIT_PAGEFAULT      = 139,
};

struct thread
{
    char name[256];
    enum thread_state state;    // State
    struct thread *parent;      // Parent thread

    virtaddr_t stack;           // Bottom of the thread's stack
    virtaddr_t stack_top;       // Top of the thread's stack.
    virtaddr_t stack_saved;     // Stack saved when a context switch occures

    virtaddr_t kstack;          // Bottom of kernel stack
    virtaddr_t kstack_top;      // Top of kernel stack

    thread_entry entry;         // Entry point

    struct spin_rwlock lock;  // Lock to protect this block
};

extern struct thread thread_table[];

status_t    thread_new(thread_entry entry, tid_t *thread_id);
void        arch_thread_new(struct thread *t);

/*
** Return the current thread, aka the thread the CPU was running before an
** interruption occured.
**
** The returned `thread` structure isn't locked, make sure you have the permissions
** to do whatever operations you want to do.
*/
static inline
struct thread *
current_thread(void)
{
    struct cpu *cpu;
    struct thread *t;

    cpu = current_cpu_acquire_read();
    t = cpu->thread;
    current_cpu_release_read();
    return (t);
}

/*
** Return the current thread (the thread the CPU was running before an
** interruption occured) with read-only permissions.
**
** NOTE: This function assumes the current thread isn't NULL.
*/
static inline
struct thread *
current_thread_acquire_read(void)
{
    struct thread *t;

    t = current_thread();
    spin_rwlock_acquire_read(&t->lock);
    return (t);
}

/*
** Return the current thread (the thread the CPU was running before an
** interruption occured) with read-write permissions.
**
** NOTE: This function assumes the current thread isn't NULL.
*/
static inline
struct thread *
current_thread_acquire_write(void)
{
    struct thread *t;

    t = current_thread();
    spin_rwlock_acquire_write(&t->lock);
    return (t);
}

/*
** Release the current thread previously acquired with read-only permissions.
**
** NOTE: This function assumes the current thread isn't NULL.
*/
static inline
void
current_thread_release_read(void)
{
    struct thread *t;

    t = current_thread();
    spin_rwlock_release_read(&t->lock);
}

/*
** Release the current thread previously acquired with read-write permissions.
**
** NOTE: This function assumes the current thread isn't NULL.
*/
static inline
void
current_thread_release_write(void)
{
    struct thread *t;

    t = current_thread();
    spin_rwlock_release_write(&t->lock);
}

#endif /* !_POSEIDON_THREAD_THREAD_H_ */