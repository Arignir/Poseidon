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
#include <poseidon/poseidon.h>
#include <poseidon/interrupt.h>
#include <poseidon/memory/pmm.h>
#include <poseidon/memory/kheap.h>
#include <lib/log.h>

static volatile uchar *apic = NULL;

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