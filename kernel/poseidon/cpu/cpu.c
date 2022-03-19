/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/cpu/cpu.h>
#include <poseidon/memory/memory.h>

/* Number of CPUs on the current system. */
uint ncpu = 0;

/* An array for all cpus. */
struct cpu cpus[KCONFIG_MAX_CPUS] = { 0 };

/* The end of `cpus`. Used for iterating over `cpus`. */
struct cpu const *cpus_end = cpus + KCONFIG_MAX_CPUS;