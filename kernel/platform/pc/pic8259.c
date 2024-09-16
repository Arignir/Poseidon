/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** A driver for the Programmable Interrupt Controller (PIC) present in (nearly)
** all PC computers.
**
** References:
**
** Programmable Interrupt Controller (8259A/8259A-2)
**   https://pdos.csail.mit.edu/6.828/2008/readings/hardware/8259A.pdf
*/

#include <arch/x86_64/io.h>
#include <arch/x86_64/interrupt.h>

/*
** Define two ports: the master and slave PIC.
*/
NEW_IO_PORT(master, 0x20);
NEW_IO_PORT(slave, 0xA0);

/*
** Define two offsets: one to send commands and one to send data.
*/
#define CMD    0x0
#define DATA   0x1

/*
** Current IRQs mask on slave:master.
** 0 = unmask, 1 = mask.
*/
static ushort pic_mask = 0;

/*
** Mask the IRQ.
**
** Note that `irq` should *NOT* be the absolute interrupt vector associated with
** the IRQ (like `INT_IRQ0`), but the actual IRQ number as understood by the
** chip, which range from 0 to 15.
*/
void
pic8259_irq_mask(uchar irq)
{
    debug_assert(irq <= 0xF);

    // Send an OCW1
    pic_mask |= (1 << irq);
    if (irq <= 0x7) {
        io_port_out8_offset(master, DATA, pic_mask & 0xFF);
    } else {
        io_port_out8_offset(slave, DATA, (pic_mask >> 8) & 0xFF);
    }
}

/*
** Unmask the IRQ.
**
** Note that `irq` should *NOT* be the absolute interrupt vector associated with
** the IRQ (like `INT_IRQ0`), but the actual IRQ number as understood by the
** chip, which range from 0 to 15.
*/
void
pic8259_irq_unmask(uchar irq)
{
    debug_assert(irq <= 0xF);

    // Unmask the IRQ
    pic_mask &= ~(1 << irq);

    // If the IRQ is handled by the slave AND the slave is masked,
    // then unmask the slave
    if (irq >= 0x8 && (pic_mask & (1 << 2))) {
        pic_mask &= ~(1 << 2);
        io_port_out8_offset(master, DATA, pic_mask & 0xFF);
    }

    // Send the OCW1 to the corresponding PIC
    if (irq <= 0x7) {
        io_port_out8_offset(master, DATA, pic_mask & 0xFF);
    } else {
        io_port_out8_offset(slave, DATA, (pic_mask >> 8) & 0xFF);
    }
}

/*
** Set the IRQ mask.
**
** For each bit of `mask`, 1 indicate that the associate IRQ is masked
** and 0 indicate that the associate IRQ is not masked.
**
** Therefore, if `mask` == 0xFFFF, all IRQ are masked, and if `mask` = 0, all
** IRQs are not masked.
*/
void
pic8259_set_irq_mask(ushort mask)
{
    // Send an OCW1
    pic_mask = mask;

    io_port_out8_offset(master, DATA, pic_mask & 0xFF);
    io_port_wait();
    io_port_out8_offset(slave, DATA, (pic_mask >> 8) & 0xFF);
}

/*
** Send an End Of Interrupt (EOI) to the master PIC.
*/
void
pic8259_master_eoi(void)
{
    // Send an OCW2 with EOI = 1.
    io_port_out8_offset(master, CMD, 0b00100000);
    io_port_wait();
}

/*
** Send an End Of Interrupt (EOI) to the slave PIC.
*/
void
pic8259_slave_eoi(void)
{
    // Send an OCW2 with EOI = 1.
    io_port_out8_offset(slave, CMD, 0b00100000);
}

