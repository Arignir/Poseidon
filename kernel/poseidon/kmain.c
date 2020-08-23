/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** The common entry point for all architecture.
**
** `kmain()` is called shortly after an architecture-dependent boot sequence and
** will call all registered init hooks.
**
** As the hooks are activated, the cpu will progress in its initialization until
** reaching the final and sufficient state to start 'init'.
*/

#include <poseidon/boot/init_hook.h>
#include <poseidon/memory/pmm.h>
#include <lib/log.h>

/*
** Trigger all init hooks and execute `init`.
**
** If an init hook doesn't return `OK`, the boot process is aborted.
**
** Poseidon's architecture-independent entry point.
*/
void
kmain(void)
{
    struct init_hook const *hook;
    status_t s;

    logln("Poseidon is initializing...");

    /* Trigger all init hooks, panic if one failed. */
    hook = find_next_init_hook(NULL, __INIT_LEVEL_EARLIEST);
    while (hook != NULL) {
        s = hook->hook();
        if (s != OK) {
            panic(
                "init hook \"%s\" failed: %s",
                hook->name,
                status_str[s]
            );
        }
        hook = find_next_init_hook(hook, hook->level);
    }

    logln("Poseidon finished its initialization.");

    /* Start init here (WIP) */
}

REGISTER_PMM_RESERVED_AREA(kernel, kernel_start, kernel_end);
