/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Implementation of `include/poseidon/memory.h` for the x86_64 architecture.
*/

#ifndef _ARCH_X86_64_API_MEMORY_H_
# define _ARCH_X86_64_API_MEMORY_H_

/*
** Ensure this file isn't included on its own.
** The file `include/poseidon/memory.h` should be included instead, which, in
** turn, includes this one.
*/
# ifndef _POSEIDON_MEMORY_H_
#  error Please include `poseidon/memory.h` instead of `arch/x86_64/api/memory.h`.
# endif /* !_POSEIDON_MEMORY_H_ */

# include <poseidon/poseidon.h>

#endif /* !_ARCH_X86_64_API_MEMORY_H_ */


