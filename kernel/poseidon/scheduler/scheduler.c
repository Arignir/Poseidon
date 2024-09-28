/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/scheduler/scheduler.h"
#include "poseidon/thread/thread.h"
#include "poseidon/interrupt.h"
#include "lib/list.h"
#include "lib/sync/spinlock.h"
#include "lib/log.h"

struct linked_list g_sched_runnable_threads = LIST_HEAD_INIT(g_sched_runnable_threads);
struct spinlock g_sched_runnable_threads_lock = SPINLOCK_DEFAULT;

/*
** Search for the next runnable thread.
**
** If not NULL, the returned thread already has its `sched_info.lock` acquired (write).
*/
static
struct thread *
find_next_thread(void)
{
    struct thread *thread;

    spinlock_acquire(&g_sched_runnable_threads_lock);
    {
        thread = list_first_entry_or_null(&g_sched_runnable_threads, struct thread, sched_info.runnable_threads);
        if (thread) {
            spin_rwlock_acquire_write(&thread->sched_info.lock);
            list_remove(&thread->sched_info.runnable_threads);
            assert(thread->sched_info.state == RUNNABLE);
        }
    }
    spinlock_release(&g_sched_runnable_threads_lock);
    return thread;
}

/*
** Find and switch to the next runnable thread.
** If no threads are available, halt and try again later.
**
** Return the stack of the new thread.
**
** NOTE: This function is called by 'enter_scheduler()', do NOT call it yourself.
** NOTE: The current thread is already lock with read-write permissions.
*/
void *
reschedule(
    void *old_sp
) {
    struct thread *new;
    struct thread *old;
    virtaddr_t stack_saved;

    /* Save the stack of the current thread and release it. */
    old = current_thread();
    if (old) {
        old->sched_info.stack_saved = old_sp;
        spin_rwlock_release_write(&old->sched_info.lock);       // Acquired in `yield()`.
        current_cpu()->thread = NULL;
    }

    /* Find the new thread or halt (to save power) */
    new = find_next_thread(); // `new` is already read-write locked
    while (!new) {
        enable_interrupts();
        halt();
        disable_interrupts();
        new = find_next_thread();
    }

    /* Switch to new thread */
    current_cpu()->thread = new;
    new->sched_info.state = RUNNING;

    //arch_set_kernel_stack((uintptr)new->kstack_top);
    //arch_vaspace_switch(new->vaspace);

    stack_saved = new->sched_info.stack_saved;
    spin_rwlock_release_write(&new->sched_info.lock);

    return stack_saved;
}

/*
** Yield the cpu to another thread.
**
** This function will return at a later time, or possibly immediately if no
** other threads are waiting to be executed (don't worry, they are guilty).
**
** _WARNING_:
** The current thread must NOT be acquired before calling this, NOR its address space.
*/
void
yield(void)
{
    struct cpu const *cpu;
    struct thread *thread;
    bool state;

    push_interrupts_state(&state);
    {
        disable_interrupts();

        cpu = current_cpu();
        thread = current_thread();

        /* Set current thread as runnable */
        if (thread) {
            spinlock_acquire(&g_sched_runnable_threads_lock);
            {
                spin_rwlock_acquire_write(&thread->sched_info.lock); /* Released in `reschedule()` */
                assert(thread->sched_info.state == RUNNING);
                thread->sched_info.state = RUNNABLE;

                list_add_tail(&g_sched_runnable_threads, &thread->sched_info.runnable_threads);
            }
            spinlock_release(&g_sched_runnable_threads_lock);
        }

        enter_scheduler(cpu->scheduler_stack_top);
    }
    pop_interrupts_state(&state);
}
