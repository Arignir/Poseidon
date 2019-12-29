/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** The boot physical memory manager provides a way to allocate physical memory
** before the main physical memory manager is initialized.
**
** It is very basic and inefficient, but that's fine as it's not used for a long
** period of time.
**
** The boot allocator redistributes a sub-part of the whole physical memory
** while the physical memory allocator is being set up. When it is done, the
** boot allocator transfers the ownership of the physical memory to the new one.
** Future call to the boot allocator will `panic()`.
*/

#include <poseidon/memory/boot_pmm.h>
#include <poseidon/boot/multiboot2.h>
#include <poseidon/boot/init_hook.h>

/* The first frame of the boot-allocable physical address space. */
__boot_data
static physaddr_t boot_pmm_first_frame;

/* The next free frame of the boot-allocable physical address space. */
__boot_data
static physaddr_t boot_pmm_next_frame;

/* The last frame of the boot-allocable physical address space. */
__boot_data
static physaddr_t boot_pmm_last_frame;

/*
** Allocate a frame using the boot physical memory manager.
**
** Return either a physical address to the beginning of the new frame, or
** PHYS_NULL if the allocation failed.
*/
__boot_text
physaddr_t
boot_pmm_alloc_frame(void)
{
    // TODO: Panic when this function is called and the main memory
    // allocator is initialized.

    if (boot_pmm_next_frame < boot_pmm_last_frame) {
        physaddr_t res = boot_pmm_next_frame;
        boot_pmm_next_frame += PAGE_SIZE;
        return (res);
    } else {
        return (PHYS_NULL);
    }
}

/*
** Set up the boot physical memory manager.
**
** Called early as an init hook.
**
** Goes through the multiboot structure to find the biggest available physical
** memory region.
*/
__boot_text
status_t
boot_pmm_init(void)
{
    struct multiboot_mmap_entry const *entry;
    struct multiboot_mmap_entry const *end;
    uint64 len;

    if (!mb_mmap) {
        return (ERR_NO_MEMORY);
    }

    len = 0;
    entry = mb_mmap->entries;
    end = (struct multiboot_mmap_entry const *)(
        (uchar const *)mb_mmap + mb_mmap->size
    );

    boot_pmm_first_frame = PHYS_NULL;
    boot_pmm_last_frame = PHYS_NULL;
    boot_pmm_next_frame = PHYS_NULL;

    // Go through each physical mapping and find the largest free one
    while (entry < end) {
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && entry->len > len) {
            boot_pmm_first_frame = entry->addr;
            boot_pmm_last_frame = entry->addr + entry->len;
            len = entry->len;
        }
        entry = (struct multiboot_mmap_entry const *)(
            (uchar *)entry + mb_mmap->entry_size
        );
    }
    if (!boot_pmm_first_frame) {
        return (ERR_NO_MEMORY);
    }
    boot_pmm_next_frame = boot_pmm_first_frame;
    return (OK);
}

REGISTER_INIT_HOOK(boot_pmm, &boot_pmm_init, INIT_LEVEL_BOOT_KALLOC - 1);
