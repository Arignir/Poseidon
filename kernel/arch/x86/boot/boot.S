/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
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
** Detect if the CPUID instruction is available.
*/
.global detect_cpuid
.type detect_cpuid, @function
detect_cpuid:
    pushfl                          // Save EFLAGS
    pushfl                          // Store EFLAGS
    xorl $0x00200000, (%esp)        // Invert the ID bit in stored EFLAGS
    popfl                           // Load stored EFLAGS (with ID bit inverted)
    pushfl                          // Store EFLAGS again (ID bit may or may not be inverted)
    pop %eax                        // eax = modified EFLAGS (ID bit may or may not be inverted)
    xorl (%esp), %eax               // eax = whichever bits were changed
    popfl                           // Restore original EFLAGS
    and $0x00200000, %eax           // eax = zero if ID bit can't be changed, else non-zero
    shrl $21, %eax                  // Cast the result to bool
    ret

/*
** Bootstrap processor kernel's stack
*/
.section .bss
.align 4096
bsp_kernel_stack_bottom:
    .skip KCONFIG_KERNEL_STACK_SIZE
bsp_kernel_stack_top:
