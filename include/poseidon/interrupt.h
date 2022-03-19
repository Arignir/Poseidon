/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to manipulate maskable interrupts.
*/

#ifndef _POSEIDON_INTERRUPT_H_
# define _POSEIDON_INTERRUPT_H_

# include <poseidon/poseidon.h>

/*
** An interrupt handler.
**
** Interrupt handlers are called when the associated interrupt is received and
** handled by the kernel. This can be at any place and time in the kernel,
** therefore such handlers need to respect some very specific conditions and
** limitation.
*/
typedef void (*interrupt_handler_t)();

bool interrupts_enabled(void);
void enable_interrupts(void);
void disable_interrupts(void);
void set_interrupts_state(bool state);
void push_interrupts_state(bool *save);
void pop_interrupts_state(bool const *save);
void halt(void);
void register_interrupt_handler(uint vector, interrupt_handler_t handler);
void unregister_interrupt_handler(uint);

#endif /* !_POSEIDON_INTERRUPT_H_ */
