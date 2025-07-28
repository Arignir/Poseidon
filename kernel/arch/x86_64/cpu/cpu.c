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
#include "lib/sync/rcu.h"
#include "lib/log.h"
#include "lib/list.h"

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
    struct linked_list *cursor;
    uint32_t apic_id;

    apic_id = apic_get_id();

    list_for_each(cursor, &g_cpus_available) {
        struct cpu *cpu;

        cpu = list_entry(cursor, struct cpu, available_cpus);
        if (cpu->apic_id == apic_id) {
            return &cpu->local_data;
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
    struct linked_list *cursor;
    bool done;

    // Boot code is size-limited, so we only do a far-jump to `start_ap()`,
    // where it's more convienent and not restricted.
    *(uchar *)(CPU_TRAMPOLINE_START) = 0xEA; // Far jump, IP, CS
    *(ushort *)(CPU_TRAMPOLINE_START + 1) = (uint16)(uintptr)&start_ap + 0x10;
    *(ushort *)(CPU_TRAMPOLINE_START + 3) = 0xFFFF;

    rcu_acquire_read();

    list_for_each(cursor, &g_cpus_available) {
        struct cpu *cpu;

        cpu = list_entry(cursor, struct cpu, available_cpus);
        if (cpu->bsp) {
            continue;
        }

        logln("Starting co-processor %zu...", cpu->id);

        assert_ok(cpu_start_ap(cpu, CPU_TRAMPOLINE_START));
    }

    // Wait until all CPUs are active.
    do {
        done = true;


        list_for_each(cursor, &g_cpus_available) {
            struct cpu *cpu;

            cpu = list_entry(cursor, struct cpu, available_cpus);

            if (!volatile_read(&cpu->active)) {
                done = false;
                break;
            }
        }

    } while (!done);

    rcu_release_read();

    logln("All co-processors are initialized & active.");
}

