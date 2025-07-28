/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/cpu/cpu.h"
#include "lib/sync/spinlock.h"
#include "lib/string.h"

struct cpu bsp;

LIST_HEAD(g_cpus_available);
LIST_HEAD(g_cpus_active);

struct spinlock g_cpus_lock = SPINLOCK_DEFAULT;

/*
** Initialise a cpu structure.
*/
void
cpu_init(
    struct cpu *cpu,
    size_t id
) {
    memset(cpu, 0, sizeof(*cpu));

    cpu->id = id;
    cpu->local_data.cpu = cpu;
}

struct cpu *
cpu_get_bsp(
    void
) {
    return &bsp;
}
