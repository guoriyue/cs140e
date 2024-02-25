#include "rpi-asm.h"

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ <0-rfe-example.c>

@ use rfe to load pc and cpsr from the 2 entry
@ array passed in r0.
MK_FN(rfe_asm)
    mov sp, r0
    rfe sp
    asm_not_reached();

@ a trampoline that loads the stack pointer
@ and calls <rfe_c_code>
MK_FN(rfe_trampoline_asm)
    mov sp, #INT_STACK_ADDR
    mov r0, sp
    bl rfe_c_code
    asm_not_reached();

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ <1-rfe-blk.c>

@ tiny change: modify rfe_asm to deal with being 
@ passed a 17-entry register array
MK_FN(blk_rfe_asm)
    @ asm_todo("implement this");
    mov sp, r0
    
    add r1, sp, #60  @ 15
    add r2, sp, #64  @ 16

    @ ldr lr, {r1}
    mov lr, r1
    msr cpsr, r2
    mov sp, r1

    @ stm r1, {sp}

    rfe sp
    asm_not_reached();

@ do not need to modify: same as original, so just
@ call it.
MK_FN(blk_trampoline_asm)
    bl rfe_trampoline_asm
    asm_not_reached();


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@ <1-rfe-load-sp.c>

@ copy-paste <blk_rfe_asm> above and change it 
@ so it:
@  1. loads r0-r14 into user mode registers
@     (hint: look back at your <0-user-sp-lr> 
@     code)
@  2. does an rfe 
MK_FN(switchto_user_asm)
    @ asm_todo("implement this");
    mov sp, r0
    
    add r1, sp, #60  @ 15
    @ add r2, sp, #64  @ 16

    @ ldr lr, {r1}
    mov sp, r1

    ldm r0, {r0-r12,sp,lr}^

    rfe sp
    asm_not_reached();


@ so we don't get linker errors 
.weak rfe_c_code
rfe_c_code:
    asm_not_reached()


MK_FN(lr_get)
    mov r0, lr
    bx lr

MK_FN(sp_get)
    mov r0, sp
    bx lr

.weak do_another_mode
do_another_mode:
    asm_not_reached()

MK_FN(switchto_anymode_asm)
    add sp, r0, #60

    add r1, r0, #52 @ 13
    add r2, r0, #56 @ 14

    mrs r3, cpsr
    msr cpsr_c, r0
    
    ldm r1, {sp}
    ldm r2, {lr}
    msr cpsr_c, r3

    ldm r0, {r0-r12}
    rfe sp
    asm_not_reached();