/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Kernel memory allocator.
** It is NOT suitable for user-space memory allocation.
**
** This is a pretty naive and straightforward implementation.
** It can definitely be improved, but I hate writing memory allocators.
** Like really, I mean it. Feel free to improve it :)
**
** `kheap_alloc_aligned()` is using a dirty trick to easily make aligned allocations.
** It calls `kheap_alloc()` with a size of `size + PAGE_SIZE` bytes and aligns
** the returned pointer to a page boundary. The old pointer in then added to
** `g_kheap_aligned_ptrs` so `kheap_free()` can find it.
**
** This definitely deserve an improvement, but eh, it's not fun enough. Big sad.
*/

#include "poseidon/memory/kheap.h"
#include "poseidon/memory/vmm.h"
#include "poseidon/boot/init_hook.h"
#include "lib/string.h"

/* `kheap_alloc()` global variables */
static struct kheap_block *g_kheap_head;
static struct kheap_block *g_kheap_tail;

/* `grow_heap()` global variables */
static size_t g_kernel_heap_size;

/* `kheap_alloc_aligned()` global variables */
static struct kheap_aligned_metadata *g_kheap_aligned_ptrs;
static size_t g_kheap_aligned_ptrs_len;

/*
** Grow the kernel heap by `inc` bytes, performing the allocation using
** `vmm_map()`.
**
** On success, the previous end of the kernel's heap is returned. `NULL` is
** returned otherwise.
*/
static
virtaddr_t
grow_heap(
    size_t inc
) {
    virtaddr_t old;
    virtaddr_t new;
    virtaddr_t round_new;
    virtaddr_t round_old;
    size_t round_add;

    old = (uchar *)kernel_heap_start + g_kernel_heap_size;
    new = (uchar *)old + inc;

    round_new = ROUND_DOWN(new, PAGE_SIZE);
    round_old = ROUND_DOWN(old, PAGE_SIZE);

    round_add = (uchar *)round_new - (uchar*)round_old;
    g_kernel_heap_size += inc;

    if (round_new > round_old) {
        status_t s;

        s = vmm_map(
            (uchar *)round_old + PAGE_SIZE,
            round_add,
            MMAP_RDWR
        );

        if (s != OK) {
            g_kernel_heap_size -= inc;
            return (NULL);
        }
    }
    return (old);
}

/*
** Find a free block of at least the given size.
*/
static
struct kheap_block *
find_free_block(
    size_t size
) {
    struct kheap_block *block;

    block = g_kheap_head;
    while (block <= g_kheap_tail) {
        assert(block->magic == KHEAP_MAGIC);
        if (!block->used && block->size >= size) {
            return (block);
        }
        block = (struct kheap_block *)((uchar *)(block + 1) + block->size);
    }
    return (NULL);
}

/*
** Split the given block at the given size, if possible.
**
** Note: size must not be greater that the block size.
*/
static
void
split_block(
    struct kheap_block *block,
    size_t size
) {
    struct kheap_block *new;
    struct kheap_block *next;

    if (block->size - size > sizeof(struct kheap_block) + 1) {
        new = (struct kheap_block *)((uchar *)(block + 1) + size);
        new->used = false;
        new->magic = KHEAP_MAGIC;
        new->size = block->size - size - sizeof(struct kheap_block);
        if (g_kheap_tail == block) {
            g_kheap_tail = new;
        }
        block->size = size;
        new->prev = block;
        next = (struct kheap_block *)((uchar *)(new + 1) + new->size);
        if (next <= g_kheap_tail) {
            next->prev = new;
        }
    }
}

/*
** Try to join two blocks into a single one, but only if they are both free.
*/
static
void
join_block(
    struct kheap_block *block
) {
    struct kheap_block *other;
    struct kheap_block *next;

    other = (struct kheap_block *)((uchar *)(block + 1) + block->size);
    if (!block->used && other <= g_kheap_tail && !other->used) {
        block->size += sizeof(struct kheap_block) + other->size;
        if (other == g_kheap_tail) {
            g_kheap_tail = block;
        }

        next = (struct kheap_block *)((uchar *)(other + 1) + other->size);
        if (next <= g_kheap_tail) {
            next->prev = block;
        }
    }
}

/*
** Given an aligned virtual address returned from `kheap_aligned()`, find
** the original virtual address (before it was aligned) so it can be passed to
** `kheap_free()`.
**
** This is achieved by going trough `g_kheap_aligned_ptrs` and finding an address
** matching the given one. It is then remove from the array and the original
** pointer is returned.
**
** This is part of a trick to easily implement a `kheap_alloc_aligned()` without
** having to heaviliy complexify the current implementation.
*/
static
virtaddr_t
find_real_from_aligned_ptr(
    virtaddr_t ptr
) {
    struct kheap_aligned_metadata *aligned_ptr;
    struct kheap_aligned_metadata *new_kheap_aligned_ptrs;

    aligned_ptr = g_kheap_aligned_ptrs;
    while (aligned_ptr < g_kheap_aligned_ptrs + g_kheap_aligned_ptrs_len) {
        if (aligned_ptr->aligned == ptr) {
            ptr = aligned_ptr->original;
            // We swap the current entry with the last one, and realloc the array
            *aligned_ptr = g_kheap_aligned_ptrs[g_kheap_aligned_ptrs_len - 1];
            g_kheap_aligned_ptrs_len -= 1;
            new_kheap_aligned_ptrs = kheap_realloc(
                g_kheap_aligned_ptrs,
                sizeof(*g_kheap_aligned_ptrs) * g_kheap_aligned_ptrs_len
            );
            if (new_kheap_aligned_ptrs) {
                g_kheap_aligned_ptrs = new_kheap_aligned_ptrs;
            }
            break;
        }
        ++aligned_ptr;
    }
    return (ptr);
}

