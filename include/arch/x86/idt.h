/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Types to manipulate an entry within the Interrupt Descriptor Table (IDT).
**
** See Intel's manuals for more information about those structures and their
** content.
*/

#ifndef _ARCH_X86_IDT_H_
# define _ARCH_X86_IDT_H_

# include <poseidon/poseidon.h>

/*
** The descriptor of an interrupt gate.
**
** The layout of this structure is defined by Intel.
*/
struct idt_interrupt_gate_descriptor
{
	uint16 offset_low: 16;
	uint16 segment_selector: 16;
	uint8 _reserved0: 8;
	uint8 type: 5;
	uint8 dpl: 2;
	uint8 present: 1;
	uint16 offset_high: 16;
} __packed;

static_assert(sizeof(struct idt_interrupt_gate_descriptor) == 8);

/*
** The descriptor of a trap gate.
**
** The layout of this structure is defined by Intel.
*/
struct idt_trap_gate_descriptor
{
	uint16 offset_low: 16;
	uint16 segment_selector: 16;
	uint8 _reserved0: 8;
	uint8 type: 5;
	uint8 dpl: 2;
	uint8 present: 1;
	uint16 offset_high: 16;
} __packed;

static_assert(sizeof(struct idt_trap_gate_descriptor) == 8);

/*
** The descriptor of a task gate.
**
** The layout of this structure is defined by Intel.
*/
struct idt_task_gate_descriptor
{
	uint16 _reserved0: 16;
	uint16 segment_selector: 16;
	uint8 _reserved1: 8;
	uint8 type: 5;
	uint8 dpl: 2;
	uint8 present: 1;
	uint16 _reserved2: 16;
} __packed;

static_assert(sizeof(struct idt_task_gate_descriptor) == 8);

/*
** A gate descriptor which is used as an entry in the IDT.
** Encapsulates the previous structures into a uniform one.
*/
struct idt_descriptor
{
	union {
		struct idt_interrupt_gate_descriptor interrupt;
		struct idt_trap_gate_descriptor trap;
		struct idt_task_gate_descriptor task;
		uint64 value;
	};
};

static_assert(sizeof(struct idt_descriptor) == 8);

/*
** A fat pointer given in parameter to LIDT.
**
** The layout of this structure is defined by Intel.
*/
struct idt_fatptr {
	uint16 limit;
	struct idt_descriptor *base;
} __packed;

static_assert(sizeof(struct idt_fatptr) == 6);

/*
** Create an interrupt gate entry, using the given address as the entry point.
** The value of other fields can also be appended.
*/
# define NEW_IDT_INTERRUPT_GATE_ENTRY(offset, ...)				\
	(struct idt_descriptor) {						\
		.interrupt = {							\
			.offset_low = (((uintptr)(offset)) & 0xFFFF),		\
			.offset_high = ((((uintptr)(offset)) >> 16) & 0xFFFF),	\
			.type = 0b01110,					\
			._reserved0 = 0,					\
			__VA_ARGS__						\
		},								\
	}

/*
** Create a trap gate entry, using the given address as the entry point.
** The value of other fields can also be appended.
*/
# define NEW_IDT_TRAP_GATE_ENTRY(offset, ...)					\
	(struct idt_descriptor) {						\
		.trap = {							\
			.offset_low = (((uintptr)(offset)) & 0xFFFF),		\
			.offset_high = ((((uintptr)(offset)) >> 16) & 0xFFFF),	\
			.type = 0b01111,					\
			._reserved0 = 0,					\
			__VA_ARGS__						\
		},								\
	}

/*
** Create a task gate entry.
** The value of other fields can also be appended.
*/
# define NEW_IDT_TASK_GATE_ENTRY(...)						\
	(struct idt_descriptor) {						\
		.task = {							\
			.type = 0b00101,					\
			._reserved0 = 0,					\
			._reserved1 = 0,					\
			._reserved2 = 0,					\
			__VA_ARGS__						\
		},								\
	}

void		setup_idt(void);

#endif /* !_ARCH_X86_IDT_H_ */
