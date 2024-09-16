/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Discover CPUs following the old Intel "Multiprocessor Specification".
**
** This discovery method is really old and not supported any more in real hardware.
** We are using it because it's easier to implemant than ACPI and can still be
** tested through QEMU.
**
** References:
**
** Intel MultiProcessor Specification:
**   https://pdos.csail.mit.edu/6.828/2008/readings/ia32/MPspec.pdf
*/

#include <poseidon/kconfig.h>

#if KCONFIG_SMP

#include <poseidon/poseidon.h>
#include <poseidon/status.h>
#include <poseidon/cpu/cpu.h>
#include <arch/x86_64/smp.h>
#include <arch/x86_64/apic.h>
#include <arch/x86_64/ioapic.h>
#include <lib/string.h>
#include <lib/checksum.h>
#include <lib/log.h>

# define TRAMPOLINE_START       (0x10000)

extern void start_ap(void);

/*
** Look for the MP Floating Pointer Structure from `start` to `start + len`.
*/
static
struct mp const *
smp_search_range(
    physaddr_t start,
    size_t len
) {
    uchar *s;
    uchar *e;

    s = (uchar *)start;
    e = (uchar *)start + len;
    while (s < e) { // FIXME Unsafe
        if (memcmp(s, "_MP_", 4) == 0 && checksum8(s, sizeof(struct mp)) == 0) {
            return ((struct mp *)s);
        }
        s += sizeof(struct mp);
    }
    return (NULL);
}

/*
** Look for the MP Floating Pointer Structure
**
** According to the spec, it is in one of the following three locations:
**   1) In the first KB of the EBDA (Extended Bios Data Area)
**   2) In the last KB of system base memory
**   3) In the BIOS ROM between 0xF0000 and 0xFFFFF
*/
static
struct mp const *
smp_search(void)
{
    struct mp const *mp;
    uchar *bda;
    physaddr_t p;

    mp = NULL;
    bda = (uchar *)0x400;
    p = (bda[0x0F] << 8) | (bda[0x0E] << 4);
    if (p != 0) {
        mp = smp_search_range(p, 1024u);
    } else {
        p = ((bda[0x014] << 8) | (bda[0x13])) * 1024u;
        if (p != 0) {
            mp = smp_search_range(p - 1024u, 1024u);
        }
    }
    return (mp ? mp : smp_search_range(0xF0000, 0x10000));
}

/*
** Detect other CPUs to try and find out if SMP is availalbe.
**
** Return `true` if it is, `false` otherwise.
*/
status_t
smp_detect(void)
{
    struct mp const *mp;
    struct mp_conf const *conf;
    struct mp_proc const *proc;
    struct mp_ioapic const *ioapic;
    uchar const *type;

    // Look for the MP table
    mp = smp_search();
    if (!mp) {
        return (ERR_NOT_FOUND);
    } else if (!mp->conf_physaddr) { // We do not support default configurations (conf_physaddr == 0).
        return (ERR_NOT_SUPPORTED);
    }

    // Retrieve the MP configuration table and ensure its signature is valid

    conf = (struct mp_conf *)(physaddr_t)(mp->conf_physaddr);

    if (memcmp(conf->signature, "PCMP", 4) || checksum8(conf, conf->length)) {
        return (ERR_BAD_STATE);
    }

    apic_map(conf->lapic_paddr);

    type = (uchar *)(conf + 1);
    while (type < (uchar *)conf + conf->length)
    {
        switch (*type)
        {
            case MP_PROCESSOR:
                proc = (struct mp_proc *)type;
                if (ncpu < KCONFIG_MAX_CPUS) {
                    cpus[ncpu].apic_id = proc->lapic_id;
                    cpus[ncpu].cpu_id = ncpu;
                    ncpu++;
                }
                type += sizeof(*proc);
                break;
            case MP_IO_APIC:
                ioapic = (struct mp_ioapic *)type;
                ioapic_map(ioapic->addr);
                type += sizeof(*ioapic);
                break;
            case MP_BUS:
            case MP_IO_INTERRUPT:
            case MP_LOCAL_INTERRUPT:
                type += 8;
                break;
            default:
                /* Unknown entries type have their length following */
                type += *(type + 1);
                break;
        }
    }
    return (OK);
}

/*
** Start the other processors
*/
void
smp_start_aps(void)
{
    struct cpu *cpu;
    struct cpu *current;

    /*
    ** Boot code is size-limited, so we only do a far-jump to boot_ap,
    ** where it's more convienent and not restricted.
    */
    current = current_cpu();
    *(uchar *)(TRAMPOLINE_START) = 0xEA; /* Far jump, IP, CS */
    *(ushort *)(TRAMPOLINE_START + 1) = (uint16)(uintptr)&start_ap + 0x10;
    *(ushort *)(TRAMPOLINE_START + 3) = 0xFFFF;

    /* Start all available cpus */
    for (cpu = cpus; cpu < cpus + ncpu; ++cpu) {
        if (cpu == current)
            continue;

        log("Starting AP processor %zu ...", cpu->cpu_id);

        assert_ok(apic_start_ap(cpu, TRAMPOLINE_START));

        while (42) {
            if (volatile_read(cpu->started)) {
                break;
            }
        }

        logln(" done!");
    }
}

#endif