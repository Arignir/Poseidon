/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _ARCH_X86_64_API_CPU_H_
# define _ARCH_X86_64_API_CPU_H_

# include <arch/x86_64/cpuid.h>
# include <poseidon/poseidon.h>

/*
** A structure representing the arch-specific values of a single CPU.
*/
struct arch_cpu
{
    struct cpuid cpuid;         // CPUID
    uint32 apic_id;             // Local APIC id
};

#endif /* !_ARCH_X86_64_API_CPU_H_ */
