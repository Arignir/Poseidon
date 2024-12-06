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
#include "poseidon/memory/memory.h"
#include "lib/log.h"

// Variable shared with the AP starting up to give it its kernel stack.
[[boot_data]]
virtaddr_t g_ap_boot_stack;

/*
** Look for the CPU-local data manually, using the current cpu's APIC ID instead
** of using the GS segment.
*/
struct cpu_local_data const *
cpu_fetch_current_cpu_local_data_manually(
    void
) {
    uint32_t apic_id;
    size_t i;

    apic_id = apic_get_id();

    for (i = 0; i < KCONFIG_MAX_CPUS; ++i) {
        if (g_cpus[i].apic_id == apic_id) {
            return &g_cpus_local_data[i];
        }
    }

    panic("Failed to manually find the CPU-local data of the current CPU.");
}

/*
** Waits for a couple of cpu clocks
*/
static void
cpu_micro_wait(
    void
) {
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
    ushort volatile * volatile wrv; // Volatile here is used to silent -Warray-bounds

    assert((addr & 0xFFF00FFF) == 0);

    // Allocate stack for the new cpu
    ap->scheduler_stack = kheap_alloc_aligned(KCONFIG_KERNEL_STACK_SIZE);
    ap->scheduler_stack_top = (uchar *)ap->scheduler_stack + KCONFIG_KERNEL_STACK_SIZE;

    if (ap->scheduler_stack == NULL) {
        return ERR_OUT_OF_MEMORY;
    }

    g_ap_boot_stack = ap->scheduler_stack_top;

    // MP Specification says that we must initialize CMOS shutdown code to
    // 0xA and set the warm reset vector (DWORD based at 40:67) to point to the
    // AP startup code before doing the universal startup algorithm.
    cmos_write(CMOS_RESET_REGISTER, 0x0A);

    // Warm reset vector is at fixed address 0x40:0x67
    wrv = (ushort *)(0x40 << 4 | 0x67);
    wrv[1] = 0;
    wrv[0] = addr >> 4;

    // Universal Startup Algorithm

    apic_ipi_send_init(ap);
    assert(apic_ipi_acked());
    cpu_micro_wait();

    for (int i = 0; i < 2; ++i) {
        apic_ipi_send_startup(ap, addr);
        cpu_micro_wait();
    }
    return OK;
}

/*
** Start all the other processors
*/
void
cpu_start_all_aps(
    void
) {
    struct cpu *cpu;

    /*
    ** Boot code is size-limited, so we only do a far-jump to boot_ap,
    ** where it's more convienent and not restricted.
    */
    *(uchar *)(CPU_TRAMPOLINE_START) = 0xEA; /* Far jump, IP, CS */
    *(ushort *)(CPU_TRAMPOLINE_START + 1) = (uint16)(uintptr)&start_ap + 0x10;
    *(ushort *)(CPU_TRAMPOLINE_START + 3) = 0xFFFF;

    /* Start all available cpus */
    for (cpu = g_cpus; cpu < g_cpus + g_cpus_len; ++cpu) {
        if (cpu == cpu_get_bsp()) {
            continue;
        }

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

