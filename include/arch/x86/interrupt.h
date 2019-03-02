/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Interrupts handling for the x86 architecture.
*/

#ifndef _ARCH_X86_INTERRUPT_H_
# define _ARCH_X86_INTERRUPT_H_

# include <poseidon/poseidon.h>
# include <arch/x86/register.h>

/*
** Exception/Interruption and their vector.
*/
enum interrupt_vector
{
	INT_DIVISION_BY_ZERO		= 0,
	INT_DEBUG,
	INT_NMI,
	INT_BREAKPOINT,
	INT_OVERFLOW,
	INT_OUT_OF_BOUNDS,
	INT_INVALID_OPCODE,
	INT_DEVICE_NA,
	INT_DOUBLE_FAULT,
	INT_INVALID_TSS			= 0xA,
	INT_SEGMENT_NOT_PRESENT,
	INT_STACK_FAULT,
	INT_GP_FAULT,
	INT_PAGE_FAULT,
	INT_FPU_EXCEPTION		= 0x10,
	INT_ALIGNMENT_CHECK,
	INT_MACHINE_CHECK,
	INT_SIMD_FP_EXCEPTION,
	INT_VIRT_EXCEPTION,
	INT_SECURITY_EXCEPTION		= 0x1E,

	INT_MAX_RESERVED_BY_INTEL	= 0x1F,
	INT_IRQ0			= 0x20,

	INT_PIT				= 0x20,
	INT_KEYBOARD			= 0x21,

	INT_APIC_TIMER			= 0x32,
	INT_APIC_ERROR			= 0x33,

	INT_SYSCALL			= 0x80,

	INT_APIC_SPURIOUS		= 0xFF,

	INT_NB				= 0x100,
};

/*
** Stack frame when an interruption/exception occures.
**
** This structure represents the content of the stack frame after an interruption
** or an exception.
**
** Most of these elemnts are pushed by the interrupt handler, but some of them
** are also pushed by the processor automatically when the exception is handled.
**
** The content and meaning of each field is described in Intel's manual.
*/
struct iframe
{
	uint32 gs;
	uint32 fs;
	uint32 es;
	uint32 ss;
	uint32 ds;
	uintptr eax;
	uintptr ecx;
	uintptr edx;
	uintptr ebx;
	uintptr _esp;
	uintptr ebp;
	uintptr esi;
	uintptr edi;
	uint32 int_vector;
	uint32 error_code;
	uintptr eip;
	uint32 cs;
	struct eflags eflags;
} __packed;

static_assert(sizeof(struct iframe) == 18 * sizeof(uint32));

#endif /* !_ARCH_X86_INTERRUPT_H_ */

