/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/cpu/cpu.h"
#include "poseidon/memory/memory.h"

/* Number of CPUs on the current system. */
uint g_cpus_len = 0;

/* An array for all cpus. */
struct cpu g_cpus[KCONFIG_MAX_CPUS] = { 0 };

/* The end of `cpus`. Used for iterating over `cpus`. */
struct cpu const *g_cpus_end = g_cpus + KCONFIG_MAX_CPUS;
