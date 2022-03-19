/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _ARCH_X86_64_IOAPIC_H_
# define _ARCH_X86_64_IOAPIC_H_

# include <poseidon/poseidon.h>
# include <poseidon/memory/memory.h>

# define IOAPIC_BASE_ADDR               ((physaddr_t)0x0FEC00000)

/*
** All I/O APIC register offset
*/
enum ioapic_reg {
    IOAPIC_ID                           = 0x00, // ID
    IOAPIC_VERSION                      = 0x01, // Version
    IOAPIC_TABLE                        = 0x10, // Table base
};

union ioapic_entry {
    struct {
        size_t vector : 8;                      // Allowed values from 0x10 to 0xFE
        size_t delivery_mode : 3;               // Type of delivery mode
        size_t dest_mode : 1;                   // 0 = physical mode, 1 = logical mode
        size_t const delivery_status : 1;       // Set if APIC is busy
        size_t polarity : 1;
        size_t const received : 1;
        size_t trigger_mode : 1;                // 0 = edge sensitive, 1 = level sensitive
        size_t mask : 1;                        // Set to mask the interrupt
        size_t : 15;
        size_t : 24;
        size_t dest : 8;
    };
    struct {
        uint32 low;
        uint32 high;
    };
    uint64 value;
};

static_assert(sizeof(union ioapic_entry) == sizeof(uint64));

/* Delivery mode */
enum ioapic_delivery_mode {
    IOAPIC_DELIVERY_MODE_FIXED          = 0x0,
    IOAPIC_DELIVERY_MODE_LOWPR          = 0x1,
    IOAPIC_DELIVERY_MODE_SMI            = 0x2,
    IOAPIC_DELIVERY_MODE_NMI            = 0x4,
    IOAPIC_DELIVERY_MODE_INIT           = 0x5,
    IOAPIC_DELIVERY_MODE_EXTINT         = 0x7,
};

/* Destination mode */
enum ioapic_dest_mode {
    IOAPIC_DEST_MODE_PHYSICAL           = 0x0,
    IOAPIC_DEST_MODE_LOGICAL            = 0x1,
};

/* Interrupt Input Pin Polarity */
enum ioapic_polarity {
    IOAPIC_POLARITY_HIGH                = 0x0,
    IOAPIC_POLARITY_LOW                 = 0x1,
};

/* Trigger Mode (R/W) */
enum ioapic_trigger {
    IOAPIC_TRIGGER_EDGE                 = 0x0,
    IOAPIC_TRIGGER_LEVEL                = 0x1,
};

/* Interrupt Mask (R/W) */
enum ioapic_mask {
    IOAPIC_UNMASK                       = 0x0,
    IOAPIC_MASK                         = 0x1,
};

/* Message Destination Address (APIC logical destination mode) */
enum ioapic_dest_type {
    IOAPIC_DEST_BROADCAST               = 0xff,
};

void ioapic_init(void);
void ioapic_map(physaddr_t);
void ioapic_mask_int(uint vector);
void ioapic_unmask_int(uint vector);

#endif /* !_ARCH_X86_64_IOAPIC_H_ */