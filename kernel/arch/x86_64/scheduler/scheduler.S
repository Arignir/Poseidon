/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2020 - The Poseidon Authors
**
\******************************************************************************/

/*
**
** void enter_scheduler(void *scheduler_stack)
**
** This function enters the scheduler by calling `reschedule()` and returns at
** a later time, when the thread is scheduled again.
**
** It enters the scheduler by following those four steps:
**   1) Push the content of all registers on the stack
**   2) Saves the current stack pointer
**   3) Switch to the given scheduler stack
**   4) Return, and by doing that effectively enter the scheduler
**
** When the scheduler returns (from an other thread switching to this one) the
** execution is resumed nicely by following those steps:
**   1) Switch to the new stack returned by the scheduler
**   2) Restore the content of the registers
**   3) Return and continue the execution of the new thread
**
** _WARNING_:
**   * Interrupts must be disabled before calling this function.
**   * The current thread must be read-write locked.
*/
.global enter_scheduler
.type enter_scheduler, @function
enter_scheduler:

    /* Push all preserved registers */
    push %rbx
    push %rbp
    push %r12
    push %r13
    push %r14
    push %r15

    /* Push RFLAGS */
    pushfq

    /* Save the current stack in RCX */
    mov %rsp, %rcx

    /* Swap the current stack with the scheduler's one */
    mov %rdi, %rsp

    /* Set the current stack (RCX) as the first argument of `reschedule()` */
    mov %rcx, %rdi

    .extern reschedule
    call reschedule

    /* Swap the scheduler's stack to the new thread's stack */
    mov %rax, %rsp

    /* Pop RFLAGS */
    popfq

    /* Pop all preserved registers */
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbp
    pop %rbx

    ret

/*
** void enter_scheduler_at(void *scheduler_stack, void (*func_ptr)(void)) __noreturn;
*/
.global enter_scheduler_at
.type enter_scheduler_at, @function
enter_scheduler_at:

    /* Swap to the scheduler stack, given in RDI*/
    mov %rdi, %rsp

    /* Call the given function */
    call *%rsi

    /* Should never reach this point */

end:
    cli
    hlt
    jmp end