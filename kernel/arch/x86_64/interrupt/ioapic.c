/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "arch/x86_64/ioapic.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/interrupt.h"
#include "poseidon/memory/pmm.h"
#include "poseidon/memory/kheap.h"

static volatile uchar *g_ioapic;

/*
** Map the I/O APIC registers to the given physical address.
*/
void
ioapic_map(
    physaddr_t pa
) {
    /* Mark the frame of the IOAPIC as allocated */
    pmm_mark_range_as_allocated(pa, pa + PAGE_SIZE);

    /* Map it to memory */
    g_ioapic = kheap_alloc_device(
        pa,
        PAGE_SIZE
    );

    assert(g_ioapic != NULL);
}

/*
** Write to an ioapic register by writting the register's offset in the
** IOREGSEL I/O APIC register, and then its value in the IOWIN register.
*/
static
void
ioapic_write(
    enum ioapic_reg reg,
    uint32 value
) {
    *(volatile uint32 *)g_ioapic = reg;
    *((volatile uint32 *)g_ioapic + 0x4) = value; // Offset is 0x10 (0x4 * 0x4)
}

/*
** Read an ioapic register by writting the register's offset in the
** IOREGSEL I/O APIC register, and then reading its value in the IOWIN register.
*/
static
uint32
ioapic_read(
    enum ioapic_reg reg
) {
    *(volatile uint32 *)g_ioapic = reg;
    return (*((volatile uint32 *)g_ioapic + 0x4)); // Offset is 0x10 (0x4 * 0x4)
}

/*
** Unmask the given interrupt vector.
*/
void
ioapic_unmask_int(
    uint vector
) {
    union ioapic_entry entry;

    entry.value = 0;
    entry.vector = vector;
    entry.delivery_mode = IOAPIC_DELIVERY_MODE_FIXED;
    entry.polarity = IOAPIC_POLARITY_HIGH;
    entry.trigger_mode = IOAPIC_TRIGGER_EDGE;
    entry.mask = IOAPIC_UNMASK;
    entry.dest_mode = IOAPIC_DEST_MODE_PHYSICAL;
    entry.dest = apic_get_id();
    ioapic_write(IOAPIC_TABLE + 2 * (vector - INT_IRQ0), entry.low);
    ioapic_write(IOAPIC_TABLE + 2 * (vector - INT_IRQ0) + 1, entry.high);
}

/*
** Mask the given interrupt vector.
*/
void
ioapic_mask_int(
    uint vector
) {
    union ioapic_entry entry;

    entry.value = 0;
    entry.vector = vector;
    entry.delivery_mode = IOAPIC_DELIVERY_MODE_FIXED;
    entry.polarity = IOAPIC_POLARITY_HIGH;
    entry.trigger_mode = IOAPIC_TRIGGER_EDGE;
    entry.mask = IOAPIC_MASK;
    entry.dest_mode = IOAPIC_DEST_MODE_PHYSICAL;
    entry.dest = apic_get_id();
    ioapic_write(IOAPIC_TABLE + 2 * (vector - INT_IRQ0), entry.low);
    ioapic_write(IOAPIC_TABLE + 2 * (vector - INT_IRQ0) + 1, entry.high);
}

/*
** Initialize the I/O APIC.
*/
void
ioapic_init(void)
{
    size_t i;
    size_t max_ints;

    assert(g_ioapic);
    max_ints = (ioapic_read(IOAPIC_VERSION) >> 16) & 0xFF;
    for (i = 0; i < max_ints; ++i) {
        ioapic_mask_int(INT_IRQ0 + i);
    }
}
