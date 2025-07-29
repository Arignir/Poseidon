/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/scheduler/scheduler.h"
#include "arch/x86_64/api/cpu.h"
#include "poseidon/thread/thread.h"
#include "poseidon/interrupt.h"
#include "lib/list.h"
#include "lib/sync/spinlock.h"

LIST_HEAD(g_sched_runnable_threads);
struct spinlock g_sched_runnable_threads_lock = SPINLOCK_DEFAULT;

/*
** Search for the next runnable thread.
*/
static
struct thread *
find_next_runnable_thread(
    void
) {
    struct thread *thread;

    spinlock_acquire(&g_sched_runnable_threads_lock);
    {
        thread = list_first_entry_or_null(&g_sched_runnable_threads, struct thread, sched_info.runnable_threads);
        if (thread) {
            list_remove(&thread->sched_info.runnable_threads);
            assert(thread->sched_info.state == RUNNABLE);
            thread->sched_info.state = RUNNING;
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

    // Save the stack of the current thread and release it.
    old = current_thread();
    if (old) {
        old->sched_info.stack_saved = old_sp;
        set_current_thread(NULL);
    }

    // Find the new thread to run or halt to save power.
    new = find_next_runnable_thread();
    while (!new) {
        enable_interrupts();
        halt();
        disable_interrupts();
        new = find_next_runnable_thread();
    }

    // Switch to new thread
    set_current_thread(new);

    //arch_set_kernel_stack((uintptr)new->kstack_top);
    //arch_vaspace_switch(new->vaspace);

    stack_saved = new->sched_info.stack_saved;

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
yield(
    void
) {
    struct cpu const *cpu;
    struct thread *thread;
    bool state;

    push_interrupts_state(&state);
    disable_interrupts();

    cpu = current_cpu();
    thread = current_thread();

    // Set current thread as runnable
    if (thread) {
        assert(thread->sched_info.state == RUNNING);
        thread->sched_info.state = RUNNABLE;

        spinlock_acquire(&g_sched_runnable_threads_lock);
        list_add_tail(&g_sched_runnable_threads, &thread->sched_info.runnable_threads);
        spinlock_release(&g_sched_runnable_threads_lock);
    }

    // The following function will return at a later point, with the current CPU possibly different.
    enter_scheduler(cpu->scheduler_stack_top);

    pop_interrupts_state(&state);
}
