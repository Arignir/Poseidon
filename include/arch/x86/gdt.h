/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Types to manipulate an entry within the Great Descriptor Table (GDT).
**
** See Intel's manual for more information about those structures and their
** content.
*/

#ifndef _ARCH_X86_GDT_H_
# define _ARCH_X86_GDT_H_

# include <poseidon/poseidon.h>

/*
** A descriptor of a data segment.
**
** The layout of this structure is defined by Intel.
*/
struct gdt_data_segment_descriptor
{
	uint16 limit_low: 16;
	uint16 base_low: 16;
	uint8 base_mid: 8;
	union {
		struct {
			uint8 accessed: 1;
			uint8 writable: 1;
			uint8 direction: 1;
			uint8 _reserved0: 1;
			uint8 _reserved1: 1;
			uint8 dpl: 2;
			uint8 present: 1;
			uint8 limit_high: 4;
			uint8 avl: 1;
			uint8 _reserved2: 1;
			uint8 big: 1;
			uint8 granularity: 1;
		};
		uint16 flags: 16;
	};
	uint8 base_high: 8;
} __packed;

static_assert(sizeof(struct gdt_data_segment_descriptor) == 8);

/*
** A descriptor of a code segment.
** The layout of this structure is defined by Intel.
*/
struct gdt_code_segment_descriptor
{
	uint16 limit_low: 16;
	uint16 base_low: 16;
	uint8 base_mid: 8;
	union {
		struct {
			uint8 accessed: 1;
			uint8 readable: 1;
			uint8 conforming: 1;
			uint8 _reserved0: 1;
			uint8 _reserved1: 1;
			uint8 dpl: 2;
			uint8 present: 1;
			uint8 limit_high: 4;
			uint8 avl: 1;
			uint8 _reserved2: 1;
			uint8 default_op_size: 1;
			uint8 granularity: 1;
		};
		uint16 flags: 16;
	};
	uint8 base_high: 8;
} __packed;

static_assert(sizeof(struct gdt_code_segment_descriptor) == 8);

/*
** A descriptor of a system segment.
**
** The layout of this structure is defined by Intel.
*/
struct gdt_system_segment_descriptor
{
	uint16 limit_low: 16;
	uint16 base_low: 16;
	uint8 base_mid: 8;
	union {
		struct {
			uint8 type: 4;
			uint8 _reserved0: 1;
			uint8 dpl: 2;
			uint8 present: 1;
			uint8 limit_high: 4;
			uint8 _reserved1: 3;
			uint8 granularity: 1;
		};
		uint16 flags: 16;
	};
	uint8 base_high: 8;
} __packed;

static_assert(sizeof(struct gdt_system_segment_descriptor) == 8);

/*
** The different valid values for the `type` field of
** `struct gdt_system_segment_descriptor`.
*/
enum gdt_system_segment_descriptor_type
{
	TSS_AVAILABLE_16	= 1,
	LDT			= 2,
	TSS_BUSY_16		= 3,
	CALL_GATE_16		= 4,
	TASK_GATE		= 5,
	INTERRUPT_GATE_16	= 6,
	TRAP_GATE_16		= 7,
	TSS_AVAILABLE_32	= 9,
	TSS_BUSY_32		= 11,
	CALL_GATE_23		= 12,
	INTERRUPT_GATE_32	= 14,
	TRAP_GATE_32		= 15,
};

/*
** A segment descriptor of the GDT.
** Encapsulates the previous structures into a uniform one.
*/
struct gdt_segment_descriptor
{
	union {
		struct gdt_code_segment_descriptor code;
		struct gdt_data_segment_descriptor data;
		struct gdt_system_segment_descriptor system;
		uint64 value: 64;
	};
};

static_assert(sizeof(struct gdt_segment_descriptor) == 8);

/*
** A fat pointer given in parameter to LGDT.
**
** The layout of this structure is defined by Intel.
*/
struct gdt_fatptr {
	uint16 limit;
	struct gdt_segment_descriptor const *base;
} __packed;

static_assert(sizeof(struct gdt_fatptr) == 6);

/*
** Create a null descriptor, with all bits set to 0.
*/
# define NEW_NULL_DESCRIPTOR						\
	(struct gdt_segment_descriptor) {				\
		.value = 0						\
	}

/*
** Create a code segment descriptor, using the given base and limit.
** The value of the other fields can also be appended.
*/
# define NEW_GDT_CODE_ENTRY(base, limit, ...)				\
	(struct gdt_segment_descriptor) {				\
		.code = {						\
			.limit_low = ((limit) & 0xFFFF),		\
			.base_low = ((base) & 0xFFFF),			\
			.base_mid = (((base) >> 16) & 0xFF),		\
			.limit_high = (((limit) >> 16) & 0xF),		\
			.base_high = (((base) >> 24 & 0xFF)), 		\
			._reserved0 = 1,				\
			._reserved1 = 1,				\
			._reserved2 = 0,				\
			__VA_ARGS__					\
		},							\
	}

/*
** Create a data segment descriptor, using the given base and limit.
** The value of the other fields can also be appended.
*/
# define NEW_GDT_DATA_ENTRY(base, limit, ...)				\
	(struct gdt_segment_descriptor) {				\
		.data = {						\
			.limit_low = ((limit) & 0xFFFF),		\
			.base_low = ((base) & 0xFFFF),			\
			.base_mid = (((base) >> 16) & 0xFF),		\
			.limit_high = (((limit) >> 16) & 0xF),		\
			.base_high = (((base) >> 24 & 0xFF)), 		\
			._reserved0 = 0,				\
			._reserved1 = 1,				\
			._reserved2 = 0,				\
			__VA_ARGS__					\
		},							\
	}

/*
** Create a system segment descriptor, using the given base and limit.
** The value of the other fields can also be appended.
*/
# define NEW_GDT_SYSTEM_ENTRY(base, limit, ...)				\
	(struct gdt_segment_descriptor) {				\
		.system = {						\
			.limit_low = ((limit) & 0xFFFF),		\
			.base_low = ((base) & 0xFFFF),			\
			.base_mid = (((base) >> 16) & 0xFF),		\
			.limit_high = (((limit) >> 16) & 0xF),		\
			.base_high = (((base) >> 24 & 0xFF)), 		\
			._reserved0 = 0,				\
			._reserved1 = 0,				\
			__VA_ARGS__					\
		},							\
	}

#endif /* !_ARCH_X86_GDT_H_ */
