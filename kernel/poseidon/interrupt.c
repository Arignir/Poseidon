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
**
** The following functions are default implementations, usually overwritten by
** the architecture.
*/

#include "poseidon/poseidon.h"
#include "poseidon/interrupt.h"

/*
** Test if interruptions are enabled.
*/
[[gnu::weak]]
bool
interrupts_enabled(
    void
) {
    unimplemented();
}

/*
** Enable interruptions.
*/
[[gnu::weak]]
void
enable_interrupts(
    void
) {
    unimplemented();
}

/*
** Disable interruptions.
*/
[[gnu::weak]]
void
disable_interrupts(
    void
) {
    unimplemented();
}

/*
** Set wether interruptions are enabled or not.
*/
[[gnu::weak]]
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
[[gnu::weak]]
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
[[gnu::weak]]
void
pop_interrupts_state(
    bool const *state
) {
    set_interrupts_state(*state);
}

/*
** Wait until an interruption is received.
*/
[[gnu::weak]]
void
halt(
    void
) {
    unimplemented();
}

/*
** Register an interrupt handler for the given vector.
*/
[[gnu::weak]]
void
register_interrupt_handler(
    uint vector [[maybe_unused]],
    interrupt_handler_t handler [[maybe_unused]]
) {
    unimplemented();
}

/*
** Unregister an interrupt handler for the given vector.
*/
[[gnu::weak]]
void
unregister_interrupt_handler(
    uint vector [[maybe_unused]]
) {
    unimplemented();
}
