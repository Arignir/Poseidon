/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "arch/x86_64/io.h"

#define CMOS_RESET_REGISTER 0xF

extern struct io_port g_cmos;
extern struct io_port g_cmos_ret;

void cmos_write(uint8_t reg, uint8_t value);
