/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Types and functions to read and manipulate x86 registers.
*/

#ifndef _ARCH_X86_REGISTER_H_
# define _ARCH_X86_REGISTER_H_

# include <poseidon/poseidon.h>

/*
** System flags and fields in the EFLAGS register.
**
** The layout of this structure is defined by Intel.
*/
struct eflags
{
	union {
		struct {
			uint8 cf: 1;
			uint8 _reserved0: 1;
			uint8 pf: 1;
			uint8 _reserved1: 1;
			uint8 af: 1;
			uint8 _reserved2: 1;
			uint8 zf: 1;
			uint8 sf: 1;
			uint8 tf: 1;
			uint8 intf: 1;
			uint8 df: 1;
			uint8 of: 1;
			uint8 iopl: 2;
			uint8 nt: 1;
			uint8 _reserved3: 1;
			uint8 rf: 1;
			uint8 vm: 1;
			uint8 ac: 1;
			uint8 vif: 1;
			uint8 vip: 1;
			uint8 id: 1;
			uint16 _reserved4: 10;
		} __packed;
		uint32 value;
	} __packed;
} __packed;

static_assert(sizeof(struct eflags) == 4);

static inline
struct eflags
get_eflags(void)
{
	struct eflags eflags;

	asm volatile (
		"pushfl;"
		"popl %0;"
		: "=rm"(eflags.value)
		:
		:
	);
	return (eflags);
}

static inline
void
set_eflags(
	struct eflags eflags
) {
	asm volatile (
		"pushl %0;"
		"popfl;"
		:
		: "g"(eflags.value)
		: "cc"
	);
}

#endif /* !_ARCH_X86_REGISTER_H_ */
