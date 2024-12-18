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
#define likely(x)           __builtin_expect((x), 1)
#define unlikely(x)         __builtin_expect((x), 0)

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
[[gnu::noreturn]] void panic(char const *fmt, ...);

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
                "assert(%s) failed (in %s at line %u).",    \
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
                "assert_ok(%s) failed: %s (in %s at line %u).",     \
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
** Function attribute to put the function in the boot text section,
** which will be freed at the end of kernel initialisation.
*/
#define boot_text                   gnu::section(".text.boot")

/*
** Function attribute to put the function in the boot data section,
** which will be freed at the end of kernel initialisation.
*/
#define boot_data                   gnu::section(".data.boot")

/*
** Function attribute to put the function in the boot rodata section,
** which will be freed at the end of kernel initialisation.
*/
#define boot_rodata                 gnu::section(".rodata.boot")

/*
** Pre-processor trick to concat a macro with an other macro or static text.
**
** Mostly used to forge a function name based on the target architecture.
*/
#define XCONCAT(a, b)               a##b
#define CONCAT(a, b)                XCONCAT(a, b)

/*
** Pre-processor trick to stringify a macro, usually used to debug a macro.
*/
#define XSTRINGIFY(a)               #a
#define STRINGIFY(a)                XSTRINGIFY(a)

/*
** Round up `x` to be `y`-aligned.
**
** `y` must be a power of two and the return value has the same type than `x`.
*/
#define ALIGN(x, y)                 ((typeof(x))(((uintptr)(x) + ((y) - 1)) & ~((y) - 1)))

/*
** Round down `x` to be `y`-aligned.
**
** `y` must be a power of two and the return value has the same type than `x`.
*/
#define ROUND_DOWN(x, y)            ((typeof(x))((uintptr)(x) & ~((y) - 1)))

/*
** Macro to access data using the properties of the `volatile` keyword.
*/

#define volatile_read(ptr)          (*(const volatile typeof(*ptr) *)(ptr))
#define volatile_read8(ptr)         (*(const volatile uint8_t *)(ptr))
#define volatile_read16(ptr)        (*(const volatile uint16_t *)(ptr))
#define volatile_read32(ptr)        (*(const volatile uint32_t *)(ptr))
#define volatile_read64(ptr)        (*(const volatile uint64_t *)(ptr))

#define volatile_write(ptr, val)    (*(volatile typeof(*ptr) *)(ptr) = (typeof(*ptr))(val))
#define volatile_write8(ptr, val)   (*(volatile uint8_t *)(ptr) = (uint8_t)(val))
#define volatile_write16(ptr, val)  (*(volatile uint16_t *)(ptr) = (uint16_t)(val))
#define volatile_write32(ptr, val)  (*(volatile uint32_t *)(ptr) = (uint32_t)(val))
#define volatile_write64(ptr, val)  (*(volatile uint64_t *)(ptr) = (uint64_t)(val))
