/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _POSEIDON_MEMORY_PMM_H_
# define _POSEIDON_MEMORY_PMM_H_

# include <poseidon/memory/memory.h>

/*
** A region of available physical memory.
*/
struct pmm_arena {
    physaddr_t start;   // Inclusive
    physaddr_t end;     // Exclusive
    uchar *bitmap;
    size_t bitmap_size;

    /*
    ** The following are intermediate variables to make next allocations faster
    */

    size_t free_frames; // Amount of frame free
    size_t next_frame;  // An index within `bitmap` that is susceptible to contain a free frame (heuristic)
};

physaddr_t pmm_alloc_frame(void);
void pmm_free_frame(physaddr_t);
status_t pmm_init(void);
void pmm_early_init(void);
void pmm_mark_range_as_allocated(physaddr_t, size_t);

/*
** A physical memory area marked as reserved for a specific purpose or device.
*/
struct pmm_reserved_area {
    char const *name;
    physaddr_t start;
    physaddr_t end;
};

/*
** Usage: `REGISTER_PMM_RESERVED_AREA(name, start, end)`
**
** A macro to easily create a new reserved physical memory area.
*/
# define REGISTER_PMM_RESERVED_AREA(n, s, e)                            \
    __aligned(sizeof(void *)) __used __section("pmm_reserved_area")     \
    static struct pmm_reserved_area const _pmm_reserved_area_##n = {    \
        .name = #n,                                                     \
        .start = (physaddr_t)(s),                                                   \
        .end = (physaddr_t)(e),                                                     \
    }

#endif /* !_POSEIDON_MEMORY_PMM_H_ */
