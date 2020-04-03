/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Interrupts handling for the x86_64 architecture.
*/

#ifndef _ARCH_X86_64_INTERRUPT_H_
# define _ARCH_X86_64_INTERRUPT_H_

# include <poseidon/poseidon.h>
# include <arch/x86_64/register.h>

/*
** Exception/Interruption and their vector.
*/
enum interrupt_vector
{
    INT_DIVISION_BY_ZERO        = 0,
    INT_DEBUG,
    INT_NMI,
    INT_BREAKPOINT,
    INT_OVERFLOW,
    INT_OUT_OF_BOUNDS,
    INT_INVALID_OPCODE,
    INT_DEVICE_NA,
    INT_DOUBLE_FAULT,
    INT_INVALID_TSS             = 0xA,
    INT_SEGMENT_NOT_PRESENT,
    INT_STACK_FAULT,
    INT_GP_FAULT,
    INT_PAGE_FAULT,
    INT_FPU_EXCEPTION           = 0x10,
    INT_ALIGNMENT_CHECK,
    INT_MACHINE_CHECK,
    INT_SIMD_FP_EXCEPTION,
    INT_VIRT_EXCEPTION,
    INT_SECURITY_EXCEPTION      = 0x1E,

    INT_MAX_RESERVED_BY_INTEL   = 0x1F,
    INT_IRQ0                    = 0x20,

    INT_PIT                     = 0x20,
    INT_KEYBOARD                = 0x21,

    INT_IRQ8                    = 0x28,
    INT_IRQ15                   = 0x30,

    INT_APIC_TIMER              = 0x32,
    INT_APIC_ERROR              = 0x33,

    INT_SYSCALL                 = 0x80,

    INT_APIC_SPURIOUS           = 0xFF,

    INT_NB                      = 0x100,
};

/*
** Stack frame when an interruption/exception occures.
**
** This structure represents the content of the stack frame after an interruption
** or an exception.
**
** Most of these elemnts are pushed by the interrupt handler, but some of them
** are also pushed by the processor automatically when the exception is handled.
*/
struct iframe
{
    uintptr rax;
    uintptr rcx;
    uintptr rdx;
    uintptr rbx;
    uintptr rbp;
    uintptr rsi;
    uintptr rdi;
    uintptr r9;
    uintptr r10;
    uintptr r11;
    uintptr r12;
    uintptr r13;
    uintptr r14;
    uintptr r15;
    uintptr int_vector;
    uintptr error_code;
    uintptr rip;
    uintptr cs;
    struct rflags rflags;
} __packed;

/*
** The different valid values for the `type` field of
** `struct idt_descriptor`.
*/
enum idt_gate_descriptor_type
{
    IDT_CALL_GATE           = 12,
    IDT_INTERRUPT_GATE      = 14,
    IDT_TRAP_GATE           = 15,
};

/*
** The descriptor of an interrupt or trap gate.
**
** The layout of this structure is defined by Intel.
*/
struct idt_descriptor
{
    uintptr offset_low: 16;
    uintptr segment_selector: 16;
    uintptr ist: 3;
    uintptr _reserved0: 5;
    uintptr type: 4;
    uintptr _reserved1: 1;
    uintptr dpl: 2;
    uintptr present: 1;
    uintptr offset_mid: 16;
    uintptr offset_high: 32;
    uintptr _reserved2: 32;
} __packed;

static_assert(sizeof(struct idt_descriptor) == 16);


/*
** A fat pointer given in parameter to LIDT.
**
** The layout of this structure is defined by Intel.
*/
struct idt_fatptr {
    uint16 limit;
    struct idt_descriptor *base;
} __packed;

static_assert(sizeof(struct idt_fatptr) == 10);

/*
** Create an interrupt gate entry, using the given address as the entry point.
** The value of other fields can also be appended.
*/
# define NEW_IDT_INTERRUPT_GATE_ENTRY(offset, ...)                  \
    (struct idt_descriptor) {                                       \
        .offset_low = (((uintptr)(offset)) & 0xFFFF),           \
        .offset_mid = ((((uintptr)(offset)) >> 16) & 0xFFFF),   \
        .offset_high = ((((uintptr)(offset)) >> 32) & 0xFFFFFFFF),\
        .type = IDT_INTERRUPT_GATE,                             \
        ._reserved0 = 0,                                        \
        ._reserved1 = 0,                                        \
        __VA_ARGS__                                             \
    }

/*
** Create a trap gate entry, using the given address as the entry point.
** The value of other fields can also be appended.
*/
# define NEW_IDT_TRAP_GATE_ENTRY(offset, ...)                       \
    (struct idt_descriptor) {                                       \
        .trap = {                                                   \
            .offset_low = (((uintptr)(offset)) & 0xFFFF),           \
            .offset_mid = ((((uintptr)(offset)) >> 16) & 0xFFFF),   \
            .offset_high = ((((uintptr)(offset)) >> 32) & 0xFFFFFFFF),\
            .type = IDT_TRAP_GATE,                                  \
            ._reserved0 = 0,                                        \
            ._reserved1 = 0,                                        \
            __VA_ARGS__                                             \
        },                                                          \
    }

void    setup_idt(void);

#endif /* !_ARCH_X86_64_INTERRUPT_H_ */
