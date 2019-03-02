/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Common definition and macro used everywhere in the kernel's code.
*/

#ifndef _POSEIDON_POSEIDON_H_
# define _POSEIDON_POSEIDON_H_

# include <stddef.h>
# include <stdint.h>
# include <stdbool.h>

/*
** A useful set of macros that act like keywords that are not available
** otherwise in C11.
*/
# define inline			__inline
# define asm			__asm__
# define restrict		__restrict
# define __pure			__attribute__((pure))
# define __const		__attribute__((const))
# define __cold			__attribute__((cold))
# define __hot			__attribute__((hot))
# define __used			__attribute__((used))
# define __unused		__attribute__((unused))
# define __packed		__attribute__((packed))
# define __weak			__attribute__((weak))
# define __aligned(x)		__attribute__((aligned (x)))
# define __section(s)		__attribute__((section (s)))
# define __noreturn		__attribute__((noreturn))
# define likely(x)		__builtin_expect((x), 1)
# define unlikely(x)		__builtin_expect((x), 0)
# define __optimize(x)		__attribute__((optimize(x)))

/* Defines some shortcuts types. */
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef unsigned long long	ulonglong;
typedef intmax_t		intmax;
typedef uintmax_t		uintmax;

/* Defines some shortcuts for sized types. */
typedef int8_t			int8;
typedef int16_t			int16;
typedef int32_t			int32;
typedef int64_t			int64;
typedef intptr_t		intptr;
typedef intptr_t		ssize_t;

typedef uint8_t			uint8;
typedef uint16_t		uint16;
typedef uint32_t		uint32;
typedef uint64_t		uint64;
typedef uintptr_t		uintptr;

/* Print a message and halt the computer. */
void				panic(char const *fmt, ...) __noreturn;

/* Panics if the given constant expression evaluates to false */
# define static_assert(e)							\
	_Static_assert(								\
		e,								\
		"(" #e ") evaluated to false"					\
	)

/* Panics if the given expression evaluates to false */
# define assert(expr)								\
	do {									\
		if (unlikely(!(expr))) {					\
			panic("assert(%s) failed (in %s at line %u).\n",	\
				#expr, __FILE__, __LINE__);			\
		}								\
	}									\
	while (0)

/*
** Place the following data in the ".data.boot" section, which will be freed
** at the end of kernel initialisation.
*/
# define __boot_data		__section(".data.boot")

/*
** Place the following constant data in the ".rodata.boot" section, which will
** be freed at the end of kernel initialisation.
*/
# define __boot_rodata		__section(".rodata.boot")

/*
** Place the following data in the ".text.boot" section, which will be freed
** at the end of kernel initialisation.
*/
# define __boot_text		__section(".text.boot")

/*
** Pre-processor trick to concat a macro with an other macro or static text.
**
** Mostly used to forge function names based on current architecture.
*/
# define XCONCAT(a, b)		a##b
# define CONCAT(a, b)		XCONCAT(a, b)

/*
** Pre-processor trick to stringify a macro, mostly used to debug macros.
*/
# define XSTRINGIFY(a)		#a
# define STRINGIFY(a)		XSTRINGIFY(a)

#endif /* !_POSEIDON_POSEIDON_H_ */
