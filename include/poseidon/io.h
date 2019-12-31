/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Architecture-independent API to read and write to peripheral devices.
**
** The IO system is made to prevent compilation if the kernel is provided with
** memory-mapped informations on a port-based architecture or vice-versa.
**
** `kconfig.h` provides a variable named `KCONFIG_ARCH_IO_METHOD` holding
** which method is used to talk to peripheral devices.
** The members of `enum io_method` are all possible values for this variable.
**
** You have a guaranteee that `KCONFIG_ARCH_IO_METHOD` will match one
** (and only one) of the values of `enum io_method`, but you must keep
** in mind that new values *may* appear one day.
*/

#ifndef _POSEIDON_IO_H_
# define _POSEIDON_IO_H_

# include <poseidon/poseidon.h>
# include <poseidon/kconfig.h>

/*
** An enumeration of all the techniques an architecture may use as its primary
** way to communicate with external devices.
*/
enum io_method
{
    IO_PORT_MAPPED          = 1, /* Port-mapped IO      */
    IO_MEMORY_MAPPED        = 2, /* Memory-mapped IO    */
};

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
    static                              			\
    struct io_port const _name = {                  \
        .port = _port                       		\
    }

#  include <arch/target/api/io.h>

/*
** Send a byte of data to the port of index `port`.
**
** The architecture-dependent equivalent of this function should have the
** following prototype:
**
** `void $ARCH_io_out8(struct io_port port, uint8 data)`
*/
static inline
void
io_out8(
    struct io_port port,
    uint8 data
) {
    ARCH_SYMBOL(io_out8)(port, data);
}

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
    ARCH_SYMBOL(io_out8)(port, data);
}

/*
** Reads a byte of data from the port of index `port`.
**
** The architecture-dependent equivalent of this function should have the
** following prototype:
**
** `uint8 $ARCH_io_in8(struct io_port port);`
*/
static inline
uint8
io_in8(
    struct io_port port
) {
    return ARCH_SYMBOL(io_in8)(port);
}

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
    return ARCH_SYMBOL(io_in8)(port);
}


# elif KCONFIG_ARCH_IO_TECHNIQUE == IO_MEMORY_MAPPED

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
    static                              			\
    struct io_port const _name = {                  \
        .address = _address                 		\
    }

#  include <arch/target/api/io.h>

/*
** Send a byte of data to the port of address `port`.
**
** The architecture-dependent equivalent of this function should have the
** following prototype:
**
** `void $ARCH_io_out8(struct io_mm port, uint8 data)`
*/
static inline
void
io_out8(
    struct io_mm port,
    uint8 data
) {
    ARCH_SYMBOL(io_out8)(port, data);
}

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
    ARCH_SYMBOL(io_out8)(port, data);
}

/*
** Reads a byte of data from the port of address `port`.
**
** The architecture-dependent equivalent of this function should have the
** following prototype:
**
** `uint8 $ARCH_io_in8(struct io_mm port);`
*/
static inline
uint8
io_in8(
    struct io_mm port
) {
    return ARCH_SYMBOL(io_in8)(port);
}

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
    return ARCH_SYMBOL(io_in8)(port);
}

# endif /* KCONFIG_ARCH_IO_TECHNIQUE */

/*
** Waits for the port to be ready to be used again.
**
** The architecture-dependent equivalent of this function should have the
** following prototype:
**
** `void $ARCH_io_delay(void);`
*/
static inline
void
io_delay(void)
{
    ARCH_SYMBOL(io_delay)();
}

#endif /* !_POSEIDON_IO_H_ */
