/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to manipulate, allocate and free memory.
**
** This file defines the most important and high-level functions to manipulate
** memory. More low-level ones can be found in `poseidon/memory/`.
*/

#ifndef _POSEIDON_MEMORY_H_
# define _POSEIDON_MEMORY_H_

# include <poseidon/poseidon.h>
# include <arch/target/api/memory.h>

/*
** Two types are defined below: `physaddr_t` and `virtaddr_t`.
** They are used by memory-related functions to create a clear distinction
** between physical and virtual addresses, therefore avoiding any confusion.
*/

/*
** A physical address.
*/
typedef uintptr physaddr_t;

/*
** A virtual address.
*/
typedef void *virtaddr_t;

/*
** The equivalent of NULL for physical addresses.
*/
# define PHYS_NULL  ((physaddr_t)0)

/*
** Constants provided by the linker that indicate the beginning and end of
** the kernel.
*/
extern char const KERNEL_PHYS_START[];
extern char const KERNEL_PHYS_END[];

#endif /* !_POSEIDON_MEMORY_H_ */
