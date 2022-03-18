/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _POSEIDON_SCHEDULER_SCHEDULER_H_
# define _POSEIDON_SCHEDULER_SCHEDULER_H_

# include <lib/list.h>
# include <lib/sync/spinlock.h>

extern struct linked_list sched_runnable_threads;
extern struct spinlock sched_runnable_threads_lock;

void    *reschedule(void *);
void    yield(void);

void    enter_scheduler(void *scheduler_stack);

#endif /* !_POSEIDON_SCHEDULER_SCHEDULER_H_ */