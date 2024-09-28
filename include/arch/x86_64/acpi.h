/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

/*
** References:
**
** Intel MultiProcessor Specification:
**   https://pdos.csail.mit.edu/6.828/2008/readings/ia32/MPspec.pdf
*/

#include <stdint.h>
#include "poseidon/memory/memory.h"
#include "poseidon/status.h"

/*
** The Root System Description Pointer (RSDP) Structure
*/
struct [[gnu::packed]] rsdp {
    uint8_t signature[8];
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t revision;
    physaddr32_t rsdt_addr;
};

static_assert(sizeof(struct rsdp) == 20);

/*
** System Descriptor Table Header
*/
struct [[gnu::packed]] sdth {
    uint8_t signature[4];
    uint32_t len;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

static_assert(sizeof(struct sdth) == 36);

/*
** Root System Descriptor Table
*/
struct [[gnu::packed]] rsdt {
    struct sdth;
    uint32_t entries[];
};

struct [[gnu::packed]] fadt_generic_addr {
    uint8_t address_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t address;
};

static_assert(sizeof(struct fadt_generic_addr) == 12);

/*
** Fixed ACPI Description Table
**
** It contains information about ACPI fixed registers, DSDT pointer, etc.
*/
struct [[gnu::packed]] fadt {
    struct sdth;

    physaddr32_t firmware_ctrl;
    physaddr32_t dsdt;
    uint8_t _reserved1;
    uint8_t preferred_pm_profile;
    uint16_t sci_int;
    uint32_t sci_cmd_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_cnt;
    uint32_t pm1a_evt_blk;
    uint32_t pm1b_evt_blk;
    uint32_t pm1a_cnt_blk;
    uint32_t pm1b_cnt_blk;
    uint32_t pm2_cnt_blk;
    uint32_t pm_timer_blk;
    uint32_t gpe0_blk;
    uint32_t gpe1_blk;
    uint8_t pm1_evt_len;
    uint8_t pm1_cnt_len;
    uint8_t pm2_cnt_len;
    uint8_t pm_tmr_len;
    uint8_t gpe0_blk_len;
    uint8_t gpe1_blk_len;
    uint8_t gpe1_base;
    uint8_t cst_cnt;
    uint16_t p_lvl2_lat;
    uint16_t p_lvl3_lat;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;
    uint16_t iapc_boot_arch;
    uint8_t _reserved2;
    uint32_t flags;
    struct fadt_generic_addr reset_register;
    uint8_t reset_value;
    uint16_t arm_boot_arch;
    uint8_t fadt_minor_version;
    uint64_t x_firmware_ctrl;
    uint64_t x_dsdt;
    struct fadt_generic_addr x_pm1a_evt_blk;
    struct fadt_generic_addr x_pm1b_evt_blk;
    struct fadt_generic_addr x_pm1a_cnt_blk;
    struct fadt_generic_addr x_pm1b_cnt_blk;
    struct fadt_generic_addr x_pm2_cnt_blk;
    struct fadt_generic_addr x_pm_tmr_blk;
    struct fadt_generic_addr x_gpe0_blk;
    struct fadt_generic_addr x_gpe1_blk;
    struct fadt_generic_addr sleep_ctrl_reg;
    struct fadt_generic_addr sleep_status_reg;
    uint64_t hypervisor_vendor_id;
};

static_assert(sizeof(struct fadt) == 276);

/*
** The header of alle entries within the MADT.
*/
struct [[gnu::packed]] madt_entry_header {
    uint8_t type;
    uint8_t length;
};

static_assert(sizeof(struct madt_entry_header) == 2);

/*
** The different values that `madt_entry_header.type`.
**
** NOTE: This enum is incomplete, only the required values are present.
*/
enum madt_entries {
    MADT_PROCESSOR_LOCAL_APIC = 0,
    MADT_IO_APIC = 1,

    // ...
};

/*
** Multiple APIC Description Table (MADT).
*/
struct [[gnu::packed]] madt {
    struct sdth;

    physaddr32_t lapic_addr;
    uint32_t flags;
    uint8_t entries[];
};

static_assert(sizeof(struct madt) == 44);

struct [[gnu::packed]] madt_processor_local_apic_entry {
    struct madt_entry_header;

    uint8_t acpi_processor_id;
    uint8_t apic_id;
    uint32_t flags;
};

static_assert(sizeof(struct madt_processor_local_apic_entry) == 8);

struct [[gnu::packed]] madt_io_apic_entry {
    struct madt_entry_header;

    uint8_t ioapic_id;
    uint8_t _reserved;
    physaddr32_t ioapic_addr;
    physaddr32_t gsi_base;
};

static_assert(sizeof(struct madt_io_apic_entry) == 12);


void acpi_init(void);
