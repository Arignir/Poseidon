/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <arch/x86/selector.h>
#include <poseidon/kconfig.h>

.section .text

.global start
.type start, @function
start:
    // Set up boot stack
    mov $bsp_kernel_stack_top, %esp

    // Clear x86 registers
    cli
    cld

    // Load the boot GDT
    .extern boot_gdt_fatptr
    mov $boot_gdt_fatptr, %eax
    lgdt (%eax)

    // Update data segment registers with the new data segment selector
    mov $KERNEL_DATA_SELECTOR, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    // Update code segment register with a new code segment selector
    jmp $KERNEL_CODE_SELECTOR, $.far_jmp
.far_jmp:

    // Save the multiboot pointer before calling any C code
    .extern mb_tag
    add $8, %ebx
    mov %ebx, (mb_tag)

    // Load the multiboot structure
    .extern multiboot_load
    call multiboot_load

    // Continue the initialisation of the kernel by calling `kmain()`.
    .extern kmain
    call kmain

    // Main shouldn't return. In case it does, halt the computer
loop:
    cli
    hlt
    jmp loop

/*
** Bootstrap processor kernel's stack
*/
.section .bss
.align 4096
bsp_kernel_stack_bottom:
    .skip KCONFIG_KERNEL_STACK_SIZE
bsp_kernel_stack_top:
