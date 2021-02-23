/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Continue the initialisation of the CPU.
*/

#include <poseidon/boot/init_hook.h>
#include <poseidon/memory/pmm.h>
#include <platform/pc/pic/pic8259.h>
#include <arch/x86_64/interrupt.h>
#include <arch/x86_64/cpuid.h>
#include <arch/x86_64/ioapic.h>
#include <arch/x86_64/apic.h>
#include <arch/x86_64/msr.h>
#include <lib/log.h>

/*
** Continue the early initialisation of the CPU.
*/
__boot_text
static
status_t
early_setup(void)
{
    setup_idt();

    cpuid_load();

    logln("Dumping CPUID:");
    cpuid_dump();

    // Test if the APIC is supported & availalbe
    if (!cpu_features.features.apic) {
        panic("Your CPU doesn't contain an APIC");
    }

    // Enable NX
    if (cpu_features.features.nx) {
        msr_or(MSR_IA32_EFER, 1 << 11);   // Execute Disable Bit Enable
    }

    return (OK);
}

REGISTER_INIT_HOOK(early_setup, &early_setup, INIT_LEVEL_ARCH_EARLY);

/*
** Continue the initialisation of the CPU.
*/
__boot_text
static
status_t
setup(void)
{

    ioapic_map(IOAPIC_BASE_ADDR);
    apic_map(APIC_BASE_ADDR);

    pic8259_init();
    ioapic_init();
    apic_init();

    return (OK);
}

REGISTER_INIT_HOOK(setup, &setup, INIT_LEVEL_ARCH);

/*
** On PC, the first mega-byte of physical memory is usually used by hardware,
** the bios, the bootloader and other stuff we don't want to mess with.
*/
REGISTER_PMM_RESERVED_AREA(pc_bios, 0x0, 0x100000);
