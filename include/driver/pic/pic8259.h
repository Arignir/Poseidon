/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#ifndef _DRIVER_PIC_8259_H_
# define _DRIVER_PIC_8259_H_

void	pic8259_master_eoi(void);
void	pic8259_slave_eoi(void);
void	pic8259_init(void);

#endif /* !_DRIVER_PIC_8259_H_ */

