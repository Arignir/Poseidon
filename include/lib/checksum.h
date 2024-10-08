/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "poseidon/poseidon.h"

/*
** Functions to ease the calculation of checksums
*/

uint8 checksum8(void const *s, size_t l);
uint16 checksum16(void const *s, size_t l);
uint32 checksum32(void const *s, size_t l);
