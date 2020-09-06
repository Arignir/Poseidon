/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** An API to manipulate IO ports for the x86_64 architecture.
**
** The x86 uses (for historical reasons) a port-based IO mechanism, described
** in this module.
**
** It is meant to be used alongside the memory-mapped IO mechanism described
** in `poseidon/io.h`.
*/

#ifndef _ARCH_X86_64_IO_H_
# define _ARCH_X86_64_IO_H_

# include <poseidon/poseidon.h>

/*
** Strong typing the port number.
*/
struct io_port
{
    uint16 port;
};

/*
** Define a new `static struct io_port const` with the given name and port.
**
** This acts as a shortcut to reduce boilerplate.
*/
# define NEW_IO_PORT(_name, _port)                  \
    static                                          \
    struct io_port const _name = {                  \
        .port = _port                               \
    }

/*
** Send a byte of data to the port of index `port`.
*/
static inline
void
io_port_out8(
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

/*
** Send a byte of data to the port of index `port + offset`.
*/
static inline
void
io_port_out8_offset(
    struct io_port port,
    uint16 offset,
    uint8 data
) {
    port.port += offset;
    io_port_out8(port, data);
}

/*
** Read a byte of data to the port of index `port`.
*/
static inline
uint8
io_port_in8(
    struct io_port port
) {
    uchar data;

    asm volatile (
        "in %1, %0"
        : "=a"(data)
        : "d"(port.port)
        :
    );
    return (data);
}

/*
** Read a byte of data to the port of index `port + offset`.
*/
static inline
uint8
io_port_in8_offset(
    struct io_port port,
    uint16 offset
) {
    port.port += offset;
    return (io_port_in8(port));
}

/*
** Wait for the previous IO operations to complete.
*/
static inline
void
io_port_wait(void)
{
    asm volatile(
        "outb %%al, $0x80"
        :
        : "a"(0)
        :
    );
}

#endif /* !_ARCH_X86_64_IO_H_ */
