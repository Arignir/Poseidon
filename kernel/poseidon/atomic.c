/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to achieve atomic operations.
**
** The following functions are default implementations, usually overwritten by
** the architecture.
*/

#include <poseidon/poseidon.h>
#include <poseidon/atomic.h>

/*
** Set `*ptr` to `new` while returning the previous value of `*ptr`.
*/
__weak
uint32
atomic_exchange(
    uint32 volatile *ptr __unused,
    uint32 new __unused
) {
    unimplemented();
}
