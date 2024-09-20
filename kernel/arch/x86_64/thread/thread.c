/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/thread/thread.h"

struct scheduler_stack
{
    uint64 rflags;
    uint64 r15;
    uint64 r14;
    uint64 r13;
    uint64 r12;
    uint64 rbp;
    uint64 rbx;
    uint64 rip;
} __packed;

void
arch_thread_new(
    struct thread *thread
) {

    /*
    ** Setup the stack so we can return from `enter_scheduler()` safely.
    */

    struct scheduler_stack stack = {
        .rflags = 0x202, // TODO Set IOPL to 3
        .r15 = 0x0,
        .r14 = 0x0,
        .r13 = 0x0,
        .r12 = 0x0,
        .rbp = 0x0,
        .rbx = 0x0,
        .rip = (uint64)thread->entry,
    };

    thread->sched_info.stack_saved = (uchar *)thread->sched_info.stack_saved - sizeof(stack);
    *(struct scheduler_stack *)thread->sched_info.stack_saved = stack;
}