/*
** `malloc()` but using memory in kernel space.
**
** TODO Make this function safer (overflow)
*/
virtaddr_t
kheap_alloc(
    size_t size
) {
    struct kheap_block *block;

    size += (size == 0);
    size = ALIGN(size, sizeof(void *));
    block = find_free_block(size);
    if (block) {
        block->used = true;
        split_block(block, size);
        goto ret_ok;
    }
    block = grow_heap(sizeof(struct kheap_block) + size);
    if (!block) {
        goto ret_err;
    }
    block->used = true;
    block->magic = KHEAP_MAGIC;
    block->size = size;
    block->prev = g_kheap_tail;
    g_kheap_tail = block;
    if (g_kheap_head == (void*)1) {
        g_kheap_head = block;
    }

ret_ok:
    return (block + 1);

ret_err:
    return (NULL);
}

/*
** `kheap_alloc()` but returning a page-aligned pointer.
**
** TODO Make this function safer (overflow)
*/
virtaddr_t
kheap_alloc_aligned(
    size_t size
) {
    virtaddr_t ptr;
    virtaddr_t aligned_ptr;
    virtaddr_t new_kheap_aligned_ptrs;

    ptr = kheap_alloc(size + PAGE_SIZE);
    aligned_ptr = ALIGN(ptr, PAGE_SIZE);

    if (ptr && ptr != aligned_ptr) {

        // Make room for the new metadata
        new_kheap_aligned_ptrs  = kheap_realloc(
            g_kheap_aligned_ptrs,
            sizeof(*g_kheap_aligned_ptrs) * (g_kheap_aligned_ptrs_len + 1)
        );

        if (!new_kheap_aligned_ptrs) {
            kheap_free(ptr);
            return (NULL);
        }

        g_kheap_aligned_ptrs = new_kheap_aligned_ptrs;

        // Write the new metadata in the array
        g_kheap_aligned_ptrs[g_kheap_aligned_ptrs_len] = (struct kheap_aligned_metadata) {
            .aligned = aligned_ptr,
            .original = ptr,
        };
        g_kheap_aligned_ptrs_len += 1;
    }
    return (aligned_ptr);
}

/*
** `kheap_alloc_aligned()`, but maps the returned address to the given physical
** addresses.
**
** `pa` and `size` must be page-aligned.
*/
virtaddr_t
kheap_alloc_device(
    physaddr_t pa,
    size_t size
) {
    virtaddr_t ptr;

    debug_assert(IS_PAGE_ALIGNED(pa));
    debug_assert(IS_PAGE_ALIGNED(size));

    ptr = kheap_alloc_aligned(size);

    if (
        ptr != NULL &&
        vmm_remap(ptr, pa, size, MMAP_RDWR, MUNMAP_FREE) == OK
    ) {
        return (ptr);
    }

    return (NULL);
}

/*
** `free()`, but using memory in kernel space.
**
** TODO: make this function safer (overflow)
*/
void
kheap_free(
    virtaddr_t ptr
) {
    struct kheap_block *block;

    if (!ptr) {
        return ;
    }
    if (IS_PAGE_ALIGNED(ptr)) {
        ptr = find_real_from_aligned_ptr(ptr);
    }
    block = (struct kheap_block *)ptr - 1;
    assert(block->used);
    assert(block->magic == KHEAP_MAGIC);
    block->used = false;
    join_block(block);
    if (block->prev) {
        join_block(block->prev);
    }
}

/*
** `realloc()`, but using memory in kernel space.
**
** TODO: make this function safer (overflow)
*/
virtaddr_t
kheap_realloc(
    virtaddr_t old_ptr,
    size_t new_size
) {
    void *ptr;
    struct kheap_block *block;

    ptr = kheap_alloc(new_size);
    if (ptr != NULL && old_ptr) {
        block = (struct kheap_block *)((char *)old_ptr - sizeof(struct kheap_block));
        memcpy(ptr, old_ptr, block->size > new_size ? new_size : block->size);
        kheap_free(old_ptr);
    }
    return (ptr);
}

/*
** `kheap_alloc`, but sets the memory content to zero.
**
** TODO: make this function safer (overflow)
*/
virtaddr_t
kheap_alloc_zero(
    size_t size
) {
    void *ptr;

    ptr = kheap_alloc(size);
    if (likely(ptr != NULL)) {
        memset(ptr, 0, size);
    }
    return (ptr);
}

status_t
kheap_init(void)
{
    g_kheap_head = (void*)1;
    g_kheap_tail = NULL;

    g_kernel_heap_size = 0;

    g_kheap_aligned_ptrs = NULL;
    g_kheap_aligned_ptrs_len = 0;

    /* `kheap_alloc()` algorithm assumes the first page is mapped */
    return (vmm_map(kernel_heap_start, PAGE_SIZE, MMAP_RDWR));
}
