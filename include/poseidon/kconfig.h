/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** The kernel's configuration.
*/

#pragma once

#define POSEIDON_KCONFIG

#include "platform/target/kconfig.h"

/*
** The following can be modified freely almost independently of the
** targeted architecture.
*/

// Enable Symmetric MultiProcessing
#define KCONFIG_SMP                             1

// Use a multiboot-compliant bootloader to set-up graphic mode.
#define KCONFIG_MULTIBOOT_FRAMEBUFFER           0
