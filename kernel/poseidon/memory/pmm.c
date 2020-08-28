/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** The Physical Memory Manager.
**
** The current implementation is a pretty naive bitmap and can definitely be
** optimized.
**
** It works with multiple chunks of continuous memory called arenas.
** Each arena is represented as a bitmap, where a single bit represents the
** availability of a frame (1 for taken, 0 for free).
*/

#include <poseidon/boot/init_hook.h>
#include <poseidon/boot/multiboot2.h>
#include <poseidon/memory/memory.h>
#include <poseidon/memory/kheap.h>
#include <poseidon/memory/pmm.h>
#include <lib/string.h>

/*
** The arena used early during the boot process, before the PMM can fully be initialized.
**
** See `poseidon/memory/memory.c` for a full explanation about why that arena
** is needed.
**
** We put its content in an extra section so te linker script can put it
** after the kernel, therefore making sure it doesn't take any virtual memory.
*/
static __boot_data struct pmm_arena boot_arena;
static __boot_data uint8 boot_arena_bitmap[KCONFIG_BOOT_KHEAP_SIZE / (4096 * 8)];
static __section(".kernel_boot_heap") uint8 kernel_boot_heap[KCONFIG_BOOT_KHEAP_SIZE];

/* A few shortcuts, to make operations more verbose. */
static __boot_data uint8 *kernel_boot_heap_start = kernel_boot_heap;
static __boot_data uint8 *kernel_boot_heap_end = (uint8 *)kernel_boot_heap + sizeof(kernel_boot_heap);

/* An array of physical memory regions. */
static struct pmm_arena *arenas = NULL;
static size_t arenas_len = 0;

/* The beginning and end of the `pmm_reserved_area` ELF section. */
extern struct pmm_reserved_area const __start_pmm_reserved_area[];
extern struct pmm_reserved_area const __stop_pmm_reserved_area[];

/*
** Calculate the index within `arena->bitmap` of the given frame.
**
** This function assumes `frame` belongs to `arena`.
*/
static inline
size_t
pmm_get_frame_idx(
    struct pmm_arena *arena,
    physaddr_t frame
) {
    return (((frame - arena->start) >> 12u) / 8u);
}

/*
** Calculate the mask needed to retrieve the status bit of the given frame.
**
** This function assumes `frame` belongs to `arena`.
*/
static inline
uint
pmm_get_frame_mask(
    struct pmm_arena *arena,
    physaddr_t frame
) {
    return (1u << (((frame - arena->start) >> 12u) % 8u));
}

/*
** Test if `frame` is marked as allocated.
**
** This function assumes `frame` belongs to `arena`.
*/
static inline
bool
pmm_is_frame_allocated(
    struct pmm_arena *arena,
    physaddr_t frame
) {
    return (arena->bitmap[pmm_get_frame_idx(arena, frame)] & pmm_get_frame_mask(arena, frame));
}

/*
** Mark `frame` as allocated.
**
** This function assumes `frame` belongs to `arena`.
*/
static inline
void
pmm_mark_as_allocated(
    struct pmm_arena *arena,
    physaddr_t frame
) {
    arena->bitmap[pmm_get_frame_idx(arena, frame)] |= pmm_get_frame_mask(arena, frame);
}

/*
** Mark a range of frames as allocated.
**
** The complexity of this function isn't very good, so it shouldn't be used
** in performance-critical situations.
*/
void
pmm_mark_range_as_allocated(
    physaddr_t frame,
    size_t len
) {
    physaddr_t end;

    debug_assert(IS_PAGE_ALIGNED(frame));
    debug_assert(IS_PAGE_ALIGNED(len));

    end = frame + len;
    while (frame < end) {
        struct pmm_arena *arena;
        bool found;

        found = false;
        arena = arenas;

        while (arena < arenas + arenas_len) {
            if (arena->start <= frame && frame < arena->end) {
                while (frame < arena->end && frame < end) {
                    arena->free_frames -= !pmm_is_frame_allocated(arena, frame);
                    pmm_mark_as_allocated(arena, frame);
                    frame += PAGE_SIZE;
                }
                found = true;
                break;
            }
            ++arena;
        }

        // Skip this frame if it doesn't belong to any arena
        if (!found) {
            frame += PAGE_SIZE;
        }
    }
}

/*
** Mark `frame` as free.
**
** This function assumes `frame` belongs to `arena`.
*/
static inline
void
pmm_mark_as_free(
    struct pmm_arena *arena,
    physaddr_t frame
) {
    arena->bitmap[pmm_get_frame_idx(arena, frame)] &= ~pmm_get_frame_mask(arena, frame);
}

/*
** Allocate a new frame within the given arena and return it, or `PHYS_NULL` if
** there is no physical memory left in that arena.
*/
static
physaddr_t
pmm_alloc_frame_in_arena(
    struct pmm_arena *arena
) {
    size_t i;
    size_t j;
    size_t final;
    bool pass;

    i = arena->next_frame;
    final = arena->bitmap_size;
    pass = false;

look_for_frame:
    while (i < final) {
        if (arena->bitmap[i] != 0xFFu) {
            j = 0u;
            // Find which frame is not taken
            while (j < 7u && (arena->bitmap[i] & (1 << j)) != 0) {
                ++j;
            }
            arena->bitmap[i] |= (1 << j); // mark the frame as taken
            arena->next_frame = i;
            arena->free_frames--;
            return (arena->start + PAGE_SIZE * (i * 8u + j));
        }
        ++i;
    }
    if (!pass) {
        final = arena->next_frame;
        i = 0;
        pass = true;
        goto look_for_frame;
    }
    return (PHYS_NULL);
}

