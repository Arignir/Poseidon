/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/cpu/cpu.h"

// Number of CPUs on the current system.
uint g_cpus_len = 0;

// An array for all cpus.
struct cpu g_cpus[KCONFIG_MAX_CPUS] = { 0 };

// The end of `g_cpus`. Used for iterating over `g_cpus`.
struct cpu const *g_cpus_end = g_cpus + KCONFIG_MAX_CPUS;

// An array of all cpu-local data .
struct cpu_local_data g_cpus_local_data[KCONFIG_MAX_CPUS] = { 0 };

/*
** Get the bootstrap processor's CPU structure.
**
** It is assumed to always be `g_cpus[0]`.
*/
struct cpu *
cpu_get_bsp(
    void
) {
    return &g_cpus[0];
}

/*
** Initialize the CPU-local table.
**
** This needs only to be called once by the BSP.
*/
void
cpu_init(
    void
) {
    size_t i;

    for (i = 0; i < KCONFIG_MAX_CPUS; ++i) {
        g_cpus_local_data[i].cpu = &g_cpus[i];
        g_cpus_local_data[i].thread = NULL;
    }
}
