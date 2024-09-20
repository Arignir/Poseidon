/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include <stddef.h>
#include <stdarg.h>

/*
** The type of a `format()` output callback.
*/
typedef int(*printf_output_func)(char const *, size_t, void *);

int format(char const *fmt, va_list va, printf_output_func out, void *params);
