/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** A driver for UART8250.
**
** The driver is, for now, only used as a logger.
** When the kernel will be advanced enough, this driver should be a character
** devince.
*/

#include "poseidon/poseidon.h"
#include "poseidon/io.h"

/*
** The UART8250 driver is only implemented for the PC platform yet.
*/
#ifdef KCONFIG_PLATFORM_PC
#define USE_UART8250

#include "arch/target/io.h"

NEW_IO_PORT(io_uart8250, 0x3F8);

#endif

#ifdef USE_UART8250

#include "poseidon/boot/init_hook.h"
#include "lib/log.h"

static
int
is_transmit_empty(
    void
) {
    return io_in8_offset(io_uart8250, 5) & 0x20;
}

static
void
putsn(
    char const *s,
    size_t len
) {
    while (len) {
        while (is_transmit_empty() == 0);
        io_out8(io_uart8250, *s);
        ++s;
        --len;
    }
}

[[boot_text]]
static
status_t
uart8250_init(
    void
) {
    /* TODO: Name the offset so they look a bit less like magic values */

    io_out8_offset(io_uart8250, 1, 0x00); // Disable all interrupts
    io_out8_offset(io_uart8250, 3, 0x80); // Enable DLAB (set baud rate divisor)
    io_out8_offset(io_uart8250, 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    io_out8_offset(io_uart8250, 1, 0x00); //                  (hi byte)
    io_out8_offset(io_uart8250, 3, 0x03); // 8 bits, no parity, one stop bit
    io_out8_offset(io_uart8250, 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    io_out8_offset(io_uart8250, 4, 0x0B); // IRQs enabled, RTS/DSR set

    return OK;
}

REGISTER_LOGGER(uart8250, &putsn);
REGISTER_INIT_HOOK(uart8250, &uart8250_init, INIT_LEVEL_BOOT_LOGGER);

#endif /* USE_UART8250 */
