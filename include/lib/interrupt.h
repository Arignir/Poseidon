/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Architecture-independent API to manipulate maskable interrupts.
*/

#ifndef _LIB_INTERRUPT_H_
# define _LIB_INTERRUPT_H_

# include <poseidon/poseidon.h>
# include <poseidon/kconfig.h>

/*
** An interrupt handler.
**
** Interrupt handlers are called when the associated interrupt is received and
** handled by the kernel. This can be at any place and time in the kernel,
** therefore such handlers need to respect some very specific conditions and
** limitation.
*/
typedef void (*interrupt_handler_t)(void);

#include <arch/current/api/interrupt.h>

/*
** Test if interrupts are enabled.
**
** Return true if they are, false otherwise.
**
** The architecture-dependant equivalent of this function should have the
** following prototype:
**
** `bool $ARCH_interrupts_enabled(void);`
*/
static inline
bool
interrupts_enabled(void)
{
	return ARCH_SYMBOL(interrupts_enabled)();
}

/*
** Enable all maskable interrupts.
**
** The architecture-dependant equivalent of this function should have the
** following prototype:
**
** `void $ARCH_enable_interrupts(void);`
*/
static inline
void
enable_interrupts(void)
{
	ARCH_SYMBOL(enable_interrupts)();
}

/*
** Disable all maskable interrupts.
**
** The architecture-dependant equivalent of this function should have the
** following prototype:
**
** `void $ARCH_disable_interrupts(void);`
*/
static inline
void
disable_interrupts(void)
{
	ARCH_SYMBOL(disable_interrupts)();
}

/*
** Enable or disable all maskable interrupts.
**
** If true is given as parameter, the maskable interrupts are enabled.
** Otherwise, the interrupts are disabled.
**
** The architecture-dependant equivalent of this function should have the
** following prototype:
**
** `void $ARCH_set_interrupts_state(bool state);`
*/
static inline
void
set_interrupts_state(
	bool state
) {
	ARCH_SYMBOL(set_interrupts_state)(state);
}

/*
** Register an interrupt handler for a specific interrupt vector.
**
** The architecture-dependant equivalent of this function should have the
** following prototype:
**
** `void $ARCH_register_interrupt_handler(uint vector, interrupt_handler_t handler);`
*/
static inline
void
register_interrupt_handler(
	uint vector,
	interrupt_handler_t handler
) {
	ARCH_SYMBOL(register_interrupt_handler)(vector, handler);
}

/*
** Unregister an interrupt handler for a specific interrupt vector.
**
** The architecture-dependant equivalent of this function should have the
** following prototype:
**
** `void $ARCH_unregister_interrupt_handler(uint vector);`
*/
static inline
void
unregister_interrupt_handler(
	uint vector
) {
	ARCH_SYMBOL(unregister_interrupt_handler)(vector);
}

/*
** Halt the computer until the next interrupt.
**
** The architecture-dependant equivalent of this function should have the
** following prototype:
**
** `void $ARCH_halt(void);`
*/
static inline
void
halt(void)
{
	ARCH_SYMBOL(halt)();
}

#endif /* !_LIB_INTERRUPT_H_ */
