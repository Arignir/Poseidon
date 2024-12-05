/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "arch/x86_64/cpuid.h"
#include "poseidon/poseidon.h"

struct cpu;

/*
** A structure representing the architecture-specific values of a single CPU.
*/
struct arch_cpu {
    struct cpuid cpuid;         // CPUID
    uint32 acpi_id;             // ACPI Processor id
    uint32 apic_id;             // Local APIC id
};

/*
** Return the cpu-local data.
**
** The CPU-local data is held in the GS segment, which we can easily
** access using the `__seg_gs` GCC builtin.
*/
static inline
struct cpu_local_data __seg_gs *
arch_fetch_current_cpu_local_data(
    void
) {
    return (struct cpu_local_data __seg_gs*)0;
}
