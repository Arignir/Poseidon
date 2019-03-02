/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Implementation of `include/lib/interrupt.h` for the x86 architecture.
*/

#ifndef _ARCH_X86_API_INTERRUPT_H_
# define _ARCH_X86_API_INTERRUPT_H_

/*
** Ensure this file isn't included on its own.
** The file `include/lib/interrupt.h` should be included instead, which, in turn,
** includes this one.
*/
# ifndef _LIB_INTERRUPT_H_
#  error Please include `lib/interrupt.h` instead of `arch/x86/api/interrupt.h`.
# endif /* !_LIB_INTERRUPT_H_ */

# include <arch/x86/register.h>
# include <arch/x86/interrupt.h>

static inline
bool
x86_interrupts_enabled(void)
{
	struct eflags eflags = get_eflags();
	return eflags.intf;
}

static inline
void
x86_enable_interrupts(void) {
	asm volatile (
		"sti;"
		:
		:
		: "cc"
	);
}

static inline
void
x86_disable_interrupts(void) {
	asm volatile (
		"cli;"
		:
		:
		: "cc"
	);
}

static inline
void
x86_set_interrupts_state(
	bool state
) {
	if (state) {
		sti();
	} else {
		cli();
	}
}

static inline
void
x86_halt(void)
{
	asm volatile(
		"hlt;"
		:
		:
		:
	);
}

static inline
void
x86_register_interrupt_handler(
	uint vector,
	interrupt_handler_t handler
) {
	extern interrupt_handler_t irq_handlers[];

	assert(vector >= INT_IRQ0 && vector < INT_NB);

	irq_handlers[vector] = handler;
}

static inline
void
x86_unregister_interrupt_handler(
	uint vector
) {
	extern interrupt_handler_t irq_handlers[];

	assert(vector >= INT_IRQ0 && vector < INT_NB);

	irq_handlers[vector] = NULL;
}

#endif /* !_ARCH_X86_API_INTERRUPT_H_ */
