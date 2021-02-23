/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _LIB_CHECKSUM_H_
# define _LIB_CHECKSUM_H_

# include <poseidon/poseidon.h>

/* Functions to ease the calculation of checksums */

uint8   checksum8(void const *s, size_t l);
uint16  checksum16(void const *s, size_t l);
uint32  checksum32(void const *s, size_t l);

#endif /* !_LIB_CHECKSUM_H_ */
