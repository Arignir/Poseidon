/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#include <arch/x86_64/interrupt.h>
#include <poseidon/poseidon.h>
#include <poseidon/interrupt.h>
#include <poseidon/cpu/cpu.h>
#include <lib/log.h>

void
exception_breakpoint(
    struct iframe *iframe
) {
    logln(
        "Breakpoint encountered (Core %u):\n"
        "    RAX=%p RBX=%p RCX=%p RDX=%p\n"
        "    RSI=%p RDI=%p RBP=%p RSP=%p\n"
        "     R8=%p  R9=%p R10=%p R11=%p\n"
        "    R12=%p R13=%p R14=%p R15=%p\n"
        "    RIP=%p RFLAGS=%08zx",
        (uint32)current_cpu()->cpu_id,
        iframe->rax,
        iframe->rbx,
        iframe->rcx,
        iframe->rdx,
        iframe->rsi,
        iframe->rdi,
        iframe->rbp,
        iframe->rsp,
        iframe->r8,
        iframe->r9,
        iframe->r10,
        iframe->r11,
        iframe->r12,
        iframe->r13,
        iframe->r14,
        iframe->r15,
        iframe->rip,
        iframe->rflags.raw
    );
}