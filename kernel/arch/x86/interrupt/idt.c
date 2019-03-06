/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Functions to manipulations the Interrupt Descriptor Table (IDT).
*/

#include <arch/x86/idt.h>
#include <arch/x86/interrupt.h>
#include <arch/x86/selector.h>
#include <lib/log.h>

__aligned(16)
static
struct idt_descriptor idt[INT_NB];

/*
** The fat pointer describing the IDT.
** It contains its address and its size (minus 1).
**
** This fat pointer is given in argument to the LIDT instruction when loading
** the IDT.
*/
__aligned(16)
struct idt_fatptr const idt_fatptr = {
	.limit = sizeof(idt) - 1,
	.base = idt,
};

/*
** Setup a default Interrupt Descriptor Table.
**
** The table is initialized with a handler for all exceptions. Interrupts'
** handler can be added using the kernel's API.
*/
__boot_text
void
setup_idt(void)
{
	extern uchar isr_bootstraps[];
	uint i;


	for (i = 0; i < INT_NB; ++i) {
		idt[i] = NEW_IDT_INTERRUPT_GATE_ENTRY(
			isr_bootstraps + i * 16, // Each isr bootstrap is 16 bytes long
			.present = true,
			.dpl = 0,
			.segment_selector = KERNEL_CODE_SELECTOR,
		);
	}
}
