/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Low-level memory-related functions and types.
*/

#pragma once

#include "poseidon/poseidon.h"
#include "poseidon/memory/memory.h"

/*
** An entry in the 4-level PML4
*/
struct [[gnu::packed]] pml4e {
    union [[gnu::packed]] {
        struct [[gnu::packed]] {
            size_t present: 1;          // Present in memory
            size_t rw: 1;               // 0 => Readonly / 1 => Readwrite
            size_t user: 1;             // 0 => Kernel page / 1 => User
            size_t wtrough: 1;          // 1 => write through caching
            size_t cache: 1;            // 1 => Cache disable
            size_t accessed: 1;         // Set by the cpu when accessed
            size_t _reserved0: 1;
            size_t _zer0: 1;            // Page size. Must be 0 for a PML4E
            size_t _reserved1: 4;
            size_t frame: 40;           // Frame address
            size_t _reserved2: 11;
            size_t xd: 1;               // Execute disable (Requires IA32_EFER.NXE = 1)
        };
        uintptr raw;
    };
};

static_assert(sizeof(struct pml4e) == sizeof(uintptr));

/*
** An entry in a page-directory-pointer-table.
*/
struct [[gnu::packed]] pdpte {
    union [[gnu::packed]] {
        struct [[gnu::packed]] {
            size_t present: 1;          // Present in memory
            size_t rw: 1;               // 0 => Readonly / 1 => Readwrite
            size_t user: 1;             // 0 => Kernel page / 1 => User
            size_t wtrough: 1;          // 1 => write through caching
            size_t cache: 1;            // 1 => Cache disable
            size_t accessed: 1;         // Set by the cpu when accessed
            size_t dirty: 1;            // Set by the cpu when written (Requires .size = 1)
            size_t size: 1;             // Page size (1 = 1GB page).
            size_t global: 1;           // Determines if translation is global (Requires .size = 1)
            size_t _reserved1: 3;
            size_t frame: 40;           // Frame address
            size_t _reserved2: 7;
            size_t keys: 4;             // Protection keys (Requires CR4.PKE = 1 and .size = 1)
            size_t xd: 1;               // Execute disable (Requires IA32_EFER.NXE = 1)
        };
        uintptr raw;
    };
};

static_assert(sizeof(struct pdpte) == sizeof(uintptr));

/*
** An entry in a page-directory.
*/
struct [[gnu::packed]] pde {
    union [[gnu::packed]] {
        struct [[gnu::packed]] {
            size_t present: 1;          // Present in memory
            size_t rw: 1;               // 0 => Readonly / 1 => Readwrite
            size_t user: 1;             // 0 => Kernel page / 1 => User
            size_t wtrough: 1;          // 1 => write through caching
            size_t cache: 1;            // 1 => Cache disable
            size_t accessed: 1;         // Set by the cpu when accessed
            size_t dirty: 1;            // Set by the cpu when written (Requires .size = 1)
            size_t size: 1;             // Page size (1 = 2MB page).
            size_t global: 1;           // Determines if translation is global (Requires .size = 1)
            size_t _reserved1: 3;
            size_t frame: 40;           // Frame address
            size_t _reserved2: 7;
            size_t keys: 4;             // Protection keys (Requires CR4.PKE = 1 and .size = 1)
            size_t xd: 1;               // Execute disable (Requires IA32_EFER.NXE = 1)
        };
        uintptr raw;
    };
};

static_assert(sizeof(struct pde) == sizeof(uintptr));

/*
** An entry in a page-table.
*/
struct [[gnu::packed]] pte {
    union [[gnu::packed]] {
        struct [[gnu::packed]] {
            size_t present: 1;          // Present in memory
            size_t rw: 1;               // 0 => Readonly / 1 => Readwrite
            size_t user: 1;             // 0 => Kernel page / 1 => User
            size_t wtrough: 1;          // 1 => write through caching
            size_t cache: 1;            // 1 => Cache disable
            size_t accessed: 1;         // Set by the cpu when accessed
            size_t dirty: 1;            // Set by the cpu when written
            size_t pat: 1;
            size_t global: 1;           // Determines if translation is global
            size_t _reserved1: 3;
            size_t frame: 40;           // Frame address
            size_t _reserved2: 7;
            size_t keys: 4;             // Protection keys (Requires CR4.PKE = 1 and .size = 1)
            size_t xd: 1;               // Execute disable (Requires IA32_EFER.NXE = 1)
        };
        uintptr raw;
    };
};

static_assert(sizeof(struct pte) == sizeof(uintptr));

/*
** A PML4, composed of 512 entries.
*/
struct pml4 {
    struct pml4e entries[512];
};


/*
** A page-directory-pointer-table, composed of 512 entries.
*/
struct pdpt {
    struct pdpte entries[512];
};

static_assert(sizeof(struct pdpt) == PAGE_SIZE);

/*
** A page directory, composed of 512 entries.
*/
struct page_directory {
    struct pde entries[512];
};

static_assert(sizeof(struct page_directory) == PAGE_SIZE);

/*
** A page directory, composed of 512 entries.
*/
struct page_table {
    struct pte entries[512];
};

static_assert(sizeof(struct page_table) == PAGE_SIZE);

/*
** A virtual address as defined by Intel.
*/
struct [[gnu::packed]] virtaddr_layout {
    union [[gnu::packed]] {
        struct [[gnu::packed]] {
            size_t offset : 12;
            size_t pt_idx : 9;
            size_t pd_idx : 9;
            size_t pdpt_idx : 9;
            size_t pml4_idx : 9;
            size_t : 16;
        };
        void const *raw;
    };
};

static_assert(sizeof(struct virtaddr_layout) == sizeof(void *));

/* Target for TLB shootdowns. */
extern virtaddr_t g_tlb_shootdown_target;
