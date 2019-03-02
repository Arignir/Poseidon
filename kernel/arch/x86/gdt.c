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
** one dynamically generated at boot used all the way after that.
**
** The GDT cannot entierely be made at compile-time. The number of segments we
** are going to need and their content aren't known at that point.
**
** For exemple, each core in an SMP-context need its own TSS descriptor, but
** the number of active core isn't a compile-time available information.
**
** Therefore, we use a static GDT at boot time, and later switch to a dynamic
** one when we have all the information we need to generate it.
**
** The boot gdt is handled in `arch/x86/boot/gdt.c`
*/

#include <arch/x86/gdt.h>

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
