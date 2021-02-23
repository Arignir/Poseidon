/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <arch/x86_64/apic.h>
#include <arch/x86_64/interrupt.h>
#include <arch/x86_64/msr.h>
#include <arch/x86_64/io.h>
#include <arch/x86_64/rdtsc.h>
#include <poseidon/poseidon.h>
#include <poseidon/interrupt.h>
#include <poseidon/memory/pmm.h>
#include <poseidon/memory/kheap.h>
#include <poseidon/status.h>
#include <poseidon/cpu/cpu.h>
#include <poseidon/io.h>
#include <lib/log.h>

# define APIC_INIT_CLOCK_WAIT	300000000ull

NEW_IO_PORT(cmos, 0x70);
NEW_IO_PORT(cmos_ret, 0x71);

static volatile uchar *apic = NULL;

/* Variable to transmit the AP's new stack when starting up */
__section(".boot_memory")
virtaddr_t ap_boot_stack;

/*
** Write to a local APIC register
*/
static inline
void
apic_write(
    enum apic_reg reg,
    uint32 value
) {
    *((volatile uint32 *)(apic + reg)) = value;
}

/*
** Read a local APIC register
*/
static inline
uint32
apic_read(
    enum apic_reg reg
) {
    return (*((volatile uint32 *)(apic + reg)));
}

/*
** Return the id of the current processor's local APIC.
*/
uint32
apic_get_id(void)
{
    return (apic_read(APIC_ID) >> 24u);
}

/*
** Map the APIC registers to the given physical address.
*/
void
apic_map(
    physaddr_t pa
) {
    /* Mark the frame of the APIC as allocated */
    pmm_mark_range_as_allocated(pa, PAGE_SIZE);

    /* Map it to memory */
    apic = kheap_alloc_device(
        pa,
        PAGE_SIZE
    );

    assert(apic);
}

/*
** Issue an end-of-interrupt signal, allowing the apic to process the next
** interrupt.
*/
void
apic_eoi(void)
{
    apic_write(APIC_EOI, 0x0);
}

/*
** Send the given IPI to the given local APIC.
*/
void
apic_send_ipi(
    uint32 dest,
    uint32 flags
) {
    apic_write(APIC_ICR_HIGH, dest << 24u);
    apic_write(APIC_ICR_LOW, flags);
}

/*
** Poll the delivery status bit untill the latest IPI is acknowledged
** by the destination core, or it timesout.
**
** FIXME: We should use fine-grained timeout
*/
bool
apic_ipi_acked(void)
{
    size_t timeout;
    uint32 reg;

    timeout = 100;
    while (--timeout > 0) {
        reg = apic_read(APIC_ICR_LOW);
        if (!(reg & APIC_ICR_PENDING)) {
            return (true);
        }
    }
    return (false);
}

/*
** Waits for a couple of cpu clocks
*/
static void
micro_wait(void)
{
    uint64 tsc;

    tsc = rdtsc();
    while (rdtsc() < tsc + APIC_INIT_CLOCK_WAIT);
}

/*
** Initialize the local APIC.
*/
void
apic_init(void)
{
    uint64 msr;

    assert(apic);

    /* Enable Local APIC by setting spurious interrupt vector */
    apic_write(APIC_SIV, APIC_SVR_ENABLED | INT_APIC_SPURIOUS);

    /* Mask unused/unsupported interrupts */
    apic_write(APIC_LVT_LINT0, APIC_LVT_MASKED);
    apic_write(APIC_LVT_LINT1, APIC_LVT_MASKED);
    apic_write(APIC_LVT_PERFCOUNT, APIC_LVT_MASKED);

    /* Set up apic Timer */
    apic_write(APIC_TIMER_DCR, APIC_TIMER_X8);
    apic_write(APIC_LVT_TIMER, APIC_TIMER_PERIODIC | INT_APIC_TIMER);
    apic_write(APIC_TIMER_ICR, 10000000);

    /* Map error interrupt */
    apic_write(APIC_LVT_ERROR, INT_APIC_ERROR);

    /* IRQs */
    register_interrupt_handler(INT_APIC_TIMER, &apic_timer_ihandler);
    register_interrupt_handler(INT_APIC_ERROR, &apic_error_ihandler);
    register_interrupt_handler(INT_APIC_SPURIOUS, &apic_spurious_ihandler);

    /* Clear Error Status Register */
    apic_write(APIC_ESR, 0x0);
    apic_write(APIC_ESR, 0x0);

    /* Clear any outstanding interrupts */
    apic_write(APIC_EOI, 0x0);

    /* Clear task priority to enable all interrupts */
    apic_write(APIC_TPR, 0x0);

    /* Enable APIC in the MSR */
    msr = msr_read(MSR_IA32_APIC_BASE);
    msr |= (1 << 11u);
    msr_write(MSR_IA32_APIC_BASE, msr);
}

/*
** Start the AP with the given apic id and make it jump at the given address.
**
** The given parameter `ap` must NOT be locked.
*/
status_t
apic_start_ap(
    struct cpu *ap,
    uintptr addr
) {
    uint32_t apic_id;
    ushort *wrv;

    assert((addr & 0xFFF00FFF) == 0);

    /* Allocate stack for the new cpu */
    ap_boot_stack = kheap_alloc_aligned(PAGE_SIZE * KCONFIG_KERNEL_STACK_SIZE);
    if (ap_boot_stack == NULL) {
        return (ERR_OUT_OF_MEMORY);
    }

    spinrwlock_acquire_write(&ap->lock);

    ap->scheduler_stack = ap_boot_stack;
    ap_boot_stack = (uchar *)ap_boot_stack + 16 * PAGE_SIZE;
    ap->scheduler_stack_top = ap_boot_stack;

    apic_id = ap->apic_id;

    spinrwlock_release_write(&ap->lock);

    /*
    ** MP Specification says that we must initialize CMOS shutdown code to
    ** 0xA and the warm reset vector (DWORD based at 40:67) to point to the
    ** AP startup code before doing the universal startup algorithm.
    */
    io_out8(cmos, 0xF); // Offset 0xF is shutdown code
    io_out8(cmos_ret, 0x0A);

    /* Warm reset vector is at fixed address 0x40:0x67 */
    wrv = (ushort *)(0x40 << 4 | 0x67);
    wrv[1] = 0;
    wrv[0] = addr >> 4;

    /* Universal Startup Algorithm */

    apic_send_ipi(apic_id, APIC_ICR_INIT);
    assert(apic_ipi_acked());
    micro_wait();

    for (int i = 0; i < 2; ++i) {
        apic_send_ipi(apic_id, APIC_ICR_STARTUP | (addr >> 12));
        micro_wait();
    }
    return (OK);
}

/*
** Handler for the APIC timer interrupt
*/
void
apic_timer_ihandler(void)
{
    log(".");
    apic_eoi();
}

/*
** Handler for the APIC error interrupt
*/
void
apic_error_ihandler(void)
{
    panic("apic error encountered");
}

/*
** Handler for the APIC spurious interrupt
*/
void
apic_spurious_ihandler(void)
{
    panic("apic spurious interruption received");
}