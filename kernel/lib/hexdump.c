/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "lib/hexdump.h"
#include "lib/log.h"

static
void
hexdump_char(
    uchar const *start,
    size_t len
) {
    size_t i;

    i = 0;
    while (i < len) {
        log("%02x ", start[i]);
        ++i;
    }

    while (i < HEXDUMP_WIDTH) {
        log("   ");
        ++i;
    }
}

static
void
hexdump_str(
    uchar const *start,
    size_t len
) {
    size_t i;
    char c;

    i = 0;
    while (i < len) {
        c = start[i];
        if (c >= ' ' && c <= '~') {     // is_printable()
            log("%c", c);
        } else {
            log(".");
        }
        ++i;
    }

    while (i < HEXDUMP_WIDTH) {
        log(" ");
        ++i;
    }
}

/*
** Dumps the given memory
*/
void
hexdump(
    uchar const *start,
    size_t size
) {
    uchar const *end;
    size_t len;

    end = start + size;
    while (start < end)
    {
        len = end - start;
        if (len >= HEXDUMP_WIDTH) {
            len = HEXDUMP_WIDTH;
        }

        log("%p: ", start);
        hexdump_char(start, len);
        log("|");
        hexdump_str(start, len);
        logln("|");

        start += HEXDUMP_WIDTH;
    }
}
