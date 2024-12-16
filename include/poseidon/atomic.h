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

#pragma once

#include "poseidon/poseidon.h"

/*
** `memorder` can be any of those constants.
**
** See the GCC documentation for more informations:
**   - https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
**   - https://gcc.gnu.org/wiki/Atomic/GCCMM/AtomicSync
*/
#define ATOMIC_RELAXED                                  __ATOMIC_RELAXED
#define ATOMIC_CONSUME                                  __ATOMIC_CONSUME
#define ATOMIC_ACQUIRE                                  __ATOMIC_ACQUIRE
#define ATOMIC_RELEASE                                  __ATOMIC_RELEASE
#define ATOMIC_ACQ_REL                                  __ATOMIC_ACQ_REL
#define ATOMIC_SEQ_CST                                  __ATOMIC_SEQ_CST

/*
** Atomically read the content of `*ptr` and return it.
*/
#define atomic_load(ptr, memorder)                      __atomic_load_n(ptr, memorder)

/*
** Atomically set the content of `*ptr` to `val`.
*/
#define atomic_store(ptr, val, memorder)                __atomic_store_n(ptr, val, memorder)

/*
** Atomically set `*ptr` to `value` while returning the previous value of `*ptr`.
*/
#define atomic_exchange(ptr, value, memorder)           __atomic_exchange_n(ptr, value, memorder)

/*
** These builtins atomically fetch the previous value of `ptr` and set `*ptr` to the
** result of the operation suggested by their name.
*/
#define atomic_fetch_add(ptr, value, memorder)          __atomic_fetch_add(ptr, value, memorder)
#define atomic_fetch_sub(ptr, value, memorder)          __atomic_fetch_sub(ptr, value, memorder)
#define atomic_fetch_or(ptr, value, memorder)           __atomic_fetch_or(ptr, value, memorder)
#define atomic_fetch_and(ptr, value, memorder)          __atomic_fetch_and(ptr, value, memorder)
#define atomic_fetch_xor(ptr, value, memorder)          __atomic_fetch_xor(ptr, value, memorder)
#define atomic_fetch_nand(ptr, value, memorder)         __atomic_fetch_nand(ptr, value, memorder)

/*
** These builtins atomically fetch the new value of `*ptr` after it is set to
** the result of the operation suggested by their name.
*/
#define atomic_add_fetch(ptr, value, memorder)          __atomic_add_fetch(ptr, value, memorder)
#define atomic_sub_fetch(ptr, value, memorder)          __atomic_sub_fetch(ptr, value, memorder)
#define atomic_or_fetch(ptr, value, memorder)           __atomic_or_fetch(ptr, value, memorder)
#define atomic_and_fetch(ptr, value, memorder)          __atomic_and_fetch(ptr, value, memorder)
#define atomic_xor_fetch(ptr, value, memorder)          __atomic_xor_fetch(ptr, value, memorder)
#define atomic_nand_fetch(ptr, value, memorder)         __atomic_nand_fetch(ptr, value, memorder)
