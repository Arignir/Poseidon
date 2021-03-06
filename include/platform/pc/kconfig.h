/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** The kernel's configuration specific to the PC platform.
*/

#ifndef _PLATFORM_PC_KCONFIG_H_
# define _PLATFORM_PC_KCONFIG_H_

/*
** Ensure this file isn't included on its own.
** The file `include/poseidon/kconfig.h` should be included instead, which, in
** turn, includes this one.
*/
# ifndef _POSEIDON_KCONFIG_H_
#  error Please include `poseidon/kconfig.h` instead of `platform/pc/kconfig.h`.
# endif /* !_POSEIDON_KCONFIG_H_ */

/*
** Warning: the following values aren't meant to be modified by the user.
*/

/* Facilitate target platform check */
# define KCONFIG_PLATFORM_PC

/* The PC platform uses both port-based and memory-based IO. */
# define KCONFIG_IO_PORT                        1
# define KCONFIG_IO_MEMORY                      1

/* Kernel stack size, in bytes. */
# define KCONFIG_KERNEL_STACK_SIZE              (16 * KCONFIG_PAGE_SIZE)

/* Thread stack size, in bytes. */
# define KCONFIG_THREAD_STACK_SIZE              (32 * KCONFIG_PAGE_SIZE)

/* Size of a pointer, in bits. */
# define KCONFIG_PTR_BITS                       64u

/* Size of a page (in bytes). */
# define KCONFIG_PAGE_SIZE                      4096u

/* Size of a frame (in bytes). */
# define KCONFIG_FRAME_SIZE                     4096u

/* Size of the boot kheap (in bytes). Atm it is 4MB. */
# define KCONFIG_BOOT_KHEAP_SIZE                (4u * 1024u * 1024u)

/* Maximum amount of CPUs theoretically available. */
# define KCONFIG_MAX_CPUS                       16

#endif /* !_KCONFIG_PLATFORM_PC_H_ */
