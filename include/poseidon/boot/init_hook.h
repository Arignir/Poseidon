/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** An init hook is a function called at a specific point in the boot process.
**
** Such function can be declared pretty much anywhere in the kernel: in a driver,
** in a library etc.
**
** Those functions are registered in an array of `struct init_hook` which is
** stored in a special section of the kernel so `kmain()` can retrieve all of
** them easily.
**
** To register a new init hook, use the macro
** `NEW_INIT_HOOK("name", &func, DESIRED_INIT_LEVEL)`, do *not* add it by hand
** within the section. The macro will do it for you.
*/

#ifndef _POSEIDON_BOOT_INIT_HOOK_H_
# define _POSEIDON_BOOT_INIT_HOOk_H_

# include <poseidon/poseidon.h>
# include <poseidon/status.h>

/*
** The different init levels that an init hook can suscribe to.
*/
enum init_level
{
	/* An init hook should *not* suscribe to __INIT_LEVEL_EARLIEST */
	__INIT_LEVEL_EARLIEST		= 0x1,

	INIT_LEVEL_ARCH_EARLY,
	INIT_LEVEL_DRIVER_EARLY,

	INIT_LEVEL_MULTIBOOT,

	INIT_LEVEL_PMM,
	INIT_LEVEL_VMM,

	INIT_LEVEL_ARCH,
	INIT_LEVEL_FILESYSTEM,
	INIT_LEVEL_DRIVERS,

	/* An init hook should *not* suscribe to __INIT_LEVEL_LATEST */
	__INIT_LEVEL_LATEST,
};

/*
** All init hooks must match the following prototype.
*/
typedef status_t (*init_hook_funcptr)(void);

struct init_hook
{
	char const *name;
	enum init_level level;
	init_hook_funcptr hook;
};

/*
** Usage: `REGISTER_INIT_HOOK("name", &func, DESIRED_INIT_LEVEL)`
**
** Registers an init hook given its name, an address to call and an init level,
** and stores it in the reserved section of the binary dedicated to init hook.
*/
# define REGISTER_INIT_HOOK(n, h, l)					\
	__aligned(sizeof(void *)) __used __section("poseidon_init_hooks")\
	static struct init_hook const _init_hook_struct_##n = {		\
		.name = #n,						\
		.level = (l),						\
		.hook = (h),						\
	}

struct init_hook const *	find_next_init_hook();

#endif /* !_POSEIDON_BOOT_INIT_HOOK_H_ */
