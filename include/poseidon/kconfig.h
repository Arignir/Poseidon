/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

/*
** Represents the current configuration of the kernel.
**
** This example is for the x86 architecture.
*/

#ifndef _POSEIDON_KCONFIG_H_
# define _POSEIDON_KCONFIG_H_

/*
** The following can be modified freely almost independently of the
** targeted architecture.
*/

// Use a multiboot-compliant bootloader to set-up graphic mode.
# define KCONFIG_MULTIBOOT_FRAMEBUFFER			0

/*
** Kernel stack size, in bytes
**
** Too little, and you'll stack overflow, too much, and you'll loose memory.
** You shouldn't put it lower than 16kB, as must kernel code expects to run with
** at least that amount of stack.
*/
# define KCONFIG_KERNEL_STACK_SIZE			16 * KCONFIG_PAGE_SIZE

/*
** The following are properties depending on the targetted architecture and
** shouldn't be modified unless you are porting Poseidon to another
** architecture.
*/

// Architecture's name
# define KCONFIG_ARCH					x86

// Sets to 1 if the targetted architecture is x86
# define KCONFIG_X86					1

/*
** Set to IO_PORT if the architecture is using port IO
** Set to IO_MM if the architecture is using memory mapped IO.
**
** IO_PORT and IO_MM are defined in `include/lib/io.h`.
*/
# define KCONFIG_ARCH_IO_METHOD				IO_PMIO

// Size of a pointer, in bits.
# define ARCH_BITS					32

// Size of a page (in bytes).
# define KCONFIG_PAGE_SIZE				4096u

// The following are shortcuts for commonly used options.
# define ARCH						KCONFIG_ARCH
# define ARCH_X86					KCONFIG_ARCH_X86
# define ARCH_BITS					KCONFIG_ARCH_BITS
# define PAGE_SIZE					KCONFIG_PAGE_SIZE

#endif /* !_KCONFIG_POSEIDON_H_ */
