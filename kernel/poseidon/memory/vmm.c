/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Architecture independant virtual memory management utilities.
*/

#include <poseidon/boot/init_hook.h>
#include <poseidon/memory/memory.h>
#include <poseidon/memory/vmm.h>
#include <poseidon/memory/pmm.h>

/*
** Map a range of contiguous virtual pages to free, random, physical frames.
**
** In case of error, no memory is retained allocated.
**
** Both `va` and `size` must be page-aligned.
*/
status_t
vmm_map(
    virtaddr_t va,
    size_t size,
    mmap_flags_t flags
) {
    uchar *origin;
    physaddr_t pa;
    status_t s;

    if (
        !IS_PAGE_ALIGNED(va) ||
        !IS_PAGE_ALIGNED(size)
    ) {
        return (ERR_INVALID_ARGS);
    }

    /* Quick-check round-up to limit failures later */
    // TODO Pre-allocate memory

    origin = va;
    while ((uchar *)va < origin + size) {
        if (vmm_is_mapped(va)) {
            return (ERR_ALREADY_MAPPED);
        }
        va = (uchar *)va + PAGE_SIZE;
    }

    va = origin;

    /* The mapping can now be performed */
    while ((uchar *)va < origin + size) {
        pa = pmm_alloc_frame();
        if (pa == PHYS_NULL) {
            s = ERR_OUT_OF_MEMORY;
            goto err;
        }

        s = vmm_map_frame(va, pa, flags);
        if (s != OK) {
            // We checked for that earlier, it shouldn't happen
            debug_assert(s != ERR_ALREADY_MAPPED);
            goto err;
        }
        va = (uchar *)va + PAGE_SIZE;
    }

    return (OK);

err:
    /* In case of error, unmap what has been done */
    vmm_unmap(origin, (uchar *)va - origin, MUNMAP_FREE);
    return (s);
}

/*
** Map a range of contiguous virtual pages to the given contiguous physical frames.
**
** This function doesn't mark the given physical range as allocated nor increases
** any reference counter.
**
** In case of error, no memory is retained allocated.
**
** Both `va` and `size` must be page-aligned.
*/
status_t
vmm_map_device(
    virtaddr_t va,
    physaddr_t pa,
    size_t size,
    mmap_flags_t flags
) {
    uchar *origin;
    status_t s;

    if (
        !IS_PAGE_ALIGNED(va) ||
        !IS_PAGE_ALIGNED(pa) ||
        !IS_PAGE_ALIGNED(size)
    ) {
        return (ERR_INVALID_ARGS);
    }

    /* Quick-check round-up to limit failures later */

    origin = va;
    while ((uchar *)va < origin + size) {
        if (vmm_is_mapped(va)) {
            return (ERR_ALREADY_MAPPED);
        }
        va = (uchar *)va + PAGE_SIZE;
    }

    va = origin;

    /* The mapping can now be performed */
    while ((uchar *)va < origin + size) {
        s = vmm_map_frame(va, pa, flags);
        if (s != OK) {
            // We checked for that earlier, it shouldn't happen
            debug_assert(s != ERR_ALREADY_MAPPED);
            return (s);
        }
        va = (uchar *)va + PAGE_SIZE;
        pa += PAGE_SIZE;
    }

    return (OK);
}

/*
** Unmap `va` and remap it to `pa`.
**
** Note:
**  * In case of failure, `va` is left unmapped.
*/
status_t
vmm_remap(
    virtaddr_t va,
    physaddr_t pa,
    size_t size,
    mmap_flags_t mmap_flags,
    munmap_flags_t munmap_flags
) {
    vmm_unmap(va, size, munmap_flags);
    return (vmm_map_device(va, pa, size, mmap_flags));
}

/*
** Unmap a range of contiguous virtual pages.
**
** Non-mapped pages are ignored.
** No operation are performed if `size` is 0.
**
** This function assumes `va` and `size` are page-aligned and will panic
** otherwise.
*/
void
vmm_unmap(
    virtaddr_t va,
    size_t size,
    munmap_flags_t flags
) {
    virtaddr_t origin;

    debug_assert(IS_PAGE_ALIGNED(va));
    debug_assert(IS_PAGE_ALIGNED(size));

    origin = va;
    while ((uchar *)va < (uchar *)origin + size) {
        vmm_unmap_frame(va, flags);
        va = (uchar *)va + PAGE_SIZE;
    }
}

/*
** Test if the given buffer is mapped and belongs to user-space.
**
** `OK` is returned if it is both mapped and belongs to user-space.
** Otherwise, `ERR_PERMISSION_DENIED` is returned.
*/
status_t
vmm_validate_user_buffer(
    void const *buffer,
    size_t len
) {
    uchar const *buff_start;
    uchar const *buff_end;

    buff_start = ROUND_DOWN(buffer, PAGE_SIZE);
    buff_end = ROUND_DOWN((uchar const *)buffer + len, PAGE_SIZE);

    do {
        if (!vmm_is_mapped_user(buff_start)) {
            return (ERR_PERMISSION_DENIED);
        }
        buff_start += PAGE_SIZE;
    } while (buff_start <= buff_end);

    return (OK);
}

/*
** Calculate the length of the given string while also ensuring it is mapped and
** belongs to userspace.
**
** `OK` is returned if it is both mapped and belongs to user-space.
** Otherwise, `ERR_PERMISSION_DENIED` is returned.
**
** The length of `str` is stored in `len` unless `len` is NULL.
*/
status_t
vmm_validate_user_str(
    char const *str,
    size_t *len
) {
    char const *str_align;
    char const *str_start;

    str_start = str;
    str_align = ROUND_DOWN(str, PAGE_SIZE);

    /* Iterate page by page and then char by char */

    while (true) {
        if (!vmm_is_mapped_user(str_align)) {
            return (ERR_PERMISSION_DENIED);
        }

        while (str < str_align + PAGE_SIZE) {
            if (*str == '\0') { // Stop here
                if (len) {
                    *len = str - str_start;
                }
                return (OK);
            }
            ++str;
        }

        str_align += PAGE_SIZE;
    }
}

/*
** The following functions are default implementations, usually overwritten by
** the architecture.
*/

/*
** Test whether the given virtual address is mapped.
*/
__weak
bool
vmm_is_mapped(
    virtaddr_const_t va __unused
) {
    unimplemented();
}

/*
** Test whether the given virtual address is mapped and belongs to user-space.
*/
__weak
bool
vmm_is_mapped_user(
    virtaddr_const_t va __unused
) {
    unimplemented();
}

/*
** Map the virtual address `va` on the physical address `pa` with the
** permission described in `flags`.
**
** This function doesn't overwrite any existing mapping, failing instead.
*/
__weak
status_t
vmm_map_frame(
    virtaddr_t va __unused,
    physaddr_t pa __unused,
    mmap_flags_t flags __unused
) {
    unimplemented();
}

/*
** Unmap the virtual address `va`.
*/
__weak
void
vmm_unmap_frame(
    virtaddr_t va __unused,
    munmap_flags_t flags __unused
) {
    unimplemented();
}
