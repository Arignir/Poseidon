/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#ifndef _ARCH_X86_API_MEMORY_H_
# define _ARCH_X86_API_MEMORY_H_

# include <poseidon/poseidon.h>
# include <poseidon/kconfig.h>

/*
** Ensure this file isn't included on its own.
** The file `include/poseidon/memory.h` should be included instead, which, in
** turn, includes this one.
*/
# ifndef _POSEIDON_MEMORY_H_
#  error Please include `poseidon/memory.h` instead of `arch/x86/api/memory.h`.
# endif /* !_POSEIDON_MEMORY_H_ */

/*
** Mask to test if an address is page-aligned.
*/
# define PAGE_SIZE_MASK     (PAGE_SIZE - 1)

/*
** Align the given number to PAGE_SIZE
*/
# define PAGE_ALIGN(x)      (ALIGN((x), PAGE_SIZE))

#endif /* !_ARCH_X86_API_MEMORY_H_ */
