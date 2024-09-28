/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "arch/x86_64/apic.h"
#include "arch/x86_64/cmos.h"
#include "arch/x86_64/rdtsc.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/apic.h"
#include "poseidon/poseidon.h"
#include "poseidon/cpu/cpu.h"
#include "poseidon/memory/kheap.h"
#include "lib/log.h"

/* The bootstrap processor. Only used at boot time. */
[[boot_data]]
static struct cpu __bsp = { .thread = NULL };

[[boot_data]]
struct cpu *bsp = &__bsp;

// Set if the BSP has been remaped to its corresponding entry within the cpu table.
static bool bsp_remapped = false;

// Variable shared with the AP starting up to give it its kernel stack.
[[boot_data]]
virtaddr_t g_ap_boot_stack;

/*
** Kernel's Bootstrap Processor boot stack.
** Will be used as the scheduler's stack.
*/
extern virtaddr_t bsp_kernel_stack_top[];
extern virtaddr_t bsp_kernel_stack_bot[];

/*
** Return the current cpu actually running this code.
**
** We can't use `apic_get_id()` if the APIC isn't set up yet, so we're
** using an other structure, `bsp`, which can be used temporarily.
** The function cpu_remap_bsp() will transfer the content of 'bsp'
** to the actuel entry of the BSP within the cpus table.
**
** TODO: Improve this using fs/gs instead of an iterative loop over `cpus`.
*/
struct cpu *
current_cpu(void)
{
    struct cpu *cpu;
    uint32 apic_id;

    if (bsp_remapped) {
        apic_id = apic_get_id();

        cpu = cpus;
        while (cpu < cpus_end) {
            if (cpu->apic_id == apic_id) {
                return (cpu);
            }
            ++cpu;
        }

        panic("Current cpu has an unknown local APIC id\n");
    }
    else {
        return (bsp);
    }
}

/*
** TODO FIXME: Remove this little hack and actually use cpus[0] to represent the BSP
*/
[[boot_text]]
void
cpu_remap_bsp(void)
{
    struct cpu *cpu;

    assert(!bsp_remapped);

    bsp_remapped = true;

    // Swap the data structure
    cpu = current_cpu();
    *cpu = *bsp;
    bsp = NULL;

    // Set the scheduler stack of the BSP
    cpu->scheduler_stack = bsp_kernel_stack_bot;
    cpu->scheduler_stack_top = bsp_kernel_stack_top;

    // Mark the current CPU as the BSP
    cpu->bsp = true;
}

/*
** Waits for a couple of cpu clocks
*/
static void
cpu_micro_wait(void)
{
    uint64 tsc;

    tsc = rdtsc();
    while (rdtsc() < tsc + 30000000ull);
}

/*
** Start the AP and make it jump at the given address.
**
** The given parameter `ap` must NOT be locked.
*/
status_t
cpu_start_ap(
    struct cpu *ap,
    uintptr addr
) {
    uint32_t apic_id;
    ushort volatile * volatile wrv; // Volatile here is used to silent -Warray-bounds

    assert((addr & 0xFFF00FFF) == 0);

    // Allocate stack for the new cpu
    ap->scheduler_stack = kheap_alloc_aligned(KCONFIG_KERNEL_STACK_SIZE);
    ap->scheduler_stack_top = (uchar *)ap->scheduler_stack + KCONFIG_KERNEL_STACK_SIZE;

    if (ap->scheduler_stack == NULL) {
        return (ERR_OUT_OF_MEMORY);
    }

    g_ap_boot_stack = ap->scheduler_stack_top;

    apic_id = ap->apic_id;

    // MP Specification says that we must initialize CMOS shutdown code to
    // 0xA and set the warm reset vector (DWORD based at 40:67) to point to the
    // AP startup code before doing the universal startup algorithm.
    cmos_write(CMOS_RESET_REGISTER, 0x0A);

    // Warm reset vector is at fixed address 0x40:0x67
    wrv = (ushort *)(0x40 << 4 | 0x67);
    wrv[1] = 0;
    wrv[0] = addr >> 4;

    // Universal Startup Algorithm

    apic_send_ipi(apic_id, APIC_ICR_INIT);
    assert(apic_ipi_acked());
    cpu_micro_wait();

    for (int i = 0; i < 2; ++i) {
        apic_send_ipi(apic_id, APIC_ICR_STARTUP | (addr >> 12));
        cpu_micro_wait();
    }
    return (OK);
}

/*
** Start all the other processors
*/
void
cpu_start_all_aps(void)
{
    struct cpu *cpu;
    struct cpu *current;

    /*
    ** Boot code is size-limited, so we only do a far-jump to boot_ap,
    ** where it's more convienent and not restricted.
    */
    current = current_cpu();
    *(uchar *)(CPU_TRAMPOLINE_START) = 0xEA; /* Far jump, IP, CS */
    *(ushort *)(CPU_TRAMPOLINE_START + 1) = (uint16)(uintptr)&start_ap + 0x10;
    *(ushort *)(CPU_TRAMPOLINE_START + 3) = 0xFFFF;

    /* Start all available cpus */
    for (cpu = cpus; cpu < cpus + ncpu; ++cpu) {
        if (cpu == current)
            continue;

        log("Starting AP processor %zu ...", cpu->cpu_id);

        assert_ok(cpu_start_ap(cpu, CPU_TRAMPOLINE_START));

        while (42) {
            if (volatile_read(cpu->started)) {
                break;
            }
        }

        logln(" done!");
    }
}

