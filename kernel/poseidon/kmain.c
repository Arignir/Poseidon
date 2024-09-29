/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
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

#include "arch/x86_64/api/cpu.h"
#include "poseidon/boot/init_hook.h"
#include "poseidon/cpu/cpu.h"
#include "poseidon/memory/pmm.h"
#include "poseidon/interrupt.h"
#include "poseidon/thread/thread.h"
#include "poseidon/scheduler/scheduler.h"
#include "lib/log.h"

int
thread_test(
    void
) {
    while (42) {
        log("%zu", current_cpu()->cpu_id);
        halt();
    }
}

/*
** Trigger all init hooks and execute `init`.
**
** If an init hook doesn't return `OK`, the boot process is aborted.
**
** Poseidon's architecture-independent entry point.
*/
void
kmain(
    void
) {
    struct init_hook const *hook;
    status_t s;
    struct thread *kthread1;
    struct thread *kthread2;
    struct thread *kthread3;
    struct thread *kthread4;

    logln("Poseidon is initializing...");
    logln("");

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

    // Create `kthread`.
    assert_ok(thread_new(thread_test, &kthread1));
    assert_ok(thread_new(thread_test, &kthread2));
    assert_ok(thread_new(thread_test, &kthread3));
    assert_ok(thread_new(thread_test, &kthread4));

    logln("");
    logln("Poseidon finished its initialization!");
    logln("");

    /* Start init here (WIP) */

    yield();

    panic("`kmain()` returned from `yield()`.");
}

REGISTER_PMM_RESERVED_AREA(kernel, kernel_start, kernel_end);
