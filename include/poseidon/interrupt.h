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

# include <arch/target/api/interrupt.h>

/*
** Test if interrupts are enabled.
**
** Return true if they are, false otherwise.
**
** Architectures must implement this function.
*/
static inline bool      interrupts_enabled(void) __arch_alias(interrupts_enabled);

/*
** Enable all maskable interrupts.
**
** Architectures must implement this function.
*/
static inline void      enable_interrupts(void) __arch_alias(enable_interrupts);

/*
** Disable all maskable interrupts.
**
** Architectures must implement this function.
*/
static inline void      disable_interrupts(void) __arch_alias(disable_interrupts);

/*
** Enable or disable all maskable interrupts.
**
** If true is given as parameter, the maskable interrupts are enabled.
** Otherwise, the interrupts are disabled.
**
** Architectures must implement this function.
*/
static inline void      set_interrupts_state(bool state) __arch_alias(set_interrupts_state);

/*
** Register an interrupt handler for a specific interrupt vector.
**
** Architectures must implement this function.
*/
static inline void      register_interrupt_handler(uint vector, interrupt_handler_t handler) __arch_alias(register_interrupt_handler);

/*
** Unregister an interrupt handler for a specific interrupt vector.
**
** Architectures must implement this function.
*/
static inline void      unregister_interrupt_handler(uint vector) __arch_alias(unregister_interrupt_handler);

/*
** Halt the computer until the next interrupt.
**
** Architectures must implement this function.
*/
static inline void      halt(void) __arch_alias(halt);

#endif /* !_POSEIDON_INTERRUPT_H_ */
