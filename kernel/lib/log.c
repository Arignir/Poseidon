/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** A logging engine that writes incoming text to registered loggers.
*/

#include "poseidon/poseidon.h"
#include "lib/sync/spinlock.h"
#include "lib/format.h"
#include "lib/log.h"
#include <stdarg.h>

// Ensure logs aren't scrambled because two cores want to log at the same time
static struct spinlock g_log_lock = SPINLOCK_DEFAULT;

[[gnu::weak]] extern struct logger const __start_poseidon_logger[];
[[gnu::weak]] extern struct logger const __stop_poseidon_logger[];

static
int
log_fmt_callback(
    char const *buffer,
    size_t len,
    void *tmp [[maybe_unused]]
) {
    struct logger const *logger;

    logger = __start_poseidon_logger;
    while (logger < __stop_poseidon_logger) {
        logger->print(buffer, len);
        ++logger;
    }
    return (len);
}

/*
** Print the given message to registered loggers.
*/
void
log(
    char const *fmt,
    ...
) {
    va_list va;
    va_start(va, fmt);

    spinlock_acquire(&g_log_lock);
    format(fmt, va, &log_fmt_callback, NULL);
    spinlock_release(&g_log_lock);

    va_end(va);
}

/*
** Print the given message to registered loggers, followed by a new line.
*/
void
logln(
    char const *fmt,
    ...
) {
    va_list va;
    va_start(va, fmt);

    spinlock_acquire(&g_log_lock);
    format(fmt, va, &log_fmt_callback, NULL);
    log_fmt_callback("\n", 1, NULL);
    spinlock_release(&g_log_lock);

    va_end(va);
}

/*
** Print the given message to registered loggers, except it uses a `va_list`
** instead of a variable number of arguments.
*/
void
vlog(
    char const *fmt,
    va_list va
) {
    spinlock_acquire(&g_log_lock);
    format(fmt, va, &log_fmt_callback, NULL);
    spinlock_release(&g_log_lock);
}

/*
** Print the given message to registered loggers, followed by a new line,
** except it uses a `va_list` instead of a variable number of arguments.
*/
void
vlogln(
    char const *fmt,
    va_list va
) {
    spinlock_acquire(&g_log_lock);
    format(fmt, va, &log_fmt_callback, NULL);
    log_fmt_callback("\n", 1, NULL);
    spinlock_release(&g_log_lock);
}
