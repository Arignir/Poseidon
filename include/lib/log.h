/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _LIB_LOG_H_
# define _LIB_LOG_H_

# include <poseidon/poseidon.h>
# include <stdarg.h>

void log(char const *, ...);
void logln(char const *, ...);
void vlog(char const *, va_list);
void vlogln(char const *, va_list);

typedef void(*putsn_hook)(char const *buffer, size_t len);

struct logger {
    char const *name;
    putsn_hook print;
};

#define REGISTER_LOGGER(n, p)                           \
    __aligned(sizeof(void *))                           \
    __used                                              \
    __section("poseidon_logger")                        \
    static                                              \
    struct logger const _logger_struct_##n = {          \
        .name = #n,                                     \
        .print = (p),                                   \
    }

#endif /* !_LIB_LOG_H_ */
