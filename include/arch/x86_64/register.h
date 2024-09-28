/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Types and functions to read and manipulate x86_64 registers.
*/

#pragma once

#include "poseidon/poseidon.h"

/*
** System flags and fields in the RFLAGS register.
**
** The layout of this structure is defined by Intel.
*/
struct rflags {
    union {
        struct {
            size_t cf: 1;
            size_t : 1;
            size_t pf: 1;
            size_t : 1;
            size_t af: 1;
            size_t : 1;
            size_t zf: 1;
            size_t sf: 1;
            size_t tf: 1;
            size_t intf: 1;
            size_t df: 1;
            size_t of: 1;
            size_t iopl: 2;
            size_t nt: 1;
            size_t : 1;
            size_t rf: 1;
            size_t vm: 1;
            size_t ac: 1;
            size_t vif: 1;
            size_t vip: 1;
            size_t id: 1;
            size_t : 10;
            size_t : 32;
        };
        uint64 raw;
    };
};

static_assert(sizeof(struct rflags) == sizeof(uint64));

static inline
struct rflags
get_rflags(
    void
) {
    struct rflags rflags;

    asm volatile (
        "pushfq;"
        "popq %0;"
        : "=rm"(rflags.raw)
        :
        :
    );
    return rflags;
}

static inline
void
set_rflags(
    struct rflags rflags
) {
    asm volatile (
        "pushq %0;"
        "popfq;"
        :
        : "g"(rflags.raw)
        : "cc"
    );
}

/*
** The Control Register 3.
**
** The layout of this structure is defined by Intel.
*/
struct cr3 {
    union {
        struct {
            size_t : 3;
            size_t pwt: 1;             // Page-level write through
            size_t pcd: 1;             // Page-level cache disable
            size_t : 7;
            size_t pagedir: 20;        // Physical address of a page directory
            size_t : 32;
        };
        uintptr value;
    };
};

static_assert(sizeof(struct cr3) == sizeof(uint64));

static inline
struct cr3
get_cr3(
    void
) {
    struct cr3 cr3;

    asm volatile (
        "mov %%cr3, %0"
        : "=rm"(cr3.value)
        :
        :
    );
    return cr3;
}

static inline
void
set_cr3(
    struct cr3 cr3
) {
    asm volatile (
        "mov %0, %%cr3"
        :
        : "g"(cr3.value)
        : "memory"
    );
}
