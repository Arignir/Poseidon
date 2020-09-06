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

uint    atomic_exchange(uint32 volatile *ptr, uint32 value);

#endif /* !_POSEIDON_INTERRUPT_H_ */
