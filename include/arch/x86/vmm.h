/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Virtual memory management for the x86 architecture.
*/

#ifndef _ARCH_X86_VMM_H_
# define _ARCH_X86_VMM_H_

# include <poseidon/poseidon.h>

/*
** An entry in the page directory
*/
struct pagedir_entry
{
    union
    {
        struct
        {
            uint32 present : 1;     /* Present in memory                */
            uint32 rw : 1;          /* 0 => Readonly / 1 => Readwrite   */
            uint32 user : 1;        /* 0 => Kernel page / 1 => User     */
            uint32 wtrough : 1;     /* 1 => write through caching       */
            uint32 cache : 1;       /* 1 => Cache disable               */
            uint32 accessed : 1;    /* set by cpu when accessed         */
            uint32 _zero : 1;       /* Must be 0                        */
            uint32 size : 1;        /* 0 => 4KiB page, 1 => 4MiB page   */
            uint32 __unusued : 4;   /* unused & reserved bits           */
            uint32 frame : 20;      /* Frame address                    */
        };
        uintptr value;
    };
};

#endif /* !_ARCH_X86_API_MEMORY_VMM_H_ */
