/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to manipulate virtual memory.
*/

#ifndef _POSEIDON_MEMORY_VMM_H_
# define _POSEIDON_MEMORY_VMM_H_

# include <poseidon/poseidon.h>
# include <poseidon/memory.h>

/*
** Arch-indepentant flags for mmap
**
** NOTE: The read-only portion of MMAP_RDEXEC is on a best-effort basis.
** Some old architectures do not support the W^X pattern.
*/
# define MMAP_KERNEL        0b00000000      // Page belongs to kernel space
# define MMAP_USER          0b00000001      // Page belongs to user space
# define MMAP_RDONLY        0b00000010      // Page is only readable
# define MMAP_RDEXEC        0b00000100      // Page is executable, read-only
# define MMAP_RDWR          0b00001000      // Page is readable and writable

/* The integer type matching the above flags. */
typedef uint mmap_flags_t;

# define MUNMAP_FREE        0b00000000      // Free the physical frame
# define MUNMAP_NO_FREE     0b00000001      // Do not free the physical frame

/* The integer type matching the above flags. */
typedef uint munmap_flags_t;

status_t                vmm_map(virtaddr_t, size_t, mmap_flags_t);
status_t                vmm_map_device(virtaddr_t, physaddr_t, size_t, mmap_flags_t);
status_t                vmm_remap(virtaddr_t, physaddr_t, size_t, mmap_flags_t, munmap_flags_t);
void                    vmm_unmap(virtaddr_t, size_t, munmap_flags_t);
status_t                vmm_validate_user_buffer(void const *, size_t);
status_t                vmm_validate_user_str(char const *, size_t *);

/*
** The following functions are arch-dependent.
*/

bool                    vmm_is_mapped(virtaddr_const_t va);
bool                    vmm_is_mapped_user(virtaddr_const_t va);
status_t                vmm_map_frame(virtaddr_t va, physaddr_t pa, mmap_flags_t flags);
void                    vmm_unmap_frame(virtaddr_t va, munmap_flags_t flags);

#endif /* !_POSEIDON_MEMORY_VMM_H_ */
