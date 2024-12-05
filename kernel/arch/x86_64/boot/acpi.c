/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Discover the available hardware following the Advanced Configuration Power
** Interface (ACPI).
**
** References:
**
** Advance Configuration Power Interface (ACPI) v6.4:
**   https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/
*/

#include "arch/x86_64/acpi.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/ioapic.h"
#include "poseidon/cpu/cpu.h"
#include "poseidon/memory/memory.h"
#include "lib/log.h"
#include "lib/checksum.h"
#include "lib/string.h"

static struct rsdp const *g_rsdp = nullptr;
static struct rsdt const *g_rsdt = nullptr;
static struct fadt const *g_fadt = nullptr;
static struct madt const *g_madt = nullptr;

/*
** Look for the Root System Description Pointer (RSDP) Structure `start` to `start + len`.
*/
static
struct rsdp const *
rsdp_search_range(
    physaddr_t start,
    size_t len
) {
    uchar const *s;
    uchar const *e;

    s = (uchar const *)start;
    e = (uchar const *)start + len;

    assert(start % 16 == 0);

    while (s < e) {
        if (memcmp(s, "RSD PTR ", 8) == 0) {
            struct rsdp const *rsdp;

            rsdp = (struct rsdp const *)s;

            // We currently do not support ACPI v2.0 since it's not easy to test on QEMU
            if (rsdp->revision == 0 && checksum8(rsdp, sizeof(struct rsdp)) == 0) {
                return rsdp;
            }
        }

        s += 16;
    }

    return nullptr;
}

/*
** Look for the Root System Description Pointer (RSDP) Structure.
**
** According to the spec, it is in one of the following three locations:
**   1) In the first KB of the EBDA (Extended Bios Data Area)
**   2) In the BIOS ROM between 0xE0000 and 0xFFFFF
*/
static
struct rsdp const *
rsdp_search(
    void
) {
    struct rsdp const *rsdp;
    uchar volatile * volatile bda; // Volatile here is used to silent -Warray-bounds
    physaddr_t ebda_addr;

    rsdp = nullptr;
    bda = (uchar *)0x400;
    ebda_addr = (bda[0x0F] << 8) | (bda[0x0E] << 4);

    if (ebda_addr != 0) {
        rsdp = rsdp_search_range(ebda_addr, 1024u);
    }

    return rsdp ?: rsdp_search_range(0xF0000, 0x10000);
}

/*
** Map the given physical address to memory and return the virtual address.
** This function takes care of everything if `table_addr` isn't page-aligned.
**
** NOTE: The pointer to send to `ptr_to_kheap_free()` is written to `ptr_to_keap_free`.
*/
void *
acpi_map_table(
    physaddr_t table_addr,
    size_t table_size,
    void **ptr_to_kheap_free
) {
    physaddr_t aligned_addr;
    size_t aligned_size;
    void *aligned_ptr;

    // Round down to a page boundary `table_addr` and calculate on how many
    // pages the ACPI table spreads on.
    aligned_addr = ROUND_DOWN(table_addr, PAGE_SIZE);
    aligned_size = ALIGN(table_addr + table_size, PAGE_SIZE) - aligned_addr;

    // Map the ACPI table to virtual memory.
    aligned_ptr = kheap_alloc_device(aligned_addr, aligned_size);
    if (!aligned_ptr) {
        return nullptr;
    }

    // Save the return value of `kheap_alloc_device()` in case the calling function
    // needs to free the allocated memory.
    if (ptr_to_kheap_free) {
        *ptr_to_kheap_free = aligned_ptr;
    }

    // Add to `aligned_ptr` the offset between the page boundary and
    // `table_addr`.
    // Therefore, the returned value has the same offset within its page than `table_addr`.
    return (char *)aligned_ptr + (table_addr - aligned_addr);
}

/*
** Parse the MADT and look for the available CPUs and I/O APIC.
*/
void
acpi_parse_madt(
    void
) {
    struct madt_entry_header const *header;

    debug_assert(g_madt);

    // Map the local APIC address
    apic_map(g_madt->lapic_addr);

    header = (struct madt_entry_header const *)g_madt->entries;
    while ((char *)header < (char *)g_madt + g_madt->len) {
        switch (header->type) {
            case MADT_PROCESSOR_LOCAL_APIC: {
                struct madt_processor_local_apic_entry const *entry;

                entry = (struct madt_processor_local_apic_entry const *)header;
                if (g_cpus_len < KCONFIG_MAX_CPUS) {
                    g_cpus[g_cpus_len].acpi_id = entry->acpi_processor_id;
                    g_cpus[g_cpus_len].apic_id = entry->apic_id;
                    g_cpus[g_cpus_len].cpu_id = g_cpus_len;
                    g_cpus_len++;
                }
                break;
            };
            case MADT_IO_APIC: {
                struct madt_io_apic_entry const *entry;

                entry = (struct madt_io_apic_entry const *)header;
                ioapic_map(entry->ioapic_addr);
                break;
            };
            default: break;
        }
        header = (struct madt_entry_header const *)((char *)header + header->length);
    }
}

/*
** Find and parse the ACPI tables.
*/
void
acpi_init(
    void
) {
    size_t rsdt_entries;
    size_t i;

    // Find the RSDP
    g_rsdp = rsdp_search();
    if (!g_rsdp) {
        panic("Failed to find the ACPI's Root Descriptor Table Pointer");
    }

    // Map the RSDT to make it readable
    g_rsdt = acpi_map_table(g_rsdp->rsdt_addr, sizeof(struct rsdt), nullptr);
    if (!g_rsdt) {
        panic("Failed to map the ACPI's Root Descriptor Table");
    }

    log("ACPI: RSDT");

    rsdt_entries = (g_rsdt->len - sizeof(struct sdth)) / 4;

    // Iterate over the RSDT's entries
    for (i = 0; i < rsdt_entries; ++i) {
        struct sdth const *sdth;
        void *unaligned_ptr;

        unaligned_ptr = nullptr;

        // Map the RSDT entry to make it readable
        sdth = acpi_map_table(g_rsdt->entries[i], PAGE_SIZE, &unaligned_ptr);
        if (!sdth) {
            panic("Failed to map an entry of the ACPI's Root Descriptor Table");
        }

        // Log its signature
        log(", %.4s", sdth->signature);

        // Check if that signature matches something we're interested in.
        if (!memcmp(sdth->signature, "FACP", 4)) {
            g_fadt = (struct fadt const *)sdth;
            continue; // Continue to the next entry to avoid freeing the mapping.
        } else  if (!memcmp(sdth->signature, "APIC", 4)) {
            g_madt = (struct madt const *)sdth;
            continue;
        }

        // Free the entries we're not interested in
        kheap_free(unaligned_ptr);
    }

    logln(".");

    if (!g_fadt || !g_madt) {
        panic("Failed to find the APIC entries necessary to boot the system");
    }

    acpi_parse_madt();
}