/*
** Allocate a new frame and return it, or `PHYS_NULL` if there is no physical
** memory left.
*/
physaddr_t
pmm_alloc_frame(void)
{
    struct pmm_arena *arena;

    arena = arenas;
    while (arena < arenas + arenas_len) {
        if (arena->free_frames) {
            return (pmm_alloc_frame_in_arena(arena));
        }
        ++arena;
    }
    return (PHYS_NULL);
}

/*
** Free a given frame.
**
** The frame may already be free, in which case this function does nothing.
*/
void
pmm_free_frame(
    physaddr_t frame
) {
    struct pmm_arena *arena;

    arena = arenas;
    while (arena < arenas + arenas_len) {
        if (arena->start <= frame && frame < arena->end) {
            arena->free_frames += (pmm_is_frame_allocated(arena, frame));
            pmm_mark_as_free(arena, frame);
            arena->next_frame = pmm_get_frame_idx(arena, frame);
        }
        ++arena;
    }
}

/*
** Setup a boot arena based on a physical memory region included in the binary.
**
** This is used to solve the egg-chicken problem at the heart of the allocator
** (see the comment at the beginning of this module).
*/
void
pmm_early_init(void)
{
    memset(&boot_arena, 0, sizeof(boot_arena));

    boot_arena.start = (physaddr_t)kernel_boot_heap_start;
    boot_arena.end = (physaddr_t)kernel_boot_heap_end;

    boot_arena.bitmap = boot_arena_bitmap;
    boot_arena.bitmap_size = sizeof(boot_arena_bitmap);
    boot_arena.free_frames = (boot_arena.end - boot_arena.start) / PAGE_SIZE;

    memset(boot_arena.bitmap, 0, boot_arena.bitmap_size);

    arenas = &boot_arena;
    arenas_len = 1;
}

/*
** Fully set-up the physical memory manager, using the boot arena initialized
** in `pmm_early_init()`.
*/
status_t
pmm_init(void)
{
    struct pmm_arena *new_arenas;
    size_t new_arenas_len;
    struct pmm_reserved_area const *pra;
    size_t mmap_len;
    size_t i;
    size_t j;

    if (!mb_mmap) {
        return (ERR_BAD_STATE);
    }

    new_arenas_len = 0;
    mmap_len = (mb_mmap->size - sizeof(*mb_mmap)) / mb_mmap->entry_size;

    /*
    ** First, we calculate how many arenas we'll need.
    */

    i = 0;
    while (i < mmap_len) {
        new_arenas_len += (mb_mmap->entries[i].type == MULTIBOOT_MEMORY_AVAILABLE);
        ++i;
    }

    /*
    ** Then we allocate and initialize all `pmm_arena`, for each and every
    ** multiboot mmap tag marked as available memory.
    */

    new_arenas = kheap_alloc_zero(sizeof(struct pmm_arena) * new_arenas_len);
    if (!new_arenas) {
        return (ERR_OUT_OF_MEMORY);
    }

    i = 0;
    j = 0;
    while (i < mmap_len) {
        struct multiboot_mmap_entry const *entry;

        entry = mb_mmap->entries + i;
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
            struct pmm_arena *arena;

            arena = new_arenas + j;

            /*
            ** Multiboot might give us a memory region that isn't page-aligned,
            ** so we round up the start address and round down the end address to
            ** have the smallest (and therefore safest) region.
            */
            arena->start = ALIGN(entry->addr, PAGE_SIZE);
            arena->end = ROUND_DOWN(entry->addr + entry->len, PAGE_SIZE);

            arena->bitmap_size = ALIGN((arena->end - arena->start) / PAGE_SIZE, 8) / 8; // One byte per 8 frames.
            arena->free_frames = (arena->end - arena->start) / PAGE_SIZE;
            arena->bitmap = kheap_alloc_zero(arena->bitmap_size);
            if (!arena->bitmap) {
                return (ERR_OUT_OF_MEMORY);
            }

            // TODO Mark as allocated the few bits over arena->end but still
            // in the bitmap

            ++j;
        }
        ++i;
    }

    /*
    ** Swap the old arena with the new ones
    */
    arenas = new_arenas;
    arenas_len = new_arenas_len;

    /*
    ** The allocator is now initialized but we still have to mark all reserved
    ** areas as allocated (including the kernel itself).
    */

    pra = __start_pmm_reserved_area;
    while (pra < __stop_pmm_reserved_area) {
		pmm_mark_range_as_allocated(pra->start, pra->end - pra->start);
        ++pra;
    }

    /*
    ** Mark the boot heap as allocated.
    ** TODO: Improve this
    */

    pmm_mark_range_as_allocated(
        (physaddr_t)kernel_boot_heap_start,
        (physaddr_t)kernel_boot_heap_end
    );

    return (OK);
}
