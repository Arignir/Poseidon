/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** A logging engine that writes incoming text to registered loggers.
*/

#include <poseidon/poseidon.h>
#include <lib/format.h>
#include <lib/log.h>
#include <stdarg.h>

extern struct logger const __start_poseidon_logger[] __weak;
extern struct logger const __stop_poseidon_logger[] __weak;

static
int
log_fmt_callback(
    char const *buffer,
    size_t len,
    void *tmp __unused
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

    format(fmt, va, &log_fmt_callback, NULL);

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

    format(fmt, va, &log_fmt_callback, NULL);
    log_fmt_callback("\n", 1, NULL);

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
    format(fmt, va, &log_fmt_callback, NULL);
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
    format(fmt, va, &log_fmt_callback, NULL);
    log_fmt_callback("\n", 1, NULL);
}
