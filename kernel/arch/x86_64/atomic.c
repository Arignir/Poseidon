/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/atomic.h>

/*
** Set `*ptr` to `new` while returning the previous value of `*ptr`.
*/
uint32
atomic_exchange(
    uint32 volatile *ptr,
    uint32 new
) {
    uint32 res;

    asm volatile(
        "lock xchgl %0, %1;"
        : "+m"(*ptr), "=a"(res)
        : "1"(new)
        : "cc"
    );

    return (res);
}
