/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#ifndef _POSEIDON_MEMORY_VMM_H_
# define _POSEIDON_MEMORY_VMM_H_

# include <poseidon/poseidon.h>
# include <poseidon/memory.h>
# include <arch/target/api/vmm.h>

/*
** Test whever the given virtual address is mapped.
**
** Architectures must implement this function.
*/
static bool vmm_is_mapped(virtaddr_t va) __arch_alias(vmm_is_mapped);

/*
** Return the framme that the given virtual address is mapped to, or PHYS_NULL
** if it isn't mapped.
**
** Architectures must implement this function.
*/
static bool vmm_get_frame(virtaddr_t va) __arch_alias(vmm_get_frame);

#endif /* !_POSEIDON_MEMORY_VMM_H_ */
