/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Functions to manipulate the Great Descriptor Table (GDT).
**
** We have two GDTs: one made compile-time used during boot time, and an other
** one dynamically generated when booting used all the way after that.
**
** The GDT cannot entierely be made at compile-time. The number of segments we
** are going to need and their content aren't all known at that point.
**
** For exemple, each core in an SMP-context need its own TSS descriptor, but
** the number of active core isn't a compile-time available information.
**
** Therefore, we use a static GDT at boot time, and later switch to a dynamic
** one when we have all the information we need to generate it.
*/

#include <arch/x86/gdt.h>

/*
** A statically allocated GDT, used at the beginning, during boot time,
** until the kernel has dynamic allocation.
*/
__boot_rodata
__aligned(16)
struct gdt_segment_descriptor const boot_gdt[] = {
    /* 0x00: Null segment selector (required by Intel) */
    NEW_NULL_DESCRIPTOR,

    /* 0x08: Kernel code segment selector */
    NEW_GDT_CODE_ENTRY(
        0x00000000,
        0xFFFFFFFF,
        .accessed = 0,
        .readable = 1,
        .conforming = 0,
        .dpl = 0,
        .present = 1,
        .avl = 0,
        .default_op_size = 1,
        .granularity = 1,
    ),

    /* 0x10: Kernel data segment selector */
    NEW_GDT_DATA_ENTRY(
        0x00000000,
        0xFFFFFFFF,
        .accessed = 0,
        .writable = 1,
        .direction = 0,
        .dpl = 0,
        .present = 1,
        .avl = 0,
        .big = 1,
        .granularity = 1,
    ),

    /* 0x18: User code segment selector */
    NEW_GDT_CODE_ENTRY(
        0x00000000,
        0xFFFFFFFF,
        .accessed = 0,
        .readable = 1,
        .conforming = 0,
        .dpl = 3,
        .present = 1,
        .avl = 0,
        .default_op_size = 1,
        .granularity = 1,
    ),

    /* 0x20: User data segment selector */
    NEW_GDT_DATA_ENTRY(
        0x00000000,
        0xFFFFFFFF,
        .accessed = 0,
        .writable = 1,
        .direction = 0,
        .dpl = 3,
        .present = 1,
        .avl = 0,
        .big = 1,
        .granularity = 1,
    ),

    /* 0x28: TSS selector (bootstrap CPU) */
    NEW_GDT_SYSTEM_ENTRY(
        0x00000000,
        0x00000000,
        .type = TSS_AVAILABLE_32,
        .dpl = 3,
        .present = 1,
        .granularity = 1,
    ),
};

/*
** The fat pointer describing the GDT.
** It contains its address and its size (minus 1).
**
** This fat pointer is given in argument to the LGDT instruction when loading
** the GDT.
*/
__boot_rodata
__aligned(16)
struct gdt_fatptr const boot_gdt_fatptr = {
    .limit = sizeof(boot_gdt) - 1,
    .base = boot_gdt,
};

/*
** A dynamically allocated GDT.
**
** For now, this dynamic GDT isn't used nor generated at all.
** (the kernel isn't advanced enough)
*/
struct gdt_segment_descriptor *gdt;

/*
** The fat pointer describing the GDT.
** It contains its address and its size (minus 1).
**
** This fat pointer is given in argument to the LGDT instruction when loading
** the GDT.
*/
__aligned(16)
struct gdt_fatptr gdt_fatptr;
