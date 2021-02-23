/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Low-level memory-related functions and types.
**
** TODO: List all the features supported.
*/

#include <arch/x86_64/memory.h>
#include <arch/x86_64/cpuid.h>
#include <poseidon/memory/pmm.h>
#include <poseidon/memory/vmm.h>
#include <lib/string.h>

/*
** Return the address of the current PML4.
**
** This function takes advantage of recursive mapping.
*/
static inline
struct pml4 *
get_pml4(void)
{
    return ((struct pml4 *)0xFFFFFFFFFFFFF000ULL);
}

/*
** Return the address of the page-directory-pointer-table mapping the address `val`.
**
** This takes advantage of recursive mapping.
*/
static inline
struct pdpt *
get_pdpt_of(
    struct virtaddr_layout val
) {
    return ((struct pdpt *)(0xFFFFFFFFFFE00000ULL | (val.pml4_idx << 12u)));
}

/*
** Return the address of the page-directory mapping the address `val`.
**
** This takes advantage of recursive mapping.
*/
static inline
struct page_directory *
get_pd_of(
    struct virtaddr_layout val
) {
    return ((struct page_directory *)(0xFFFFFFFFC0000000ULL | (val.pml4_idx << 21u) | (val.pdpt_idx << 12u)));
}

/*
** Return the address of the page table mapping the address `val`.
**
** This takes advantage of recursive mapping.
*/
static inline
struct page_table *
get_pt_of(
    struct virtaddr_layout val
) {
    return ((struct page_table *)(0xFFFFFF8000000000ULL | (val.pml4_idx << 30u) | (val.pdpt_idx << 21u) | (val.pd_idx << 12u)));
}

/*
** Invalidate the TLB cache of the given address.
**
** This function should be called each time the mapping is modified.
*/
static
void
tlb_invalidate_page(
    virtaddr_t va
) {
    asm volatile(
        "invlpg (%%rax)"
        :
        : "a"(va)
        :
    );
}

/*
** The following functions are implementations of the virtual memory manager
** API for the x86_64 architecture.
**
** See `poseidon/memory/vmm.h`.
*/

/*
** Test whether the given virtual address is mapped.
**
** This is done by testing every entry in the paging structure, ensuring they
** all have the `present` flag set.
*/
bool
vmm_is_mapped(
    virtaddr_const_t va
) {
    struct virtaddr_layout val;

    val.raw = va;

    return (
        get_pml4()->entries[val.pml4_idx].present &&
        get_pdpt_of(val)->entries[val.pdpt_idx].present &&
        get_pd_of(val)->entries[val.pd_idx].present &&
        get_pt_of(val)->entries[val.pt_idx].present
    );
}

/*
** Test whether the given virtual address is mapped and belongs to user-space.
**
** This is done by testing every entry in the paging structure, ensuring they
** all have both the `present` and `user` flag set.
*/
bool
vmm_is_mapped_user(
    virtaddr_const_t va
) {
    struct virtaddr_layout val;

    val.raw = va;

    return (
        get_pml4()->entries[val.pml4_idx].present &&
        get_pml4()->entries[val.pml4_idx].user &&
        get_pdpt_of(val)->entries[val.pdpt_idx].present &&
        get_pdpt_of(val)->entries[val.pdpt_idx].user &&
        get_pd_of(val)->entries[val.pd_idx].present &&
        get_pd_of(val)->entries[val.pd_idx].user &&
        get_pt_of(val)->entries[val.pt_idx].present &&
        get_pt_of(val)->entries[val.pt_idx].user
    );
}

