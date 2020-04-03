/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Segments selectors used by the kernel.
*/

#ifndef _ARCH_X86_64_SELECTOR_H_
# define _ARCH_X86_64_SELECTOR_H_

/*
** Those descriptors should be the same for both the boot gdt and the dynamic
** gdt, so that any data structure depending on them don't need to be udpated
** when the kernel switches to the later one.
**
** (See `arch/x86_64/boot/gdt.c` for more informations about the boot and
** dynamic gdt.)
*/

# define KERNEL_CODE_SELECTOR       0x08
# define KERNEL_DATA_SELECTOR       0x10
# define USER_CODE_SELECTOR         0x18
# define USER_DATA_SELECTOR         0x20

#endif /* !_ARCH_X86_64_SELECTOR_H_ */
