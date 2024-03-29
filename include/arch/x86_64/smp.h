/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/kconfig.h>

#if !defined(_ARCH_X86_64_SMP_H_) && KCONFIG_SMP
# define _ARCH_X86_64_SMP_H_

# include <poseidon/poseidon.h>
# include <poseidon/memory/memory.h>

# define TRAMPOLINE_START       (0x10000)

/*
** References:
**
** Intel MultiProcessor Specification:
**   https://pdos.csail.mit.edu/6.828/2008/readings/ia32/MPspec.pdf
*/

/*
** MP Floating Pointer Structure
** Always starts with the "_MP_" signature.
*/
struct mp {
    uchar const signature[4];           // _MP_
    uint32 const conf_physaddr;         // Physical address of the MP configuration table
    uchar const length;                 // Length of the table / 16. Should be 1
    uchar const spec_rev;               // Revision of the MP spec
    uchar const checksum;               // All bytes must add up to 0
    uchar const type;                   // MP system config type
    uchar const imcrp;                  // IMCRP
    uchar const reserved[3];            // Reserved. Should be 0
} __packed;

static_assert(sizeof(struct mp) == 16);

/*
** MP Configuration Table
** Contains, among others, the physical address of the local APIC.
*/
struct mp_conf {
    uchar const signature[4];           // PCMP
    ushort const length;                // Table length
    uchar const spec_rev;               // Revision of the MP spec
    uchar const checksum;               // All bytes must add up to 0
    uchar const oem_id[8];              // String that identifies manufacturer
    uchar const product_id[12];         // String that identifies product family
    uint32_t const oemtable;            // Pointer to oem-defined conf table, or 0
    ushort const oemlength;             // Length of oem-defined conf table
    ushort const entry_count;           // Number of entries
    uint32_t const lapic_paddr;         // Physical address of local APIC
    ushort const xlength;               // Extended table length
    uchar const xchecksum;              // All bytes of the xtable must add up to 0
    uchar const reserved;               // Reserved. Should be 0
} __packed;

static_assert(sizeof(struct mp_conf) == 4 * 11);

/*
** A processor entry within the MP Configuration Table
*/
struct mp_proc {
    uchar const type;                   // Entry Type (0)
    uchar const lapic_id;               // Local APIC ID
    uchar const lapic_version;          // Local APIC version number
    uchar const flags;                  // CPU flags
    uchar const signature[4];           // CPU signature
    uint const feature;                 // CPU features
    uchar const reserved[8];            // Reserved
} __packed;

static_assert(sizeof(struct mp_proc) == 20);

/*
** A bus entry within the MP Configuration Table
*/
struct mp_bus {
    uchar const type;                   // Entry type (1)
    uchar const id;                     // Bus Id
    uchar const signature[6];           // String that identifies the bus type
} __packed;

static_assert(sizeof(struct mp_bus) == 8);

/*
** An I/O APIC entry within the MP Configuration Table
*/
struct mp_ioapic {
    uchar const type;                   // Entry type (2)
    uchar const id;                     // I/O APIC Id
    uchar const version;                // I/O APIC Version number
    uchar const flags;                  // I/O APIC Flags
    uint32 const addr;                  // I/O APIC address
} __packed;

static_assert(sizeof(struct mp_ioapic) == 8);

/*
** An I/O Interrupt entry within the MP Configuration Table
*/
struct mp_ioint {
    uchar const type;                   // Entry type (3)
    uchar const int_type;               // Interrupt type
    ushort const flags;                 // I/O Interrupt Flags
    uchar const bus_id;                 // Source Bus ID
    uchar const bus_irq;                // Source Bus IRQ
    uchar const apic_id;                // Destination APIC ID
    uchar const apic_int;               // Destination APIC int number
} __packed;

static_assert(sizeof(struct mp_ioint) == 8);

/*
** A Local Interrupt entry within the MP Configuration Table
*/
struct mp_lint {
    uchar const type;                   // Entry type (3)
    uchar const int_type;               // Interrupt type
    ushort const flags;                 // Local Interrupt Flags
    uchar const bus_id;                 // Source Bus ID
    uchar const bus_irq;                // Source Bus IRQ
    uchar const apic_id;                // Destination Local APIC ID
    uchar const apic_int;               // Destination Local APIC lint number
} __packed;

static_assert(sizeof(struct mp_lint) == 8);

/*
** The different values that the 'entry type' field of MP Configuration Table
** entries can be. It is possible that some entries contains other type code.
** If it's the case, they should be ignore.
*/
enum MP_ENTRY_TYPE {
    MP_PROCESSOR                        = 0x0,
    MP_BUS                              = 0x1,
    MP_IO_APIC                          = 0x2,
    MP_IO_INTERRUPT                     = 0x3,
    MP_LOCAL_INTERRUPT                  = 0x4,
};

status_t smp_detect(void);
void smp_start_aps(void);

#endif /* !_ARCH_X86_SMP_H_ */
