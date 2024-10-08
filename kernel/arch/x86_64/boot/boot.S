/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** References:
**
** Intel Architecture Software Manual, volume 3A: System Programming Guide, Part 1
**   https://software.intel.com/sites/default/files/managed/7c/f1/253668-sdm-vol-3a.pdf
**
** The Multiboot2 Specification version 2.0:
**   https://www.gnu.org/software/grub/manual/multiboot2/multiboot.pdf
**
*/

#include "poseidon/kconfig.h"
#include "poseidon/boot/multiboot2.h"
#include "arch/x86_64/selector.h"

.section .text
.code32

/*
** Kernel starting point.
**
** The bootloader drops us here according to the multiboot's specification.
**
** The eax register should contain the multiboot2 bootloader's magic value, and
** we should crash if it's not the case.
** The ebx register should contain a pointer to the multiboot2 structure, which
** we'll save for later use.
**
** At this stage, the CPU is in 32-bits protected mode and paging is disabled.
** We must enable paging and go to long mode before executing any Rust code.
**
** We cannot dereference any address unless we do the translation to its
** physical address counterpart ourselves. This is done using the V2P() macro.
**
** NOTE: The BSS section is already cleaned by the bootloader (according to the
** the multiboot specification, see the note at the end of section 3.1.5)
**
*/
.global start
.type start, @function
start:
    cld                             // Clear direction flag
    cli                             // Clear interrupt flag
    mov $bsp_kernel_stack_top, %esp // Set-up kernel stack

    call ensure_multiboot2          // Ensure we have been booted via multiboot2
    call detect_cpuid               // Ensure CPUID is available, panics overwise
    call detect_ia_32e              // Ensure IA-32e mode is available.

    call setup_paging               // Set up paging without enabling it
    call enable_paging              // Enable long-mode and paging, leaving
                                    // the CPU in a 32-bit compatiblity mode.

    .extern g_gdt_fatptr
    lgdt (g_gdt_fatptr)             // Load the 64-bit GDT

    // Transition to 64-bit by reloading the code segment register
    // with the new 64-bit code segment selector
    ljmp $KERNEL_CODE_SELECTOR, $start64

.code64

start64:

    // Reload all data segment registers with the new 64-bit data segment selector
    mov $KERNEL_DATA_SELECTOR, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    // Clear the screen
    mov $0xB8000, %rdi
    mov $0x0F200F200F200F20, %rax
    mov $500, %rcx
    rep stosq

    // Call the kernel's common entry point.
    .extern kmain
    call kmain

    // If the kernel returns (it shouldn't), loop as a safety measure
.hlt_loop:
    cli
    hlt
    jmp .hlt_loop

.code32

ensure_multiboot2:
    cmp $MULTIBOOT2_BOOTLOADER_MAGIC, %eax
    jne .no_multiboot2

    .extern g_mb_tag_pa
    .extern g_mb_tag_size
    movl (%ebx), %ecx
    movl %ecx, g_mb_tag_len         // Save the multiboot2 segment length for later use
    add $8, %ebx
    movl %ebx, g_mb_tag_pa          // Save the multiboot2 segment pointer for later use.

    ret

.no_multiboot2:
    mov $boot_panic_no_multiboot2, %edi
    call boot_panic                 // boot_panic never returns

/*
** Detect if the CPUID instruction is available.
** Panic if it is not.
*/
.global detect_cpuid
.type detect_cpuid, @function
detect_cpuid:
    pushfl                          // Mov EFLAGS to EAX through the stack
    pop %eax

    mov %eax, %ecx                  // Save EFLAGS

    xor $(1 << 21), %eax            // Flip the ID bit

    push %eax                       // Copy eax to EFLAGS
    popfl

    pushfl                          // Copy EFLAGS back to EAX (with the ID bit
    pop %eax                        // flipped if CPUID is supported)

    push %ecx                       // Restore the EFLAGS we saved earlier
    popfl

    cmp %ecx, %eax                  // Test if the bit flip was reverted by the CPU,
                                    // indicating that the CPUID instruction is supported

    je .no_cpuid                    // If CPUID isn't supported, panic
    ret

.no_cpuid:
    mov $boot_panic_no_cpuid, %edi
    call boot_panic                 // boot_panic never returns

/*
** Detect if long-mode is available.
** Panic if it is not.
*/
detect_ia_32e:
    mov $0x80000000, %eax           // Retrieve the highest CPUID function supported
    cpuid
    cmp $0x80000001, %eax           // Test if the extended processor information
    jb .no_ia_32e                   // can be retrieved

    mov $0x80000001, %eax           // Retrieve the extended processor information
    cpuid
    test $(1 << 29), %edx           // Test if the "Long Mode" bit is set
    jz .no_ia_32e

    ret

