/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** The Great Descriptor Table (GDT).
*/

#include "arch/x86_64/gdt.h"
#include "poseidon/memory/memory.h"

/*
** A statically allocated GDT.
**
** We need one TSS per CPU, so the size of the GDT depends on the maximum
** number of supported CPUs.
*/
[[gnu::aligned(16)]]
struct  gdt_segment_descriptor const gdt[] = {
    /* 0x00: Null segment selector (required by Intel) */
    NEW_NULL_DESCRIPTOR,

    /* 0x08: Kernel code segment selector */
    NEW_GDT_CODE_ENTRY(
        0x00000000,
        0x00000000,
        .accessed = 0,
        .readable = 1,
        .conforming = 0,
        .dpl = 0,
        .present = 1,
        .avl = 0,
        .long_mode = 1,
        .granularity = 1,
    ),

    /* 0x10: Kernel data segment selector */
    NEW_GDT_DATA_ENTRY(
        0x00000000,
        0x00000000,
        .accessed = 0,
        .writable = 1,
        .direction = 0,
        .dpl = 0,
        .present = 1,
        .avl = 0,
        .long_mode = 1,
        .granularity = 1,
    ),

    /* 0x18: User code segment selector */
    NEW_GDT_CODE_ENTRY(
        0x00000000,
        0x00000000,
        .accessed = 0,
        .readable = 1,
        .conforming = 0,
        .dpl = 3,
        .present = 1,
        .avl = 0,
        .long_mode = 1,
        .granularity = 1,
    ),

    /* 0x20: User data segment selector */
    NEW_GDT_DATA_ENTRY(
        0x00000000,
        0x00000000,
        .accessed = 0,
        .writable = 1,
        .direction = 0,
        .dpl = 3,
        .present = 1,
        .avl = 0,
        .long_mode = 1,
        .granularity = 1,
    ),

    /* 0x28: TSS selector (bootstrap CPU) */
    NEW_GDT_SYSTEM_ENTRY(
        0x00000000,
        0x00000000,
        .type = GDT_TSS_AVAILABLE_64,
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
[[boot_rodata]]
[[gnu::aligned(16)]]
struct gdt_fatptr const gdt_fatptr = {
    .limit = sizeof(gdt) - 1,
    .base = gdt,
};
