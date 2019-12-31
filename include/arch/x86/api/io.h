/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Implementation of `include/lib/io.h` for the x86 architecture.
*/

#ifndef _ARCH_X86_API_IO_H_
# define _ARCH_X86_API_IO_H_

/*
** Ensure this file isn't included on its own.
** The file `include/poseidon/io.h` should be included instead, which, in turn,
** includes this one.
*/
# ifndef _POSEIDON_IO_H_
#  error Please include `poseidon/io.h` instead of `arch/x86/api/io.h`.
# endif /* !_POSEIDON_IO_H_ */

# include <poseidon/poseidon.h>

static inline
void
x86_io_out8(
    struct io_port port,
    uint8 data
) {
    asm volatile (
        "out %0, %1"
        :
        : "a"(data), "d"(port.port)
        :
    );
}

static inline
uint8
x86_io_in8(struct io_port port)
{
    uchar data;

    asm volatile (
        "in %1, %0"
        : "=a"(data)
        : "d"(port.port)
        :
    );
    return (data);
}

static inline
void
x86_io_delay(void)
{
    asm volatile(
        "outb %%al, $0x80"
        :
        : "a"(0)
        :
    );
}

#endif /* !_ARCH_X86_API_IO_H_ */
