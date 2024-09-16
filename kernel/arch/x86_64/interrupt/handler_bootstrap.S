/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include <arch/x86_64/selector.h>

/*
** Create 256 interrupt service routine bootstraps.
**
** Those bootstraps will quite immediately call the ISR wrapper, that will
** push the common registers onto the stack and call the common interrupt
** handler.
*/

.section .text
.code64

.align 16
.global isr_bootstraps
isr_bootstraps:

.set i, 0
.rept 256
    .align 16           // We make sure each isr is 16 bytes long
    .if (i == 8 || (i >= 10 && i <= 14) || i == 17)
        pushq $i        // Interrupt vector
        jmp isr_wrapper
    .else
        pushq $0        // Fake error code
        pushq $i        // Interrupt vector
        jmp isr_wrapper
    .endif
    .set i, i + 1
.endr

.global isr_bootstrap_len
isr_bootstrap_len:
    .quad 16

isr_wrapper:
    // Clear the direction flag
    cld

    // Push the general-purpose registers
    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12
    pushq %r11
    pushq %r10
    pushq %r9
    pushq %r8
    pushq %rdi
    pushq %rsi
    pushq %rbp
    pushq %rbx
    pushq %rdx
    pushq %rcx
    pushq %rax

    mov %rsp, %rdi      // Pass the iframe using rdi

    .extern common_int_handler
    call common_int_handler

    popq %rax
    popq %rcx
    popq %rdx
    popq %rbx
    popq %rbp
    popq %rsi
    popq %rdi
    popq %r8
    popq %r9
    popq %r10
    popq %r11
    popq %r12
    popq %r13
    popq %r14
    popq %r15

    add $16, %rsp       // Pop interrupt vector and error code
    iretq               // Return from interrupt
