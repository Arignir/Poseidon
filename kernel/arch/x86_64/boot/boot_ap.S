/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <arch/x86_64/selector.h>

.extern gdt

.section .text
.code16

.global start_ap
.type start_ap, @function
start_ap:
    cli

    // Setup the data segment
    mov $0xFFFF, %ax
    mov %ax, %ds
    mov %ax, %ss

    // Load the 64-bits GDT
    .extern gdt_fatptr
    lgdt %ds:(gdt_fatptr - 0xFFFF0)

    // Enable protected mode (without paging)
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    jmp 0f
0:

    .extern boot_pml4
    mov $boot_pml4, %eax            // Load the address of the PML4 into CR3
    mov %eax, %cr3

    mov %cr4, %eax
    or $(1 << 5), %eax              // Enable PAE
    mov %eax, %cr4

    mov $0xC0000080, %ecx
    rdmsr                           // Read the EFER MSR
    or $(1 <<  8), %eax             // Set the LM-bit
    or $(1 << 11), %eax             // Set the NX-bit
    wrmsr                           // Update the EFER MSR

    mov %cr0, %eax
    or $(1 << 31), %eax             // Enable paging
    mov %eax, %cr0

    ljmpl  $KERNEL_CODE_SELECTOR, $start64

.code64
start64:
    // Reload all data segment registers with the new 64-bit data segment selector
    mov $KERNEL_DATA_SELECTOR, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    mov (ap_boot_stack), %rsp        // Reset kernel stack

    .extern ap_setup
    call ap_setup

catch_fire:
    hlt
    jmp catch_fire