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

static inline
uint64
rdtsc(
    void
) {
    uint32 a;
    uint32 d;

    asm volatile(
        "rdtsc"
        : "=a"(a), "=d"(d)
        :
        :
    );
    return ((uint64)d << 32u) | a;
}
