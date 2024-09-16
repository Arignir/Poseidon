/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _ARCH_X86_64_CPU_H_
# define _ARCH_X86_64_CPU_H_

# include <poseidon/poseidon.h>
# include <poseidon/cpu/cpu.h>

/* The bootstrap processor. Used only at boot time. */
extern struct cpu *bsp;

/* Set if the BSP has been remapped to its corresponding entry within the cpu table. */
extern bool bsp_remapped;

void cpu_remap_bsp(void);

#endif /* !_ARCH_X86_64_CPU_H_ */