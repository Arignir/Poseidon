/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
** Discover CPUs following the old Intel "Multiprocessor Specification".
**
** This discovery method is really old and not supported any more in real hardware.
** We are using it because it's easier to implemant than ACPI and can still be
** tester through QEMU.
**
** References:
**
** Intel MultiProcessor Specification:
**   https://pdos.csail.mit.edu/6.828/2008/readings/ia32/MPspec.pdf
*/

#include <poseidon/kconfig.h>
#include <poseidon/poseidon.h>

#if KCONFIG_SMP

/*
** Detect other CPUs to try and find out if SMP is availalbe.
**
** Return `true` if it is, `false` otherwise.
*/
bool
smp_detect(void)
{
    return (false);
}


#endif