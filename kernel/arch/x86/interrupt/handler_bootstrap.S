/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#include <arch/x86/selector.h>

/*
** Create 256 interrupt service routine bootstraps.
**
** Those bootstraps will quite immediately call the ISR wrapper, that will
** push the common registers onto the stack and call the common interrupt
** handler.
*/

.section .text
.align 16
.global isr_bootstraps
isr_bootstraps:

.set i, 0
.rept 256
    .align 16           // We make sure each isr is 16 bytes long
    .if (i == 8 || (i >= 10 && i <= 14) || i == 17)
        pushl $i        // Interrupt vector
        jmp isr_wrapper
    .else
        pushl $0        // Fake error code
        pushl $i        // Interrupt vector
        jmp isr_wrapper
    .endif
    .set i, i + 1
.endr

isr_wrapper:
    pushal              // Push edi, esi, ebp, esp, ebx, edx, ecx, eax

    push %ds            // Save segment registers
    push %ss
    push %es
    push %fs
    push %gs

    mov $KERNEL_DATA_SELECTOR, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    push %esp           // Push iframe onto the stack

    .extern common_int_handler
    call common_int_handler

    add $0x4, %esp      // Pop iframe

    pop %gs             // Restore segment registers
    pop %fs
    pop %es
    pop %ss
    pop %ds

    popal               // Pop edi, esi, ebp, esp, ebx, edx, ecx, eax
    add $0x8, %esp      // Pop interrupt vector and error code
    iret                // Return from interrupt (pop eip, cs, eflags, ss, esp)
