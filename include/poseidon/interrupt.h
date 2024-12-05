/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to manipulate maskable interrupts.
*/

#pragma once

#include "poseidon/poseidon.h"

/*
** An interrupt handler.
**
** Interrupt handlers are called when the associated interrupt is received and
** handled by the kernel. This can be at any place and time in the kernel,
** therefore such handlers need to respect some very specific conditions and
** limitation.
*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
typedef void (*interrupt_handler_t)(...);
#pragma GCC diagnostic pop

/*
** Implement safe wrappers around the arch-dependent API.
*/

#include "arch/target/api/interrupt.h"

/*
** Test if interruptions are enabled.
*/
static inline
bool
interrupts_enabled(
   void
) {
   return arch_interrupts_enabled();
}

/*
** Enable interruptions.
*/
static inline
void
enable_interrupts(
   void
) {
   return arch_enable_interrupts();
}

/*
** Disable interruptions.
*/
static inline
void
disable_interrupts(
   void
) {
   return arch_disable_interrupts();
}

/*
** Set wether interruptions are enabled or not.
*/
static inline
void
set_interrupts_state(
    bool state
) {
    if (state) {
        enable_interrupts();
    } else {
        disable_interrupts();
    }
}

/*
** Quite similar to `interrupts_enabled()`, this function
** stores the current interrupt flag in `*save`.
*/
static inline
void
push_interrupts_state(
    bool *state
) {
    *state = interrupts_enabled();
}

/*
** Quite similar to `set_interrupts_state()`, this function
** sets the interrupt flag to the content of `*save`.
*/
static inline
void
pop_interrupts_state(
    bool const *state
) {
    set_interrupts_state(*state);
}

/*
** Set wether interruptions are enabled or not.
*/
static inline
void
halt(
    void
) {
   arch_halt();
}

/*
** Register a handler to be called when the specified interrupt vector is received.
*/
static inline
void
register_interrupt_handler(
   uint vector,
   interrupt_handler_t handler
) {
   arch_register_interrupt_handler(vector, handler);
}

/*
** Unregister the handler of the given interrupt vector.
*/
static inline
void
unregister_interrupt_handler(
   uint vector
) {
   arch_unregister_interrupt_handler(vector);
}
