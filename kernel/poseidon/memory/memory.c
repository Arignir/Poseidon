/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Initialize the memory components of the kernel.
**
** Memory is an interesting and complex issue, especially for a kernel.
**
** It is splitted into two different modules: the physical memory manager (pmm)and the
** virtual memory manager (vmm). The first one manages physical frames while the
** second one maps virtual addresses to those physical addresses.
**
** Now here's the isue: the amount of memory the PMM will handle is unknown at
** compile time. Yet, we still need data structures that are proportionate (in size)
** to the amount of memory available.
**
** There are a couple of ways to handle this issue. The one used here is to
** make the PMM operate on a default area called the "boot heap", with a size
** known at compile time. We use that area to allocate the data structures
** needed to handle all the discovered physicial memory areas and then add
** those areas to the list of areas the PMM can pick from.
*/

#include <poseidon/boot/multiboot2.h>
#include <poseidon/boot/init_hook.h>
#include <poseidon/memory/memory.h>
#include <poseidon/memory/pmm.h>
#include <poseidon/memory/kheap.h>
#include <poseidon/status.h>
#include <lib/log.h>

static
status_t
memory_init(void)
{
    /*
    ** Initialize the physical memory manager using a reserved area of the binary
    ** (called the boot heap) as a free region of physical memory.
    */
    pmm_early_init();

    /*
    ** Initialize the algorithm used to manage the kernel's heap.
    */
    assert_ok(kheap_init());

    /*
    ** Load the multiboot segment and discover the different memory regions.
    */
    assert_ok(multiboot_load());

    /*
    ** Initialize the physical memory manager using dynamic memory
    ** depending on the different memory regions.
    **
    ** This also marks the kernel, boot heap and all area reserved by drivers as allocated.
    */
    assert_ok(pmm_init());

    /*
    ** Mark the multiboot segment as allocated
    **
    ** (We can't do that in `pmm_init()` since it's a dynamic address, unknown
    ** at compile-time)
    */
    pmm_mark_range_as_allocated(
        ROUND_DOWN(mb_tag_pa, PAGE_SIZE),
        ALIGN(mb_tag_pa + mb_tag_len, PAGE_SIZE)
    );

    logln("Dynamic memory allocator initialized.");

    return (OK);
}

REGISTER_INIT_HOOK(memory_init, &memory_init, INIT_LEVEL_MEMORY);
