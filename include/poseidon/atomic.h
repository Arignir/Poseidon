/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to achieve atomic operations.
*/

#ifndef _POSEIDON_ATOMIC_H_
# define _POSEIDON_ATOMIC_H_

# include <poseidon/poseidon.h>

/*
** `memorder` can be any of those constants.
**
** See the GCC documentation for more informations:
**   - https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
*/
# define ATOMIC_RELAXED                                 __ATOMIC_RELAXED
# define ATOMIC_CONSUME                                 __ATOMIC_CONSUME
# define ATOMIC_ACQUIRE                                 __ATOMIC_ACQUIRE
# define ATOMIC_RELEASE                                 __ATOMIC_RELEASE
# define ATOMIC_ACQ_REL                                 __ATOMIC_ACQ_REL
# define ATOMIC_SEQ_CST                                 __ATOMIC_SEQ_CST

/*
** Set `*ptr` to `value` while returning the previous value of `*ptr`.
*/
# define atomic_exchange(ptr, value, memorder)          __atomic_exchange_n(ptr, value, memorder)

/*
** These builtins atomically fetch the previous value and set `*ptr` to the
** result of the operation suggested by their name.
*/
# define atomic_fetch_add(ptr, value, memorder)         __atomic_fetch_add(ptr, value)
# define atomic_fetch_sub(ptr, value, memorder)         __atomic_fetch_sub(ptr, value)
# define atomic_fetch_or(ptr, value, memorder)          __atomic_fetch_or(ptr, value)
# define atomic_fetch_and(ptr, value, memorder)         __atomic_fetch_and(ptr, value)
# define atomic_fetch_xor(ptr, value, memorder)         __atomic_fetch_xor(ptr, value)
# define atomic_fetch_nand(ptr, value, memorder)        __atomic_fetch_nand(ptr, value)

/*
** These builtins atomically fetch the new value of `*ptr` after it is set to
** the result of the operation suggested by their name.
*/
# define atomic_add_fetch(ptr, value)                   __atomic_add_fetch(ptr, value)
# define atomic_sub_fetch(ptr, value)                   __atomic_sub_fetch(ptr, value)
# define atomic_or_fetch(ptr, value)                    __atomic_or_fetch(ptr, value)
# define atomic_and_fetch(ptr, value)                   __atomic_and_fetch(ptr, value)
# define atomic_xor_fetch(ptr, value)                   __atomic_xor_fetch(ptr, value)
# define atomic_nand_fetch(ptr, value)                  __atomic_nand_fetch(ptr, value)

#endif /* !_POSEIDON_INTERRUPT_H_ */
