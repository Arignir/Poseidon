/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-dependent API to manipulate virtual memory.
*/

#pragma once

#include "poseidon/memory/memory.h"
#include "poseidon/memory/vmm.h"

bool arch_vmm_is_mapped(virtaddr_const_t va);
bool arch_vmm_is_mapped_user(virtaddr_const_t va);
status_t arch_vmm_map_frame(virtaddr_t va, physaddr_t pa, mmap_flags_t flags);
void arch_vmm_unmap_frame(virtaddr_t va, munmap_flags_t flags);
