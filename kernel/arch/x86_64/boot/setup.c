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
#include <poseidon/cpu/cpu.h>
#include <poseidon/scheduler/scheduler.h>
#include <platform/pc/pic8259.h>
#include <arch/x86_64/interrupt.h>
#include <arch/x86_64/cpuid.h>
#include <arch/x86_64/ioapic.h>
#include <arch/x86_64/apic.h>
#include <arch/x86_64/msr.h>
#include <arch/x86_64/cpu.h>
#include <arch/x86_64/smp.h>
#include <lib/log.h>

static void    common_setup(void);

/*
** Continue the early initialisation of the bootstrap CPU.
*/
__boot_text
static
status_t
bsp_early_setup(void)
{
    struct cpu *bsp;

    bsp = current_cpu();

    idt_setup();

    cpuid_load(&bsp->cpuid);

    logln("Dumping CPUID:");
    cpuid_dump(&bsp->cpuid);

    // Test if the APIC is supported & availalbe
    if (!bsp->cpuid.features.apic) {
        panic("Your CPU doesn't contain an APIC");
    }

    return (OK);
}

REGISTER_INIT_HOOK(bsp_early_setup, &bsp_early_setup, INIT_LEVEL_ARCH_EARLY);

/*
** Continue the initialisation of the bootstrap CPU.
*/
__boot_text
static
status_t
bsp_setup(void)
{
    bool smp_enabled;

    /* If SMP is enabled, detect other CPUs */
#if KCONFIG_SMP
    smp_enabled = (smp_detect() == OK);
#else
    smp_enabled = false;
#endif /* KCONFIG_SMP */

    /* Else, set the only processor to default values */
    if (!smp_enabled) {
        ncpu = 1;
        ioapic_map(IOAPIC_BASE_ADDR);
        apic_map(APIC_BASE_ADDR);
        cpus[0].apic_id = apic_get_id();
        cpus[0].cpu_id = 0;
    }

    pic8259_init();
    ioapic_init();
    apic_init();

    cpu_remap_bsp();

    common_setup();

#if KCONFIG_SMP
    logln("Number of cpus: %u", ncpu);

    smp_start_aps();    // Start other processors
#endif /* KCONFIG_SMP */


    return (OK);
}

REGISTER_INIT_HOOK(bsp_setup, &bsp_setup, INIT_LEVEL_ARCH);

/*
** Continue the early initialisation of the other CPUs.
*/
__boot_text
void
ap_setup(void)
{
    struct cpu *cpu;

    apic_init();

    cpu = current_cpu();
    cpuid_load(&cpu->cpuid);

    common_setup();

    yield();
}

/*
** Common initialisation for both the BSP and the APs.
*/
__boot_text
static
void
common_setup(void)
{
    struct cpu *cpu;

    idt_load();

    cpu = current_cpu();
    cpu->started = true;
}

/*
** On PC, the first mega-byte of physical memory is usually used by hardware,
** the bios, the bootloader and other stuff we don't want to mess with.
*/
REGISTER_PMM_RESERVED_AREA(pc_bios, 0x0, 0x100000);
