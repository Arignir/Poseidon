/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to manipulate, allocate and free memory.
**
** This file defines the most important and high-level functions to manipulate
** memory.
*/

#pragma once

#include "poseidon/poseidon.h"

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
** A virtual address where the content shouldn't be modified.
*/
typedef const void *virtaddr_const_t;

/*
** The equivalent of NULL for physical addresses.
*/
#define PHYS_NULL               ((physaddr_t)0)

/*
** Shortcuts to make the code easier to read.
*/
#define PAGE_SIZE               (KCONFIG_PAGE_SIZE)
#define FRAME_SIZE              (KCONFIG_FRAME_SIZE)

/*
** Mask to test if an address is page-aligned
*/
#define PAGE_SIZE_MASK          ((PAGE_SIZE) - 1u)

/*
** Test if a given address is a multiple of PAGE_SIZE
*/
#define IS_PAGE_ALIGNED(x)      (!((uintptr)(x) & PAGE_SIZE_MASK))

extern uint8 kernel_start[];                    // Beginning of the kernel (virtual)
extern uint8 kernel_end[];                      // End of the kernel (virtual)
extern uint8 kernel_heap_start[];               // Beginning of the kernel's heap (virtual)
