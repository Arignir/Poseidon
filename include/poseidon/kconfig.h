/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Represents the target configuration of the kernel.
**
** This example is for the x86 architecture.
*/

#ifndef _POSEIDON_KCONFIG_H_
# define _POSEIDON_KCONFIG_H_

/*
** The following can be modified freely almost independently of the
** targeted architecture.
*/

/* Use a multiboot-compliant bootloader to set-up graphic mode. */
# define KCONFIG_MULTIBOOT_FRAMEBUFFER          0

/*
** Kernel stack size, in bytes
**
** Too little, and you'll stack overflow. Too much, and you'll loose memory.
** You shouldn't put it lower than 16kB: the kernel expects to run with
** at least that amount of stack.
*/
# define KCONFIG_KERNEL_STACK_SIZE              (16 * KCONFIG_PAGE_SIZE)

/*
** The following are options meaningful only if the target archecture is
** x86. They are ignored otherwise.
*/

/*
** Use Physical Address Extension.
**
** If enabled, the kernel can use up to 64GB of RAM in 32-bits mode.
*/
# define KCONFIG_X86_USE_PAE                    0

/*
** The following are properties depending on the targetted architecture and
** shouldn't be modified unless you are porting Poseidon to another
** architecture.
*/

/* Architecture's name. */
# define KCONFIG_ARCH                           x86

/* Sets to true if the targetted architecture is x86. */
# define KCONFIG_ARCH_X86                       1

/*
** Set to IO_PORT_MAPPED if the architecture is using port IO
** Set to IO_MEMORY_MAPPED if the architecture is using memory mapped IO.
**
** Those constant are defined in `include/poseidon/io.h`.
*/
# define KCONFIG_ARCH_IO_METHOD                 IO_PORT_MAPPED

/* Size of a pointer, in bits. */
# define KCONFIG_ARCH_BITS                      32

/* Size of a page (in bytes). */
# define KCONFIG_PAGE_SIZE                      4096u

/* The following are shortcuts for commonly used options. */
# define ARCH                                   KCONFIG_ARCH
# define ARCH_X86                               KCONFIG_ARCH_X86
# define ARCH_BITS                              KCONFIG_ARCH_BITS
# define PAGE_SIZE                              KCONFIG_PAGE_SIZE

#endif /* !_KCONFIG_POSEIDON_H_ */
