/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/interrupt.h>
#include <poseidon/cpu/cpu.h>
#include <arch/x86_64/apic.h>
#include <arch/x86_64/interrupt.h>
#include <lib/log.h>
#include <stdarg.h>

/*
** Print an error message and halt the computer.
**
** This function never returns.
*/
void
panic(
    char const *fmt,
    ...
) {
    va_list va;

    /* Disable interruptions */
    disable_interrupts();

    /* Send an IPI to stop all other cores. */
    apic_send_ipi(
        0,
        INT_PANIC | APIC_ICR_FIXED | APIC_ICR_BROADCAST | APIC_ICR_LEVEL
    );

    apic_ipi_acked();

    va_start(va, fmt);

    logln("");
    log("Kernel panicked: ");
    vlogln(fmt, va);

    va_end(va);

    while (42) {
        halt();
    }
}
