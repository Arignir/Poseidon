/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/poseidon.h"
#include "poseidon/thread/thread.h"
#include "poseidon/memory/kheap.h"
#include "poseidon/scheduler/scheduler.h"
#include "lib/sync/spinrwlock.h"
#include "lib/list.h"
#include "lib/string.h"

static struct linked_list g_threads_list = LIST_HEAD_INIT(g_threads_list);
static tid_t g_next_pid = 1;

/*
** Set the new name of the thread
*/
void
thread_set_name(
    struct thread *thread,
    char const *name
) {
    strncpy(thread->name, name, sizeof(thread->name) - 1);
    thread->name[sizeof(thread->name) - 1] = '\0';
}

/*
** Create both the user and kernel stacks of the given thread.
**
** The kernel stack is created only if t->sched_info.kstack == NULL, otherwise it
** reuses the existing kernel stack.
**
** This assumes the given thread and it's virtual address space are both
** locked as writers.
*/
status_t
thread_create_stacks(
    struct thread *thread
) {
    /* Allocate the user stack */
    thread->sched_info.stack = kheap_alloc_aligned(KCONFIG_THREAD_STACK_SIZE);
    if (!thread->sched_info.stack) {
        return (ERR_OUT_OF_MEMORY);
    }
    thread->sched_info.stack_top = (uchar *)thread->sched_info.stack + KCONFIG_THREAD_STACK_SIZE;

    /* Allocate the kernel stack */
    if (!thread->sched_info.kstack) {
        thread->sched_info.kstack = kheap_alloc_aligned(KCONFIG_KERNEL_STACK_SIZE);

        /* In case of failure, we free the user stack previously allocated*/
        if (!thread->sched_info.kstack) {
            kheap_free(thread->sched_info.stack);
            return (ERR_OUT_OF_MEMORY);
        }

        thread->sched_info.kstack_top = (uchar *)thread->sched_info.kstack + KCONFIG_KERNEL_STACK_SIZE;
    }

    thread->sched_info.stack_saved = thread->sched_info.kstack_top;
    return (OK);
}

/*
** Create a new thread starting at the given entry point.
**
** The thread's structure of the newly created thread is stored in `*thread`.
** If the thread couldn't be created, `*thread` is set to `NULL`.
*/
status_t
thread_new(
    thread_entry entry,
    struct thread **pthread
) {
    struct thread *thread;
    status_t s;

    /* Set the initial value of `*thread`, so we can easily return in case of failure. */
    *pthread = NULL;

    /* First, find an unused thread in `thread_table` */
    thread = kheap_alloc_zero(sizeof(*thread));

    if (!thread) {
        return (ERR_OUT_OF_MEMORY);
    }

    thread->tid = g_next_pid++;             // Set the thread's TID
    thread->entry = entry;                  // Set the thread's entry point
    thread->sched_info.state = RUNNABLE;    // Set the state of the new thread to `RUNNABLE`
    thread->sched_info.lock = SPIN_RWLOCK_DEFAULT;

    if (current_thread()) {
        thread->parent = current_thread();                  // Set the current thread as the new thread's parent
        thread_set_name(thread, current_thread()->name);    // And copy its name (FIXME)
    } else {
        thread->parent = NULL;
        thread_set_name(thread, "kthread");                 // Hard-code the name to `kthread`.
    }

    s = thread_create_stacks(thread);   // Create two stacks (user and kernel) for this process
    if (s != OK) {
        kheap_free(thread);
        return (s);
    }

    arch_thread_new(thread);    // Initialize the arch-dependant side of this thread.

    list_add_tail(&g_threads_list, &thread->threads);

    spinlock_acquire(&g_sched_runnable_threads_lock);
    list_add_tail(&g_sched_runnable_threads, &thread->sched_info.runnable_threads);
    spinlock_release(&g_sched_runnable_threads_lock);

    *pthread = thread;

    return (OK);
}
