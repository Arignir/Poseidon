/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
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
    .set isr_bootstrap_start, .
    .align 16           // We make sure each isr is 16 bytes long
    .if (i == 8 || (i >= 10 && i <= 14) || i == 17)
        nop
        nop
        pushq $i        // Interrupt vector
        jmp isr_wrapper
    .else
        pushq $0        // Fake error code
        pushq $i        // Interrupt vector
        jmp isr_wrapper
    .endif
    // Calculate the length of an interrupt bootstrap
    .set $isr_bootstrap_len, . - isr_bootstrap_start
    .set i, i + 1
.endr

.global isr_bootstrap_len
isr_bootstrap_len:
    .quad $isr_bootstrap_len

isr_wrapper:
    // Clear the direction flag
    cld

    // Push the general-purpose registers
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %rdi
    push %rsi
    push %rbp
    push %rbx
    push %rdx
    push %rcx
    push %rax

    mov %rsp, %rdi      // Pass the iframe using rdi

    .extern common_int_handler
    call common_int_handler

    pop %rax
    pop %rcx
    pop %rdx
    pop %rbx
    pop %rbp
    pop %rsi
    pop %rdi
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15

    add $16, %rsp       // Pop interrupt vector and error code
    iretq               // Return from interrupt
