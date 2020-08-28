/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture-independent API to read and write to peripheral devices.
**
** There are multiple ways architectures implements IO. This module is a generic
** implementation of memory-mapped IO.
**
** Some architectures may complement this implementation with extra functions.
**
** The PC platform also implements port-based IO, described in
** `arch/x86_64/api/io.h`.
**
** This module also implements an extra, generic, api that automatically
** redirects the requests to the platform's preffered IO technique's
** implementation.
*/

#ifndef _POSEIDON_IO_H_
# define _POSEIDON_IO_H_

# include <poseidon/poseidon.h>
# include <poseidon/memory/memory.h>

/*
** Strong typing the address of a memory-mapped io port.
**
** Note that `address` must be a virtual address.
*/
struct io_mm
{
    virtaddr_t address;
};

/*
** Define a new `static struct io_mm const` with the given name and address.
**
** This acts as a shortcut to reduce boilerplate.
*/
# define NEW_IO_MM(_name, _address)                 \
    static                                          \
    struct io_port const _name = {                  \
        .address = _address                         \
    }

/*
** Send a byte of data to the port of address `port`.
*/
static inline
void
io_mm_out8(
    struct io_mm port,
    uint8 data
) {
    *(uint8 *)port.address = data;
}

/*
** Send a byte of data to the port of address `port + offset`.
*/
static inline
void
io_mm_out8_offset(
    struct io_mm port,
    ushort offset,
    uint8 data
) {
    port.address = (uint8 *)port.address + offset;
    io_mm_out8(port, data);
}

/*
** Read a byte of data from the port of address `port`.
*/
static inline
uint8
io_mm_in8(
    struct io_mm port
) {
    return *(uint8 *)port.address;
}

/*
** Read a byte of data from the port of address `port + offset`.
*/
static inline
uint8
io_mm_in8_offset(
    struct io_mm port,
    ushort offset
) {
    port.address += offset;
    return io_in8(port);
}

/*
** The following is a generic API that redirects to the favorite IO technique
** for all platforms.
*/

# ifdef KCONFIG_PLATFORM_PC
#  define io_out8(...)              io_port_out8(__VA_ARGS__)
#  define io_out8_offset(...)       io_port_out8_offset(__VA_ARGS__)
#  define io_in8(...)               io_port_in8(__VA_ARGS__)
#  define io_in8_offset(...)        io_port_in8_offset(__VA_ARGS__)
# else
#  define io_out8(...)              io_mm_out8(__VA_ARGS__)
#  define io_out8_offset(...)       io_mm_out8_offset(__VA_ARGS__)
#  define io_in8(...)               io_mm_in8(__VA_ARGS__)
#  define io_in8_offset(...)        io_mm_in8_offset(__VA_ARGS__)
# endif /* KCONFIG_PLATFORM_PC */

#endif /* !_POSEIDON_IO_H_ */
