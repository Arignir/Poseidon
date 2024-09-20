/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "arch/x86_64/io.h"
#include "poseidon/boot/init_hook.h"
#include "poseidon/io.h"
#include "platform/pc/vga.h"
#include "lib/string.h"
#include "lib/log.h"

NEW_IO_PORT(vga_ctrl, 0x3D4);
NEW_IO_PORT(vga_data, 0x3D5);

static uint const VGA_WIDTH = 80u;
static uint const VGA_HEIGHT = 25u;

static struct vga vga = {
    .attrib = 0,
    .buffer = (uint16 *)0xB8000,
    .cursor_x = 0,
    .cursor_y = 0,
};

/*
** Sets the current color for next outputs
*/
void
vga_set_color(
    enum vga_color bg,
    enum vga_color fg
) {
    vga.attrib = ((bg << 4u) | (fg & 0x0F)) << 8u;
}

/*
** Moves the visual cursor to it's real coordinates.
*/
static
void
move_cursor(void)
{
    uint32 tmp;

    tmp = vga.cursor_y * VGA_WIDTH + vga.cursor_x;

    io_out8(vga_ctrl, 14);
    io_out8(vga_data, tmp >> 8);
    io_out8(vga_ctrl, 15);
    io_out8(vga_data, tmp);
}

/*
** Clears the screen
*/
void
vga_clear(void)
{
    uint i;
    uint16 blank;

    i = 0;
    blank = vga.attrib | 0x20;
    while (i < VGA_WIDTH)
    {
        *(vga.buffer + i) = blank;
        ++i;
    }
    i = 1;
    while (i < VGA_HEIGHT)
    {
        memcpy(vga.buffer + VGA_WIDTH * i, vga.buffer, sizeof(uint16) * VGA_WIDTH);
        ++i;
    }
    vga.cursor_x = 0;
    vga.cursor_y = 0;
    move_cursor();
}

/*
** Scrolls the text
*/
static
void
vga_scroll(void)
{
    uint16 blank;
    size_t i = 0;

    i = 0;
    blank = vga.attrib | 0x20;
    memmove(vga.buffer, vga.buffer + VGA_WIDTH, (VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(uint16));
    while (i < VGA_WIDTH)
    {
        *(vga.buffer + (VGA_HEIGHT - 1) * VGA_WIDTH + i) = blank;
        ++i;
    }
}

/*
** Prints a single character on the screen.
** Doesn't move the cursor
*/
static
int
vga_naked_putchar(
    int c
) {
    switch (c)
    {
    case '\n': /* new line */
        vga.cursor_x = 0;
        vga.cursor_y++;
        break;
    case '\t': /* tabulation */
        vga.cursor_x = (vga.cursor_x + 8u) & ~7u;
        break;
    case '\r': /* carriage return */
        vga.cursor_x = 0;
        break;
    case '\b': /* backspace */
        if (vga.cursor_x == 0) {
            vga.cursor_y -= (vga.cursor_y > 0);
            vga.cursor_x = VGA_WIDTH - 1;
        }
        else {
            vga.cursor_x -= (vga.cursor_x > 0);
        }
        break;
    default:
        *(vga.buffer + vga.cursor_y * VGA_WIDTH + vga.cursor_x) =
            vga.attrib | (uchar)c;
        vga.cursor_x += 1;
        break;
    }
    vga.cursor_y += (vga.cursor_x >= VGA_WIDTH);
    vga.cursor_x *= (vga.cursor_x < VGA_WIDTH);
    while (vga.cursor_y >= VGA_HEIGHT) {
        vga_scroll();
        vga.cursor_y -= 1;
    }
    return (1);
}

/*
** Prints a character on the screen
*/
int
vga_putchar(
    int c
) {
    int ret;

    ret = vga_naked_putchar(c);
    move_cursor();
    return (ret);
}

/*
** Prints an array of characters on the screen
*/
int
vga_puts(
    char const *str
) {
    char const *s;

    s = str;
    while (*str) {
        vga_naked_putchar(*str);
        ++str;
    }
    move_cursor();
    return (str - s);
}

/*
** Print a known-size array of characters on the screen
*/
int
vga_putsn(
    char const *str,
    size_t n
) {
    size_t old_n;

    old_n = n;
    while (n) {
        vga_naked_putchar(*str);
        ++str;
        --n;
    }
    move_cursor();
    return (old_n);
}

/*
** Wrap `vga_putsn` to make it logger-compliant.
*/
static
void
vga_log(
    char const *str,
    size_t n
) {
    vga_putsn(str, n);
}

/*
** Initialize the vga driver.
**
** Called early by the arch_XXX_setup() function for
** debugging
*/
status_t
vga_init(void)
{
    // Clear the screen
    vga_set_color(VGA_BLACK, VGA_WHITE);
    vga_clear();

    // Print welcome message
    vga_puts("VGA Enabled:");
    for (int i = 0; i <= 0xF; ++i) {
        vga_set_color(i, i);
        vga_putchar(' ');
    }

    // Prepare the next messages
    vga_set_color(VGA_BLACK, VGA_WHITE);
    vga_putchar('\n');

    return (OK);
}

REGISTER_LOGGER(vga, &vga_log);
REGISTER_INIT_HOOK(vga, &vga_init, INIT_LEVEL_BOOT_LOGGER);
