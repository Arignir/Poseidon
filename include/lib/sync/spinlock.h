/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _LIB_SYNC_SPINSYNC_H_
# define _LIB_SYNC_SPINSYNC_H_

# include <poseidon/poseidon.h>
# include <poseidon/atomic.h>

/*
** A simple spin lock.
*/
struct spinlock {
    uint lock;                          // Is the lock held?
};

# define SPINLOCK_DEFAULT               \
    ((struct spinlock) {                \
        .lock = 0,                      \
    })

/*
** Init a spinlock
*/
static inline
void
spinlock_init(
    struct spinlock *sl
) {
    *sl = (struct spinlock)SPINLOCK_DEFAULT;
}

/*
** Acquire a spinlock
*/
static inline
void
spinlock_acquire(
    struct spinlock *sl
) {
    while (atomic_exchange(&sl->lock, 1, ATOMIC_ACQUIRE));
}

/*
** Release a spinlock
*/
static inline
void
spinlock_release(
    struct spinlock *sl
) {
    atomic_exchange(&sl->lock, 0, ATOMIC_RELEASE);
}

#endif /* !_LIB_SYNC_SPINSYNC_H_ */