/*
** Map the virtual address `va` to `pa` with the given permissions.
**
** Note:
**   * Any missing intermediate page-table will be allocated on the fly.
**   * Missing intermediate page-table won't be freed if the final allocation
**     failed, meaning the memory isn't identical as it was before the call
**     if the function fails.
**   * The virtual address needs to be page-aligned.
**   * The physical address needs to be page-aligned.
**   * Any PML4E, PDPTE or PDE added mid-way will have the most flexible permissions
**     (URWX), giving the requested permissions only to the final page table
**     entry.
*/
status_t
vmm_map_frame(
    virtaddr_t va,
    physaddr_t pa,
    mmap_flags_t flags
) {
    struct virtaddr_layout val;
    struct pml4e *pml4e;
    struct pdpte *pdpte;
    struct pde *pde;
    struct pte *pte;

    debug_assert(IS_PAGE_ALIGNED(va));
    debug_assert(IS_PAGE_ALIGNED(pa));

    val.raw = va;

    pml4e = get_pml4()->entries + val.pml4_idx;
    if (!pml4e->present) {
        physaddr_t frame = pmm_alloc_frame();
        if (frame == PHYS_NULL) {
            return (ERR_OUT_OF_MEMORY);
        }

        /* Map high-level page tables with the most flexible permissions */
        pml4e->raw = frame; // This also unsets all flags
        pml4e->present = true;
        pml4e->rw = true;
        pml4e->user = true;

        void *pdpt = get_pdpt_of(val);
        tlb_invalidate_page(pdpt);
        memset(pdpt, 0, PAGE_SIZE);
    }

    pdpte = get_pdpt_of(val)->entries + val.pdpt_idx;
    if (!pdpte->present) {
        physaddr_t frame = pmm_alloc_frame();
        if (frame == PHYS_NULL) {
            return (ERR_OUT_OF_MEMORY);
        }

        /* Map high-level page tables with the most flexible permissions */
        pdpte->raw = frame; // This also unsets all flags
        pdpte->present = true;
        pdpte->rw = true;
        pdpte->user = true;

        void *pd = get_pd_of(val);
        tlb_invalidate_page(pd);
        memset(pd, 0, PAGE_SIZE);
    }

    pde = get_pd_of(val)->entries + val.pd_idx;
    if (!pde->present) {
        physaddr_t frame = pmm_alloc_frame();
        if (frame == PHYS_NULL) {
            return (ERR_OUT_OF_MEMORY);
        }

        /* Map high-level page tables with the most flexible permissions */
        pde->raw = frame; // This also unsets all flags
        pde->present = true;
        pde->rw = true;
        pde->user = true;

        void *pt = get_pt_of(val);
        tlb_invalidate_page(pt);
        memset(pt, 0, PAGE_SIZE);
    }

    pte = get_pt_of(val)->entries + val.pt_idx;

    if (pte->present) {
        return (ERR_ALREADY_MAPPED);
    }

    pte->raw = pa; // This also unsets all flags
    pte->present = true;
    pte->rw = (bool)(flags & MMAP_RDWR);
    pte->user = (bool)(flags & MMAP_USER);
    pte->xd = !(bool)(flags & MMAP_EXEC);

    tlb_invalidate_page(va);

    return (OK);
}

/*
** Unmap the virtual address `va`.
**
** Note:
**   * Any intermediate pt/pd/pdpt isn't freed, even if it is empty.
*/
void
vmm_unmap_frame(
    virtaddr_t va,
    munmap_flags_t flags
) {
    struct virtaddr_layout val;
    struct pte *pte;

    debug_assert(IS_PAGE_ALIGNED(va));

    val.raw = va;

    if (
        get_pml4()->entries[val.pml4_idx].present &&
        get_pdpt_of(val)->entries[val.pdpt_idx].present &&
        get_pd_of(val)->entries[val.pd_idx].present &&
        get_pt_of(val)->entries[val.pt_idx].present
    ) {
        pte = get_pt_of(val)->entries + val.pt_idx;

        if (~(flags & MUNMAP_NO_FREE)) {
            pmm_free_frame(pte->frame << 12u);
        }

        memset(pte, 0, sizeof(*pte));
        tlb_invalidate_page(va);
    }
}
