/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
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

	/* Trigger all init hooks, panic if one failed. */
	hook = find_next_init_hook(NULL, __INIT_LEVEL_EARLIEST);
	while (hook != NULL) {
		if (hook->hook() != OK) {
			panic(
				"the init hook \"%s\" failed to complete successfully.",
				hook->name
			);
		}
		hook = find_next_init_hook(hook, hook->level);
	}

	logln("Hello Kernel World!");

	/* Start init here (WIP) */
}
