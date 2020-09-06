/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to achieve atomic operations.
*/

#ifndef _POSEIDON_ATOMIC_H_
# define _POSEIDON_ATOMIC_H_

# include <poseidon/poseidon.h>

/*
** Set `*ptr` to `value` while returning the previous value of `*ptr`.
*/
# define atomic_exchange(ptr, value)            __sync_lock_test_and_set(ptr, value)

/*
** Write `new` to `*ptr` if `*ptr == old`.
**
** Return true if `new` was written, false otherwise.
*/
# define atomic_compare_and_swap(ptr, old, new) __sync_bool_compare_and_swap(ptr, old, new)

/*
** These builtins atomically fetch the previous value and set `*ptr` to the
** result of the operation suggested by their name.
*/
# define atomic_fetch_and_add(ptr, value)       __sync_fetch_and_add(ptr, value)
# define atomic_fetch_and_sub(ptr, value)       __sync_fetch_and_sub(ptr, value)
# define atomic_fetch_and_or(ptr, value)        __sync_fetch_and_or(ptr, value)
# define atomic_fetch_and_and(ptr, value)       __sync_fetch_and_and(ptr, value)
# define atomic_fetch_and_xor(ptr, value)       __sync_fetch_and_xor(ptr, value)
# define atomic_fetch_and_nand(ptr, value)      __sync_fetch_and_nand(ptr, value)

/*
** These builtins atomically fetch the new value of `*ptr` after it is set to
** the result of the operation suggested by their name.
*/
# define atomic_add_and_fetch(ptr, value)       __sync_add_and_fetch(ptr, value)
# define atomic_sub_and_fetch(ptr, value)       __sync_sub_and_fetch(ptr, value)
# define atomic_or_and_fetch(ptr, value)        __sync_or_and_fetch(ptr, value)
# define atomic_and_and_fetch(ptr, value)       __sync_and_and_fetch(ptr, value)
# define atomic_xor_and_fetch(ptr, value)       __sync_xor_and_fetch(ptr, value)
# define atomic_nand_and_fetch(ptr, value)      __sync_nand_and_fetch(ptr, value)

#endif /* !_POSEIDON_INTERRUPT_H_ */
