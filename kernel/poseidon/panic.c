/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/interrupt.h>
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

    disable_interrupts();

    va_start(va, fmt);

    logln("");
    log("Kernel panicked: ");
    vlogln(fmt, va);

    va_end(va);

    while (42) {
        halt();
    }
}
