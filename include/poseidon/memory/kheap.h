/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _POSEIDON_MEMORY_KHEAP_H_
# define _POSEIDON_MEMORY_KHEAP_H_

# include <poseidon/poseidon.h>
# include <poseidon/memory/memory.h>

/*
** Each allocation made with `kheap_alloc()` belongs to a block. Each block is
** linked to the previous one, therefore forming a linked list.
**
** This header is placed at the beginning of the block, right before the memory
** pointed by the return value of `kheap_alloc()`.
**
** Therefore, the next block can easily be deduced: it is at
** `(uchar *)block + block->size`.
*/
struct kheap_block {
    bool used;
    uint32 magic;
    size_t size;
    struct kheap_block *prev;
};

static_assert(sizeof(struct kheap_block) % sizeof(void *) == 0);

/*
** Each block's header must have their `magic` field equal to this or
** it has been corrupted.
*/
# define KHEAP_MAGIC  0xf9f66427

/*
** We are using a dirty trick to have a quick and functional `kheap_alloc_aligned()`.
**
** It needs to store the original pointer returned by `kheap_alloc()` before
** aligning it to a page boundary.
*/
struct kheap_aligned_metadata {
    virtaddr_t aligned;
    virtaddr_t original;
};

status_t kheap_init(void);
virtaddr_t kheap_alloc(size_t);
virtaddr_t kheap_alloc_aligned(size_t);
virtaddr_t kheap_alloc_device(physaddr_t, size_t);
virtaddr_t kheap_realloc(virtaddr_t, size_t);
virtaddr_t kheap_alloc_zero(size_t);
void kheap_free(virtaddr_t);

#endif /* !_POSEIDON_MEMORY_KHEAP_H_ */
