/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "arch/x86_64/register.h"
#include "poseidon/interrupt.h"

extern interrupt_handler_t g_irq_handlers[];

/*
** Test if interruptions are enabled.
*/
static inline
bool
arch_interrupts_enabled(
    void
) {
    struct rflags rflags;

    rflags = get_rflags();
    return rflags.intf;
}

/*
** Enable interruptions.
*/
static inline
void
arch_enable_interrupts(
    void
) {
    asm volatile (
        "sti"
        :
        :
        : "cc"
    );
}

/*
** Disable interruptions.
*/
static inline
void
arch_disable_interrupts(
    void
) {
    asm volatile (
        "cli"
        :
        :
        : "cc"
    );
}

/*
** Set wether interruptions are enabled or not.
*/
static inline
void
arch_halt(
    void
) {
    asm volatile(
        "hlt"
        :
        :
        :
    );
}

/*
** Register a handler to be called when the specified interrupt vector is received.
*/
static inline
void
arch_register_interrupt_handler(
    uint vector,
    interrupt_handler_t handler
) {
    debug_assert(vector >= INT_IRQ0 && vector < INT_NB);

    g_irq_handlers[vector] = handler;
}

/*
** Unregister the handler of the given interrupt vector.
*/
static inline
void
arch_unregister_interrupt_handler(
    uint vector
) {
    debug_assert(vector >= INT_IRQ0 && vector < INT_NB);

    g_irq_handlers[vector] = NULL;
}