/*
** Initialize the obsolete 8259A PIC and entierely mask it.
**
** We also remap the IRQs in case of a suprious interrupt, which can
** still be fired even if the PIC is entierely masked.
**
** This function does not use the init hook system because it is required at
** a very specific moment.
*/
__boot_text
status_t
pic8259_init(void)
{
    /*
    ** The PIC is initialized using a set of Initialization Command Words (ICW).
    **
    ** Those commands indicate how to initialize the PIC, with which parameter
    ** etc. They are 8 bits long.
    **
    ** The following few chunks of documentation explain the ICW for both
    ** the MCS-80/85 and i386 architecture, but we are only interested in the
    ** i386 side of it.
    **
    ** On i386, A5-A10 and ADI are ignored and the Interrupt Vector Address
    ** (aka T3-T7) are inserted in the five most significant bits of the
    ** vectoring byte.
    ** Therefore, they act as a way to remap the interrupt vector.
    */

    /*
    ** The first one (ICW1) is as follow:
    **
    ** | A7 | A6 | A5 | 1 | LTIM | ADI | SNGL | IC4 |
    **
    **   A5-A7: Interrupt Vector Address (MCS-80/85 only!)
    **   LTIM:
    **     0 = edge triggered mode
    **     1 = level triggered mode
    **   ADI: Call address interval
    **     0 = 4
    **     1 = 8
    **   SNGL: 1 = Single, 0 = Cascade mode
    **   IC4: Indicate that a forth ICW is needed (ICW4)
    */
    uchar icw1 = 0b00010001; // Turn IC4 and mandatory bit D4 on.

    io_port_out8_offset(master, CMD, icw1);
    io_port_wait();

    io_port_out8_offset(slave, CMD, icw1);
    io_port_wait();

    /*
    ** The second one (ICW2) is as follow:
    **
    ** | A15/T7 | A14/T6 | A13/T5 | A12/T4 | A11/T3 | A10 | A9 | A8 |
    **
    **   A8-A15: Interrupt Vector Address (MCS-80/85 only!)
    **   T3-T7: Interrupt Vector Address (i386 only!)
    */
    uchar icw2_master = INT_IRQ0; // Remap the interrupts to 0x20-0x27
    uchar icw2_slave = INT_IRQ8; // Remap the interrupts to 0x28-0x2F

    io_port_out8_offset(master, DATA, icw2_master);
    io_port_wait();

    io_port_out8_offset(slave, DATA, icw2_slave);
    io_port_wait();

    /*
    ** The third one (ICW3) is used only if ICW1.SNGL = 0.
    **
    ** Its meaning differs for the slave and the master PIC:
    **
    ** For master:
    **   | S7 | S6 | S5 | S4 | S3 | S2 | S1 | S0 |
    **
    **   S0-7: Indicate that the slave is present.
    **
    ** For slave:
    **   | 0 | 0 | 0 | 0 | 0 | ID2 | ID1 | ID0 |
    **
    **   ID0-2: The slave ID
    **
    ** We have one slave, with ID 2 connected at IRQ2.
    */
    uchar icw3_master = 0b00000100;
    uchar icw3_slave = 0b00000010;

    io_port_out8_offset(master, DATA, icw3_master);
    io_port_wait();

    io_port_out8_offset(slave, DATA, icw3_slave);
    io_port_wait();

    /*
    ** The forth one (ICW4) is used only if ICW1.IC4 = 1.
    **
    ** | 0 | 0 | 0 | SFNM | BUF | M/S | AOEI | µPM |
    **
    **   SFNM: Special fully nested mode
    **   BUF: Buffered mode
    **   M/S: If ICW4.BUF = 1:
    **     0 = Programmed to be a slave
    **     1 = Programmed to be a master
    *      If ICW4.BUF = 0, M/S has no function.
    **   AOEI: Automatic end of interrupt
    **   µPM: Micro Processor Mode.
    **     0 = MCS-80
    **     1 = 8086
    */
    uchar icw4 = 0b00000011; // Automatic EOI, 8086 mode, non-buffered.

    io_port_out8_offset(master, DATA, icw4);
    io_port_wait();

    io_port_out8_offset(slave, DATA, icw4);
    io_port_wait();

    // Mask all IRQs
    pic8259_set_irq_mask(0xFFFF);

    return (OK);
}
