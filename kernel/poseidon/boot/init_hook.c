/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#include <poseidon/poseidon.h>
#include <poseidon/boot/init_hook.h>

/* Init hooks are located within the 'poseidon_init_hooks' section */
extern struct init_hook const __start_poseidon_init_hooks[] __weak;
extern struct init_hook const __stop_poseidon_init_hooks[] __weak;

/*
** Find the next uncalled init hook with an init level above or equal the given
** one.
*/
__boot_text
struct init_hook const *
find_next_init_hook(
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
        if (hook->level < level || (found && found->level <= hook->level)) {
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
