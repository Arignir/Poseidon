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

/* The PC platform uses port-based IO. */
# define KCONFIG_IO_METHOD                      (IO_PORT_MAPPED)

/* Kernel stack size, in bytes. */
# define KCONFIG_KERNEL_STACK_SIZE              (16 * KCONFIG_PAGE_SIZE)

/* Size of a pointer, in bits. */
# define KCONFIG_PTR_BITS                       64u

/* Size of a page (in bytes). */
# define KCONFIG_PAGE_SIZE                      4096u

/* Size of a frame (in bytes). */
# define KCONFIG_FRAME_SIZE                     4096u

#endif /* !_KCONFIG_PLATFORM_PC_H_ */
