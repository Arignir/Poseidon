/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Implementation of `include/poseidon/interrupt.h` for the x86_64 architecture.
*/

#ifndef _ARCH_X86_64_API_INTERRUPT_H_
# define _ARCH_X86_64_API_INTERRUPT_H_

/*
** Ensure this file isn't included on its own.
** The file `include/poseidon/interrupt.h` should be included instead, which, in
** turn, includes this one.
*/
# ifndef _POSEIDON_INTERRUPT_H_
#  error Please include `poseidon/interrupt.h` instead of `arch/x86_64/api/interrupt.h`.
# endif /* !_POSEIDON_INTERRUPT_H_ */

# include <poseidon/poseidon.h>
# include <arch/x86_64/register.h>
# include <arch/x86_64/interrupt.h>

static inline
bool
x86_64_interrupts_enabled(void)
{
    struct rflags rflags = get_rflags();
    return rflags.intf;
}

static inline
void
x86_64_enable_interrupts(void) {
    asm volatile (
        "sti;"
        :
        :
        : "cc"
    );
}

static inline
void
x86_64_disable_interrupts(void) {
    asm volatile (
        "cli;"
        :
        :
        : "cc"
    );
}

static inline
void
x86_64_set_interrupts_state(
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
x86_64_halt(void)
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
x86_64_register_interrupt_handler(
    uint vector,
    interrupt_handler_t handler
) {
    extern interrupt_handler_t irq_handlers[];

    assert(vector >= INT_IRQ0 && vector < INT_NB);

    irq_handlers[vector] = handler;
}

static inline
void
x86_64_unregister_interrupt_handler(
    uint vector
) {
    extern interrupt_handler_t irq_handlers[];

    assert(vector >= INT_IRQ0 && vector < INT_NB);

    irq_handlers[vector] = NULL;
}

#endif /* !_ARCH_X86_64_API_INTERRUPT_H_ */
