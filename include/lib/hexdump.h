/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _LIB_HEXDUMP_H_
# define _LIB_HEXDUMP_H_

# include <poseidon/poseidon.h>

# define HEXDUMP_WIDTH        16

void    hexdump(uchar const *start, size_t size);

#endif /* !_LIB_HEXDUMP_H_ */
