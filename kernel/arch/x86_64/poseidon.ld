/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
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

    KERNEL_START = .;

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

    . = ALIGN(4K);
    PROVIDE(__start_bss = .);

    /* Read-write data (uninitialized) and stack */
    .bss ALIGN(4K) :
    {
        *(COMMON);
        *(.bss);
    }

    . = ALIGN(4K);
    PROVIDE(__stop_bss = .);

    KERNEL_END = .;
}
