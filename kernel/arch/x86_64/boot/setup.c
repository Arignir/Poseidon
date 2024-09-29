/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Continue the initialisation of the CPU.
*/

#include "arch/x86_64/api/cpu.h"
#include "arch/x86_64/interrupt.h"
#include "arch/x86_64/cpuid.h"
#include "arch/x86_64/ioapic.h"
#include "arch/x86_64/apic.h"
#include "arch/x86_64/acpi.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/msr.h"
#include "poseidon/boot/init_hook.h"
#include "poseidon/memory/pmm.h"
#include "poseidon/cpu/cpu.h"
#include "poseidon/scheduler/scheduler.h"
#include "platform/pc/pic8259.h"
#include "lib/log.h"

static void    common_setup(void);

/*
** Kernel's Bootstrap Processor boot stack.
** Will be used as the scheduler's stack.
*/
extern virtaddr_t bsp_kernel_stack_top[];
extern virtaddr_t bsp_kernel_stack_bot[];

/*
** Continue the early initialisation of the bootstrap CPU.
*/
[[boot_text]]
static
status_t
bsp_early_setup(
    void
) {
    struct cpu *cpu;

    idt_setup();
    cpu_init();

    // The BSP's CPU-local data are always at index zero.
    msr_write(MSR_IA32_GSBASE, (uint64_t)&g_cpus_local_data[0]);

    cpu = current_cpu();

    // Paranoid check to make sure the cpu-local data are working correctly
    debug_assert(cpu == cpu_get_bsp());

    cpu->bsp = true;
    cpu->scheduler_stack = bsp_kernel_stack_bot;
    cpu->scheduler_stack_top = bsp_kernel_stack_top;

    cpuid_load(&cpu->cpuid);

    logln("Dumping CPUID:");
    cpuid_dump(&cpu->cpuid);

    // Test if the APIC is supported & availalbe
    if (!cpu->cpuid.features.apic) {
        panic("Your CPU doesn't contain an APIC");
    }

    return OK;
}

REGISTER_INIT_HOOK(bsp_early_setup, &bsp_early_setup, INIT_LEVEL_ARCH_EARLY);

/*
** Continue the initialisation of the bootstrap CPU.
*/
[[boot_text]]
static
status_t
bsp_setup(
    void
) {
    acpi_init();
    pic8259_init();
    ioapic_init();
    apic_init();

    common_setup();

#if KCONFIG_SMP
    logln("Number of cpus: %u", g_cpus_len);

    // Start other processors
    cpu_start_all_aps();
#endif /* KCONFIG_SMP */


    return OK;
}

REGISTER_INIT_HOOK(bsp_setup, &bsp_setup, INIT_LEVEL_ARCH);

/*
** Continue the early initialisation of the other CPUs.
*/
[[boot_text]]
void
ap_setup(
    void
) {
    struct cpu *cpu;

    apic_init();

    msr_write(MSR_IA32_GSBASE, (uint64_t)cpu_find_current_cpu_local_data_manually());

    cpu = current_cpu();
    cpuid_load(&cpu->cpuid);

    common_setup();

    yield();
}

/*
** Common initialisation for both the BSP and the APs.
*/
[[boot_text]]
static
void
common_setup(
    void
) {
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
