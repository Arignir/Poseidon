/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "arch/x86_64/api/cpu.h"
#include "poseidon/memory/memory.h"
#include "poseidon/poseidon.h"
#include "poseidon/cpu/cpu.h"
#include "lib/sync/spinrwlock.h"
#include "lib/list.h"

typedef uint32 tid_t;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
typedef int (*thread_entry)();
#pragma GCC diagnostic pop

/*
** An enum of all states a thread can be in.
*/
enum thread_state : uint32_t {
    NONE = 0,
    RUNNABLE,
    RUNNING,
    ZOMBIE,
};

/*
** Strings to print each states.
*/
[[maybe_unused]]
static char const *thread_state_str[] = {
    [NONE]              = "NONE",
    [RUNNABLE]          = "RUNNABLE",
    [RUNNING]           = "RUNNING",
    [ZOMBIE]            = "ZOMBIE",
};

/*
** Exit status a process can exit with.
*/
enum exit_status {
    EXIT_SUCCESS        = 0,
    EXIT_FAILURE        = 1,
    EXIT_PAGEFAULT      = 139,
};

/*
** A thread.
**
** The thread can either be a kernel or userland thread, and two
** threads might share the same address space (forming a process).
** Those distinctions are meaningless at this level of abstraction.
*/
struct thread {
    // The following are considered read-only past the thread's creation.
    tid_t tid;                                  // Thread's TID
    char name[256];                             // Thread's name
    struct thread *parent;                      // Parent thread
    thread_entry entry;                         // Entry point

    // Beside RCU-protected linked lists, the following fields can only be accessed
    // by the CPU running this thread.
    struct {
        virtaddr_t stack;                       // Bottom of the thread's stack
        virtaddr_t stack_top;                   // Top of the thread's stack.
        virtaddr_t stack_saved;                 // Stack saved when a context switch occures

        virtaddr_t kstack;                      // Bottom of kernel stack
        virtaddr_t kstack_top;                  // Top of kernel stack

        uint32_t state;                         // State. Must be atomically accessed.
        struct linked_list runnable_threads;    // List of runnable threads, used by the scheduler.
    } sched_info;

    struct linked_list threads;                 // List of all threads
};

status_t thread_new(thread_entry entry, struct thread **thread);
