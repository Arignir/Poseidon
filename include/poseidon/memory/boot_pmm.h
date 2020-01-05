/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _POSEIDON_MEMORY_BOOT_PMM_H_
# define _POSEIDON_MEMORY_BOOT_PMM_H_

# include <poseidon/memory.h>
# include <poseidon/poseidon.h>

status_t    boot_pmm_init(void);
physaddr_t  boot_pmm_alloc_frame(void);

#endif /* !_POSEIDON_MEMORY_BOOT_PMM_H_ */
