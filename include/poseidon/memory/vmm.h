/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to manipulate virtual memory.
*/

#pragma once

#include "poseidon/poseidon.h"
#include "poseidon/memory/memory.h"

/*
** Arch-indepentant flags for mmap
**
** NOTE: Those flags are on a best-effort basis.
** For example, some old architectures do not support the NX bit.
*/
#define MMAP_KERNEL         0b00000000      // Page belongs to kernel space. Contradicts `MMAP_USER`.
#define MMAP_USER           0b00000001      // Page belongs to user space. Contradicts `MMAP_KERNEL`.
#define MMAP_RDONLY         0b00000000      // Page is only readable. Contradicts `MMAP_RDWR`.
#define MMAP_RDWR           0b00000010      // Page is readable and writable. Contradicts `MMAP_RDONLY`.
#define MMAP_EXEC           0b00000100      // Page is executable.
                                            // Can be used with both `MMAP_RDONLY` and `MMAP_RDWR`.

/* The integer type matching the above flags. */
typedef uint mmap_flags_t;

#define MUNMAP_FREE         0b00000000      // Free the physical frame
#define MUNMAP_NO_FREE      0b00000001      // Do not free the physical frame

/* The integer type matching the above flags. */
typedef uint munmap_flags_t;

status_t vmm_map(virtaddr_t, size_t, mmap_flags_t);
status_t vmm_map_device(virtaddr_t, physaddr_t, size_t, mmap_flags_t);
status_t vmm_remap(virtaddr_t, physaddr_t, size_t, mmap_flags_t, munmap_flags_t);
void vmm_unmap(virtaddr_t, size_t, munmap_flags_t);
status_t vmm_validate_user_buffer(void const *, size_t);
status_t vmm_validate_user_str(char const *, size_t *);

/*
** Implement safe wrappers around the arch-dependent API.
*/

#include "arch/target/api/vmm.h"

/*
** Test whether the given virtual address is mapped.
*/
static inline
bool
vmm_is_mapped(
    virtaddr_const_t va
) {
    return arch_vmm_is_mapped(va);
}

/*
** Test whether the given virtual address is mapped and belongs to user-space.
*/
static inline
bool
vmm_is_mapped_user(
    virtaddr_const_t va
) {
    return arch_vmm_is_mapped_user(va);
}

/*
** Map the virtual address `va` on the physical address `pa` with the
** permission described in `flags`.
**
** This function doesn't overwrite any existing mapping, failing instead.
*/
static inline
status_t
vmm_map_frame(
    virtaddr_t va,
    physaddr_t pa,
    mmap_flags_t flags
) {
    return arch_vmm_map_frame(va, pa, flags);
}

/*
** Unmap the virtual address `va`.
*/
static inline
void
vmm_unmap_frame(
    virtaddr_t va,
    munmap_flags_t flags
) {
    return arch_vmm_unmap_frame(va, flags);
}
