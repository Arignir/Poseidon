/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#include <lib/checksum.h>

uint8
checksum8(
    void const *start,
    size_t len
) {
    uint8 res;
    unsigned char const *c;
    unsigned char const *end;

    res = 0;
    c = start;
    end = c + len;
    while (c < end) {
        res += *c;
        ++c;
    }
    return (res);
}

uint16
checksum16(
    void const *start,
    size_t len
) {
    uint16 res;
    unsigned char const *c;
    unsigned char const *end;

    res = 0;
    c = start;
    end = c + len;
    while (c < end) {
        res += *c;
        ++c;
    }
    return (res);
}

uint32
checksum32(
    void const *start,
    size_t len
) {
    uint32 res;
    unsigned char const *c;
    unsigned char const *end;

    res = 0;
    c = start;
    end = c + len;
    while (c < end) {
        res += *c;
        ++c;
    }
    return (res);
}
