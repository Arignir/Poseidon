/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Common definition and macro used everywhere in the kernel's code.
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "poseidon/status.h"
#include "poseidon/kconfig.h"

/*
** A useful set of macros that act like keywords that are not available
** otherwise in C11.
*/
#define inline              __inline
#define asm                 __asm__
#define restrict            __restrict
#define __always_inline     __attribute__((always_inline))
#define __pure              __attribute__((pure))
#define __const             __attribute__((const))
#define __cold              __attribute__((cold))
#define __hot               __attribute__((hot))
#define __used              __attribute__((used))
#define __unused            __attribute__((unused))
#define __packed            __attribute__((packed))
#define __weak              __attribute__((weak))
#define __weakref(x)        __attribute__((weakref(x)))
#define __alias(x)          __attribute__((alias(x)))
#define __aligned(x)        __attribute__((aligned(x)))
#define __section(s)        __attribute__((section(s)))
#define __noreturn          __attribute__((noreturn))
#define likely(x)           __builtin_expect((x), 1)
#define unlikely(x)         __builtin_expect((x), 0)
#define __optimize(x)       __attribute__((optimize(x)))

// Defines some shortcuts types.
typedef unsigned char       uchar;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;
typedef unsigned long long  ulonglong;
typedef intmax_t            intmax;
typedef uintmax_t           uintmax;

// Defines some shortcuts for sized types.
typedef int8_t              int8;
typedef int16_t             int16;
typedef int32_t             int32;
typedef int64_t             int64;
typedef intptr_t            intptr;

typedef uint8_t             uint8;
typedef uint16_t            uint16;
typedef uint32_t            uint32;
typedef uint64_t            uint64;
typedef uintptr_t           uintptr;

// Return the length of a statically allocated array
#define ARRAY_LENGTH(x)     (sizeof(x) / sizeof(x[0]))

/* Print a message and halt the computer. */
void panic(char const *fmt, ...) __noreturn;

// Panic saying the current function isn't implemented
#define unimplemented()                             \
    panic(                                          \
        "The function %s() was called but is not implemented (%s, line %u).", \
        __func__,                                   \
        __FILE__,                                   \
        __LINE__                                    \
    )

// Panic if the given constant expression evaluates to false
#define static_assert(e)                                            \
    _Static_assert(                                                 \
        e,                                                          \
        "(" #e ") evaluated to false (in " __FILE__ " at line)"     \
    )

// Panic if the given expression evaluates to `false`
#define assert(expr)                                        \
    do {                                                    \
        if (unlikely(!(expr))) {                            \
            panic(                                          \
                "assert(%s) failed (in %s at line %u).\n",  \
                #expr,                                      \
                __FILE__,                                   \
                __LINE__                                    \
            );                                              \
        }                                                   \
    }                                                       \
    while (0)

// Panic if the given expression does not evaluate to `OK`
#define assert_ok(expr)                                             \
    do {                                                            \
        status_t __s = (expr);                                      \
        if (unlikely(__s != OK)) {                                  \
            panic(                                                  \
                "assert_ok(%s) failed: %s (in %s at line %u).\n",   \
                #expr,                                              \
                status_str[__s],                                    \
                __FILE__,                                           \
                __LINE__                                            \
            );                                                      \
        }                                                           \
    }                                                               \
    while (0)

// Assertions that are only effective in debug mode
#if DEBUG
#define debug_assert(expr)          assert(expr)
#define debug_assert_ok(expr)       assert_ok(expr)
#else
#define debug_assert(expr)
#define debug_assert_ok(expr)
#endif /* DEBUG */

/*
** Put the following data in the ".data.boot" section, which will be freed
** at the end of kernel initialisation.
*/
#define __boot_data         __section(".data.boot")

/*
** Put the following constant data in the ".rodata.boot" section, which will
** be freed at the end of kernel initialisation.
*/
#define __boot_rodata       __section(".rodata.boot")

/*
** Put the following data in the ".text.boot" section, which will be freed
** at the end of kernel initialisation.
*/
#define __boot_text         __section(".text.boot")

/*
** Pre-processor trick to concat a macro with an other macro or static text.
**
** Mostly used to forge a function name based on the target architecture.
*/
#define XCONCAT(a, b)       a##b
#define CONCAT(a, b)        XCONCAT(a, b)

/*
** Pre-processor trick to stringify a macro, usually used to debug a macro.
*/
#define XSTRINGIFY(a)       #a
#define STRINGIFY(a)        XSTRINGIFY(a)

/*
** Round up `x` to be `y`-aligned.
**
** `y` must be a power of two and the return value has the same type than `x`.
*/
#define ALIGN(x, y)         ((typeof(x))(((uintptr)(x) + ((y) - 1)) & ~((y) - 1)))

/*
** Round down `x` to be `y`-aligned.
**
** `y` must be a power of two and the return value has the same type than `x`.
*/
#define ROUND_DOWN(x, y)    ((typeof(x))((uintptr)(x) & ~((y) - 1)))

#define volatile_read(x)    (*(const volatile typeof(x) *)&(x))
#define volatile_write(x)   (*(volatile typeof(x) *)&(x) = (val))
