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
** The IO system is made to prevent compilation if the kernel is provided with
** memory-mapped informations on a port-based architecture or vice-versa.
**
** `kconfig.h` provides a variable named `KCONFIG_ARCH_IO_METHOD` telling
** which method is used to talk to peripheral devices.
** It must match one of the  `IO_*` constants.
**
** You have a guaranteee that `KCONFIG_ARCH_IO_METHOD` will match one (and only
** one) of those constants, but you must keep in mind that new constants *may*
** appear one day.
*/

#ifndef _POSEIDON_IO_H_
# define _POSEIDON_IO_H_

# include <poseidon/poseidon.h>
# include <poseidon/kconfig.h>

/*
** An enumeration of all the techniques an architecture may use as its primary
** way to communicate with external devices.
*/
# define IO_PORT_MAPPED         1 /* Port-mapped IO     */
# define IO_MEMORY_MAPPED       2 /* Memory-Mapped IO   */

# if KCONFIG_ARCH_IO_METHOD == IO_PORT_MAPPED

/*
** Strong typing the port number.
*/
struct io_port
{
    ushort port;
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

#  include <arch/target/api/io.h>

/*
** Send a byte of data to the port of index `port`.
**
** Architectures must implement this function.
*/
static inline void    io_out8(struct io_port port, uint8 data) __arch_alias(io_out8);

/*
** Send a byte of data to the port of index `port + offset`.
*/
static inline
void
io_out8_offset(
    struct io_port port,
    ushort offset,
    uint8 data
) {
    port.port += offset;
    io_out8(port, data);
}

/*
** Reads a byte of data from the port of index `port`.
**
** Architectures must implement this function.
*/
static inline uint8   io_in8(struct io_port port) __arch_alias(io_in8);

/*
** Reads a byte of data from the port of index `port + offset`.
*/
static inline
uint8
io_in8_offset(
    struct io_port port,
    ushort offset
) {
    port.port += offset;
    return io_in8(port);
}

# elif KCONFIG_ARCH_IO_METHOD == IO_MEMORY_MAPPED

/*
** Strong typing the address of a memory-mapped io port.
*/
struct io_mm
{
    uintptr address;
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

#  include <arch/target/api/io.h>

/*
** Send a byte of data to the port of address `port`.
*/
static inline void      io_out8(struct io_mm port, uint8 data) __arch_alias(io_out8);

/*
** Send a byte of data to the port of address `port + offset`.
*/
static inline
void
io_out8_offset(
    struct io_mm port,
    ushort offset,
    uint8 data
) {
    port.address += offset;
    io_out8(port, data);
}

/*
** Reads a byte of data from the port of address `port`.
**
** The architecture-dependent equivalent of this function should have the
** following prototype:
**
** `uint8 $ARCH_io_in8(struct io_mm port);`
*/
static inline uint8     io_in8(struct io_mm port) __arch_alias(io_in8);

/*
** Reads a byte of data from the port of address `port + offset`.
*/
static inline
uint8
io_in8_offset(
    struct io_mm port,
    ushort offset
) {
    port.address += offset;
    return io_in8(port);
}

# endif /* KCONFIG_ARCH_IO_TECHNIQUE */

/*
** Waits for the port to be ready to be used again.
**
** Architectures must implement this function.
*/
static inline void  io_delay(void) __arch_alias(io_delay);

#endif /* !_POSEIDON_IO_H_ */
