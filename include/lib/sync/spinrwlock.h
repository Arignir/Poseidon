/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _LIB_SYNC_SPINRWSYNC_H_
# define _LIB_SYNC_SPINRWSYNC_H_

# include <poseidon/poseidon.h>
# include <poseidon/atomic.h>
# include <lib/sync/spinlock.h>

/*
** A structure representing a read-write lock backed by spinlocks.
*/
struct spinrwlock
{
    int volatile readcount;
    struct spinlock resource_access;
    struct spinlock readcount_access;
    struct spinlock service_queue;
};

# define SPINRWLOCK_DEFAULT                                 \
    ((struct spinrwlock) {                                  \
        .readcount = 0,                                     \
        .resource_access = SPINLOCK_DEFAULT,                \
        .readcount_access = SPINLOCK_DEFAULT,               \
        .service_queue = SPINLOCK_DEFAULT,                  \
    })

static inline
void
spinrwlock_init(
    struct spinrwlock *rw
) {
    *rw = SPINRWLOCK_DEFAULT;
}

static inline
void
spinrwlock_acquire_read(
    struct spinrwlock *rw
) {
    spinlock_acquire(&rw->service_queue);
    spinlock_acquire(&rw->readcount_access);

    if (atomic_add_and_fetch(&rw->readcount, 1) == 1) {
        spinlock_acquire(&rw->resource_access);
    }

    spinlock_release(&rw->readcount_access);
    spinlock_release(&rw->service_queue);
}

static inline
void
spinrwlock_release_read(
    struct spinrwlock *rw
) {
    spinlock_acquire(&rw->readcount_access);

    if (atomic_add_and_fetch(&rw->readcount, -1) == 0) {
        spinlock_release(&rw->resource_access);
    }

    spinlock_release(&rw->readcount_access);
}

static inline
void
spinrwlock_acquire_write(
    struct spinrwlock *rw
) {
    spinlock_acquire(&rw->service_queue);
    spinlock_acquire(&rw->resource_access);
    spinlock_release(&rw->service_queue);
}

static inline
void
spinrwlock_release_write(
    struct spinrwlock *rw
) {
    spinlock_release(&rw->resource_access);
}

#endif /* !_LIB_SYNC_SPINRWSYNC_H_ */
