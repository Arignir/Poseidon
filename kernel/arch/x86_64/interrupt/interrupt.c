/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "arch/x86_64/interrupt.h"
#include "poseidon/interrupt.h"
#include "lib/log.h"

interrupt_handler_t g_irq_handlers[INT_NB];

/*
** Dispatch the execution to the appropriate interrupt handler.
**
** This function is the common entry point of all interrupt service routine.
*/
void
common_int_handler(
    struct iframe *iframe
) {
    switch (iframe->int_vector) {
        case INT_BREAKPOINT:
            exception_breakpoint(iframe);
            break;
        case INT_DIVISION_BY_ZERO ... INT_NMI:
        case INT_OVERFLOW ... INT_MAX_RESERVED_BY_INTEL:
            // Panic on unhandled exceptions
            panic("Unhandled exception %#x", iframe->int_vector);
            break;
        case INT_IRQ0 ... INT_NB:
            if (g_irq_handlers[iframe->int_vector]) {
                g_irq_handlers[iframe->int_vector](iframe);
            } else {
                logln(
                    "Unhandled IRQ %#x",
                    iframe->int_vector
                );
            }
            break;
    }
}
