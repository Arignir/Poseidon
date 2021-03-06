/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/scheduler/scheduler.h>
#include <poseidon/thread/thread.h>
#include <lib/log.h>

struct scheduler scheduler =
{
    .next = thread_table,
    .lock = SPINLOCK_DEFAULT,
};

static
struct thread *
scheduler_get_next(void)
{
    struct thread *next;

    spinlock_acquire(&scheduler.lock);
    next = scheduler.next;
    spinlock_release(&scheduler.lock);
    return (next);

}

static
void
scheduler_update_next(
    struct thread *next
) {
    spinlock_acquire(&scheduler.lock);
    scheduler.next = next;
    spinlock_release(&scheduler.lock);
}

/*
** Search for the next runnable thread.
**
** If not NULL, the returned thread is already read-write locked.
*/
static
struct thread *
find_next_thread(void)
{
    bool pass;
    struct thread *next;
    struct thread const *limit;
    struct thread *t;

    pass = false;
    next = scheduler_get_next();
    t = next + 1;
    limit = thread_table + KCONFIG_MAX_THREADS;

look_for_next:
    while (t < limit)
    {
        spin_rwlock_acquire_read(&t->lock);
        if (t->state == RUNNABLE) {
            spin_rwlock_release_read(&t->lock);
            spin_rwlock_acquire_write(&t->lock);
            if (t->state == RUNNABLE) { // Ensure the state hasn't changed
                scheduler_update_next(t);
                return (t);
            }
            spin_rwlock_release_write(&t->lock);
        } else {
            spin_rwlock_release_read(&t->lock);
        }
        ++t;
    }
    if (!pass)
    {
        t = (struct thread *)thread_table;
        limit = next + 1;
        pass = true;
        goto look_for_next;
    }
    return (NULL);
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
    struct cpu *cpu;
    struct thread *new;
    struct thread *old;

    /* Save the stack of the current thread and release it. */
    old = current_thread();
    if (old) {
        old->stack_saved = old_sp;
        current_thread_release_write();

        cpu = current_cpu_acquire_write();
        cpu->thread = NULL;
        current_cpu_release_write();
    }

    /* Find the new thread or halt (to save power) */
    new = find_next_thread(); // `new` is already read-write locked
    while (!new) {
        enable_interrupts();
        halt();
        disable_interrupts();
        new = find_next_thread();
    }

    new->state = RUNNING;

    /* Switch to new thread */
    cpu = current_cpu_acquire_write();
    cpu->thread = new;
    current_cpu_release_write();

    //arch_set_kernel_stack((uintptr)new->kstack_top);
    //arch_vaspace_switch(new->vaspace);

    current_thread_release_write();

    return (new->stack_saved);
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
    void *stack_top;
    struct cpu const *cpu;
    struct thread *thread;
    bool state;

    push_interrupts_state(&state);
    {
        disable_interrupts();

        cpu = current_cpu_acquire_read();

        /* Set current thread as runnable */
        if (cpu->thread) {
            thread = cpu->thread;
            spin_rwlock_acquire_write(&thread->lock); /* Released in scheduler() */
            assert(thread->state == RUNNING);
            thread->state = RUNNABLE;
        }

        stack_top = cpu->scheduler_stack_top;

        current_cpu_release_read();

        enter_scheduler(stack_top);
    }
    pop_interrupts_state(&state);
}