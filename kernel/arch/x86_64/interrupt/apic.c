/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "arch/x86_64/apic.h"
#include "arch/x86_64/interrupt.h"
#include "arch/x86_64/msr.h"
#include "arch/x86_64/memory.h"
#include "poseidon/poseidon.h"
#include "poseidon/interrupt.h"
#include "poseidon/thread/thread.h"
#include "poseidon/scheduler/scheduler.h"
#include "poseidon/memory/pmm.h"
#include "poseidon/memory/kheap.h"

static volatile uchar *g_apic = NULL;

/*
** Write to a local APIC register
*/
static inline
void
apic_write(
    enum apic_reg reg,
    uint32 value
) {
    *((volatile uint32 *)(g_apic + reg)) = value;
}

/*
** Read a local APIC register
*/
static inline
uint32
apic_read(
    enum apic_reg reg
) {
    return *((volatile uint32 *)(g_apic + reg));
}

/*
** Return the id of the current processor's local APIC.
*/
uint32
apic_get_id(
    void
) {
    return apic_read(APIC_ID) >> 24u;
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
    g_apic = kheap_alloc_device(
        pa,
        PAGE_SIZE
    );

    assert(g_apic);
}

/*
** Issue an end-of-interrupt signal, allowing the apic to process the next
** interrupt.
*/
void
apic_eoi(
    void
) {
    apic_write(APIC_EOI, 0x0);
}

/*
** Send the given IPI to the given local APIC.
*/
void
apic_send_ipi(
    uint32 apic_id,
    uint32 flags
) {
    apic_write(APIC_ICR_HIGH, apic_id << 24u);
    apic_write(APIC_ICR_LOW, flags);
}

/*
** Poll the delivery status bit untill the latest IPI is acknowledged
** by the destination core, or it timesout.
**
** FIXME: We should use fine-grained timeout
*/
bool
apic_ipi_acked(
    void
) {
    size_t timeout;
    uint32 reg;

    timeout = 100;
    while (--timeout > 0) {
        reg = apic_read(APIC_ICR_LOW);
        if (!(reg & APIC_ICR_PENDING)) {
            return true;
        }
    }
    return false;
}

/*
** Initialize the local APIC.
*/
void
apic_init(
    void
) {
    uint64 msr;

    assert(g_apic);

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
    register_interrupt_handler(INT_APIC_TIMER, (interrupt_handler_t)&apic_timer_ihandler);
    register_interrupt_handler(INT_APIC_ERROR, (interrupt_handler_t)&apic_error_ihandler);
    register_interrupt_handler(INT_APIC_SPURIOUS, (interrupt_handler_t)&apic_spurious_ihandler);
    register_interrupt_handler(INT_PANIC, (interrupt_handler_t)&apic_panic_ihandler);
    register_interrupt_handler(INT_TLB, (interrupt_handler_t)&apic_tlb_ihandler);

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
apic_timer_ihandler(
    void
) {
    bool in_thread;

    in_thread = (current_thread());
    apic_eoi();
    if (in_thread) {
        yield();
    }
}

/*
** Handler for the APIC error interrupt
*/
void
apic_error_ihandler(
    void
) {
    panic("apic error encountered");
}

/*
** Handler for the APIC spurious interrupt
*/
void
apic_spurious_ihandler(
    void
) {
    panic("apic spurious interruption received");
}

/*
** Handler for the panic IPI
**
** This IPI is fired by a processor when panicking, asking
** all the others to halt indefinitely.
*/
void
apic_panic_ihandler(
    void
) {
    disable_interrupts();
    while (42) {
        halt();
    }
}

/*
** Handler for the tlb shootdown IPI
**
** When a core modifies the paging structure and invalidates the TLB it must
** notify the other cores to invalidate their TLB too.
**
** Soooo, some people write some nice and efficient algorithms to handle this shit,
** but eeeh, we'll go for the quick & dirty way instead :D
*/
void
apic_tlb_ihandler(
    void
) {
    asm volatile(
        "invlpg (%%rax)"
        :
        : "a"(g_tlb_shootdown_target)
        :
    );
    apic_eoi();
}
