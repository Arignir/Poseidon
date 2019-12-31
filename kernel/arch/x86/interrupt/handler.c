/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#include <arch/x86/interrupt.h>
#include <driver/pic/pic8259.h>
#include <poseidon/interrupt.h>
#include <lib/log.h>

interrupt_handler_t irq_handlers[INT_NB - INT_IRQ0];

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
        case 0 ... INT_MAX_RESERVED_BY_INTEL:
            // Panic on unhandled exceptions
            panic("Unhandled exception %#x", iframe->int_vector);
            break;
        case INT_IRQ0 ... INT_NB:
            if (irq_handlers[iframe->int_vector]) {
                irq_handlers[iframe->int_vector]();
            } else {
                logln(
                    "Unhandled IRQ %#x",
                    iframe->int_vector
                );
            }

            // Reset master/slave PIC if it's in the IRQ range
            if (iframe->int_vector >= INT_IRQ0 && iframe->int_vector <= INT_IRQ15) {
                if (iframe->int_vector >= INT_IRQ8) {
                    pic8259_slave_eoi();
                }
                pic8259_master_eoi();
            }
            break;
    }
}
