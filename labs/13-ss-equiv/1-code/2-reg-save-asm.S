#include "rpi-asm.h"

@ save all the general registers and the spsr in 
@ ascending order onto a block of memory on the
@ stack.  the pointer to the base of this to 
@ do_syscall (defined in `2-reg-save.c` and 
@ `2-reg-save-full.c`).
@
swi_trampoline:
    @ asm_todo("implement this")
    
    @push {r0-r12}
    @stm r0, {sp}
    @stm r1, {sp}
    @stm r2, {sp}



    @mov r0, lr
    @mov r1, sp
    @mrs r2, spsr
    @push {r0-r2}
    @push {r4-r12}
    @mov r4, sp
    @sub sp, sp, #4
    @ldm lr, {sp}
    @add sp, sp, #4
    @sub sp, sp, #4
    @ldm r14, {sp}
    @mrs r3, spsr
    @push {r3}


    @mov lr, sp
    
    sub sp, sp, #64

    @stm sp, {r0-r12}
    str r0, [sp]
    add r0, sp, #4
    str r1, [r0]
    add r0, sp, #8
    str r2, [r0]
    add r0, sp, #12
    str r3, [r0]
    add r0, sp, #16
    str r4, [r0]


    add r0, sp, #20
    str r5, [r0]
    add r0, sp, #24
    str r6, [r0]
    add r0, sp, #28
    str r7, [r0]
    add r0, sp, #32
    str r8, [r0]
    add r0, sp, #36
    str r9, [r0]
    add r0, sp, #40
    str r10, [r0]
    add r0, sp, #44
    str r11, [r0]
    add r0, sp, #48
    str r12, [r0]

    @sub sp, sp, #52
    add r0, sp, #56
    stm r0, {lr}^
    @str r1, [r0]
    add r0, sp, #52
    stm r0, {sp}^
    @str r1, [r0]
    add r0, sp, #60
    @str r1, lr
    str lr, [r0]
    add r0, sp, #66
    mrs r3, spsr
    str r3, [r0]
    @str lr, [r0]
    @str lr, [sp, #56]!

    @stm sp, {lr}
    
    @mrs r3, spsr
    @str r3, [sp, #60]!

    mov r0, sp

    
    @mov r0, sp
    bl do_syscall
    @ note: this trampoline does not resume.
    asm_not_reached();

@ only handle single step.
.align 5; 
.globl swi_test_handlers
swi_test_handlers:
    b unhandled_reset
    b unhandled_undefined_instruction
    b swi_trampoline
    b unhandled_prefetch_abort 
    b unhandled_data_abort
    b unhandled_interrupt
    b unhandled_fiq


.weak do_syscall
do_syscall:
    asm_not_reached();
