/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "lib/list.h"
#include "lib/sync/spinlock.h"

extern struct linked_list sched_runnable_threads;
extern struct spinlock sched_runnable_threads_lock;

void *reschedule(void *);
void yield(void);

void enter_scheduler(void *scheduler_stack);
