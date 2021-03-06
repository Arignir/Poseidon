/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/poseidon.h>
#include <poseidon/thread/thread.h>
#include <poseidon/memory/kheap.h>
#include <lib/sync/spinrwlock.h>
#include <lib/string.h>

/* Thread table */
struct thread thread_table[KCONFIG_MAX_THREADS] = { 0 };

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
** Return the Thread Identifier (TID) of the given thread.
*/
tid_t
thread_get_tid(
    struct thread *thread
) {
    return (thread - thread_table);
}

/*
** Find a thread with the `NONE` state.
**
** Return `NULL` if no thread were found.
*/
static
struct thread *
thread_find_free(void)
{
    struct thread *thread;

    thread = thread_table;
    while (thread < thread_table + KCONFIG_MAX_THREADS) {
        spin_rwlock_acquire_read(&thread->lock);
        if (thread->state == NONE) {
            spin_rwlock_release_read(&thread->lock);
            spin_rwlock_acquire_write(&thread->lock);
            if (thread->state == NONE) { /* Ensure state hasn't changed */
                return (thread);
            }
            spin_rwlock_release_write(&thread->lock);
        } else {
            spin_rwlock_release_read(&thread->lock);
        }
        ++thread;
    }
    return (NULL);
}

/*
** Create both the user and kernel stacks of the given thread.
**
** The kernel stack is created only if t->kstack == NULL, otherwise it
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
    thread->stack = kheap_alloc_aligned(KCONFIG_THREAD_STACK_SIZE);
    if (!thread->stack) {
        return (ERR_OUT_OF_MEMORY);
    }
    thread->stack_top = (uchar *)thread->stack + KCONFIG_THREAD_STACK_SIZE;

    /* Allocate the kernel stack */
    if (!thread->kstack) {
        thread->kstack = kheap_alloc_aligned(KCONFIG_KERNEL_STACK_SIZE);

        /* In case of failure, we free the user stack previously allocated*/
        if (!thread->kstack) {
            kheap_free(thread->stack);
            return (ERR_OUT_OF_MEMORY);
        }

        thread->kstack_top = (uchar *)thread->kstack + KCONFIG_KERNEL_STACK_SIZE;
    }

    thread->stack_saved = thread->kstack_top;
    return (OK);
}

/*
** Create a new thread starting at the given entry point.
**
** The thread identifier (tid) of the newly created thread is stored in `*thread_id`.
** If the thread couldn't be created, `*thread_id` is set to `0`.
*/
status_t
thread_new(
    thread_entry entry,
    tid_t *thread_id
) {
    struct thread *thread;
    status_t s;

    /* Set the initial value of `*thread_id`, so we can easily return in case of failure. */
    *thread_id = 0;

    /* First, find an unused thread in `thread_table` */
    thread = thread_find_free();

    if (!thread) {
        return (ERR_NO_MORE_ID);
    }

    memset(thread, 0, sizeof(*thread));

    thread->entry = entry;          // Set the thread's entry point
    thread->state = RUNNABLE;       // Set the state of the new thread to `RUNNABLE`

    if (current_thread()) {
        thread->parent = current_thread();                  // Set the current thread as the new thread's parent
        thread_set_name(thread, current_thread()->name);    // And copy its name (FIXME)
    } else {
        thread->parent = NULL;
        thread_set_name(thread, "kthread");                 // Hard-code the name to `kthread`.
    }

    s = thread_create_stacks(thread);   // Create two stacks (user and kernel) for this process
    if (s != OK) {
        return (s);
    }

    spin_rwlock_release_write(&thread->lock);

    arch_thread_new(thread);    // Initialize the arch-dependant side of this thread.

    *thread_id = thread_get_tid(thread);

    return (OK);
}