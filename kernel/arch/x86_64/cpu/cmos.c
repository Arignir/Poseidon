/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/io.h"

NEW_IO_PORT(cmos_reg, 0x70);
NEW_IO_PORT(cmos_data, 0x71);

void
cmos_write(
        uint8_t reg,
        uint8_t value
) {
        io_out8(cmos_reg, reg);
        io_out8(cmos_data, value);
}
