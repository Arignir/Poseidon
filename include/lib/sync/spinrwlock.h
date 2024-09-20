/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "poseidon/poseidon.h"
#include "poseidon/atomic.h"
#include "lib/sync/spinlock.h"

/*
** A structure representing a read-write lock backed by spinlocks.
*/
struct spin_rwlock
{
    int volatile readcount;
    struct spinlock resource_access;
    struct spinlock readcount_access;
    struct spinlock service_queue;
};

#define SPIN_RWLOCK_DEFAULT                                 \
    ((struct spin_rwlock) {                                 \
        .readcount = 0,                                     \
        .resource_access = SPINLOCK_DEFAULT,                \
        .readcount_access = SPINLOCK_DEFAULT,               \
        .service_queue = SPINLOCK_DEFAULT,                  \
    })

static inline
void
spin_rwlock_init(
    struct spin_rwlock *rw
) {
    *rw = SPIN_RWLOCK_DEFAULT;
}

static inline
void
spin_rwlock_acquire_read(
    struct spin_rwlock *rw
) {
    spinlock_acquire(&rw->service_queue);
    spinlock_acquire(&rw->readcount_access);

    if (atomic_add_fetch(&rw->readcount, 1, ATOMIC_ACQ_REL) == 1) {
        spinlock_acquire(&rw->resource_access);
    }

    spinlock_release(&rw->readcount_access);
    spinlock_release(&rw->service_queue);
}

static inline
void
spin_rwlock_release_read(
    struct spin_rwlock *rw
) {
    spinlock_acquire(&rw->readcount_access);

    if (atomic_add_fetch(&rw->readcount, -1, ATOMIC_ACQ_REL) == 0) {
        spinlock_release(&rw->resource_access);
    }

    spinlock_release(&rw->readcount_access);
}

static inline
void
spin_rwlock_acquire_write(
    struct spin_rwlock *rw
) {
    spinlock_acquire(&rw->service_queue);
    spinlock_acquire(&rw->resource_access);
    spinlock_release(&rw->service_queue);
}

static inline
void
spin_rwlock_release_write(
    struct spin_rwlock *rw
) {
    spinlock_release(&rw->resource_access);
}