.no_ia_32e:
    mov $boot_panic_no_ia_32e, %edi
    call boot_panic

/*
** Set up paging without enabling it
**
** This function sets up a 4-level paging using 512 4KB pages, therefore
** mapping at most the first 2MB of virtual memory on its physical counterpart.
*/
setup_paging:
    mov $boot_pdpt, %eax
    or $0b000000000011, %eax        // Present + Writable
    mov %eax, (boot_pml4)

    mov $boot_pd, %eax
    or $0b000000000011, %eax        // Present + Writable
    mov %eax, (boot_pdpt)

    mov $boot_pt, %eax
    or $0b000000000011, %eax        // Present + Writable
    mov %eax, (boot_pd)

    // Iterate over all 512 entries of the PT, and map them using 4KB pages.
    mov $0, %ecx
.map_pt:
    mov $4096, %eax                 // 4KB Page
    mul %ecx                        // eax *= ecx

    cmp $kernel_end, %eax            // Stop if we have mapped the whole kernel
    jge .map_pt_end

    or $0b000000000011, %eax        // Present + Writable
    mov %eax, boot_pt(,%ecx,8)      // Map the entry

    inc %ecx
    cmp $512, %ecx
    jne .map_pt

.map_pt_end:

    // Setup recursive mapping
    mov $boot_pml4, %eax
    or $0b000000000011, %eax        // Present + Writable
    mov %eax, (boot_pml4 + 511*8)

    ret

/*
** Enable paging and long mode.
**
** NOTE: The CPU is in compatibility mode when leaving this function.
** A 64-bit code segment must be loaded to fully transition into 64-bit.
*/
enable_paging:
    mov %cr0, %eax
    and $(~(1 << 31)), %eax         // Disable paging
    mov %eax, %cr0

    mov $boot_pml4, %eax            // Load the address of the PML4 into CR3
    mov %eax, %cr3

    mov %cr4, %eax
    or $(1 << 5), %eax              // Set the PAE bit
    mov %eax, %cr4

    mov $0xC0000080, %ecx
    rdmsr                           // Read the EFER MSR
    or $(1 <<  8), %eax              // Set the LM-bit
    or $(1 << 11), %eax              // Set the NX-bit
    wrmsr                           // Update the EFER MSR

    mov %cr0, %eax
    or $(1 << 31), %eax             // Enable paging
    mov %eax, %cr0

    ret


/*
** Print an error message to the user using the VGA display.
** This function never returns.
**
** This is so early in the boot process that we can't rely on the
** usual panic utilities nor the usual logging drivers.
**
** It expects a pointer to a zero-terminated string in edi.
*/
boot_panic:
    mov $0xB8000, %eax              // Cursor
    mov $0x07, %bh                  // Set the formatting

.loop:
    movb (%edi), %bl                // Fetch the next char to print

    cmpb $0, %bl                    // Test if it is the end of the string
    je .endloop

    movw %bx, (%eax)                // Print the char
    add $2, %eax

    inc %edi                        // Increment the string pointer and start again
    jmp .loop

.endloop:
    cli
    hlt
    jmp .endloop

.section .bss

/*
** Bootstrap processor's kernel stack
*/
.align 4096
.global bsp_kernel_stack_bot
bsp_kernel_stack_bot:
    .skip KCONFIG_KERNEL_STACK_SIZE
.global bsp_kernel_stack_top
bsp_kernel_stack_top:

/*
** Boot PML4, used to transition into long mode, before paging is properly initialized
*/
.align 4096
.global boot_pml4
boot_pml4:
    .skip 512 * 8

/*
** Boot PDPT, used to transition into long mode, before paging is properly initialized
*/
.align 4096
boot_pdpt:
    .skip 512 * 8

/*
** Boot PD, used to transition into long mode, before paging is properly initialized
*/
.align 4096
boot_pd:
    .skip 512 * 8

/*
** Boot PT, used to transition into long mode, before paging is properly initialized
*/
.align 4096
boot_pt:
    .skip 512 * 8

.section .rodata

/*
** Error messages displayed on boot, before any drivers are available
*/
boot_panic_no_multiboot2:
    .string "Error: the kernel didn't boot using a multiboot2-compliant bootloader."
boot_panic_no_cpuid:
    .string "Error: the CPUID instruction isn't available."
boot_panic_no_ia_32e:
    .string "Error: the CPU doesn't support 64-bit instructions."
