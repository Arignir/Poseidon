/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include "poseidon/status.h"
#include "poseidon/poseidon.h"
#include "poseidon/boot/init_hook.h"
#include "poseidon/boot/multiboot2.h"
#include "poseidon/memory/memory.h"
#include "poseidon/memory/kheap.h"

/*
** The multiboot structure, filled by the bootloader.
** This variable is set early in the boot process.
*/
physaddr_t g_mb_tag_pa = PHYS_NULL;

/*
** Size of the multiboot structure, filled by the bootloader.
** This variable is set early in the boot process.
*/
uint32 g_mb_tag_len = 0;

/*
** A virtual address that points to `mb_tag_pa` after setup.
*/
struct multiboot_tag const *g_mb_tag = NULL;

/*
** The aligned pointer returned by `kheap_alloc_device()`,
** aligned to a page boundary, so that it can be freed at a later stage.
*/
virtaddr_t g_aligned_mb_tag = NULL;

/*
** Pointers to the most common multiboot tags.
**
** See `poseidon/boot/multiboot2.h` for their content.
*/

struct multiboot_tag_string const *g_mb_cmdline = NULL;
struct multiboot_tag_string const *g_mb_bootloader = NULL;
struct multiboot_tag_basic_meminfo const *g_mb_meminfo = NULL;
struct multiboot_tag_mmap const *g_mb_mmap = NULL;
struct multiboot_tag_framebuffer const *g_mb_fb = NULL;

/*
** Map the multiboot structure, and parse it to save pointers to the most
** useful tags.
*/
status_t
multiboot_load(void)
{
    struct multiboot_tag const *tag;
    physaddr_t aligned_tag_pa;
    size_t aligned_tag_len;

    assert(g_mb_tag_pa != PHYS_NULL);
    assert(g_mb_tag_len > 0);

    /*
    ** Round down to a page boundary `mb_tag_pa` and calculate on how many
    ** pages the multiboot tags spreads on.
    */
    aligned_tag_pa = ROUND_DOWN(g_mb_tag_pa, PAGE_SIZE);
    aligned_tag_len = ALIGN(g_mb_tag_pa + g_mb_tag_len, PAGE_SIZE) - aligned_tag_pa;

    /*
    ** Map the multiboot structure to virtual memory.
    */
    g_aligned_mb_tag = kheap_alloc_device(aligned_tag_pa, aligned_tag_len);

    if (!g_aligned_mb_tag) {
        return ERR_OUT_OF_MEMORY;
    }

    /*
    ** Add to `aligned_mb_tag` the offset between the frame boundary and
    ** `mb_tag_pa`.
    ** Therefore, `mb_tag` has the same offset than `mb_tag_pa`.
    */
    g_mb_tag = (struct multiboot_tag const *)((uchar *)g_aligned_mb_tag + g_mb_tag_pa - aligned_tag_pa);

    /*
    ** We can now iterate on each tag and retrieve any wanted information.
    */

    tag = g_mb_tag;
    while (tag->type != MULTIBOOT_TAG_TYPE_END)
    {
        switch (tag->type)
        {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                g_mb_cmdline = (struct multiboot_tag_string const *)tag;
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                g_mb_bootloader = (struct multiboot_tag_string const *)tag;
                break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                g_mb_meminfo = (struct multiboot_tag_basic_meminfo const *)tag;
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                g_mb_mmap = (struct multiboot_tag_mmap const *)tag;
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                g_mb_fb = (struct multiboot_tag_framebuffer const *)tag;
                break;
        }

        // Each tag is aligned on the next 8-byte boundary.
        tag = (struct multiboot_tag const *)ALIGN(
            (uchar const *)tag + tag->size,
            8
        );
    }
    return OK;
}
