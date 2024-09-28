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

/*
** A structure representing the arch-specific values of a single CPU.
*/
struct arch_cpu {
    struct cpuid cpuid;         // CPUID
    uint32 acpi_id;             // ACPI Processor id
    uint32 apic_id;             // Local APIC id
};
