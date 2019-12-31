/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** A driver for UART8250.
**
** The driver is, for now, only used as a logger.
** When the kernel will be advanced enough, we will be able to add
** it as a character device.
*/

#include <poseidon/kconfig.h>
#include <poseidon/boot/init_hook.h>
#include <poseidon/io.h>
#include <lib/log.h>

/*
** The UART8250 driver is only implemented for port-based IO yet.
*/
#if KCONFIG_ARCH_IO_TECHNIQUE == IO_PORT
# define USE_UART8250

NEW_IO_PORT(uart8250_port, 0x3F8);

#endif

#ifdef USE_UART8250

static
int
is_transmit_empty(void)
{
    return (io_in8_offset(uart8250_port, 5) & 0x20);
}

static void
putsn(
    char const *s,
    size_t len
) {
    while (len) {
        while (is_transmit_empty() == 0);
        io_out8(uart8250_port, *s);
        ++s;
        --len;
    }
}

__boot_text
static
status_t
uart8250_init(void)
{
    /* TODO: Name the offset so they look a bit less like magic values */

    io_out8_offset(uart8250_port, 1, 0x00); // Disable all interrupts
    io_out8_offset(uart8250_port, 3, 0x80); // Enable DLAB (set baud rate divisor)
    io_out8_offset(uart8250_port, 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    io_out8_offset(uart8250_port, 1, 0x00); //                  (hi byte)
    io_out8_offset(uart8250_port, 3, 0x03); // 8 bits, no parity, one stop bit
    io_out8_offset(uart8250_port, 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    io_out8_offset(uart8250_port, 4, 0x0B); // IRQs enabled, RTS/DSR set

    return (OK);
}

REGISTER_LOGGER(uart8250, &putsn);
REGISTER_INIT_HOOK(uart8250, &uart8250_init, INIT_LEVEL_DRIVER_EARLY);

#endif /* USE_UART8250 */
