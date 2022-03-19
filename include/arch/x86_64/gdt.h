/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Types to manipulate an entry within the Great Descriptor Table (GDT).
**
** See Intel's manual for more information about those structures and their
** content.
*/

#ifndef _ARCH_X86_64_GDT_H_
# define _ARCH_X86_64_GDT_H_

# include <poseidon/poseidon.h>

/*
** A descriptor of a code segment.
** The layout of this structure is defined by Intel.
*/
struct gdt_code_segment_descriptor {
    size_t limit_low: 16;
    size_t base_low: 16;
    size_t base_mid: 8;
    union {
        struct {
            size_t accessed: 1;
            size_t readable: 1;
            size_t conforming: 1;
            size_t _reserved0: 1;
            size_t _reserved1: 1;
            size_t dpl: 2;
            size_t present: 1;
            size_t limit_high: 4;
            size_t avl: 1;
            size_t long_mode: 1;
            size_t default_op_size: 1;
            size_t granularity: 1;
        } __packed;
        size_t flags: 16;
    } __packed;
    size_t base_high: 8;
} __packed;

static_assert(sizeof(struct gdt_code_segment_descriptor) == 8);

/*
** A descriptor of a data segment.
**
** The layout of this structure is defined by Intel.
*/
struct gdt_data_segment_descriptor {
    size_t limit_low: 16;
    size_t base_low: 16;
    size_t base_mid: 8;
    union {
        struct {
            size_t accessed: 1;
            size_t writable: 1;
            size_t direction: 1;
            size_t _reserved0: 1;
            size_t _reserved1: 1;
            size_t dpl: 2;
            size_t present: 1;
            size_t limit_high: 4;
            size_t avl: 1;
            size_t long_mode: 1;
            size_t big: 1;
            size_t granularity: 1;
        } __packed;
        size_t flags: 16;
    } __packed;
    size_t base_high: 8;
} __packed;

static_assert(sizeof(struct gdt_data_segment_descriptor) == 8);

/*
** A descriptor of a system segment.
**
** The layout of this structure is defined by Intel.
*/
struct gdt_system_segment_descriptor {
    size_t limit_low: 16;
    size_t base_low: 16;
    size_t base_mid: 8;
    union {
        struct {
            size_t type: 4;
            size_t _reserved0: 1;
            size_t dpl: 2;
            size_t present: 1;
            size_t limit_high: 4;
            size_t _reserved1: 3;
            size_t granularity: 1;
        } __packed;
        size_t flags: 16;
    } __packed;
    size_t base_high: 8;
} __packed;

static_assert(sizeof(struct gdt_system_segment_descriptor) == 8);

/*
** The different valid values for the `type` field of
** `struct gdt_system_segment_descriptor`.
*/
enum gdt_system_segment_descriptor_type {
    GDT_TSS_AVAILABLE_16        = 1,
    GDT_LDT                     = 2,
    GDT_TSS_BUSY_16             = 3,
    GDT_CALL_GATE_16            = 4,
    GDT_TASK_GATE               = 5,
    GDT_INTERRUPT_GATE_16       = 6,
    GDT_TRAP_GATE_16            = 7,
    GDT_TSS_AVAILABLE_32        = 9,
    GDT_TSS_AVAILABLE_64        = 9,
    GDT_TSS_BUSY_32             = 11,
    GDT_TSS_BUSY_64             = 11,
    GDT_CALL_GATE_32            = 12,
    GDT_CALL_GATE_64            = 12,
    GDT_INTERRUPT_GATE_32       = 14,
    GDT_INTERRUPT_GATE_64       = 14,
    GDT_TRAP_GATE_32            = 15,
    GDT_TRAP_GATE_64            = 15,
};

/*
** A segment descriptor of the GDT.
** Encapsulates the previous structures into a uniform one.
*/
struct gdt_segment_descriptor {
    union {
        struct gdt_code_segment_descriptor code;
        struct gdt_data_segment_descriptor data;
        struct gdt_system_segment_descriptor system;
        uint64 raw : 64;
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

static_assert(sizeof(struct gdt_fatptr) == 10);

/*
** Create a null descriptor, with all bits set to 0.
*/
# define NEW_NULL_DESCRIPTOR                        \
    ((struct gdt_segment_descriptor) {              \
        .raw = 0                                    \
    })
/*
** Create a code segment descriptor, using the given base and limit.
** The value of the other fields can also be appended.
*/
# define NEW_GDT_CODE_ENTRY(base, limit, ...)               \
    ((struct gdt_segment_descriptor) {                      \
        .code = {                                           \
            .limit_low = ((limit) & 0xFFFF),                \
            .base_low = ((base) & 0xFFFF),                  \
            .base_mid = (((base) >> 16) & 0xFF),            \
            .limit_high = (((limit) >> 16) & 0xF),          \
            .base_high = (((base) >> 24 & 0xFF)),           \
            ._reserved0 = 1,                                \
            ._reserved1 = 1,                                \
            __VA_ARGS__                                     \
        },                                                  \
    })

/*
** Create a data segment descriptor, using the given base and limit.
** The value of the other fields can also be appended.
*/
# define NEW_GDT_DATA_ENTRY(base, limit, ...)               \
    ((struct gdt_segment_descriptor) {                      \
        .data = {                                           \
            .limit_low = ((limit) & 0xFFFF),                \
            .base_low = ((base) & 0xFFFF),                  \
            .base_mid = (((base) >> 16) & 0xFF),            \
            .limit_high = (((limit) >> 16) & 0xF),          \
            .base_high = (((base) >> 24 & 0xFF)),           \
            ._reserved0 = 0,                                \
            ._reserved1 = 1,                                \
            __VA_ARGS__                                     \
        },                                                  \
    })

/*
** Create a system segment descriptor, using the given base and limit.
** The value of the other fields can also be appended.
*/
# define NEW_GDT_SYSTEM_ENTRY(base, limit, ...)             \
    ((struct gdt_segment_descriptor) {                      \
        .system = {                                         \
            .limit_low = ((limit) & 0xFFFF),                \
            .base_low = ((base) & 0xFFFF),                  \
            .base_mid = (((base) >> 16) & 0xFF),            \
            .limit_high = (((limit) >> 16) & 0xF),          \
            .base_high = (((base) >> 24 & 0xFF)),           \
            ._reserved0 = 0,                                \
            ._reserved1 = 0,                                \
            __VA_ARGS__                                     \
        },                                                  \
    })

#endif /* !_ARCH_X86_64_GDT_H_ */
