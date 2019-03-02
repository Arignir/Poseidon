/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#include <arch/x86/interrupt.h>
#include <lib/interrupt.h>
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
	switch (iframe->int_vector){
		case INT_IRQ0...INT_NB:
			if (irq_handlers[iframe->int_vector]) {
				irq_handlers[iframe->int_vector]();
			} else {
				panic(
					"Unhandled interrupt request %#x",
					iframe->int_vector
				);
			}
			break;
		default:
			panic("Unhandled interrupt %#x", iframe->int_vector);
			break;
	}
}
