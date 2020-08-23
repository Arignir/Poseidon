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
**
** The following functions are default implementations, usually overwritten by
** the architecture.
*/

#include <poseidon/poseidon.h>
#include <poseidon/interrupt.h>

/*
** Test if interruptions are enabled.
*/
__weak
bool
interrupts_enabled(void)
{
    unimplemented();
}

/*
** Enable interruptions.
*/
__weak
void
enable_interrupts(void)
{
    unimplemented();
}

/*
** Disable interruptions.
*/
__weak
void
disable_interrupts(void)
{
    unimplemented();
}

/*
** Set wether interruptions are enabled or not.
*/
__weak
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
** Wait until an interruption is received.
*/
__weak
void
halt(void)
{
    unimplemented();
}

/*
** Register an interrupt handler for the given vector.
*/
__weak
void
register_interrupt_handler(
    uint vector,
    interrupt_handler_t handler
) {
    unimplemented();
}

/*
** Unregister an interrupt handler for the given vector.
*/
__weak
void
unregister_interrupt_handler(
    uint vector
) {
    unimplemented();
}
