/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

OUTPUT_FORMAT(elf64-x86-64);

ENTRY(start)

SECTIONS
{

    /*
    ** Place the kernel at 1MiB, a conventional place for kernels to be loaded
    ** at by the bootloader
    */
    . = 1M;

    PROVIDE(kernel_start = .);

    /*
    ** Place the multiboot header on top of everything
    ** Then put the .text section
    */
    .text ALIGN(4K) :
    {
        KEEP(*(.multiboot));
        *(.text);
        . = ALIGN(4K);
        *(.text.boot);
    }

    /* Read-only data */
    .rodata ALIGN(4K) :
    {
        *(.rodata);
        . = ALIGN(4K);
        *(.rodata.boot);
    }

    /* Read-write data (initialized) */
    .data ALIGN(4K) :
    {
        *(.data);
        . = ALIGN(4K);
        *(.data.boot);
    }

    /* Read-write data (uninitialized) and stack */
    .bss ALIGN(4K) :
    {
        *(COMMON);
        *(.bss);
    }

    . = ALIGN(4K);

    PROVIDE(kernel_end = .);

    /*
    ** This is where the heap begins in the virtual address space.
    **
    ** Do not worry, the kheap allocator won't erase the content
    ** of `kernel_boot_heap` (defined below). That arary is
    ** a bunch of physical frames distributed by the frame allocator
    ** at boot time. They aren't identity mapped.
    */
    PROVIDE(kernel_heap_start = .);

    /*
    ** Provide a bit of RAM to feed the boot heap with known-free physical memory.
    **
    ** See `poseidon/memory/pmm.c` for an explanation about why put this section
    ** after the kernel.
    */
    .kernel_boot_heap ALIGN(4K) :
    {
        *(.kernel_boot_heap);
    }
}
