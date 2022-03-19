/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

/*
** The kernel's configuration.
*/

#ifndef _POSEIDON_KCONFIG_H_
# define _POSEIDON_KCONFIG_H_

# include <platform/target/kconfig.h>

/*
** The following can be modified freely almost independently of the
** targeted architecture.
*/

/* Enable Symmetric MultiProcessing */
# define KCONFIG_SMP                            1

/* Use a multiboot-compliant bootloader to set-up graphic mode. */
# define KCONFIG_MULTIBOOT_FRAMEBUFFER          0

#endif /* !_KCONFIG_POSEIDON_H_ */
