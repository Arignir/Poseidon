/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

OUTPUT_FORMAT(elf32-i386);

ENTRY(start)

SECTIONS
{
	/*
	** Place the kernel at 1MiB, a conventional place for kernels to be loaded
	** at by the bootloader
	*/
	. = 1M;

	KERNEL_PHYS_START = .;

	/*
	** Place the multiboot header on top of everything
	** Then put the .text section
	*/
	.text BLOCK(4K) : ALIGN(4K)
	{
		KEEP(*(.multiboot))
		*(.text)
		. = ALIGN(4K);
		*(.text.boot)
	}

	/* Read-only data */
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
		. = ALIGN(4K);
		*(.rodata.boot)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
		. = ALIGN(4K);
		*(.data.boot)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

	KERNEL_PHYS_END = .;
}


