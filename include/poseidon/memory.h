/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Architecture-independent API to manipulate, allocate and free the memory.
*/

#ifndef _POSEIDON_MEMORY_H_
# define _POSEIDON_MEMORY_H_

# include <poseidon/poseidon.h>
# include <arch/target/api/memory.h>

/*
** A physical address.
*/
typedef uintptr physaddr_t;

/*
** A virtual address.
**
** This type is mostly used inside the memory allocators, to create a clear
** distinction between physical and virtual addresses and avoid any confusion.
*/
typedef void *virtaddr_t;

/*
** The equivalent of NULL for physical addresses.
*/
# define PHYS_NULL	((physaddr_t)0)

/*
** Constants provided by the linker that indicate the beginning and end of
** the kernel.
*/
extern char const KERNEL_PHYS_START[];
extern char const KERNEL_PHYS_END[];

#endif /* !_POSEIDON_MEMORY_H_ */
