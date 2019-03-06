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

/* Init hooks are located within the 'poseidon_init_hooks' section */
extern struct init_hook const __start_poseidon_init_hooks[] __weak;
extern struct init_hook const __stop_poseidon_init_hooks[] __weak;

/*
** Find the next uncalled hook which init level is above or equal the given one.
*/
__boot_text
static
struct init_hook const *
find_next_hook(
	struct init_hook const *last,
	enum init_level level
) {
	struct init_hook const *hook;
	struct init_hook const *found;
	bool seen_last;

	found = NULL;
	seen_last = false;
	for (hook = __start_poseidon_init_hooks; hook < __stop_poseidon_init_hooks; ++hook)
	{
		seen_last |= (hook == last);
		if (hook->level < level
			|| (found && found->level <= hook->level)) {
			continue;
		}
		if (hook->level > level) {
			found = hook;
			continue;
		}
		if (hook->level == level && seen_last && hook != last) {
			found = hook;
			break;
		}
	}
	return (found);
}

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
	hook = find_next_hook(NULL, __INIT_LEVEL_EARLIEST);
	while (hook != NULL) {
		if (hook->hook() != OK) {
			panic(
				"the init hook \"%s\" failed to complete successfully.",
				hook->name
			);
		}
		hook = find_next_hook(hook, hook->level);
	}

	logln("Hello Kernel World!");

	/* Start init here (WIP) */
}
