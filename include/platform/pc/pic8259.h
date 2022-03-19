/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2022 - The Poseidon Authors
**
\******************************************************************************/

#ifndef _PLATFORM_PC_PIC8259_H_
# define _PLATFORM_PC_PIC8259_H_

# include <poseidon/poseidon.h>

void pic8259_irq_mask(uchar irq);
void pic8259_irq_unmask(uchar irq);
void pic8259_set_irq_mask(ushort mask);
void pic8259_master_eoi(void);
void pic8259_slave_eoi(void);
void pic8259_init(void);

#endif /* !_PLATFORM_PC_PIC8259_H_ */

