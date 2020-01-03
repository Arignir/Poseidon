/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Implementation of `include/poseidon/memory/vmm.h` for the x86 architecture.
*/

#ifndef _ARCH_X86_API_VMM_H_
# define _ARCH_X86_API_VMM_H_

/*
** Ensure this file isn't included on its own.
** The file `include/poseidon/memory/vmm.h` should be included instead, which,
** in turn, includes this one.
*/
# ifndef _POSEIDON_MEMORY_VMM_H_
#  error Please include `poseidon/memory/vmm.h` instead of `arch/x86/api/vmm.h`.
# endif /* !_POSEIDON_MEMORY_VMM_H_ */

/*
** We don't have any arch-dependent type or static inline function to define.
** Therefore, this file is empty.
*/

#endif /* !_ARCH_X86_API_VMM_H_ */
