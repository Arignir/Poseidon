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

/*
** A list of all MSRs.
**
** Feel free to complete this list as you need.
*/
enum msr_list {
    MSR_IA32_APIC_BASE      = 0x1B,
    MSR_IA32_EFER           = 0xC0000080,
    MSR_IA32_FSBASE         = 0xC0000100,
    MSR_IA32_GSBASE         = 0xC0000101,
    MSR_IA32_KERNEL_GSBASE  = 0xC0000102,
};

struct [[gnu::packed]] msr_ia32_efer {
    union {
        struct {
            size_t syscall: 1;          // SYSCALL enable
            size_t : 7;
            size_t ia_32e_enabled: 1;   // IA-32e mode enabled
            size_t : 1;
            size_t ia_32e_active: 1;    // IA-32e mode active
            size_t nx: 1;               // Execute Disable bit enable
            size_t : 4;
            size_t : 16;
        };
        uint64 raw;
    };
};

static_assert(sizeof(struct msr_ia32_efer) == sizeof(uint64));

/*
** Write `value` to the model-specific register named by `msr`.
*/
static inline
void
msr_write(
    uint32 msr,
    uint64 value
) {
    /*
    ** We don't have to mask `value` to reduce it to two 32-bits because
    ** the high-order bits of rax, rdx and rcx are ignored by wrmsr.
    */
    asm volatile (
        "wrmsr"
        :
        : "a"(value), "d"(value >> 32), "c"(msr)
        :
    );
}

/*
** Read the content of the model-specific register named by `msr`.
*/
static inline
uint64
msr_read(
    uint32 msr
) {
    uint32 eax;
    uint32 edx;

    asm volatile (
        "rdmsr"
        :
            "=a"(eax),
            "=d"(edx)
        : "c"(msr)
        :
    );

    return (uint64)eax | ((uint64)edx << 32u);
}

static inline
void
msr_and(
    uint32 msr,
    uint64 mask
) {
    msr_write(msr, msr_read(msr) & mask);
}

static inline
void
msr_or(
    uint32 msr,
    uint64 mask
) {
    msr_write(msr, msr_read(msr) | mask);
}
