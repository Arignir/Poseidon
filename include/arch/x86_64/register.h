/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Types and functions to read and manipulate x86_64 registers.
*/

#ifndef _ARCH_X86_64_REGISTER_H_
# define _ARCH_X86_64_REGISTER_H_

# include <poseidon/poseidon.h>

/*
** System flags and fields in the RFLAGS register.
**
** The layout of this structure is defined by Intel.
*/
struct rflags
{
    union {
        struct {
            uint8 cf: 1;
            uint8 _reserved0: 1;
            uint8 pf: 1;
            uint8 _reserved1: 1;
            uint8 af: 1;
            uint8 _reserved2: 1;
            uint8 zf: 1;
            uint8 sf: 1;
            uint8 tf: 1;
            uint8 intf: 1;
            uint8 df: 1;
            uint8 of: 1;
            uint8 iopl: 2;
            uint8 nt: 1;
            uint8 _reserved3: 1;
            uint8 rf: 1;
            uint8 vm: 1;
            uint8 ac: 1;
            uint8 vif: 1;
            uint8 vip: 1;
            uint8 id: 1;
            uint16 _reserved4: 10;
            uint32 _reserved5: 32;
        } __packed;
        uint32 value;
    } __packed;
} __packed;

static_assert(sizeof(struct rflags) == sizeof(uint64));

static inline
struct rflags
get_rflags(void)
{
    struct rflags rflags;

    asm volatile (
        "pushfq;"
        "popq %0;"
        : "=rm"(rflags.value)
        :
        :
    );
    return (rflags);
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
        : "g"(rflags.value)
        : "cc"
    );
}

/*
** The Control Register 3.
**
** The layout of this structure is defined by Intel.
*/
struct cr3
{
    union {
        struct {
            uintptr _ignored_0: 3;
            uintptr pwt: 1;             // Page-level write through
            uintptr pcd: 1;             // Page-level cache disable
            uintptr _ignored_1: 7;
            uintptr pagedir: 20;        // Physical address of a page directory
            uintptr _reserved_1: 32;
        };
        uintptr value;
    };
};

static_assert(sizeof(struct cr3) == sizeof(uint64));

static inline
struct cr3
get_cr3(void)
{
    struct cr3 cr3;

    asm volatile (
        "mov %%cr3, %0"
        : "=rm"(cr3.value)
        :
        :
    );
    return (cr3);
}

static inline
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

#endif /* !_ARCH_X86_64_REGISTER_H_ */
